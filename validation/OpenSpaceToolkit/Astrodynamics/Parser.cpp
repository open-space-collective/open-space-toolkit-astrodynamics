/// Apache License 2.0

#include <optional>

#include <OpenSpaceToolkit/Astrodynamics/Parser.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace validation
{

Dictionary Parser::ParseYaml(const String& aPathToData, const String& aScenarioName)
{
    const String scenarioPath = String::Format("{0}/{1}.yaml", aPathToData, aScenarioName);

    const File scenarioFile = File::Path(Path::Parse(scenarioPath));

    return CntrObject::Load(scenarioFile, CntrObject::Format::YAML).getDictionary();
}

Table Parser::ParseCSV(const String& aPathToData, const String& aScenarioName, const Tool& aTool)
{
    const String csvPath =
        String::Format("{0}/{1}/{2}.csv", aPathToData, CrossValidator::ToolToPath(aTool), aScenarioName);

    return Table::Load(File::Path(Path::Parse(csvPath)), Table::Format::CSV, true);
}

SatelliteSystem Parser::CreateSatelliteSystem(const Dictionary& aDictionary)
{
    const Dictionary spacecraft = aDictionary["data"]["spacecraft"].getDictionary();

    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    std::optional<PropulsionSystem> propulsionSystem = std::nullopt;
    if (spacecraft.hasValueForKey("thruster"))
    {
        propulsionSystem = PropulsionSystem(
            spacecraft["thruster"]["data"]["thrust"].getReal(),
            spacecraft["thruster"]["data"]["specific-impulse"].getReal()
        );
    }

    return {
        Mass::Kilograms(0.0),
        satelliteGeometry,
        Matrix3d::Identity(),
        spacecraft["drag-cross-section"].getReal(),
        spacecraft["drag-coefficient"].getReal(),
        propulsionSystem
    };
}

State Parser::CreateInitialState(const Dictionary& aDictionary, const SatelliteSystem& aStatelliteSystem)
{
    const Dictionary spacecraft = aDictionary["data"]["spacecraft"].getDictionary();
    // Make entire satellite wet mass
    const Mass wetMass = Mass::Kilograms(spacecraft["mass"].getReal());

    const Dictionary orbit = spacecraft["orbit"].getDictionary();

    if (orbit["data"]["date"]["time-scale"].getString() != "UTC")
    {
        throw ostk::core::error::runtime::Wrong("Time scale", orbit["data"]["date"]["time-scale"].getString());
    }
    const Instant initialInstant = Instant::DateTime(
        DateTime::Parse(orbit["data"]["date"]["value"].getString(), DateTime::Format::ISO8601), Scale::UTC
    );

    if (orbit["data"]["frame"].getString() != "GCRF")
    {
        throw ostk::core::error::runtime::Wrong("Initial Condition frame", orbit["data"]["frame"].getString());
    }
    const Shared<const Frame> frame = Frame::GCRF();

    VectorXd coordinates(9);
    if (orbit["type"].getString() == "CARTESIAN")
    {
        const Position position = Position::Meters(
            Vector3d(orbit["data"]["x"].getReal(), orbit["data"]["y"].getReal(), orbit["data"]["z"].getReal()), frame
        );

        const Velocity velocity = Velocity::MetersPerSecond(
            Vector3d(orbit["data"]["vx"].getReal(), orbit["data"]["vy"].getReal(), orbit["data"]["vz"].getReal()), frame
        );

        coordinates << position.getCoordinates(), velocity.getCoordinates(), wetMass.inKilograms(),
            aStatelliteSystem.getCrossSectionalSurfaceArea(), aStatelliteSystem.getDragCoefficient();
    }
    else if (orbit["type"].getString() == "KEPLERIAN")
    {
        throw ostk::core::error::runtime::Wrong("Orbit type", orbit["type"].getString());
    }

    const Array<Shared<const CoordinateSubset>> subsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
        CoordinateSubset::SurfaceArea(),
        CoordinateSubset::DragCoefficient()
    };

    return StateBuilder(frame, subsets).build(initialInstant, coordinates);
}

Environment Parser::CreateEnvironment(const Dictionary& aDictionary)
{
    Array<Shared<const Object>> celestials = Array<Shared<const Object>>::Empty();

    Shared<EarthGravitationalModel> earthGravitationalModelSPtr = nullptr;
    Shared<EarthAtmosphericModel> earthAtmosphericModelSPtr = nullptr;

    for (const auto& force : aDictionary["data"]["sequence"]["forces"].getArray())
    {
        if (force["type"].getString() == "GRAVITY")
        {
            if (force["data"]["body"].getString() == "EARTH")
            {
                if (force["data"]["model"].getString() == "EGM96")
                {
                    earthGravitationalModelSPtr = std::make_shared<EarthGravitationalModel>(
                        EarthGravitationalModel::Type::EGM96,
                        std::nullopt,
                        force["data"]["degree"].getInteger(),
                        force["data"]["order"].getInteger()
                    );
                }
                else
                {
                    throw ostk::core::error::runtime::Wrong(
                        "Earth gravitational model", force["data"]["model"].getString()
                    );
                }
            }
            else if (force["data"]["body"].getString() == "SUN")
            {
                celestials.add(std::make_shared<const Sun>(Sun::Default()));
            }
            else if (force["data"]["body"].getString() == "MOON")
            {
                celestials.add(std::make_shared<const Moon>(Moon::Default()));
            }
            else
            {
                throw ostk::core::error::runtime::Wrong("Planet", force["data"]["body"].getString());
            }
        }
        else if (force["type"].getString() == "ATMOSPHERIC_DRAG")
        {
            if (force["data"]["body"].getString() == "EARTH")
            {
                if (force["data"]["model"].getString() == "NRLMSISE00")
                {
                    earthAtmosphericModelSPtr =
                        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::NRLMSISE00);
                }
                else if (force["data"]["model"].getString() == "EXPONENTIAL")
                {
                    earthAtmosphericModelSPtr =
                        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential);
                }
                else
                {
                    throw ostk::core::error::runtime::Wrong(
                        "Earth atmospheric model", force["data"]["model"].getString()
                    );
                }
            }
            else
            {
                throw ostk::core::error::runtime::Wrong("Celestial Body", force["data"]["body"].getString());
            }
        }
        else
        {
            throw ostk::core::error::runtime::Wrong("Force type", force["type"].getString());
        }
    }

    // No magnetic model is used by the validation cases
    celestials.add(std::make_shared<const Earth>(
        Earth::FromModels(earthGravitationalModelSPtr, nullptr, earthAtmosphericModelSPtr)
    ));

    return Environment(Instant::J2000(), celestials);
}

Sequence Parser::CreateSequence(
    const Dictionary& aDictionary,
    const SatelliteSystem& aSatelliteSystem,
    const Array<Shared<Dynamics>>& aDynamicsArray
)
{
    // Create numerical solver
    const NumericalSolver numericalSolver = [&aDictionary]() -> NumericalSolver
    {
        if (aDictionary["data"]["sequence"]["propagator"]["type"].getString() == "RUNGE_KUTTA_DORMAND_PRINCE_45")
        {
            return NumericalSolver(
                NumericalSolver::LogType::NoLog,
                NumericalSolver::StepperType::RungeKuttaFehlberg78,
                aDictionary["data"]["sequence"]["propagator"]["data"]["initial-step"].getReal(),
                aDictionary["data"]["sequence"]["propagator"]["data"]["relative-tolerance"].getReal(),
                aDictionary["data"]["sequence"]["propagator"]["data"]["absolute-tolerance"].getReal()
            );
        }

        if (aDictionary["data"]["sequence"]["propagator"]["type"].getString() == "RUNGE_KUTTA_4")
        {
            return NumericalSolver::FixedStepSize(
                NumericalSolver::StepperType::RungeKutta4,
                aDictionary["data"]["sequence"]["propagator"]["data"]["step"].getReal()
            );
        }

        throw ostk::core::error::runtime::Wrong(
            "Propagator type", aDictionary["data"]["sequence"]["propagator"]["type"].getString()
        );
    }();

    Array<Segment> segments = Array<Segment>::Empty();
    for (const auto& segment : aDictionary["data"]["sequence"]["segments"].getArray())
    {
        segments.add(Parser::CreateSegment(segment.getDictionary(), aSatelliteSystem, aDynamicsArray, numericalSolver));
    }

    return {
        segments,
        numericalSolver,
        Array<Shared<Dynamics>>::Empty(),
        Duration::Seconds(aDictionary["data"]["sequence"]["max-duration"].getReal()),
    };
}

Array<Instant> Parser::CreateComparisonInstants(
    const Dictionary& aDictionary, const Instant& anInitialInstant, const Instant& aFinalInstant
)
{
    const double step = aDictionary["data"]["output"]["step"].getReal();

    const Interval interval = Interval::Closed(anInitialInstant, aFinalInstant);
    const Duration stepDuration = Duration::Seconds(step);
    Array<Instant> comparisonInstants = interval.generateGrid(stepDuration);

    const Size estimatedNumberOfComparisonInstants =
        static_cast<Size>(std::round(interval.getDuration().inSeconds() / step)) + 1;

    // Sanitize comparison instant array to avoid numerical errors from segment stop conditions and numerical solver
    const Duration durationTolerance = Duration::Microseconds(100.0);

    Duration instantDelta =
        (comparisonInstants[comparisonInstants.getSize() - 1] - comparisonInstants[comparisonInstants.getSize() - 2])
            .getAbsolute();

    if ((instantDelta < durationTolerance) || ((stepDuration - instantDelta) < durationTolerance))
    {
        if (estimatedNumberOfComparisonInstants < comparisonInstants.getSize())
        {
            comparisonInstants.pop_back();
        }
    }
    else
    {
        throw ostk::core::error::RuntimeError("Comparison instants are not equally spaced.");
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
    if (segmentDictionary["data"]["stop-condition"]["type"].getString() == "RELATIVE_TIME")
    {
        conditionSPtr = std::make_shared<RealCondition>(RealCondition::DurationCondition(
            RealCondition::Criterion::StrictlyPositive,
            Duration::Seconds(segmentDictionary["data"]["stop-condition"]["data"]["duration"].getReal())
        ));
    }
    else if (segmentDictionary["data"]["stop-condition"]["type"].getString() == "ABSOLUTE_TIME")
    {
        if (segmentDictionary["data"]["stop-condition"]["data"]["time-scale"].getString() == "UTC")
        {
            conditionSPtr = std::make_shared<InstantCondition>(InstantCondition(
                InstantCondition::Criterion::StrictlyPositive,
                Instant::DateTime(
                    DateTime::Parse(
                        segmentDictionary["data"]["stop-condition"]["data"]["value"].getString(),
                        DateTime::Format::ISO8601
                    ),
                    Scale::UTC
                )
            ));
        }
        else
        {
            throw ostk::core::error::runtime::Wrong(
                "Time scale", segmentDictionary["data"]["stop-condition"]["data"]["time-scale"].getString()
            );
        }
    }
    else
    {
        throw ostk::core::error::runtime::Wrong(
            "Segment event condition type", segmentDictionary["data"]["stop-condition"]["type"].getString()
        );
    }

    // Create thrust or coast segment
    if (segmentDictionary["type"].getString() == "COAST")
    {
        return Segment::Coast("coast", conditionSPtr, aDynamicsArray, aNumericalSolver);
    }
    else if (segmentDictionary["type"].getString() == "CONSTANT_DIRECTION_MANEUVER")
    {
        if ((segmentDictionary["data"]["attitude"]["type"].getString() != "LOF") ||
            (segmentDictionary["data"]["attitude"]["data"]["lof"].getString() != "VNC"))
        {
            throw ostk::core::error::runtime::Wrong(
                "Attitude type or local orbital type", segmentDictionary["data"]["attitude"]["type"].getString()
            );
        }

        if (segmentDictionary["data"]["attitude"]["data"]["parent"].getString() != "GCRF")
        {
            throw ostk::core::error::runtime::Wrong(
                "Maneuver parent frame", segmentDictionary["data"]["attitude"]["data"]["parent"].getString()
            );
        }

        // Create thruster dynamics
        const Vector3d maneuverVector = {
            segmentDictionary["data"]["attitude"]["data"]["direction"].getArray()[0].getReal(),
            segmentDictionary["data"]["attitude"]["data"]["direction"].getArray()[1].getReal(),
            segmentDictionary["data"]["attitude"]["data"]["direction"].getArray()[2].getReal()
        };

        const Shared<const Frame> frame = Frame::GCRF();
        const Shared<const ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(
            ConstantThrust(LocalOrbitalFrameDirection(maneuverVector, LocalOrbitalFrameFactory::VNC(frame)))
        );
        const Shared<Thruster> aThrusterDynamicSPtr =
            std::make_shared<Thruster>(Thruster(aSatelliteSystem, constantThrustSPtr));

        return Segment::Maneuver("maneuver", conditionSPtr, aThrusterDynamicSPtr, aDynamicsArray, aNumericalSolver);
    }
    else
    {
        throw ostk::core::error::runtime::Wrong("Segment type", segmentDictionary["type"].getString());
    }
}

}  // namespace validation
}  // namespace astrodynamics
}  // namespace ostk
