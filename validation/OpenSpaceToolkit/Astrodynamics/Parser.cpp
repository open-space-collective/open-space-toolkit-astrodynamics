/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Parser.hpp>

namespace ostk
{
namespace astro
{
namespace validation
{

Dictionary Parser::ParseYaml(const String& aPathToData, const String& aScenarioName)
{
    const String scenarioPath = String::Format("{0}/scenarios/{1}.yaml", aPathToData, aScenarioName);

    const File scenarioFile = File::Path(Path::Parse(scenarioPath));

    return CntrObject::Load(scenarioFile, CntrObject::Format::YAML).accessDictionary();
}

Table Parser::ParseCSV(const String& aPathToData, const String& aScenarioName, const Tool& aTool)
{
    String csvPath;
    if (aTool == Tool::GMAT)
    {
        csvPath = String::Format("{0}/gmat_astrodynamics/{1}.csv", aPathToData, aScenarioName);
    }
    else if (aTool == Tool::OREKIT)
    {
        csvPath = String::Format("{0}/orekit_astrodynamics/{1}.csv", aPathToData, aScenarioName);
    }
    else
    {
        throw ostk::core::error::runtime::Wrong("Tool not recognized.");
    }

    return Table::Load(File::Path(Path::Parse(csvPath)), Table::Format::CSV, true);
}

SatelliteSystem Parser::CreateSatelliteSystem(const Dictionary& aDictionary)
{
    const Dictionary spacecraft = aDictionary["data"]["spacecraft"].accessDictionary();

    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    PropulsionSystem propulsionSystem = PropulsionSystem::Undefined();
    if (spacecraft.hasValueForKey("thruster"))
    {
        propulsionSystem = PropulsionSystem(
            spacecraft["thruster"]["data"]["thrust"].accessReal(),
            spacecraft["thruster"]["data"]["specific-impulse"].accessReal()
        );
    }

    return {
        Mass::Kilograms(0.0),
        satelliteGeometry,
        Matrix3d::Identity(),
        spacecraft["drag-cross-section"].accessReal(),
        spacecraft["drag-coefficient"].accessReal(),
        propulsionSystem
    };
}

State Parser::CreateInitialState(const Dictionary& aDictionary, const SatelliteSystem& aStatelliteSystem)
{
    const Dictionary spacecraft = aDictionary["data"]["spacecraft"].accessDictionary();
    // Make entire satellite wet mass
    const Mass wetMass = Mass::Kilograms(spacecraft["mass"].accessReal());

    const Dictionary orbit = spacecraft["orbit"].accessDictionary();

    Instant initialInstant = Instant::Undefined();
    if (orbit["data"]["date"]["time-scale"].accessString() == "UTC")
    {
        initialInstant = Instant::DateTime(
            DateTime::Parse(orbit["data"]["date"]["value"].accessString(), DateTime::Format::ISO8601), Scale::UTC
        );
    }
    else
    {
        throw ostk::core::error::runtime::Wrong("Time scale");
    }

    const Shared<const Frame> frame =
        orbit["data"]["frame"].accessString() == "GCRF" ? Frame::GCRF() : Frame::Undefined();

    VectorXd coordinates(9);

    if (orbit["type"].accessString() == "CARTESIAN")
    {
        const Position position = Position::Meters(
            Vector3d(orbit["data"]["x"].accessReal(), orbit["data"]["y"].accessReal(), orbit["data"]["z"].accessReal()),
            frame
        );

        const Velocity velocity = Velocity::MetersPerSecond(
            Vector3d(
                orbit["data"]["vx"].accessReal(), orbit["data"]["vy"].accessReal(), orbit["data"]["vz"].accessReal()
            ),
            frame
        );

        coordinates << position.getCoordinates(), velocity.getCoordinates(), wetMass.inKilograms(),
            aStatelliteSystem.getCrossSectionalSurfaceArea(), aStatelliteSystem.getDragCoefficient();
    }
    else if (orbit["type"].accessString() == "KEPLERIAN")
    {
        throw ostk::core::error::runtime::Wrong("KEPLERIAN initial conditions not yet supported.");
    }

    const Array<Shared<const CoordinatesSubset>> subsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
        CoordinatesSubset::SurfaceArea(),
        CoordinatesSubset::DragCoefficient()
    };

    return StateBuilder(frame, subsets).build(initialInstant, coordinates);
}

Environment Parser::CreateEnvironment(const Dictionary& aDictionary)
{
    Array<Shared<Object>> celestials = Array<Shared<Object>>::Empty();

    EarthGravitationalModel::Type earthGravitationalModelType = EarthGravitationalModel::Type::Undefined;
    Integer earthGravitationalModelDegree = 0;
    Integer earthGravitationalModelOrder = 0;
    EarthMagneticModel::Type earthMagneticModelType = EarthMagneticModel::Type::Undefined;
    EarthAtmosphericModel::Type earthAtmosphericModelType = EarthAtmosphericModel::Type::Undefined;

    for (const auto& force : aDictionary["data"]["sequence"]["forces"].accessArray())
    {
        if (force["type"].accessString() == "GRAVITY")
        {
            if (force["data"]["body"].accessString() == "EARTH")
            {
                if (force["data"]["model"].accessString() == "EGM96")
                {
                    earthGravitationalModelType = EarthGravitationalModel::Type::EGM96;
                    earthGravitationalModelDegree = force["data"]["degree"].accessInteger();
                    earthGravitationalModelOrder = force["data"]["order"].accessInteger();
                }
                else
                {
                    throw ostk::core::error::runtime::Wrong("Earth gravitational");
                }
            }
            else if (force["data"]["body"].accessString() == "SUN")
            {
                celestials.add(std::make_shared<Sun>(Sun::Default()));
            }
            else if (force["data"]["body"].accessString() == "MOON")
            {
                celestials.add(std::make_shared<Moon>(Moon::Default()));
            }
            else
            {
                throw ostk::core::error::runtime::Wrong("Planet");
            }
        }
        else if (force["type"].accessString() == "ATMOSPHERIC_DRAG")
        {
            if (force["data"]["body"].accessString() == "EARTH")
            {
                if (force["data"]["model"].accessString() == "NRLMSISE00")
                {
                    earthAtmosphericModelType = EarthAtmosphericModel::Type::NRLMSISE00;
                }
                else if (force["data"]["model"].accessString() == "EXPONENTIAL")
                {
                    earthAtmosphericModelType = EarthAtmosphericModel::Type::Exponential;
                }
                else
                {
                    throw ostk::core::error::runtime::Wrong("Earth atmospheric model");
                }
            }
            else
            {
                throw ostk::core::error::runtime::Wrong("Celestial Body");
            }
        }
        else
        {
            throw ostk::core::error::runtime::Wrong("Force type");
        }
    }

    celestials.add(std::make_shared<Earth>(Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(
            earthGravitationalModelType,
            Directory::Undefined(),
            earthGravitationalModelDegree,
            earthGravitationalModelOrder
        ),
        std::make_shared<EarthMagneticModel>(earthMagneticModelType),
        std::make_shared<EarthAtmosphericModel>(earthAtmosphericModelType)
    )));

    return Environment(Instant::J2000(), celestials);
}

Sequence Parser::CreateSequence(
    const Dictionary& aDictionary,
    const SatelliteSystem& aSatelliteSystem,
    const Array<Shared<Dynamics>>& aDynamicsArray
)
{
    // Create numerical solver
    NumericalSolver numericalSolver = NumericalSolver::Undefined();
    if (aDictionary["data"]["sequence"]["propagator"]["type"].accessString() == "RUNGE_KUTTA_DORMAND_PRINCE_45")
    {
        numericalSolver = NumericalSolver(
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaDopri5,
            aDictionary["data"]["sequence"]["propagator"]["data"]["initial-step"].accessReal(),
            aDictionary["data"]["sequence"]["propagator"]["data"]["relative-tolerance"].accessReal(),
            aDictionary["data"]["sequence"]["propagator"]["data"]["absolute-tolerance"].accessReal()
        );
    }
    else if (aDictionary["data"]["sequence"]["propagator"]["type"].accessString() == "RUNGE_KUTTA_4")
    {
        numericalSolver = NumericalSolver::FixedStepSize(
            NumericalSolver::StepperType::RungeKutta4,
            aDictionary["data"]["sequence"]["propagator"]["data"]["step"].accessReal()
        );
    }
    else
    {
        throw ostk::core::error::runtime::Wrong("Propagator type");
    }

    Array<Segment> segments = Array<Segment>::Empty();
    for (const auto& segment : aDictionary["data"]["sequence"]["segments"].accessArray())
    {
        segments.add(
            Parser::CreateSegment(segment.accessDictionary(), aSatelliteSystem, aDynamicsArray, numericalSolver)
        );
    }

    return {
        segments,
        numericalSolver,
        Array<Shared<Dynamics>>::Empty(),
        Duration::Seconds(aDictionary["data"]["sequence"]["max-duration"].accessReal()),
        0,
    };
}

Array<Instant> Parser::CreateComparisonInstants(
    const Dictionary& aDictionary, const Instant& anInitialInstant, const Instant& aFinalInstant
)
{
    const Real step = aDictionary["data"]["output"]["step"].accessReal();

    const Interval interval = Interval::Closed(anInitialInstant, aFinalInstant);
    Array<Instant> comparisonInstants = interval.generateGrid(Duration::Seconds(step));

    const Size estimatedNumberOfComparisonInstants =
        Real(std::round(interval.getDuration().inSeconds() / step)).toInteger() + 1;

    // Sanitize comparison instant array to avoid numerical errors from segment stop conditions and numerical solver
    const Duration durationTolerance = Duration::Microseconds(5);

    if (estimatedNumberOfComparisonInstants != comparisonInstants.getSize())
    {
        const Duration instantDelta = (comparisonInstants[comparisonInstants.getSize() - 1] -
                                       comparisonInstants[comparisonInstants.getSize() - 2])
                                          .getAbsolute();
        if (instantDelta > durationTolerance)
        {
            throw ostk::core::error::RuntimeError("Comparison instants not exactly lining up.");
        }
        else
        {
            comparisonInstants.pop_back();
        }
    }

    return comparisonInstants;
}

Segment Parser::CreateSegment(
    const Dictionary& segmentDictionary,
    const SatelliteSystem& aSatelliteSystem,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const NumericalSolver& aNumericalSolver
)
{
    // Determine segment duration
    Shared<RealCondition> conditionSPtr = nullptr;
    if (segmentDictionary["data"]["stop-condition"]["type"].accessString() == "RELATIVE_TIME")
    {
        conditionSPtr = std::make_shared<RealCondition>(RealCondition::DurationCondition(
            RealCondition::Criterion::StrictlyPositive,
            Duration::Seconds(segmentDictionary["data"]["stop-condition"]["data"]["duration"].accessReal())
        ));
    }
    else if (segmentDictionary["data"]["stop-condition"]["type"].accessString() == "ABSOLUTE_TIME")
    {
        if (segmentDictionary["data"]["stop-condition"]["data"]["time-scale"].accessString() == "UTC")
        {
            conditionSPtr = std::make_shared<InstantCondition>(InstantCondition(
                InstantCondition::Criterion::StrictlyPositive,
                Instant::DateTime(
                    DateTime::Parse(
                        segmentDictionary["data"]["stop-condition"]["data"]["value"].accessString(),
                        DateTime::Format::ISO8601
                    ),
                    Scale::UTC
                )
            ));
        }
        else
        {
            throw ostk::core::error::runtime::Wrong("Time scale");
        }
    }
    else
    {
        throw ostk::core::error::runtime::Wrong("Segment event condition type.");
    }

    // Create thrust or coast segment
    if (segmentDictionary["type"].accessString() == "COAST")
    {
        return Segment::Coast("coast", conditionSPtr, aDynamicsArray, aNumericalSolver);
    }
    else if (segmentDictionary["type"].accessString() == "CONSTANT_DIRECTION_MANEUVER")
    {
        if ((segmentDictionary["data"]["attitude"]["type"].accessString() != "LOF") ||
            (segmentDictionary["data"]["attitude"]["data"]["lof"].accessString() != "VNC"))
        {
            throw ostk::core::error::runtime::Wrong("Attitude type or local orbital type");
        }

        const Shared<const Frame> frame =
            segmentDictionary["data"]["attitude"]["data"]["parent"].accessString() == "GCRF" ? Frame::GCRF()
                                                                                             : Frame::Undefined();

        // Create thruster dynamics
        const Vector3d maneuverVector = {
            segmentDictionary["data"]["attitude"]["data"]["direction"].accessArray()[0].accessReal(),
            segmentDictionary["data"]["attitude"]["data"]["direction"].accessArray()[1].accessReal(),
            segmentDictionary["data"]["attitude"]["data"]["direction"].accessArray()[2].accessReal()
        };

        const Shared<ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(
            ConstantThrust(LocalOrbitalFrameDirection(maneuverVector, LocalOrbitalFrameFactory::VNC(frame)))
        );
        const Shared<Thruster> aThrusterDynamicSPtr =
            std::make_shared<Thruster>(Thruster(aSatelliteSystem, constantThrustSPtr));

        return Segment::Maneuver("maneuver", conditionSPtr, aThrusterDynamicSPtr, aDynamicsArray, aNumericalSolver);
    }
    else
    {
        throw ostk::core::error::runtime::Wrong("Segment type");
    }
}

}  // namespace validation
}  // namespace astro
}  // namespace ostk
