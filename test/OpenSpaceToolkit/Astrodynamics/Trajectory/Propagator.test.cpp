/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::container::Tuple;
using ostk::core::filesystem::Directory;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
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
using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::dynamics::ThirdBodyGravity;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::model::Tabulated;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator : public ::testing::Test
{
   protected:
    State stateGenerator(
        const Instant& anInstant, const Vector3d& anAcceleration = {0.0, 0.0, 0.0}, const Real& aMassFlowRate = -1e-5
    )
    {
        VectorXd coordinates(10);
        coordinates << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, anAcceleration(0), anAcceleration(1), anAcceleration(2),
            aMassFlowRate;

        return State(anInstant, coordinates, Frame::GCRF(), Maneuver::RequiredCoordinateSubsets);
    }

    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        const PropulsionSystem propulsionSystem = {0.1, 1500.0};

        this->satelliteGeometry_ = satelliteGeometry;
        this->propulsionSystem_ = propulsionSystem;

        this->satelliteSystem_ = {
            this->satelliteDryMass_,
            satelliteGeometry_,
            Matrix3d::Identity(),
            1.0,
            2.1,
            propulsionSystem_,
        };

        this->earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
        this->defaultDynamics_ = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSpherical_),
        };

        this->defaultPropagator_ = {defaultNumericalSolver_, defaultDynamics_};
    }

    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const NumericalSolver defaultRKD5_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);
    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);
    const Mass satelliteWetMass_ =
        Mass(satelliteDryMass_.inKilograms() + propellantMass_.inKilograms(), Mass::Unit::Kilogram);
    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();
    Composite satelliteGeometry_ = Composite::Undefined();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();

    // Setup maneuver defaults
    const Array<State> defaultManeuverStates_ = {
        stateGenerator(Instant::J2000(), {1e-3, 0.0, 0.0}, -1.1e-5),
        stateGenerator(Instant::J2000() + Duration::Seconds(60.0), {0.0, 1e-3, 0.0}, -0.9e-5),
    };

    const Maneuver defaultManeuver_ = {defaultManeuverStates_, Interpolator::Type::Linear};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Constructor)
{
    {
        EXPECT_NO_THROW(Propagator(defaultNumericalSolver_, defaultDynamics_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, CopyConstructor)
{
    {
        Propagator propagator(defaultPropagator_);

        EXPECT_EQ(*propagator.accessCoordinateBroker(), *defaultPropagator_.accessCoordinateBroker());

        propagator.setDynamics({std::make_shared<AtmosphericDrag>(std::make_shared<Celestial>(
            Earth::AtmosphericOnly(std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential))
        ))});

        // Ensure that a deep copy has occured and the original propagator and its memvar shared pointers are untouched
        EXPECT_NE(*propagator.accessCoordinateBroker(), *defaultPropagator_.accessCoordinateBroker());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, CopyAssignmentOperator)
{
    {
        Propagator propagator = defaultPropagator_;

        EXPECT_EQ(*propagator.accessCoordinateBroker(), *defaultPropagator_.accessCoordinateBroker());

        propagator.setDynamics({std::make_shared<AtmosphericDrag>(std::make_shared<Celestial>(
            Earth::AtmosphericOnly(std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential))
        ))});

        // Ensure that a deep copy has occured and the original propagator and its memvar shared pointers are untouched
        EXPECT_NE(*propagator.accessCoordinateBroker(), *defaultPropagator_.accessCoordinateBroker());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, EqualToOperator)
{
    {
        const Propagator propagator_x = {defaultPropagator_};
        EXPECT_TRUE(defaultPropagator_ == propagator_x);
    }

    {
        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        const Propagator propagator_1 = {numericalSolver_1, defaultDynamics_};
        EXPECT_FALSE(defaultPropagator_ == propagator_1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, NotEqualToOperator)
{
    {
        const Propagator propagator_x = {defaultPropagator_};
        EXPECT_FALSE(defaultPropagator_ != propagator_x);
    }

    {
        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        const Propagator propagator_1 = {numericalSolver_1, defaultDynamics_};
        EXPECT_TRUE(defaultPropagator_ != propagator_1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, IsDefined)
{
    {
        EXPECT_TRUE(defaultPropagator_.isDefined());
    }

    {
        const Propagator propagatorWithEmptyDynamicsArray = {defaultNumericalSolver_};
        EXPECT_FALSE(propagatorWithEmptyDynamicsArray.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Undefined)
{
    {
        EXPECT_NO_THROW(Propagator::Undefined());

        EXPECT_FALSE(Propagator::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultPropagator_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultPropagator_.print(std::cout, true));
        EXPECT_NO_THROW(defaultPropagator_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, AccessCoordinateBroker)
{
    {
        Shared<CoordinateBroker> brokerSPtr = defaultPropagator_.accessCoordinateBroker();
        EXPECT_TRUE(brokerSPtr != nullptr);
        EXPECT_EQ(brokerSPtr->getNumberOfCoordinates(), 6);
        EXPECT_EQ(brokerSPtr->getNumberOfSubsets(), 2);
    }

    {
        EXPECT_ANY_THROW(Propagator::Undefined().accessCoordinateBroker());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, AccessNumericalSolver)
{
    {
        EXPECT_EQ(defaultPropagator_.accessNumericalSolver(), defaultNumericalSolver_);
    }

    {
        EXPECT_ANY_THROW(Propagator::Undefined().accessNumericalSolver());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Getters)
{
    {
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
    }

    {
        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, SetDynamics)
{
    {
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 2);

        const Shared<Dynamics> centralBodyGravity = std::make_shared<CentralBodyGravity>(earthSpherical_);

        defaultPropagator_.setDynamics({centralBodyGravity});

        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 1);

        defaultPropagator_.setDynamics({});

        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 0);
        EXPECT_FALSE(defaultPropagator_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, AddDynamics)
{
    {
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 2);

        const Shared<Dynamics> centralBodyGravity = std::make_shared<CentralBodyGravity>(earthSpherical_);

        defaultPropagator_.addDynamics(centralBodyGravity);

        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 3);

        defaultPropagator_.addDynamics(centralBodyGravity);

        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 4);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, ClearDynamics)
{
    {
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_GE(defaultPropagator_.getDynamics().getSize(), 1);

        defaultPropagator_.clearDynamics();

        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, CalculateStateAt)
{
    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
    };

    // Setup instants
    const Array<Instant> instantArray = {
        Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC),
    };

    for (const Instant& instant : instantArray)
    {
        EXPECT_NO_THROW(defaultPropagator_.calculateStateAt(state, instant));
    }

    {
        // Test that the propagator respects the input Frame
        const State outputStateGCRF = defaultPropagator_.calculateStateAt(state, instantArray[0]);
        const State outputStateITRF =
            defaultPropagator_.calculateStateAt(state.inFrame(Frame::ITRF()), instantArray[0]);

        EXPECT_EQ(outputStateGCRF.getFrame(), Frame::GCRF());
        EXPECT_EQ(outputStateITRF.getFrame(), Frame::ITRF());

        const State outputStateGCRF2 = outputStateITRF.inFrame(Frame::GCRF());

        EXPECT_EQ(outputStateGCRF.getInstant(), outputStateGCRF2.getInstant());
        EXPECT_EQ(outputStateGCRF.getFrame(), outputStateGCRF2.getFrame());
        EXPECT_EQ(outputStateGCRF.accessCoordinateBroker(), outputStateGCRF2.accessCoordinateBroker());

        // Get the coordinates from the two states
        const VectorXd coordinates1 = outputStateGCRF.getCoordinates();
        const VectorXd coordinates2 = outputStateGCRF2.getCoordinates();

        // Check that the two vectors have the same size
        EXPECT_EQ(coordinates1.size(), coordinates2.size());

        // Check that each element of the two vectors is close
        for (int i = 0; i < coordinates1.size(); ++i)
        {
            EXPECT_NEAR(coordinates1(i), coordinates2(i), 1e-5);
        }
    }

    {
        // Test that the propagator respects the input State dimensions

        VectorXd coords(8);
        coords << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 1, 2;

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra1", 1)),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra2", 1)),
        };

        const State bigState = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            coords,
            gcrfSPtr_,
            coordinateSubsets,
        };

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const State outputState = defaultPropagator_.calculateStateAt(bigState, instantArray[0]);

        EXPECT_NE(bigState, outputState);
        EXPECT_EQ(bigState.getCoordinateSubsets(), outputState.getCoordinateSubsets());
        EXPECT_EQ(outputState.getCoordinates().size(), 8);
    }

    // Test that propagation across maneuvers is different than propagation without
    {
        VectorXd coords(7);
        coords << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 100.0;

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            CoordinateSubset::Mass(),
        };

        // Setup relevant instants
        const Instant beforeManeuverInstant = defaultManeuverStates_[0].accessInstant() - Duration::Seconds(60.0);
        const Instant afterManeuverInstant =
            defaultManeuverStates_[defaultManeuverStates_.getSize() - 1].accessInstant() + Duration::Seconds(60.0);

        const State stateBeforeManeuver = {
            beforeManeuverInstant,
            coords,
            gcrfSPtr_,
            coordinateSubsets,
        };

        EXPECT_NE(
            defaultPropagator_.calculateStateAt(stateBeforeManeuver, afterManeuverInstant),
            defaultPropagator_.calculateStateAt(stateBeforeManeuver, afterManeuverInstant, {defaultManeuver_})
        );
    }

    // Check failure of adding wrong dynamics before actually propagating
    {
        // Too many central body gravities
        {
            Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};
            propagator.setDynamics(
                defaultDynamics_ + Array<Shared<Dynamics>>(1, std::make_shared<CentralBodyGravity>(earthSpherical_))
            );

            EXPECT_THROW(propagator.calculateStateAt(state, state.getInstant()), ostk::core::error::RuntimeError);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, CalculateStateAt_Condition)
{
    // State and instant and condition setup
    const State state = {
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
    };
    const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);
    const InstantCondition condition = {
        InstantCondition::Criterion::StrictlyPositive,
        state.accessInstant() + Duration::Seconds(60.0),
    };

    const Propagator propagator = {defaultRKD5_, defaultDynamics_};

    {
        const NumericalSolver::ConditionSolution conditionSolution =
            propagator.calculateStateToCondition(state, endInstant, condition);

        const State endState = conditionSolution.state;

        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
        EXPECT_TRUE(endState.accessInstant() < endInstant);
        EXPECT_LT((endState.accessInstant() - condition.getInstant()).inSeconds(), 1e-7);

        const InstantCondition failureCondition = {
            InstantCondition::Criterion::StrictlyPositive,
            state.accessInstant() + Duration::Seconds(7000.0),
        };

        EXPECT_FALSE(propagator.calculateStateToCondition(state, endInstant, failureCondition).conditionIsSatisfied);
    }

    {
        const NumericalSolver::ConditionSolution conditionSolutionGCRF =
            propagator.calculateStateToCondition(state, endInstant, condition);

        const NumericalSolver::ConditionSolution conditionSolutionITRF =
            propagator.calculateStateToCondition(state.inFrame(Frame::ITRF()), endInstant, condition);

        // Test that the propagator respects the input Frame
        const State outputStateGCRF = conditionSolutionGCRF.state;
        const State outputStateITRF = conditionSolutionITRF.state;

        EXPECT_EQ(outputStateGCRF.getFrame(), Frame::GCRF());
        EXPECT_EQ(outputStateITRF.getFrame(), Frame::ITRF());

        const State outputStateGCRF2 = outputStateITRF.inFrame(Frame::GCRF());

        EXPECT_EQ(outputStateGCRF.getFrame(), outputStateGCRF2.getFrame());
        EXPECT_EQ(outputStateGCRF.accessCoordinateBroker(), outputStateGCRF2.accessCoordinateBroker());

        // Get the coordinates from the two states
        const VectorXd coordinates1 = outputStateGCRF.getCoordinates();
        const VectorXd coordinates2 = outputStateGCRF2.getCoordinates();

        // Check that the two vectors have the same size
        EXPECT_EQ(coordinates1.size(), coordinates2.size());

        // Check that each element of the two vectors is close
        EXPECT_VECTORS_ALMOST_EQUAL(coordinates1, coordinates2, 5e-5);
    }

    {
        // Test that the propagator respects the input State dimensions

        VectorXd coords(8);
        coords << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 1, 2;

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra1", 1)),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra2", 1)),
        };

        const State bigState = {
            state.getInstant(),
            coords,
            gcrfSPtr_,
            coordinateSubsets,
        };

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(propagator.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const NumericalSolver::ConditionSolution conditionSolution =
            propagator.calculateStateToCondition(bigState, endInstant, condition);

        const State outputState = conditionSolution.state;

        EXPECT_NE(bigState, outputState);
        EXPECT_EQ(bigState.getCoordinateSubsets(), outputState.getCoordinateSubsets());
        EXPECT_EQ(outputState.getCoordinates().size(), 8);
    }

    // Test that propagation with maneuvers is different than propagation without
    {
        VectorXd coords(7);
        coords << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 100.0;

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            CoordinateSubset::Mass(),
        };

        // Setup instants and condition
        const Instant beforeManeuverInstant = defaultManeuverStates_[0].accessInstant() - Duration::Seconds(60.0);
        const Instant afterManeuverInstant =
            defaultManeuverStates_[defaultManeuverStates_.getSize() - 1].accessInstant() + Duration::Seconds(60.0);

        const InstantCondition afterManeuverCondition = {
            InstantCondition::Criterion::StrictlyPositive,
            afterManeuverInstant,
        };

        const State beforeManeuverState = {
            beforeManeuverInstant,
            coords,
            gcrfSPtr_,
            coordinateSubsets,
        };

        const NumericalSolver::ConditionSolution conditionSolution = propagator.calculateStateToCondition(
            beforeManeuverState, afterManeuverCondition.getInstant() + Duration::Seconds(1.0), afterManeuverCondition
        );

        const NumericalSolver::ConditionSolution conditionSolutionWithManeuvers = propagator.calculateStateToCondition(
            beforeManeuverState,
            afterManeuverCondition.getInstant() + Duration::Seconds(1.0),
            afterManeuverCondition,
            {defaultManeuver_}
        );

        EXPECT_NE(conditionSolution.state, conditionSolutionWithManeuvers.state);
    }

    // Check failure of adding wrong dynamics before actually propagating
    {
        // Too many central body gravities
        {
            defaultPropagator_.setDynamics(
                defaultDynamics_ + Array<Shared<Dynamics>>(1, std::make_shared<CentralBodyGravity>(earthSpherical_))
            );

            EXPECT_THROW(
                defaultPropagator_.calculateStateToCondition(state, endInstant, condition),
                ostk::core::error::RuntimeError
            );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, CalculateStatesAt)
{
    // Test exception for unsorted instant array
    {
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Setup instants
        Array<Instant> instantArray = Array<Instant>::Empty();
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC));

        EXPECT_ANY_THROW(defaultPropagator_.calculateStatesAt(state, instantArray));
    }

    /// Test full state results against reference trajectory
    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/"
                               "Propagated/CalculateStatesAt_StateValidation.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC));

        referencePositionArray.add({
            referenceRow[1].accessReal(),
            referenceRow[2].accessReal(),
            referenceRow[3].accessReal(),
        });
        referenceVelocityArray.add({
            referenceRow[4].accessReal(),
            referenceRow[5].accessReal(),
            referenceRow[6].accessReal(),
        });
    }

    // Test forward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 0;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_),
        };

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].getPosition();
            const Velocity velocity = propagatedStateArray[i].getVelocity();

            const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm();
            const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm();

            ASSERT_GT(2e-7, positionError);
            ASSERT_GT(2e-10, velocityError);
            ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]);

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // Test forward + backward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 30;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_),
        };

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].getPosition();
            const Velocity velocity = propagatedStateArray[i].getVelocity();

            const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm();
            const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm();

            ASSERT_GT(2e-7, positionError);
            ASSERT_GT(2e-10, velocityError);
            ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]);

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // Test backward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 60;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_),
        };

        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].getPosition();
            const Velocity velocity = propagatedStateArray[i].getVelocity();

            const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm();
            const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm();

            ASSERT_GT(2e-7, positionError);
            ASSERT_GT(2e-10, velocityError);
            ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]);

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    {
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Test that the propagator respects the input Frame
        const Array<State> outputStatesGCRF = defaultPropagator_.calculateStatesAt(state, instantArray);
        const Array<State> outputStatesITRF =
            defaultPropagator_.calculateStatesAt(state.inFrame(Frame::ITRF()), instantArray);

        // Validation loop
        for (size_t i = 0; i < outputStatesGCRF.getSize(); i++)
        {
            EXPECT_EQ(outputStatesGCRF[i].getFrame(), Frame::GCRF());
            EXPECT_EQ(outputStatesITRF[i].getFrame(), Frame::ITRF());

            const State outputStateGCRF2 = outputStatesITRF[i].inFrame(Frame::GCRF());

            EXPECT_EQ(outputStatesGCRF[i].getInstant(), outputStateGCRF2.getInstant());
            EXPECT_EQ(outputStatesGCRF[i].getFrame(), outputStateGCRF2.getFrame());
            EXPECT_EQ(outputStatesGCRF[i].accessCoordinateBroker(), outputStateGCRF2.accessCoordinateBroker());

            // Get the coordinates from the two states
            const VectorXd coordinates1 = outputStatesGCRF[i].getCoordinates();
            const VectorXd coordinates2 = outputStateGCRF2.getCoordinates();

            // Check that the two vectors have the same size
            EXPECT_EQ(coordinates1.size(), coordinates2.size());

            // Check that each element of the two vectors is close
            for (int j = 0; j < coordinates1.size(); ++j)
            {
                EXPECT_NEAR(coordinates1(j), coordinates2(j), 1e-3);
            }
        }
    }

    {
        // Test that the propagator respects the input State dimensions

        VectorXd coords(8);
        coords << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 1, 2;

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra1", 1)),
            std::make_shared<CoordinateSubset>(CoordinateSubset("extra2", 1)),
        };

        const State bigState = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            coords,
            gcrfSPtr_,
            coordinateSubsets,
        };

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const Array<State> outputStates = defaultPropagator_.calculateStatesAt(bigState, instantArray);

        for (size_t i = 0; i < outputStates.getSize(); i++)
        {
            EXPECT_NE(bigState, outputStates[i]);
            EXPECT_EQ(bigState.getCoordinateSubsets(), outputStates[i].getCoordinateSubsets());
            EXPECT_EQ(outputStates[i].getCoordinates().size(), 8);
        }
    }

    // Test that propagation over maneuvers works
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 0;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_),
        };

        // Create state with higher dimension for propagating with maneuvers
        const StateBuilder maneuverStateBuilder = StateBuilder(state) + CoordinateSubset::Mass();
        VectorXd maneuverStateCoords(7);
        maneuverStateCoords << state.getCoordinates(), 100.0;
        const State maneuverState = maneuverStateBuilder.build(state.getInstant(), maneuverStateCoords);

        // Create a maneuver occuring after the start instant but before the end of the reference data
        Array<State> maneuverStates = Array<State>::Empty();
        for (Size i = 0; i < defaultManeuverStates_.getSize(); i++)
        {
            maneuverStates.add(stateGenerator(startInstant + i * Duration::Seconds(60.0), {1e-1, 1e-1, 1e-1}));
        }

        const Maneuver maneuver = {maneuverStates, Interpolator::Type::Linear};

        // Remove first instant from instant array so that we can check that both propagators do not produce the same
        // result
        Array<Instant> instantArrayWithoutFirstInstant(instantArray);
        instantArrayWithoutFirstInstant.remove(instantArray[0]);

        // Propagate both with and without maneuvers
        const Array<State> propagatedStateArrayWithManeuvers =
            defaultPropagator_.calculateStatesAt(maneuverState, instantArrayWithoutFirstInstant, {maneuver});

        const Array<State> propagatedStateArray =
            defaultPropagator_.calculateStatesAt(state, instantArrayWithoutFirstInstant);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArrayWithManeuvers.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].getPosition();
            const Velocity velocity = propagatedStateArray[i].getVelocity();

            const Position positionWithManeuvers = propagatedStateArrayWithManeuvers[i].getPosition();
            const Velocity velocityWithManeuvers = propagatedStateArrayWithManeuvers[i].getVelocity();

            const double positionError =
                (position.accessCoordinates() - positionWithManeuvers.accessCoordinates()).norm();
            const double velocityError =
                (velocity.accessCoordinates() - velocityWithManeuvers.accessCoordinates()).norm();

            ASSERT_GT(positionError, 1.0e-15);
            ASSERT_GT(velocityError, 1.0e-15);
            ASSERT_EQ(propagatedStateArrayWithManeuvers[i].getInstant(), instantArrayWithoutFirstInstant[i]);

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific, std::ios::floatfield);
            // std::cout << "Position error is: " << positionError << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError << "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed, std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // Check failure of adding wrong dynamics
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 0;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_),
        };

        // Too many central body gravities
        {
            Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};
            propagator.setDynamics(
                defaultDynamics_ + Array<Shared<Dynamics>>(1, std::make_shared<CentralBodyGravity>(earthSpherical_))
            );

            EXPECT_THROW(propagator.calculateStatesAt(state, {state.getInstant()}), ostk::core::error::RuntimeError);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Maneuvers)
{
    const Instant epoch = Instant::J2000();

    VectorXd coordinates(7);
    coordinates << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 100.0;

    const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    };

    const State initialState = {epoch - Duration::Seconds(60.0), coordinates, gcrfSPtr_, coordinateSubsets};
    const Instant endInstant = epoch + Duration::Seconds(200.0);

    // The default maneuver spans [0 s, 60 s] after the epoch
    const Maneuver secondManeuver = {
        {
            stateGenerator(epoch + Duration::Seconds(61.0)),
            stateGenerator(epoch + Duration::Seconds(121.0)),
        },
        Interpolator::Type::Linear
    };

    // Maneuvers are validated by every way of propagating
    {
        const InstantCondition condition = {InstantCondition::Criterion::StrictlyPositive, endInstant};

        const auto expectAllThrow = [&](const Array<Maneuver>& aManeuverArray, const String& aMessage)
        {
            const Array<std::function<void()>> propagations = {
                [&]()
                {
                    defaultPropagator_.calculateStateAt(initialState, endInstant, aManeuverArray);
                },
                [&]()
                {
                    defaultPropagator_.calculateStatesAt(initialState, {endInstant}, aManeuverArray);
                },
                [&]()
                {
                    defaultPropagator_.calculateStateToCondition(initialState, endInstant, condition, aManeuverArray);
                },
            };

            for (const auto& propagation : propagations)
            {
                try
                {
                    propagation();
                    FAIL() << "Expected an exception";
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(aMessage, e.getMessage());
                }
            }
        };

        expectAllThrow({defaultManeuver_, defaultManeuver_}, "Maneuvers cannot be duplicated.");

        const Maneuver overlappingManeuver = {
            {
                stateGenerator(epoch + Duration::Seconds(30.0)),
                stateGenerator(epoch + Duration::Seconds(90.0)),
            },
            Interpolator::Type::Linear
        };
        expectAllThrow({defaultManeuver_, overlappingManeuver}, "Maneuvers cannot overlap in time.");
        expectAllThrow({overlappingManeuver, defaultManeuver_}, "Maneuvers cannot overlap in time.");

        // Touching at a single instant is an overlap too
        const Maneuver touchingManeuver = {
            {
                stateGenerator(epoch + Duration::Seconds(60.0)),
                stateGenerator(epoch + Duration::Seconds(90.0)),
            },
            Interpolator::Type::Linear
        };
        expectAllThrow({defaultManeuver_, touchingManeuver}, "Maneuvers cannot overlap in time.");

        // A maneuver needs at least two states
        const Maneuver singleStateManeuver = {{stateGenerator(epoch + Duration::Seconds(300.0))}};
        EXPECT_THROW(
            defaultPropagator_.calculateStateAt(initialState, endInstant, {singleStateManeuver}),
            ostk::core::error::RuntimeError
        );
    }

    // Maneuvers need the mass to be in the state
    {
        const State stateWithoutMass = {
            epoch - Duration::Seconds(60.0),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        EXPECT_NO_THROW(defaultPropagator_.calculateStateAt(stateWithoutMass, endInstant));
        EXPECT_ANY_THROW(defaultPropagator_.calculateStateAt(stateWithoutMass, endInstant, {defaultManeuver_}));
    }

    // The order the maneuvers are given in does not matter
    {
        const State state =
            defaultPropagator_.calculateStateAt(initialState, endInstant, {defaultManeuver_, secondManeuver});
        const State reversedState =
            defaultPropagator_.calculateStateAt(initialState, endInstant, {secondManeuver, defaultManeuver_});

        EXPECT_EQ(state, reversedState);
        EXPECT_NE(state, defaultPropagator_.calculateStateAt(initialState, endInstant));
    }

    // Maneuvers outside the propagated interval have no effect
    {
        const Instant beforeManeuverInstant = epoch - Duration::Seconds(30.0);

        EXPECT_EQ(
            defaultPropagator_.calculateStateAt(
                initialState, beforeManeuverInstant, {defaultManeuver_, secondManeuver}
            ),
            defaultPropagator_.calculateStateAt(initialState, beforeManeuverInstant)
        );
    }

    // The interpolation type of a maneuver is what is used to evaluate its profile while propagating
    {
        const Array<State> states = {
            stateGenerator(epoch + Duration::Seconds(0.0), {0.0, 0.0, 0.0}, -1.0e-5),
            stateGenerator(epoch + Duration::Seconds(30.0), {1.0e-2, 0.0, 0.0}, -1.0e-5),
            stateGenerator(epoch + Duration::Seconds(60.0), {0.0, 0.0, 0.0}, -1.0e-5),
            stateGenerator(epoch + Duration::Seconds(90.0), {0.0, 1.0e-2, 0.0}, -1.0e-5),
        };

        const State linearState = defaultPropagator_.calculateStateAt(
            initialState, endInstant, {Maneuver(states, Interpolator::Type::Linear)}
        );
        const State rationalState = defaultPropagator_.calculateStateAt(
            initialState, endInstant, {Maneuver(states, Interpolator::Type::BarycentricRational)}
        );
        const State defaultState = defaultPropagator_.calculateStateAt(initialState, endInstant, {Maneuver(states)});

        EXPECT_NE(linearState, rationalState);
        EXPECT_EQ(rationalState, defaultState);
    }

    // Propagating with maneuvers does not modify the propagator
    {
        const Propagator reference = defaultPropagator_;

        defaultPropagator_.calculateStateAt(initialState, endInstant, {defaultManeuver_});

        EXPECT_EQ(defaultPropagator_, reference);
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(defaultPropagator_.getDynamics().getSize(), 2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, ShortManeuvers)
{
    // A 2 s maneuver that starts 31 s after the epoch: shorter than the 30 s fixed step below, and it falls between
    // the stages of a step that starts at the epoch, so an integrator that is unaware of it never samples the thrust.
    const Instant epoch = Instant::J2000();
    const Real acceleration = 1.0;
    const Real massFlowRate = -1.0e-3;
    const Real initialMass = 100.0;

    const Maneuver maneuver = {
        {
            stateGenerator(epoch + Duration::Seconds(31.0), {acceleration, 0.0, 0.0}, massFlowRate),
            stateGenerator(epoch + Duration::Seconds(32.0), {acceleration, 0.0, 0.0}, massFlowRate),
            stateGenerator(epoch + Duration::Seconds(33.0), {acceleration, 0.0, 0.0}, massFlowRate),
        },
        Interpolator::Type::Linear
    };

    const Real maneuverDeltaV = maneuver.calculateDeltaV();
    const Real maneuverDeltaMass = maneuver.calculateDeltaMass().inKilograms();
    EXPECT_DOUBLE_EQ(maneuverDeltaV, 2.0);
    EXPECT_DOUBLE_EQ(maneuverDeltaMass, 2.0e-3);

    // The same kind of maneuver, but with instants that are not whole seconds, to exercise the reconstruction of the
    // instants at the boundaries
    const Maneuver fractionalManeuver = {
        {
            stateGenerator(epoch + Duration::Seconds(31.123456789), {acceleration, 0.0, 0.0}, massFlowRate),
            stateGenerator(epoch + Duration::Seconds(32.000000123), {acceleration, 0.0, 0.0}, massFlowRate),
            stateGenerator(epoch + Duration::Seconds(33.987654321), {acceleration, 0.0, 0.0}, massFlowRate),
        },
        Interpolator::Type::Linear
    };

    const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    };

    const auto stateAt = [&](const Duration& anOffset, const Real& aMass) -> State
    {
        VectorXd coordinates(7);
        coordinates << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, aMass;

        return {epoch + anOffset, coordinates, gcrfSPtr_, coordinateSubsets};
    };

    const auto massOf = [](const State& aState) -> double
    {
        return aState.extractCoordinate(CoordinateSubset::Mass())[0];
    };

    const Array<NumericalSolver> numericalSolvers = {
        NumericalSolver::FixedStepSize(NumericalSolver::StepperType::RungeKutta4, 30.0),
        defaultRKD5_,
        defaultNumericalSolver_,
    };

    for (const NumericalSolver& numericalSolver : numericalSolvers)
    {
        const Propagator propagator = {numericalSolver, defaultDynamics_};

        // calculateStateAt: the whole maneuver is applied, once
        {
            const State initialState = stateAt(Duration::Seconds(0.0), initialMass);
            const Instant endInstant = epoch + Duration::Seconds(40.0);

            const State coastState = propagator.calculateStateAt(initialState, endInstant);
            const State maneuverState = propagator.calculateStateAt(initialState, endInstant, {maneuver});

            EXPECT_EQ(maneuverState.accessInstant(), endInstant);
            EXPECT_NEAR(massOf(maneuverState), initialMass - maneuverDeltaMass, 1e-9);

            const Vector3d deltaVelocity = maneuverState.extractCoordinate(CartesianVelocity::Default()) -
                                           coastState.extractCoordinate(CartesianVelocity::Default());
            EXPECT_NEAR(deltaVelocity(0), maneuverDeltaV, 1e-2);
            EXPECT_NEAR(deltaVelocity(1), 0.0, 1e-2);
            EXPECT_NEAR(deltaVelocity(2), 0.0, 1e-2);
        }

        // calculateStatesAt: before, during, exactly at the end of, and after the maneuver
        {
            const State initialState = stateAt(Duration::Seconds(0.0), initialMass);
            const Array<Instant> instants = {
                epoch + Duration::Seconds(20.0),
                epoch + Duration::Seconds(32.0),
                epoch + Duration::Seconds(33.0),
                epoch + Duration::Seconds(40.0),
            };
            const Array<double> expectedMasses = {
                initialMass,
                initialMass - 0.5 * maneuverDeltaMass,
                initialMass - maneuverDeltaMass,
                initialMass - maneuverDeltaMass,
            };

            const Array<State> states = propagator.calculateStatesAt(initialState, instants, {maneuver});

            ASSERT_EQ(states.getSize(), instants.getSize());
            for (Size i = 0; i < states.getSize(); ++i)
            {
                EXPECT_EQ(states[i].accessInstant(), instants[i]);
                EXPECT_NEAR(massOf(states[i]), expectedMasses[i], 1e-9) << "instant index " << i;
            }
        }

        // calculateStatesAt backwards: the maneuver is un-applied
        {
            const State initialState = stateAt(Duration::Seconds(40.0), initialMass - maneuverDeltaMass);
            const Array<Instant> instants = {
                epoch + Duration::Seconds(20.0),
                epoch + Duration::Seconds(32.0),
                epoch + Duration::Seconds(33.0),
            };
            const Array<double> expectedMasses = {
                initialMass,
                initialMass - 0.5 * maneuverDeltaMass,
                initialMass - maneuverDeltaMass,
            };

            const Array<State> states = propagator.calculateStatesAt(initialState, instants, {maneuver});

            ASSERT_EQ(states.getSize(), instants.getSize());
            for (Size i = 0; i < states.getSize(); ++i)
            {
                EXPECT_EQ(states[i].accessInstant(), instants[i]);
                EXPECT_NEAR(massOf(states[i]), expectedMasses[i], 1e-9) << "instant index " << i;
            }

            EXPECT_NEAR(
                massOf(propagator.calculateStateAt(initialState, epoch + Duration::Seconds(20.0), {maneuver})),
                initialMass,
                1e-9
            );
        }

        // Starting in the middle of the maneuver applies only the remainder
        {
            const State initialState = stateAt(Duration::Seconds(32.0), initialMass);

            EXPECT_NEAR(
                massOf(propagator.calculateStateAt(initialState, epoch + Duration::Seconds(40.0), {maneuver})),
                initialMass - 0.5 * maneuverDeltaMass,
                1e-9
            );
        }

        // calculateStateToCondition: the maneuver is applied before the condition is reached
        {
            const State initialState = stateAt(Duration::Seconds(0.0), initialMass);
            const InstantCondition condition = {
                InstantCondition::Criterion::StrictlyPositive,
                epoch + Duration::Seconds(40.0),
            };

            const NumericalSolver::ConditionSolution conditionSolution = propagator.calculateStateToCondition(
                initialState, epoch + Duration::Seconds(100.0), condition, {maneuver}
            );

            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_LT(std::abs((conditionSolution.state.accessInstant() - condition.getInstant()).inSeconds()), 1e-3);
            EXPECT_NEAR(massOf(conditionSolution.state), initialMass - maneuverDeltaMass, 1e-9);
        }

        // calculateStateToCondition: an event in the middle of the maneuver
        {
            const State initialState = stateAt(Duration::Seconds(0.0), initialMass);
            const InstantCondition condition = {
                InstantCondition::Criterion::StrictlyPositive,
                epoch + Duration::Seconds(32.0),
            };

            const NumericalSolver::ConditionSolution conditionSolution = propagator.calculateStateToCondition(
                initialState, epoch + Duration::Seconds(100.0), condition, {maneuver}
            );

            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_LT(std::abs((conditionSolution.state.accessInstant() - condition.getInstant()).inSeconds()), 1e-3);
            EXPECT_NEAR(massOf(conditionSolution.state), initialMass - 0.5 * maneuverDeltaMass, 1e-5);
        }

        // calculateStateToCondition: the propagation ends exactly at, or in the middle of, the last maneuver
        for (const Maneuver& lastManeuver : {maneuver, fractionalManeuver})
        {
            const Instant maneuverStartInstant = lastManeuver.getInterval().getStart();
            const Instant maneuverEndInstant = lastManeuver.getInterval().getEnd();

            const State initialState = stateAt(Duration::Seconds(0.0), initialMass);
            const InstantCondition neverSatisfiedCondition = {
                InstantCondition::Criterion::StrictlyPositive,
                epoch + Duration::Seconds(1000.0),
            };

            for (const Instant& endInstant :
                 {maneuverEndInstant, maneuverStartInstant + (maneuverEndInstant - maneuverStartInstant) / 2.0})
            {
                const NumericalSolver::ConditionSolution conditionSolution = propagator.calculateStateToCondition(
                    initialState, endInstant, neverSatisfiedCondition, {lastManeuver}
                );

                EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
                EXPECT_NEAR((conditionSolution.state.accessInstant() - endInstant).inSeconds(), 0.0, 1e-6);
                EXPECT_NEAR(
                    massOf(conditionSolution.state),
                    initialMass + massFlowRate * (endInstant - maneuverStartInstant).inSeconds(),
                    1e-9
                );
            }
        }

        // calculateStateToCondition backwards: the maneuver is un-applied before the condition is reached
        {
            const State initialState = stateAt(Duration::Seconds(40.0), initialMass - maneuverDeltaMass);

            // Strictly negative is the criterion that triggers when propagating towards earlier instants
            const InstantCondition conditionBeforeManeuver = {
                InstantCondition::Criterion::StrictlyNegative,
                epoch + Duration::Seconds(20.0),
            };

            const NumericalSolver::ConditionSolution solutionBeforeManeuver =
                propagator.calculateStateToCondition(initialState, epoch, conditionBeforeManeuver, {maneuver});

            EXPECT_TRUE(solutionBeforeManeuver.conditionIsSatisfied);
            EXPECT_LT(
                std::abs(
                    (solutionBeforeManeuver.state.accessInstant() - conditionBeforeManeuver.getInstant()).inSeconds()
                ),
                1e-3
            );
            EXPECT_NEAR(massOf(solutionBeforeManeuver.state), initialMass, 1e-9);

            const InstantCondition conditionInManeuver = {
                InstantCondition::Criterion::StrictlyNegative,
                epoch + Duration::Seconds(32.0),
            };

            const NumericalSolver::ConditionSolution solutionInManeuver =
                propagator.calculateStateToCondition(initialState, epoch, conditionInManeuver, {maneuver});

            EXPECT_TRUE(solutionInManeuver.conditionIsSatisfied);
            EXPECT_LT(
                std::abs((solutionInManeuver.state.accessInstant() - conditionInManeuver.getInstant()).inSeconds()),
                1e-3
            );
            EXPECT_NEAR(massOf(solutionInManeuver.state), initialMass - 0.5 * maneuverDeltaMass, 1e-5);
        }

        // Several maneuvers, given out of order
        {
            const Maneuver laterManeuver = {
                {
                    stateGenerator(epoch + Duration::Seconds(51.0), {acceleration, 0.0, 0.0}, massFlowRate),
                    stateGenerator(epoch + Duration::Seconds(52.0), {acceleration, 0.0, 0.0}, massFlowRate),
                    stateGenerator(epoch + Duration::Seconds(53.0), {acceleration, 0.0, 0.0}, massFlowRate),
                },
                Interpolator::Type::Linear
            };

            const State initialState = stateAt(Duration::Seconds(0.0), initialMass);
            const Array<Instant> instants = {
                epoch + Duration::Seconds(45.0),
                epoch + Duration::Seconds(52.0),
                epoch + Duration::Seconds(60.0),
            };
            const Array<double> expectedMasses = {
                initialMass - maneuverDeltaMass,
                initialMass - 1.5 * maneuverDeltaMass,
                initialMass - 2.0 * maneuverDeltaMass,
            };

            const Array<State> states = propagator.calculateStatesAt(initialState, instants, {laterManeuver, maneuver});

            ASSERT_EQ(states.getSize(), instants.getSize());
            for (Size i = 0; i < states.getSize(); ++i)
            {
                EXPECT_NEAR(massOf(states[i]), expectedMasses[i], 1e-9) << "instant index " << i;
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, Default)
{
    {
        EXPECT_NO_THROW(Propagator::Default());
        EXPECT_TRUE(Propagator::Default().isDefined());
    }

    {
        Environment environment = Environment::Default();
        EXPECT_NO_THROW(Propagator::Default(environment));
        EXPECT_NO_THROW(Propagator::Default(environment).isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, FromEnvironment)
{
    {
        Environment environment = Environment::Default();
        EXPECT_NO_THROW(Propagator::FromEnvironment(defaultNumericalSolver_, environment));

        const Propagator propagator = Propagator::FromEnvironment(defaultNumericalSolver_, environment);

        // default environment has 3 bodies (Earth Sun and Moon) with gravitational fields
        // and 1 default dynamics of integrating the Position Derivative
        EXPECT_TRUE(propagator.getDynamics().getSize() == 4);

        EXPECT_TRUE(static_cast<const CentralBodyGravity&>(*propagator.getDynamics()[0]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[1]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[2]).isDefined());
    }

    {
        const Array<Shared<const Object>> celestials = {
            std::make_shared<Earth>(Earth::FromModels(
                std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
                std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
                std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
            )),
            std::make_shared<Moon>(Moon::Default()),
            std::make_shared<Sun>(Sun::Default()),
        };
        Environment environment = Environment(Instant::J2000(), celestials);
        const Propagator propagator = Propagator::FromEnvironment(defaultNumericalSolver_, environment);

        EXPECT_TRUE(propagator.getDynamics().getSize() == 5);

        EXPECT_TRUE(static_cast<const CentralBodyGravity&>(*propagator.getDynamics()[0]).isDefined());
        EXPECT_TRUE(static_cast<const AtmosphericDrag&>(*propagator.getDynamics()[1]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[2]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[3]).isDefined());
        EXPECT_TRUE(static_cast<const PositionDerivative&>(*propagator.getDynamics()[4]).isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagator, ValidateDynamicsSet)
{
    const State state = {
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
    };

    {
        // Too many central body gravities
        {
            Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};
            propagator.addDynamics(std::make_shared<CentralBodyGravity>(earthSpherical_));

            EXPECT_THROW(
                {
                    try
                    {
                        propagator.calculateStateAt(state, state.getInstant());
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("Propagator needs exactly one Central Body Gravity Dynamics.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }

        // Too many PositionDerivatives
        {
            Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};
            propagator.addDynamics(std::make_shared<PositionDerivative>());

            EXPECT_THROW(
                {
                    try
                    {
                        propagator.calculateStateAt(state, state.getInstant());
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("Propagator needs exactly one Position Derivative Dynamics.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }

        // Missing PositionDerivatives or CentralBodyGravity
        {
            Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};
            propagator.setDynamics({std::make_shared<CentralBodyGravity>(earthSpherical_)});

            EXPECT_THROW(
                {
                    try
                    {
                        propagator.calculateStateAt(state, state.getInstant());
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("Propagator needs exactly one Position Derivative Dynamics.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );

            propagator.setDynamics({std::make_shared<PositionDerivative>()});

            EXPECT_THROW(
                {
                    try
                    {
                        propagator.calculateStateAt(state, state.getInstant());
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("Propagator needs exactly one Central Body Gravity Dynamics.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }

        // Wrong number of AtmosphericDrags
        {
            Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};
            propagator.addDynamics(std::make_shared<AtmosphericDrag>(std::make_shared<Celestial>(Earth::AtmosphericOnly(
                std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
            ))));
            propagator.addDynamics(std::make_shared<AtmosphericDrag>(std::make_shared<Celestial>(Earth::AtmosphericOnly(
                std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
            ))));

            EXPECT_THROW(
                {
                    try
                    {
                        propagator.calculateStateAt(state, state.getInstant());
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("Propagator can have at most one Atmospheric Drag Dynamics.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }

        // Wrong number of Thruster Dynamics
        {
            const Shared<const ConstantThrust> constantThrustSPtr_ = std::make_shared<ConstantThrust>(
                LocalOrbitalFrameDirection({1.0, 0.0, 0.0}, LocalOrbitalFrameFactory::VNC(Frame::GCRF()))
            );

            const Shared<Thruster> defaultConstantThrust_ =
                std::make_shared<Thruster>(SatelliteSystem::Default(), constantThrustSPtr_);

            Propagator propagator = {defaultNumericalSolver_, defaultDynamics_};
            propagator.addDynamics(defaultConstantThrust_);
            propagator.addDynamics(defaultConstantThrust_);

            EXPECT_THROW(
                {
                    try
                    {
                        propagator.calculateStateAt(state, state.getInstant());
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("Propagator can have at most one Thruster Dynamics.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }
    }
}
