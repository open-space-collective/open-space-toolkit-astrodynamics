/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/CSSISpaceWeather.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solvers/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::ctnr::Tuple;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::filesystem::Directory;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Size;

using ostk::math::geometry::d3::objects::Composite;
using ostk::math::geometry::d3::objects::Cuboid;
using ostk::math::geometry::d3::objects::Point;
using ostk::math::object::Matrix3d;
using ostk::math::object::MatrixXd;
using ostk::math::object::Vector3d;
using ostk::math::object::VectorXd;

using ostk::physics::data::Scalar;
using ostk::physics::Environment;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::environment::ephemerides::Analytical;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Time;
using ostk::physics::time::Scale;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using ostk::physics::environment::object::celestial::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using SWManager = ostk::physics::environment::atmospheric::earth::Manager;
using ostk::physics::environment::atmospheric::earth::CSSISpaceWeather;

using ostk::astro::eventcondition::InstantCondition;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::Orbit;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;
using ostk::astro::trajectory::LocalOrbitalFrameDirection;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::Dynamics;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::SatelliteSystemBuilder;
using ostk::astro::dynamics::PositionDerivative;
using ostk::astro::dynamics::CentralBodyGravity;
using ostk::astro::dynamics::ThirdBodyGravity;
using ostk::astro::dynamics::AtmosphericDrag;
using ostk::astro::dynamics::Thruster;
using ostk::astro::dynamics::Tabulated;
using ostk::astro::guidancelaw::ConstantThrust;
using ostk::astro::guidancelaw::QLaw;
using ostk::astro::solvers::FiniteDifferenceSolver;

/* UNIT TESTS */
class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        const PropulsionSystem propulsionSystem = {
            Scalar(0.1, PropulsionSystem::thrustSIUnit),
            Scalar(1500.0, PropulsionSystem::specificImpulseSIUnit),
        };

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

    const NumericalSolver defaultRK4_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
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

    const Shared<CoordinatesBroker> dragCoordinatesBrokerSPtr_ = std::make_shared<CoordinatesBroker>(CoordinatesBroker(
        {CartesianPosition::Default(),
         CartesianVelocity::Default(),
         CoordinatesSubset::Mass(),
         CoordinatesSubset::SurfaceArea(),
         CoordinatesSubset::DragCoefficient()}
    ));
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Constructor)
{
    {
        EXPECT_NO_THROW(Propagator(defaultNumericalSolver_, defaultDynamics_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, EqualToOperator)
{
    {
        const Propagator propagator_x = {defaultPropagator_};
        EXPECT_TRUE(defaultPropagator_ == propagator_x);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, NotEqualToOperator)
{
    {
        const Propagator propagatorX = {defaultPropagator_};
        EXPECT_FALSE(defaultPropagator_ != propagatorX);

        const NumericalSolver numericalSolver1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        const Propagator propagator1 = {numericalSolver1, defaultDynamics_};
        EXPECT_TRUE(defaultPropagator_ != propagator1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, IsDefined)
{
    {
        EXPECT_TRUE(defaultPropagator_.isDefined());
    }

    {
        const Propagator propagatorWithEmptyDynamicsArray = {defaultNumericalSolver_};
        EXPECT_FALSE(propagatorWithEmptyDynamicsArray.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Undefined)
{
    {
        EXPECT_NO_THROW(Propagator::Undefined());

        EXPECT_FALSE(Propagator::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultPropagator_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultPropagator_.print(std::cout, true));
        EXPECT_NO_THROW(defaultPropagator_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, AccessCoordinatesBroker)
{
    {
        EXPECT_TRUE(defaultPropagator_.accessCoordinatesBroker() != nullptr);
    }

    {
        EXPECT_ANY_THROW(Propagator::Undefined().accessCoordinatesBroker());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, AccessNumericalSolver)
{
    {
        EXPECT_NO_THROW(defaultPropagator_.accessNumericalSolver());
    }

    {
        EXPECT_ANY_THROW(Propagator::Undefined().accessNumericalSolver());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Getters)
{
    {
        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
    }

    {
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, SetDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 2);

        const Shared<Dynamics> centralBodyGravity = std::make_shared<CentralBodyGravity>(earthSpherical_);

        defaultPropagator_.setDynamics({centralBodyGravity});

        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 1);

        defaultPropagator_.setDynamics({});

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 0);
        EXPECT_FALSE(defaultPropagator_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, AddDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 2);

        const Shared<Dynamics> centralBodyGravity = std::make_shared<CentralBodyGravity>(earthSpherical_);

        defaultPropagator_.addDynamics(centralBodyGravity);

        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 3);

        defaultPropagator_.addDynamics(centralBodyGravity);

        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 4);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, ClearDynamics)
{
    {
        EXPECT_TRUE(defaultPropagator_.getNumberOfCoordinates() == 6);
        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() >= 1);

        defaultPropagator_.clearDynamics();

        EXPECT_TRUE(defaultPropagator_.getDynamics().getSize() == 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, CalculateStateAt)
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
        EXPECT_EQ(outputStateGCRF.accessCoordinatesBroker(), outputStateGCRF2.accessCoordinatesBroker());

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

        const Array<Shared<const CoordinatesSubset>> coordinatesSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinatesSubset>(CoordinatesSubset("extra1", 1)),
            std::make_shared<CoordinatesSubset>(CoordinatesSubset("extra2", 1)),
        };

        const State bigState = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            coords,
            gcrfSPtr_,
            coordinatesSubsets,
        };

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const State outputState = defaultPropagator_.calculateStateAt(bigState, instantArray[0]);

        EXPECT_NE(bigState, outputState);
        EXPECT_EQ(bigState.getCoordinatesSubsets(), outputState.getCoordinatesSubsets());
        EXPECT_EQ(outputState.getCoordinates().size(), 8);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, CalculateStateAt_Condition)
{
    {
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Setup instants
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);

        const InstantCondition condition = {
            InstantCondition::Criterion::StrictlyPositive,
            state.accessInstant() + Duration::Seconds(60.0),
        };

        const Propagator propagator = {defaultRKD5_, defaultDynamics_};

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
        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Setup instants
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);

        const InstantCondition condition = {
            InstantCondition::Criterion::StrictlyPositive,
            state.accessInstant() + Duration::Seconds(60.0),
        };

        const Propagator propagator = {defaultRKD5_, defaultDynamics_};

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
        EXPECT_EQ(outputStateGCRF.accessCoordinatesBroker(), outputStateGCRF2.accessCoordinatesBroker());

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

        const Array<Shared<const CoordinatesSubset>> coordinatesSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinatesSubset>(CoordinatesSubset("extra1", 1)),
            std::make_shared<CoordinatesSubset>(CoordinatesSubset("extra2", 1)),
        };

        const State bigState = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            coords,
            gcrfSPtr_,
            coordinatesSubsets,
        };

        // Setup instants
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);

        const InstantCondition condition = {
            InstantCondition::Criterion::StrictlyPositive,
            bigState.accessInstant() + Duration::Seconds(60.0),
        };

        const Propagator propagator = {defaultRKD5_, defaultDynamics_};

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(propagator.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const NumericalSolver::ConditionSolution conditionSolution =
            propagator.calculateStateToCondition(bigState, endInstant, condition);

        const State outputState = conditionSolution.state;

        EXPECT_NE(bigState, outputState);
        EXPECT_EQ(bigState.getCoordinatesSubsets(), outputState.getCoordinatesSubsets());
        EXPECT_EQ(outputState.getCoordinates().size(), 8);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, CalculateStatesAt)
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
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
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
            EXPECT_EQ(outputStatesGCRF[i].accessCoordinatesBroker(), outputStateGCRF2.accessCoordinatesBroker());

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

        const Array<Shared<const CoordinatesSubset>> coordinatesSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            std::make_shared<CoordinatesSubset>(CoordinatesSubset("extra1", 1)),
            std::make_shared<CoordinatesSubset>(CoordinatesSubset("extra2", 1)),
        };

        const State bigState = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            coords,
            gcrfSPtr_,
            coordinatesSubsets,
        };

        // Confirm the propagator only needs 6 dimensions
        EXPECT_EQ(defaultPropagator_.getNumberOfCoordinates(), 6);
        EXPECT_EQ(bigState.getCoordinates().size(), 8);

        const Array<State> outputStates = defaultPropagator_.calculateStatesAt(bigState, instantArray);

        for (size_t i = 0; i < outputStates.getSize(); i++)
        {
            EXPECT_NE(bigState, outputStates[i]);
            EXPECT_EQ(bigState.getCoordinatesSubsets(), outputStates[i].getCoordinatesSubsets());
            EXPECT_EQ(outputStates[i].getCoordinates().size(), 8);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Default)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, FromEnvironment)
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
        const Array<Shared<Object>> celestials = {
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

// /* VALIDATION TESTS */
// /* Force model validation tests */
// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody)
{
    // Two body vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_TwoBody_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Propagate all states
        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(2e-3, positionErrorGCRF);
            ASSERT_GT(2e-6, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // Two body vs STK
    {
        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_TwoBody_2hr_run.csv"
            )),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referencePositionArray_ITRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_ITRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC));

            referencePositionArrayGCRF.add({
                referenceRow[1].accessReal(),
                referenceRow[2].accessReal(),
                referenceRow[3].accessReal(),
            });
            referenceVelocityArrayGCRF.add({
                referenceRow[4].accessReal(),
                referenceRow[5].accessReal(),
                referenceRow[6].accessReal(),
            });

            referencePositionArray_ITRF.add({
                referenceRow[7].accessReal(),
                referenceRow[8].accessReal(),
                referenceRow[9].accessReal(),
            });
            referenceVelocityArray_ITRF.add({
                referenceRow[10].accessReal(),
                referenceRow[11].accessReal(),
                referenceRow[12].accessReal(),
            });
        }

        const Shared<const Frame> itrfSPtr = Frame::ITRF();

        // Current state and instant setup
        const Instant startInstant = instantArray[0];

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Propagate all states
        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(2e-7, positionErrorGCRF);
            ASSERT_GT(2e-10, velocityErrorGCRF);

            // ITRF Compare
            const Position position_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).getPosition();
            const Velocity velocity_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).getVelocity();

            const double positionError_ITRF =
                (position_ITRF.accessCoordinates() - referencePositionArray_ITRF[i]).norm();
            const double velocityError_ITRF =
                (velocity_ITRF.accessCoordinates() - referenceVelocityArray_ITRF[i]).norm();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(2e-2, positionError_ITRF);
            ASSERT_GT(2e-5, velocityError_ITRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "ITRF Position error is: " << positionError_ITRF << "m" << std::endl;
            // std::cout << "ITRF Velocity error is: " << velocityError_ITRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        }
    }
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, OrekitSpherical)
{
    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 7546.053290, 0.0}, gcrfSPtr_),
    };

    const State finalState = defaultPropagator_.calculateStateAt(state, state.getInstant() + Duration::Minutes(60.0));

    VectorXd expectedCoordinates(6);
    expectedCoordinates << -5172890.413848375000, -4716058.194055955000, 0000000.000000000000, 05083.946625786208,
        -05576.415230079790, 00000.000000000000;

    VectorXd actualCoordinates(6);
    actualCoordinates.segment(0, 3) = finalState.extractCoordinate(CartesianPosition::Default());
    actualCoordinates.segment(3, 3) = finalState.extractCoordinate(CartesianVelocity::Default());

    const VectorXd residuals = actualCoordinates - expectedCoordinates;

    EXPECT_TRUE((residuals.array() < 1e-7).all()) << String::Format("Residual: {}", residuals.maxCoeff());
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_EGM2008)
{
    // EGM2008 100x100 perturbation only vs STK EGM2008
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_EGM2008_100x100_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Create dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM2008(100, 100));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(1.5e-1, positionErrorGCRF);
            ASSERT_GT(1.5e-4, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM2008 360x360, 100x100, 70x70, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_EGM2008_100x100_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth_360 = std::make_shared<Celestial>(Earth::EGM2008(360, 360));
        const Shared<Celestial> earth_100 = std::make_shared<Celestial>(Earth::EGM2008(100, 100));
        const Shared<Celestial> earth_70 = std::make_shared<Celestial>(Earth::EGM2008(70, 70));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM2008(45, 45));

        const Array<Shared<Dynamics>> EGM2008dynamics_360 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_360),
        };
        const Array<Shared<Dynamics>> EGM2008dynamics_100 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_100),
        };
        const Array<Shared<Dynamics>> EGM2008dynamics_70 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_70),
        };
        const Array<Shared<Dynamics>> EGM2008dynamics_45 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_45),
        };
        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Setup Propagator
        const Propagator propagator_360 = {defaultNumericalSolver_, EGM2008dynamics_360};
        const Propagator propagator_100 = {defaultNumericalSolver_, EGM2008dynamics_100};
        const Propagator propagator_70 = {defaultNumericalSolver_, EGM2008dynamics_70};
        const Propagator propagator_45 = {defaultNumericalSolver_, EGM2008dynamics_45};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = propagator_360.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_100 = propagator_100.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_70 = propagator_70.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF_360 = propagatedStateArray_360[i].getPosition();
            const Velocity velocityGCRF_360 = propagatedStateArray_360[i].getVelocity();
            const Position positionGCRF_100 = propagatedStateArray_100[i].getPosition();
            const Velocity velocityGCRF_100 = propagatedStateArray_100[i].getVelocity();
            const Position positionGCRF_70 = propagatedStateArray_70[i].getPosition();
            const Velocity velocityGCRF_70 = propagatedStateArray_70[i].getVelocity();
            const Position positionGCRF_45 = propagatedStateArray_45[i].getPosition();
            const Velocity velocityGCRF_45 = propagatedStateArray_45[i].getVelocity();

            // 360 vs 100
            const double positionErrorGCRF_360_100 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_100.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_100 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_100.accessCoordinates()).norm();
            ASSERT_GT(2e-3, positionErrorGCRF_360_100);
            ASSERT_GT(2e-6, velocityErrorGCRF_360_100);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 100 Position error is: " << positionErrorGCRF_360_100 << "m" << std::endl;
            // std::cout << "360 vs 100 Velocity error is: " << velocityErrorGCRF_360_100 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 70
            const double positionErrorGCRF_360_70 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_70.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_70 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_70.accessCoordinates()).norm();
            ASSERT_GT(5e-2, positionErrorGCRF_360_70);
            ASSERT_GT(5e-5, velocityErrorGCRF_360_70);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 70 Position error is: " << positionErrorGCRF_360_70 << "m" << std::endl;
            // std::cout << "360 vs 70 Velocity error is: " << velocityErrorGCRF_360_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionErrorGCRF_360_45 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_45.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_45 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_45.accessCoordinates()).norm();
            ASSERT_GT(5e-1, positionErrorGCRF_360_45);
            ASSERT_GT(5e-4, velocityErrorGCRF_360_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 45 Position error is: " << positionErrorGCRF_360_45 << "m" << std::endl;
            // std::cout << "360 vs 45 Velocity error is: " << velocityErrorGCRF_360_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_EGM96)
{
    // EGM96 360x360 perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_EGM96_360x360_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM96(360, 360));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(9e-2, positionErrorGCRF);
            ASSERT_GT(9e-5, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM96 70x70 perturbation only vs STK WGS84EGM96
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84EGM96_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM96(70, 70));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(1.5e-4, positionErrorGCRF);
            ASSERT_GT(1.5e-7, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM96 180x180, 90x90, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_EGM96_360x360_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Create dynamics
        const Shared<Celestial> earth_360 = std::make_shared<Celestial>(Earth::EGM96(360, 360));
        const Shared<Celestial> earth_180 = std::make_shared<Celestial>(Earth::EGM96(180, 180));
        const Shared<Celestial> earth_90 = std::make_shared<Celestial>(Earth::EGM96(90, 90));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM96(45, 45));

        const Array<Shared<Dynamics>> EGM96Dynamics_360 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_360),
        };
        const Array<Shared<Dynamics>> EGM96Dynamics_180 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_180),
        };
        const Array<Shared<Dynamics>> EGM96Dynamics_90 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_90),
        };
        const Array<Shared<Dynamics>> EGM96Dynamics_45 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_45),
        };

        // Setup Propagator
        const Propagator propagator_360 = {defaultNumericalSolver_, EGM96Dynamics_360};
        const Propagator propagator_180 = {defaultNumericalSolver_, EGM96Dynamics_180};
        const Propagator propagator_90 = {defaultNumericalSolver_, EGM96Dynamics_90};
        const Propagator propagator_45 = {defaultNumericalSolver_, EGM96Dynamics_45};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = propagator_360.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_180 = propagator_180.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_90 = propagator_90.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF_360 = propagatedStateArray_360[i].getPosition();
            const Velocity velocityGCRF_360 = propagatedStateArray_360[i].getVelocity();
            const Position positionGCRF_180 = propagatedStateArray_180[i].getPosition();
            const Velocity velocityGCRF_180 = propagatedStateArray_180[i].getVelocity();
            const Position positionGCRF_90 = propagatedStateArray_90[i].getPosition();
            const Velocity velocityGCRF_90 = propagatedStateArray_90[i].getVelocity();
            const Position positionGCRF_45 = propagatedStateArray_45[i].getPosition();
            const Velocity velocityGCRF_45 = propagatedStateArray_45[i].getVelocity();

            // 360 vs 180
            const double positionErrorGCRF_360_180 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_180.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_180 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_180.accessCoordinates()).norm();
            ASSERT_GT(9e-7, positionErrorGCRF_360_180);
            ASSERT_GT(9e-10, velocityErrorGCRF_360_180);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 180 Position error is: " << positionErrorGCRF_360_180 << "m" << std::endl;
            // std::cout << "360 vs 180 Velocity error is: " << velocityErrorGCRF_360_180 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 90
            const double positionErrorGCRF_360_90 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_90.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_90 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_90.accessCoordinates()).norm();
            ASSERT_GT(7e-3, positionErrorGCRF_360_90);
            ASSERT_GT(7e-6, velocityErrorGCRF_360_90);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 90 Position error is: " << positionErrorGCRF_360_90 << "m" << std::endl;
            // std::cout << "360 vs 90 Velocity error is: " << velocityErrorGCRF_360_90 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionErrorGCRF_360_45 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_45.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_45 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_45.accessCoordinates()).norm();
            ASSERT_GT(5e-1, positionErrorGCRF_360_45);
            ASSERT_GT(5e-4, velocityErrorGCRF_360_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 45 Position error is: " << positionErrorGCRF_360_45 << "m" << std::endl;
            // std::cout << "360 vs 45 Velocity error is: " << velocityErrorGCRF_360_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_EGM84)
{
    // EGM84 70x70 perturbation only vs STK EGM84
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM84(70, 70));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(7e-5, positionErrorGCRF);
            ASSERT_GT(7e-8, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM84 180x180, 70x70, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth_180 = std::make_shared<Celestial>(Earth::EGM84(180, 180));
        const Shared<Celestial> earth_70 = std::make_shared<Celestial>(Earth::EGM84(70, 70));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM84(45, 45));

        const Array<Shared<Dynamics>> EGM84Dynamics_180 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_180),
        };
        const Array<Shared<Dynamics>> EGM84Dynamics_70 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_70),
        };
        const Array<Shared<Dynamics>> EGM84Dynamics_45 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_45),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Setup Propagator
        const Propagator propagator_180 = {defaultNumericalSolver_, EGM84Dynamics_180};
        const Propagator propagator_70 = {defaultNumericalSolver_, EGM84Dynamics_70};
        const Propagator propagator_45 = {defaultNumericalSolver_, EGM84Dynamics_45};

        // Propagate all states
        const Array<State> propagatedStateArray_180 = propagator_180.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_70 = propagator_70.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF_180 = propagatedStateArray_180[i].getPosition();
            const Velocity velocityGCRF_180 = propagatedStateArray_180[i].getVelocity();
            const Position positionGCRF_70 = propagatedStateArray_70[i].getPosition();
            const Velocity velocityGCRF_70 = propagatedStateArray_70[i].getVelocity();
            const Position positionGCRF_45 = propagatedStateArray_45[i].getPosition();
            const Velocity velocityGCRF_45 = propagatedStateArray_45[i].getVelocity();

            // 180 vs 70
            const double positionErrorGCRF_180_70 =
                (positionGCRF_180.accessCoordinates() - positionGCRF_70.accessCoordinates()).norm();
            const double velocityErrorGCRF_180_70 =
                (velocityGCRF_180.accessCoordinates() - velocityGCRF_70.accessCoordinates()).norm();
            ASSERT_GT(6e-2, positionErrorGCRF_180_70);
            ASSERT_GT(6e-5, velocityErrorGCRF_180_70);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 70 Position error is: " << positionErrorGCRF_180_70 << "m" << std::endl;
            // std::cout << "180 vs 70 Velocity error is: " << velocityErrorGCRF_180_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 180 vs 45
            const double positionErrorGCRF_180_45 =
                (positionGCRF_180.accessCoordinates() - positionGCRF_45.accessCoordinates()).norm();
            const double velocityErrorGCRF_180_45 =
                (velocityGCRF_180.accessCoordinates() - velocityGCRF_45.accessCoordinates()).norm();
            ASSERT_GT(3e-1, positionErrorGCRF_180_45);
            ASSERT_GT(3e-4, velocityErrorGCRF_180_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 45 Position error is: " << positionErrorGCRF_180_45 << "m" << std::endl;
            // std::cout << "180 vs 45 Velocity error is: " << velocityErrorGCRF_180_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Sun_Moon)
{
    // Sun+Moon perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_TwoBody_Sun_Moon_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> moonSpherical_ = std::make_shared<Celestial>(Moon::Spherical());
        const Shared<Celestial> sunSpherical_ = std::make_shared<Celestial>(Sun::Spherical());
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSpherical_),
            std::make_shared<ThirdBodyGravity>(moonSpherical_),
            std::make_shared<ThirdBodyGravity>(sunSpherical_),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(2e-3, positionErrorGCRF);
            ASSERT_GT(2e-6, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Sun)
{
    // Sun perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_TwoBody_Sun_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // dynamics setup
        const Shared<Celestial> sunSpherical = std::make_shared<Celestial>(Sun::Spherical());
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSpherical_),
            std::make_shared<ThirdBodyGravity>(sunSpherical),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(2e-3, positionErrorGCRF);
            ASSERT_GT(2e-6, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Moon)
{
    // Moon perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_TwoBody_Moon_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // dynamics setup
        const Shared<Celestial> moonSpherical = std::make_shared<Celestial>(Moon::Spherical());
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSpherical_),
            std::make_shared<ThirdBodyGravity>(moonSpherical),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(2e-3, positionErrorGCRF);
            ASSERT_GT(2e-6, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, OrekitExponential)
{
    const Real mass = satelliteDryMass_.inKilograms();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

    VectorXd initialStateVector(9);
    initialStateVector << 7000000.0, 0.0, 0.0, 0.0, 7546.053290, 0.0, mass, surfaceArea, dragCoefficient;

    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC),
        initialStateVector,
        gcrfSPtr_,
        dragCoordinatesBrokerSPtr_
    };

    // Setup Propagator
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
    );

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };
    const Propagator propagator = {defaultNumericalSolver_, dynamics};

    const State finalState = propagator.calculateStateAt(state, state.getInstant() + Duration::Seconds(60.0 * 60.0));

    VectorXd expectedCoordinates(9);
    expectedCoordinates << -5172889.585695211000, -4716058.453899897000, 0000000.000008572841, 05083.947538266920,
        -05576.414764013522, -00000.000000010687, satelliteDryMass_.inKilograms(), surfaceArea, dragCoefficient;

    VectorXd actualCoordinates(9);
    actualCoordinates.segment(0, 3) = finalState.extractCoordinate(CartesianPosition::Default());
    actualCoordinates.segment(3, 3) = finalState.extractCoordinate(CartesianVelocity::Default());
    actualCoordinates.segment(6, 1) = finalState.extractCoordinate(CoordinatesSubset::Mass());
    actualCoordinates.segment(7, 1) = finalState.extractCoordinate(CoordinatesSubset::SurfaceArea());
    actualCoordinates.segment(8, 1) = finalState.extractCoordinate(CoordinatesSubset::DragCoefficient());

    const VectorXd residuals = actualCoordinates - expectedCoordinates;

    ASSERT_TRUE((residuals.array() < 1e-6).all()) << String::Format("Residual: {}", residuals.maxCoeff());
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_Drag_Constant_Exponential_500km)
{
    // Earth with Exponential atmospheric drag compared against OREKit
    {
        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                   "Orekit_Drag_Exponential_500km_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(
                DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
            ));

            referencePositionArrayGCRF.add(
                Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
            );
        }

        // Setup dynamics
        const Earth earth = Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
        );
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSPtr),
            std::make_shared<AtmosphericDrag>(earthSPtr),
        };

        const Real mass = satelliteSystem_.getMass().inKilograms();
        const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
        const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

        VectorXd initialStateVector(9);
        initialStateVector << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0], mass, surfaceArea,
            dragCoefficient;

        // Current state and instant setup
        const State state = {instantArray[0], initialStateVector, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

        // Setup Propagator model and orbit
        const Propagator propagator = {defaultRK4_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(2e-3, positionErrorGCRF);
            ASSERT_GT(2e-6, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Instant is: " << instantArray[i] << std::endl;
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

// TODO remove this test
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_Drag_Constant_Exponential_320km)
{
    // Earth with Exponential atmospheric drag compared against OREKit
    {
        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                   "Orekit_Drag_Exponential_320km_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(
                DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
            ));

            referencePositionArrayGCRF.add(
                Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
            );
        }

        // Setup dynamics
        const Earth earth = Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
        );
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSPtr),
            std::make_shared<AtmosphericDrag>(earthSPtr),
        };

        const Real mass = satelliteSystem_.getMass().inKilograms();
        const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
        const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

        // Setup initial conditions
        VectorXd initialCoordinates(9);
        initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0], mass, surfaceArea,
            dragCoefficient;

        const State state = {instantArray[0], initialCoordinates, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

        // Setup Propagator model and orbit
        const Propagator propagator = {defaultRK4_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);
        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();
            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();
            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(4e-3, positionErrorGCRF);  // TBI: Investigate wrt 500km case
            ASSERT_GT(5e-6, velocityErrorGCRF);  // TBI: Investigate wrt 500km case

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Instant is: " << instantArray[i] << std::endl;
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

// TODO remove this test
TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator,
    PropAccuracy_Drag_Constant_NRLMSISE00_470km_Small_Cross_Section
)
{
    {
        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                   "Orekit_Drag_NRLMSISE00_470km_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceAccelerationArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(
                DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
            ));

            referencePositionArrayGCRF.add(
                Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
            );
            referenceAccelerationArrayGCRF.add(
                Vector3d(referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal())
            );
        }

        // Use the same space weather input file that Orekit uses
        SWManager::Get().reset();
        CSSISpaceWeather swData = CSSISpaceWeather::LoadLegacy(File::Path(Path::Parse(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/SpaceWeather-All-v1.2.txt"
        )));
        SWManager::Get().loadCSSISpaceWeather(swData);

        const Sun sun = Sun::Default();
        const Shared<Celestial> sunSPtr = std::make_shared<Celestial>(sun);

        // Setup dynamics
        const Earth earth = Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(
                EarthAtmosphericModel::Type::NRLMSISE00,
                EarthAtmosphericModel::InputDataType::CSSISpaceWeatherFile,
                Real::Undefined(),
                Real::Undefined(),
                Real::Undefined(),
                Frame::ITRF(),
                EarthGravitationalModel::WGS84.equatorialRadius_,
                EarthGravitationalModel::WGS84.flattening_,
                sunSPtr
            )
        );
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram),
            satelliteGeometry,
            Matrix3d::Identity(),
            1.0,
            2.2,
        };

        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSPtr),
            std::make_shared<AtmosphericDrag>(earthSPtr),
        };

        // Setup initial conditions
        const Real mass = satelliteDryMass_.inKilograms();
        const Real surfaceArea = satelliteSystem.getCrossSectionalSurfaceArea();
        const Real dragCoefficient = satelliteSystem.getDragCoefficient();

        // Setup initial conditions
        VectorXd initialCoordinates(9);
        initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0], mass, surfaceArea,
            dragCoefficient;

        const State state = {instantArray[0], initialCoordinates, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

        // Setup Propagator model and orbit
        const Propagator propagator = {defaultRK4_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(8e-3, positionErrorGCRF);
            ASSERT_GT(1e-5, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Instant is: " << instantArray[i] << std::endl;
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    SWManager::Get().reset();
}

// TODO remove this test
TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator,
    PropAccuracy_Drag_Constant_NRLMSISE00_470km_Large_Cross_Section
)
{
    {
        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                   "Orekit_Drag_NRLMSISE00_Large_Cross_Section_470km_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceAccelerationArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(
                DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
            ));

            referencePositionArrayGCRF.add(
                Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
            );
            referenceAccelerationArrayGCRF.add(
                Vector3d(referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal())
            );
        }

        // Use the same space weather input file that Orekit uses
        SWManager::Get().reset();
        CSSISpaceWeather swData = CSSISpaceWeather::LoadLegacy(File::Path(Path::Parse(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/SpaceWeather-All-v1.2.txt"
        )));
        SWManager::Get().loadCSSISpaceWeather(swData);

        const Sun sun = Sun::Default();
        const Shared<Celestial> sunSPtr = std::make_shared<Celestial>(sun);

        // Setup dynamics
        const Earth earth = Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(
                EarthAtmosphericModel::Type::NRLMSISE00,
                EarthAtmosphericModel::InputDataType::CSSISpaceWeatherFile,
                Real::Undefined(),
                Real::Undefined(),
                Real::Undefined(),
                Frame::ITRF(),
                EarthGravitationalModel::WGS84.equatorialRadius_,
                EarthGravitationalModel::WGS84.flattening_,
                sunSPtr
            )
        );
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram),
            satelliteGeometry,
            Matrix3d::Identity(),
            500.0,
            2.2,
        };

        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSPtr),
            std::make_shared<AtmosphericDrag>(earthSPtr),
        };

        // Setup initial conditions
        const Real mass = satelliteDryMass_.inKilograms();
        const Real surfaceArea = satelliteSystem.getCrossSectionalSurfaceArea();
        const Real dragCoefficient = satelliteSystem.getDragCoefficient();

        // Setup initial conditions
        VectorXd initialCoordinates(9);
        initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0], mass, surfaceArea,
            dragCoefficient;

        const State state = {instantArray[0], initialCoordinates, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

        // Setup Propagator model and orbit
        const Propagator propagator = {defaultRK4_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(5.0, positionErrorGCRF);
            ASSERT_GT(5e-3, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Instant is: " << instantArray[i] << std::endl;
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    SWManager::Get().reset();
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, TabulatedDynamics_ConstantThrust)
{
    VectorXd coordinates(9);
    coordinates << 1204374.4983743676, -6776950.422456586, 0.0, -967.6410027260863, -171.96557073856417,
        7546.1119092033205, 110.0, 500.0, 2.2;

    const State initialState = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0, 0), Scale::UTC),
        coordinates,
        Frame::GCRF(),
        dragCoordinatesBrokerSPtr_,
    };

    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM96, Directory::Undefined(), 20, 20),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
    );
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const PropulsionSystem propulsionSystem = {
        Scalar(1e-1, PropulsionSystem::thrustSIUnit),
        Scalar(3000.0, PropulsionSystem::specificImpulseSIUnit),
    };

    const SatelliteSystem satelliteSystem = SatelliteSystemBuilder::Default()
                                                .withPropulsionSystem(propulsionSystem)
                                                .withDryMass(Mass::Kilograms(100.0))
                                                .build();

    const Shared<ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(ConstantThrust::Intrack());

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
        std::make_shared<Thruster>(satelliteSystem, constantThrustSPtr),
    };

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        1.0,
        1e-12,
        1e-12,
    };

    // Create contribution profile
    Propagator propagator = {numericalSolver, dynamics};
    const State endStatePropagated =
        propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Hours(1.5));

    // TBI: For some reason it's pushing the initial state twice
    const Array<State> tmpStates = propagator.accessNumericalSolver().getObservedStates();
    const Array<State> states(tmpStates.begin() + 1, tmpStates.end());

    MatrixXd contributions(states.getSize(), 4);
    for (Size i = 0; i < states.getSize(); ++i)
    {
        const VectorXd contribution =
            dynamics[3]->computeContribution(states[i].accessInstant(), states[i].accessCoordinates(), Frame::GCRF());
        contributions.row(i) = contribution;
    }
    const Array<Instant> instants = states.map<Instant>(
        [](const auto& state)
        {
            return state.accessInstant();
        }
    );

    const Tabulated tabulated = {
        instants,
        contributions,
        {CartesianVelocity::Default(), CoordinatesSubset::Mass()},
        gcrfSPtr_,
    };

    // Re-propagate with tabulated dynamics
    const Array<Shared<Dynamics>> tabulatedDynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
        std::make_shared<Tabulated>(tabulated),
    };

    const NumericalSolver tabulatedNumericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        3.0,
        1e-12,
        1e-12,
    };

    Propagator tabulatedPropagator = {tabulatedNumericalSolver, tabulatedDynamics};
    const State endStateTabulated =
        propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Hours(1.5));

    for (Size i = 0; i < endStateTabulated.getSize(); ++i)
    {
        EXPECT_NEAR(endStatePropagated.accessCoordinates()[i], endStateTabulated.accessCoordinates()[i], 1e-14);
    }
}

/* Propagation Interval validation test */
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_IntervalSelfComparison)
{
    // Test that calculateStatesAt returns the same answer if states are requested at short or long intervals
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Propagate all states at short interval
        const Array<State> propagatedStateArray_short = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Propagate all states at long interval
        Array<Instant> instantArray_longInterval = Array<Instant>::Empty();
        const size_t indexIncrement = 10;
        for (size_t i = 0; i < instantArray.getSize(); i += indexIncrement)
        {
            instantArray_longInterval.add(instantArray[i]);
        }
        const Array<State> propagatedStateArray_long =
            defaultPropagator_.calculateStatesAt(state, instantArray_longInterval);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Run once every X times
            if (i % indexIncrement == 0)
            {
                // GCRF Compare
                const Position positionShort = propagatedStateArray_short[i].getPosition();
                const Velocity velocityShort = propagatedStateArray_short[i].getVelocity();
                const Position positionLong = propagatedStateArray_long[i / indexIncrement].getPosition();
                const Velocity velocityLong = propagatedStateArray_long[i / indexIncrement].getVelocity();

                const double positionErrorGCRF =
                    (positionShort.accessCoordinates() - positionLong.accessCoordinates()).norm();
                const double velocityErrorGCRF =
                    (velocityShort.accessCoordinates() - velocityLong.accessCoordinates()).norm();

                ASSERT_EQ(*Frame::GCRF(), *positionShort.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *velocityShort.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *positionLong.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *velocityLong.accessFrame());

                ASSERT_GT(1.0e-7, positionErrorGCRF);
                ASSERT_GT(1.0e-7, velocityErrorGCRF);

                // Results console output

                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
                // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }
    }

    // Test that calculateStateAt returns the almost the same answer if states are requested at short or long intervals
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        testing::internal::CaptureStdout();

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Propagator output generated
            const State stateShort = defaultPropagator_.calculateStateAt(state, instantArray[i]);

            // Run once every X times
            if (i % 10 == 0)
            {
                const State stateLong = defaultPropagator_.calculateStateAt(state, instantArray[i]);

                // GCRF Compare
                const Position positionShort = stateShort.getPosition();
                const Velocity velocityShort = stateShort.getVelocity();
                const Position positionLong = stateLong.getPosition();
                const Velocity velocityLong = stateLong.getVelocity();

                const double positionErrorGCRF =
                    (positionShort.accessCoordinates() - positionLong.accessCoordinates()).norm();
                const double velocityErrorGCRF =
                    (velocityShort.accessCoordinates() - velocityLong.accessCoordinates()).norm();

                ASSERT_GT(3e-8, positionErrorGCRF);
                ASSERT_GT(3e-11, velocityErrorGCRF);

                // Results console output

                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
                // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }

        EXPECT_TRUE(testing::internal::GetCapturedStdout().empty());
    }
}

/* Stepper validation test */
TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_StepperTypeSelfComparison
)
{
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Construct default numerical solver
        const NumericalSolver numericalSolver54 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15,
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Setup Propagator
        const Propagator propagator54 = {numericalSolver54, defaultDynamics_};

        // Propagate all states
        const Array<State> propagatedStateArray54 = propagator54.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray78 = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position54 = propagatedStateArray54[i].getPosition();
            const Velocity velocity54 = propagatedStateArray54[i].getVelocity();
            const Position position78 = propagatedStateArray78[i].getPosition();
            const Velocity velocity78 = propagatedStateArray78[i].getVelocity();

            const double positionErrorGCRF = (position54.accessCoordinates() - position78.accessCoordinates()).norm();
            const double velocityErrorGCRF = (velocity54.accessCoordinates() - velocity78.accessCoordinates()).norm();

            ASSERT_GT(5e-7, positionErrorGCRF);
            ASSERT_GT(5e-10, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}
