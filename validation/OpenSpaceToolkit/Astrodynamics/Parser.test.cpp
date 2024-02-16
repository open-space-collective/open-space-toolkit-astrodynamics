/// Apache License 2.0

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Astrodynamics/Parser.hpp>

using ostk::astrodynamics::validation::Quantity;
using ostk::astrodynamics::validation::QuantityComparison;
using ostk::astrodynamics::validation::Tool;
using ostk::astrodynamics::validation::ToolComparison;
using ostk::astrodynamics::validation::Parser;

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::container::Tuple;
using CntrObject = ostk::core::container::Object;
using ostk::core::container::Dictionary;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::filesystem::Directory;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::type::Size;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using AtmosphericModel = ostk::physics::environment::atmospheric::Model;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using GravitationalModel = ostk::physics::environment::gravitational::Model;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::ThirdBodyGravity;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::Segment;
using ostk::astrodynamics::trajectory::Sequence;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_Validation_Parser : public ::testing::Test
{
   protected:
    const String pathToData_ = {"/app/validation/OpenSpaceToolkit/Astrodynamics/data/test"};
    const SatelliteSystem defaultSatelliteSystem_ = SatelliteSystem::Default();
    const Array<Shared<Dynamics>> defaultDynamics_ = Dynamics::FromEnvironment(Environment::Default());
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_Parser, CreateSatelliteSystem)
{
    const String testScenario = {"test_parser_create_satellite_system"};
    const Dictionary dataTree = Parser::ParseYaml(pathToData_, testScenario);

    {
        const SatelliteSystem satelliteSystem =
            Parser::CreateSatelliteSystem(dataTree["success-no-prop-system"].accessDictionary());

        EXPECT_EQ(satelliteSystem.getMass(), Mass::Kilograms(0.0));
        EXPECT_EQ(satelliteSystem.getCrossSectionalSurfaceArea(), 1.0);
        EXPECT_EQ(satelliteSystem.getDragCoefficient(), 2.2);
        EXPECT_THROW(satelliteSystem.getPropulsionSystem(), ostk::core::error::runtime::Undefined);
    }

    {
        const SatelliteSystem satelliteSystem =
            Parser::CreateSatelliteSystem(dataTree["success-prop-system"].accessDictionary());

        EXPECT_EQ(satelliteSystem.getMass(), Mass::Kilograms(0.0));
        EXPECT_EQ(satelliteSystem.getCrossSectionalSurfaceArea(), 1.0);
        EXPECT_EQ(satelliteSystem.getDragCoefficient(), 2.2);
        EXPECT_EQ(satelliteSystem.getPropulsionSystem(), PropulsionSystem(0.01, 3000.0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_Parser, CreateInitialState)
{
    const String testScenario = {"test_parser_create_initial_state"};
    const Dictionary dataTree = Parser::ParseYaml(pathToData_, testScenario);

    VectorXd coordinates(9);
    coordinates << Vector3d(100.0, 200.0, 300.0), Vector3d(-1.0, -2.0, -3.0), 100.0,
        defaultSatelliteSystem_.getCrossSectionalSurfaceArea(), defaultSatelliteSystem_.getDragCoefficient();

    const State defaultInitialState =
        StateBuilder(
            Frame::GCRF(),
            {
                CartesianPosition::Default(),
                CartesianVelocity::Default(),
                CoordinateSubset::Mass(),
                CoordinateSubset::SurfaceArea(),
                CoordinateSubset::DragCoefficient(),
            }
        )
            .build(Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC), coordinates);

    {
        const State initialState =
            Parser::CreateInitialState(dataTree["success-cartesian"].accessDictionary(), defaultSatelliteSystem_);

        EXPECT_EQ(initialState, defaultInitialState);
    }

    {
        const Dictionary dictionary = dataTree["failure-keplerian"].accessDictionary();

        EXPECT_THROW(
            {
                try
                {
                    Parser::CreateInitialState(dictionary, defaultSatelliteSystem_);
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{KEPLERIAN initial conditions not yet supported} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Dictionary dictionary = dataTree["failure-time-scale"].accessDictionary();

        EXPECT_THROW(
            {
                try
                {
                    Parser::CreateInitialState(dictionary, defaultSatelliteSystem_);
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Time scale} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Dictionary dictionary = dataTree["failure-frame"].accessDictionary();

        EXPECT_THROW(
            {
                try
                {
                    Parser::CreateInitialState(dictionary, defaultSatelliteSystem_);
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Initial Condition frame} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_Parser, CreateEnvironment)
{
    const String testScenario = {"test_parser_create_environment"};
    const Dictionary dataTree = Parser::ParseYaml(pathToData_, testScenario);

    {
        const Environment environment = Parser::CreateEnvironment(dataTree["success-forces-all"].accessDictionary());

        const Shared<const Celestial> earthSPtr = environment.accessCelestialObjectWithName("Earth");

        Shared<const EarthGravitationalModel> earthGravitationalModelSPtr =
            std::dynamic_pointer_cast<const EarthGravitationalModel>(earthSPtr->accessGravitationalModel());
        EXPECT_EQ(earthGravitationalModelSPtr->getType(), EarthGravitationalModel::Type::EGM96);
        EXPECT_EQ(earthGravitationalModelSPtr->getDegree(), 60);
        EXPECT_EQ(earthGravitationalModelSPtr->getOrder(), 60);

        Shared<const EarthAtmosphericModel> earthAtmosphericModelSPtr =
            std::dynamic_pointer_cast<const EarthAtmosphericModel>(earthSPtr->accessAtmosphericModel());
        EXPECT_EQ(earthAtmosphericModelSPtr->getType(), EarthAtmosphericModel::Type::NRLMSISE00);

        EXPECT_NO_THROW(environment.accessCelestialObjectWithName("Sun"));
        EXPECT_NO_THROW(environment.accessCelestialObjectWithName("Moon"));
    }

    {
        const Environment environment =
            Parser::CreateEnvironment(dataTree["success-forces-central-grav-only"].accessDictionary());

        const Shared<const Celestial> earthSPtr = environment.accessCelestialObjectWithName("Earth");

        Shared<const EarthGravitationalModel> earthGravitationalModelSPtr =
            std::dynamic_pointer_cast<const EarthGravitationalModel>(earthSPtr->accessGravitationalModel());
        EXPECT_EQ(earthGravitationalModelSPtr->getType(), EarthGravitationalModel::Type::EGM96);
        EXPECT_EQ(earthGravitationalModelSPtr->getDegree(), 10);
        EXPECT_EQ(earthGravitationalModelSPtr->getOrder(), 10);

        EXPECT_FALSE(earthSPtr->accessAtmosphericModel()->isDefined());

        EXPECT_THROW(environment.accessCelestialObjectWithName("Sun"), ostk::core::error::RuntimeError);
        EXPECT_THROW(environment.accessCelestialObjectWithName("Moon"), ostk::core::error::RuntimeError);
    }

    {
        const Environment environment =
            Parser::CreateEnvironment(dataTree["success-forces-central-grav-and-one-third-body"].accessDictionary());

        const Shared<const Celestial> earthSPtr = environment.accessCelestialObjectWithName("Earth");

        Shared<const EarthGravitationalModel> earthGravitationalModelSPtr =
            std::dynamic_pointer_cast<const EarthGravitationalModel>(earthSPtr->accessGravitationalModel());

        EXPECT_EQ(earthGravitationalModelSPtr->getType(), EarthGravitationalModel::Type::EGM96);
        EXPECT_EQ(earthGravitationalModelSPtr->getDegree(), 60);
        EXPECT_EQ(earthGravitationalModelSPtr->getOrder(), 60);

        EXPECT_FALSE(earthSPtr->accessAtmosphericModel()->isDefined());

        EXPECT_THROW(environment.accessCelestialObjectWithName("Sun"), ostk::core::error::RuntimeError);
        EXPECT_NO_THROW(environment.accessCelestialObjectWithName("Moon"));
    }

    {
        const Environment environment =
            Parser::CreateEnvironment(dataTree["success-atmosphere-exponential"].accessDictionary());

        const Shared<const Celestial> earthSPtr = environment.accessCelestialObjectWithName("Earth");

        Shared<const EarthGravitationalModel> earthGravitationalModelSPtr =
            std::dynamic_pointer_cast<const EarthGravitationalModel>(earthSPtr->accessGravitationalModel());
        EXPECT_EQ(earthGravitationalModelSPtr->getType(), EarthGravitationalModel::Type::EGM96);
        EXPECT_EQ(earthGravitationalModelSPtr->getDegree(), 60);
        EXPECT_EQ(earthGravitationalModelSPtr->getOrder(), 60);

        Shared<const EarthAtmosphericModel> earthAtmosphericModelSPtr =
            std::dynamic_pointer_cast<const EarthAtmosphericModel>(earthSPtr->accessAtmosphericModel());
        EXPECT_EQ(earthAtmosphericModelSPtr->getType(), EarthAtmosphericModel::Type::Exponential);

        EXPECT_THROW(environment.accessCelestialObjectWithName("Sun"), ostk::core::error::RuntimeError);
        EXPECT_THROW(environment.accessCelestialObjectWithName("Moon"), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_Parser, CreateSequenceAndCreateSegment)
{
    const String testScenario = {"test_parser_create_sequence"};
    const Dictionary dataTree = Parser::ParseYaml(pathToData_, testScenario);

    const NumericalSolver defaultNumericalSolverVariable = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaDopri5, 45.0, 1.0e-10, 1.0e-10
    };

    const RealCondition defaultDurationCondition =
        RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(60.0));

    const InstantCondition defaultInstantCondition1 = InstantCondition(
        InstantCondition::Criterion::StrictlyPositive, Instant::DateTime(DateTime(2023, 1, 1, 0, 3, 0), Scale::UTC)
    );
    const InstantCondition defaultInstantCondition2 = InstantCondition(
        InstantCondition::Criterion::StrictlyPositive, Instant::DateTime(DateTime(2023, 1, 1, 0, 4, 0), Scale::UTC)
    );

    const Vector3d defaultThrustDirection1 = Vector3d(0.0, 1.0, 0.0);
    const Thruster defaultThrusterDynamics1 = {
        defaultSatelliteSystem_,
        std::make_shared<ConstantThrust>(ConstantThrust(
            LocalOrbitalFrameDirection(defaultThrustDirection1, LocalOrbitalFrameFactory::VNC(Frame::GCRF()))
        ))
    };

    const Vector3d defaultThrustDirection2 = Vector3d(0.0, 0.0, 1.0);
    const Thruster defaultThrusterDynamics2 = {
        defaultSatelliteSystem_,
        std::make_shared<ConstantThrust>(ConstantThrust(
            LocalOrbitalFrameDirection(defaultThrustDirection2, LocalOrbitalFrameFactory::VNC(Frame::GCRF()))
        ))
    };

    {
        const Sequence sequence = Parser::CreateSequence(
            dataTree["success-multiple-coast-and-burn"].accessDictionary(), defaultSatelliteSystem_, defaultDynamics_
        );

        EXPECT_EQ(sequence.getSegments().getSize(), 4);
        EXPECT_EQ(sequence.getNumericalSolver(), defaultNumericalSolverVariable);
        EXPECT_EQ(sequence.getDynamics(), Array<Shared<Dynamics>>::Empty());
        EXPECT_EQ(sequence.getMaximumPropagationDuration(), Duration::Seconds(240.1));

        // First coast segment
        const Segment segment1 = sequence.getSegments()[0];
        EXPECT_EQ(segment1.getType(), Segment::Type::Coast);
        const RealCondition durationConditionSegment1 =
            *std::dynamic_pointer_cast<RealCondition>(segment1.getEventCondition());
        EXPECT_EQ(durationConditionSegment1.getTarget(), defaultDurationCondition.getTarget());
        EXPECT_EQ(durationConditionSegment1.getCriterion(), defaultDurationCondition.getCriterion());
        EXPECT_EQ(segment1.getNumericalSolver(), defaultNumericalSolverVariable);
        EXPECT_EQ(segment1.getDynamics(), defaultDynamics_);

        // First burn segment
        const Segment segment2 = sequence.getSegments()[1];
        EXPECT_EQ(segment2.getType(), Segment::Type::Maneuver);
        const RealCondition durationConditionSegment2 =
            *std::dynamic_pointer_cast<RealCondition>(segment2.getEventCondition());
        EXPECT_EQ(durationConditionSegment2.getTarget(), defaultDurationCondition.getTarget());
        EXPECT_EQ(durationConditionSegment2.getCriterion(), defaultDurationCondition.getCriterion());
        EXPECT_EQ(segment2.getNumericalSolver(), defaultNumericalSolverVariable);
        const Array<Shared<Dynamics>> dynamicsSegment2 = segment2.getDynamics();
        EXPECT_NE(dynamicsSegment2, defaultDynamics_);
        for (const auto& dynamics : dynamicsSegment2)
        {
            const Shared<const Thruster> thrusterSPtr = std::dynamic_pointer_cast<Thruster>(dynamics);
            if (thrusterSPtr)
            {
                EXPECT_TRUE(thrusterSPtr->isDefined());
                EXPECT_EQ(thrusterSPtr->getSatelliteSystem(), defaultThrusterDynamics1.getSatelliteSystem());
                const Shared<const ConstantThrust> constantThrustSPtr =
                    std::dynamic_pointer_cast<const ConstantThrust>(thrusterSPtr->getGuidanceLaw());
                const LocalOrbitalFrameDirection localThrustDirection = constantThrustSPtr->getLocalThrustDirection();
                EXPECT_EQ(localThrustDirection.getValue(), defaultThrustDirection1);
                EXPECT_EQ(
                    localThrustDirection.getLocalOrbitalFrameFactory()->getProviderType(),
                    LocalOrbitalFrameTransformProvider::Type::VNC
                );
            }
        }

        // Second coast segment
        const Segment segment3 = sequence.getSegments()[2];
        EXPECT_EQ(segment3.getType(), Segment::Type::Coast);
        const InstantCondition instantConditionSegment3 =
            *std::dynamic_pointer_cast<InstantCondition>(segment3.getEventCondition());
        EXPECT_EQ(instantConditionSegment3.getTarget(), defaultInstantCondition1.getTarget());
        EXPECT_EQ(instantConditionSegment3.getCriterion(), defaultInstantCondition1.getCriterion());
        EXPECT_EQ(segment3.getNumericalSolver(), defaultNumericalSolverVariable);
        EXPECT_EQ(segment3.getDynamics(), defaultDynamics_);

        // Second burn segment
        const Segment segment4 = sequence.getSegments()[3];
        EXPECT_EQ(segment4.getType(), Segment::Type::Maneuver);
        const InstantCondition instantConditionSegment4 =
            *std::dynamic_pointer_cast<InstantCondition>(segment4.getEventCondition());
        EXPECT_EQ(instantConditionSegment4.getTarget(), defaultInstantCondition2.getTarget());
        EXPECT_EQ(instantConditionSegment4.getCriterion(), defaultInstantCondition2.getCriterion());
        EXPECT_EQ(segment4.getNumericalSolver(), defaultNumericalSolverVariable);
        const Array<Shared<Dynamics>> dynamicsSegment4 = segment4.getDynamics();
        EXPECT_NE(dynamicsSegment4, defaultDynamics_);
        for (const auto& dynamics : dynamicsSegment4)
        {
            const Shared<const Thruster> thrusterSPtr = std::dynamic_pointer_cast<Thruster>(dynamics);
            if (thrusterSPtr)
            {
                EXPECT_TRUE(thrusterSPtr->isDefined());
                EXPECT_EQ(thrusterSPtr->getSatelliteSystem(), defaultThrusterDynamics2.getSatelliteSystem());
                const Shared<const ConstantThrust> constantThrustSPtr =
                    std::dynamic_pointer_cast<const ConstantThrust>(thrusterSPtr->getGuidanceLaw());
                const LocalOrbitalFrameDirection localThrustDirection = constantThrustSPtr->getLocalThrustDirection();
                EXPECT_EQ(localThrustDirection.getValue(), defaultThrustDirection2);
                EXPECT_EQ(
                    localThrustDirection.getLocalOrbitalFrameFactory()->getProviderType(),
                    LocalOrbitalFrameTransformProvider::Type::VNC
                );
            }
        }
    }

    const NumericalSolver defaultNumericalSolverFixed =
        NumericalSolver::FixedStepSize(NumericalSolver::StepperType::RungeKutta4, 20.0);
    {
        const Sequence sequence = Parser::CreateSequence(
            dataTree["success-fixed-propagator"].accessDictionary(), defaultSatelliteSystem_, defaultDynamics_
        );

        EXPECT_EQ(sequence.getSegments().getSize(), 1);
        EXPECT_EQ(sequence.getNumericalSolver(), defaultNumericalSolverFixed);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_Parser, CreateComparisonInstants)
{
    const String testScenario = {"test_parser_create_comparison_instants"};
    const Dictionary dataTree = Parser::ParseYaml(pathToData_, testScenario);
    const Dictionary outputStep = dataTree["success"].accessDictionary();

    const Instant defaultInitialInstant = Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC);
    const Instant defaultFinalInstant = defaultInitialInstant + Duration::Seconds(120.0);

    {
        const Array<Instant> comparisonInstants =
            Parser::CreateComparisonInstants(outputStep, defaultInitialInstant, defaultFinalInstant);

        EXPECT_EQ(comparisonInstants.getSize(), 3);
        EXPECT_EQ(comparisonInstants[0], defaultInitialInstant);
        EXPECT_EQ(comparisonInstants[1], defaultInitialInstant + Duration::Seconds(60.0));
        EXPECT_EQ(comparisonInstants[2], defaultFinalInstant);
    }

    {
        const Array<Instant> comparisonInstants = Parser::CreateComparisonInstants(
            outputStep, defaultInitialInstant, defaultFinalInstant + Duration::Microseconds(1.0)
        );

        EXPECT_EQ(comparisonInstants.getSize(), 3);
        EXPECT_EQ(comparisonInstants[0], defaultInitialInstant);
        EXPECT_EQ(comparisonInstants[1], defaultInitialInstant + Duration::Seconds(60.0));
        EXPECT_EQ(comparisonInstants[2], defaultFinalInstant);
    }

    {
        const Array<Instant> comparisonInstants = Parser::CreateComparisonInstants(
            outputStep, defaultInitialInstant, defaultFinalInstant - Duration::Microseconds(1.0)
        );

        EXPECT_EQ(comparisonInstants.getSize(), 3);
        EXPECT_EQ(comparisonInstants[0], defaultInitialInstant);
        EXPECT_EQ(comparisonInstants[1], defaultInitialInstant + Duration::Seconds(60.0));
        EXPECT_EQ(comparisonInstants[2], defaultFinalInstant - Duration::Microseconds(1.0));
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Parser::CreateComparisonInstants(
                        outputStep, defaultInitialInstant, defaultFinalInstant + Duration::Microseconds(101.0)
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Comparison instants are not equally spaced.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Parser::CreateComparisonInstants(
                        outputStep, defaultInitialInstant, defaultFinalInstant - Duration::Microseconds(101.0)
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Comparison instants are not equally spaced.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}
