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
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
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
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::ctnr::Tuple;
using ostk::core::fs::File;
using ostk::core::fs::Path;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Size;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;

using ostk::physics::Environment;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::env::Object;
using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::env::ephem::Analytical;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::eventcondition::InstantCondition;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::ThirdBodyGravity;
using ostk::astro::flight::system::dynamics::AtmosphericDrag;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, ostk::physics::units::Time::Unit::Second);

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

        this->satelliteSystem_ = {
            Mass(100.0, Mass::Unit::Kilogram),
            satelliteGeometry,
            Matrix3d::Identity(),
            1.0,
            2.1,
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
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();
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

        const State endState = propagator.calculateStateAt(state, endInstant, condition);

        EXPECT_TRUE(endState.accessInstant() < endInstant);
        EXPECT_LT((endState.accessInstant() - condition.getInstant()).inSeconds(), 1e-7);

        const InstantCondition failureCondition = {
            InstantCondition::Criterion::StrictlyPositive,
            state.accessInstant() + Duration::Seconds(7000.0),
        };

        EXPECT_ANY_THROW(propagator.calculateStateAt(state, endInstant, failureCondition));
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
}

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
    actualCoordinates.segment(0, 3) = finalState.extractCoordinates(CartesianPosition::Default());
    actualCoordinates.segment(3, 3) = finalState.extractCoordinates(CartesianVelocity::Default());

    const VectorXd residuals = actualCoordinates - expectedCoordinates;

    EXPECT_TRUE((residuals.array() < 1e-7).all()) << String::Format("Residual: {}", residuals.maxCoeff());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, OrekitExponential)
{
    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 7546.053290, 0.0}, gcrfSPtr_),
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
        std::make_shared<AtmosphericDrag>(earthSPtr, satelliteSystem_),
    };
    const Propagator propagator = {defaultNumericalSolver_, dynamics};

    const State finalState = propagator.calculateStateAt(state, state.getInstant() + Duration::Seconds(60.0 * 60.0));

    VectorXd expectedCoordinates(6);
    expectedCoordinates << -5172889.585695211000, -4716058.453899897000, 0000000.000008572841, 05083.947538266920,
        -05576.414764013522, -00000.000000010687;

    VectorXd actualCoordinates(6);
    actualCoordinates.segment(0, 3) = finalState.extractCoordinates(CartesianPosition::Default());
    actualCoordinates.segment(3, 3) = finalState.extractCoordinates(CartesianVelocity::Default());

    const VectorXd residuals = actualCoordinates - expectedCoordinates;

    ASSERT_TRUE((residuals.array() < 1e-6).all()) << String::Format("Residual: {}", residuals.maxCoeff());
}

// /* VALIDATION TESTS */
// /* Force model validation tests */
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_Drag_Constant_Exponential_500km)
{
    // Earth with Exponential atmospheric drag compared against OREKit
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2023-01-01 00:00:00.000"), Scale::UTC);

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
            std::make_shared<AtmosphericDrag>(earthSPtr, satelliteSystem_),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

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
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_Drag_Constant_Exponential_320km)
{
    // Earth with Exponential atmospheric drag compared against OREKit
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2023-01-01 00:00:00.000"), Scale::UTC);

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
            std::make_shared<AtmosphericDrag>(earthSPtr, satelliteSystem_),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

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
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_Drag_Constant_NRLMSISE_Orekit_450km
)
{
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2023-01-01 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                   "Orekit_Drag_base_plus_drag_test.csv")),
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
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::NRLMSISE00)
        );
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSPtr),
            std::make_shared<AtmosphericDrag>(earthSPtr, satelliteSystem_),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

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

            ASSERT_GT(9e-4, positionErrorGCRF);
            ASSERT_GT(9e-7, velocityErrorGCRF);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Default)
{
    {
        EXPECT_NO_THROW(Propagator::Default());
        EXPECT_TRUE(Propagator::Default().isDefined());
    }

    {
        Environment environment = Environment::Default();
        EXPECT_NO_THROW(Propagator::Default(environment));
        EXPECT_NO_THROW(Propagator::Default(environment, satelliteSystem_));
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
        const Propagator propagator =
            Propagator::FromEnvironment(defaultNumericalSolver_, environment, satelliteSystem_);

        EXPECT_TRUE(propagator.getDynamics().getSize() == 5);

        EXPECT_TRUE(static_cast<const CentralBodyGravity&>(*propagator.getDynamics()[0]).isDefined());
        EXPECT_TRUE(static_cast<const AtmosphericDrag&>(*propagator.getDynamics()[1]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[2]).isDefined());
        EXPECT_TRUE(static_cast<const ThirdBodyGravity&>(*propagator.getDynamics()[3]).isDefined());
        EXPECT_TRUE(static_cast<const PositionDerivative&>(*propagator.getDynamics()[4]).isDefined());
    }
}

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator_Data_Success
    : public OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator,
      public ::testing::WithParamInterface<Tuple<Instant, Instant, Duration, String>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Input_Data_Spans_Valid,
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator_Data_Success,
    testing::Values(std::make_tuple(
        Instant::Now() - Duration::Weeks(52 * 5),
        Instant::Now() - Duration::Weeks(52 * 4.9),
        Duration::Days(7),
        "NRLMSISE Past Data limit pass"
    )
                    // [TBI]: This test case fails sporadically because the actual data files are never updated based on
                    // age, meaning they can get stale.

                    // std::make_tuple(
                    //     Instant::Now() + Duration::Weeks(52 * 0.9),
                    //     Instant::Now() + Duration::Weeks(52 * 1.0),
                    //     Duration::Days(7),
                    //     "IERS Finals2000A Future Data limit pass"
                    // )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator_Data_Success, EdgeCases_Input_Data_Horizon)
{
    // long-horizon, low granularity propagation to test input data availability

    // currently limited by:
    // -5 years in the past due to CSSI space weather data when computing drag
    // +1 year in the future due to finals2000a prediction span when computing polar motion for frame conversions

    const auto parameters = GetParam();

    const Instant startInstant = std::get<0>(parameters);
    const Instant endInstant = std::get<1>(parameters);
    const Duration step = std::get<2>(parameters);

    const State state = {
        startInstant,
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
    };

    const NumericalSolver RK4 = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        step.inSeconds(),
        1.0e-15,
        1.0e-15,
    };

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM2008),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::NRLMSISE00)
    );

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr, satelliteSystem_),
    };

    // Setup Propagator model and orbit
    const Propagator propagator = {
        RK4,
        dynamics,
    };

    // Propagate all states
    EXPECT_NO_THROW(propagator.calculateStateAt(state, endInstant));
}

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator_Data_Failure
    : public OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator,
      public ::testing::WithParamInterface<Tuple<Instant, Instant, Duration, String>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Input_Data_Spans_Invalid,
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator_Data_Failure,
    testing::Values(
        std::make_tuple(
            Instant::Now() - Duration::Weeks(52 * 6.0),
            Instant::Now() - Duration::Weeks(52 * 5.9),
            Duration::Days(7),
            "NRLMSISE Past Data limit fail"
        ),
        std::make_tuple(
            Instant::Now() + Duration::Weeks(52 * 1.0),
            Instant::Now() + Duration::Weeks(52 * 1.1),
            Duration::Days(7),
            "IERS Finals2000A Future Data limit fail"
        )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator_Data_Failure, EdgeCases_Input_Data_Horizon)
{
    // long-horizon, low granularity propagation to test input data availability

    // currently limited by:
    // -5 years in the past due to CSSI space weather data when computing drag
    // +1 year in the future due to finals2000a prediction span when computing polar motion for frame conversions

    auto parameters = GetParam();

    Instant startInstant = std::get<0>(parameters);
    Instant endInstant = std::get<1>(parameters);
    Duration step = std::get<2>(parameters);

    const State state = {
        startInstant,
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
    };

    const NumericalSolver RK4 = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        step.inSeconds(),
        1.0e-15,
        1.0e-15,
    };

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM2008),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::NRLMSISE00)
    );

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr, satelliteSystem_),
    };

    // Setup Propagator model and orbit
    const Propagator propagator = {RK4, dynamics};

    // Propagate all states
    EXPECT_THROW(propagator.calculateStateAt(state, endInstant), ostk::core::error::RuntimeError);
}
