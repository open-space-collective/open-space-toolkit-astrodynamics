/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

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

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::fs::File;
using ostk::core::fs::Path;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::env::Object;
using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using ostk::physics::units::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::ThirdBodyGravity;
using ostk::astro::trajectory::Orbit;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::orbit::models::Propagated;

/* UNIT TESTS */
class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
        defaultDynamics_.add(std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthSpherical_)));

        this->defaultPosition_ = Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_);
        this->defaultVelocity_ = Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_);

        this->defaultState_ = {this->defaultInstant_, this->defaultPosition_, this->defaultVelocity_};

        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        this->satelliteSystem_ = {Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 1.0, 2.1};
        propagator_ = Propagator(defaultNumericalSolver_, defaultDynamics_);
    }

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();
    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

    const Instant defaultInstant_ = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);

    Position defaultPosition_ = Position::Undefined();
    Velocity defaultVelocity_ = Velocity::Undefined();
    State defaultState_ = State::Undefined();

    Shared<Celestial> earthSpherical_ = nullptr;
    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();

    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Propagator propagator_ = Propagator::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Constructor)
{
    // State array setup
    Array<State> stateArray = Array<State>::Empty();
    stateArray.add(defaultState_);
    const Instant instant_1 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
    const State state_1 = {
        instant_1, Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_), Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr_)};
    stateArray.add(state_1);

    // Constructor without putting into orbit object
    {
        // Setup Propagated model
        EXPECT_NO_THROW(Propagated(propagator_, defaultState_));
        EXPECT_NO_THROW(Propagated(propagator_, stateArray));
    }

    // Constructor with putting into orbit object
    {
        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, defaultState_};
        const Propagated propagatedModel_0 = {propagator_, stateArray};

        // Setup Orbit model
        EXPECT_NO_THROW(Orbit(propagatedModel, earthSpherical_));
        EXPECT_NO_THROW(Orbit(propagatedModel_0, earthSpherical_));
    }

    // Constructor with empty cachedStateArray
    {
        EXPECT_THROW(
            {
                try
                {
                    const Propagated propagatedModel = Propagated(propagator_, Array<State>::Empty());
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(e.what(), String("{Propagated} is undefined."));
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, EqualToOperator)
{
    {
        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, defaultState_};
        const Propagated propagatedModel_x = {propagatedModel};
        EXPECT_TRUE(propagatedModel == propagatedModel_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        const Propagated propagatedModel_1 = {{numericalSolver_1, defaultDynamics_}, defaultState_};
        EXPECT_FALSE(propagatedModel == propagatedModel_1);

        // State array setup, state Array construction with array of length one and same start
        Array<State> stateArray = Array<State>::Empty();
        stateArray.add(defaultState_);
        const Propagated propagatedModel_2 = {propagator_, stateArray};
        EXPECT_TRUE(propagatedModel == propagatedModel_2);

        // State array setup, state Array construction with array of length two and a different second state
        const Instant instant_0 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
        const State state_0 = {
            instant_0,
            Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr_)};
        stateArray.add(state_0);
        const Propagated propagatedModel_3 = {propagator_, stateArray};
        EXPECT_FALSE(propagatedModel == propagatedModel_3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, NotEqualToOperator)
{
    {
        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, defaultState_};
        const Propagated propagatedModel_x = {propagatedModel};
        EXPECT_FALSE(propagatedModel != propagatedModel_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        const Propagated propagatedModel_1 = {{numericalSolver_1, defaultDynamics_}, defaultState_};
        EXPECT_TRUE(propagatedModel != propagatedModel_1);

        // State array setup, state Array construction with array of length one and same start
        Array<State> stateArray = Array<State>::Empty();
        stateArray.add(defaultState_);
        const Propagated propagatedModel_2 = {propagator_, stateArray};
        EXPECT_FALSE(propagatedModel != propagatedModel_2);

        // State array setup, state Array construction with array of length two and a different second state
        const Instant instant_0 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
        const State state_0 = {
            instant_0,
            Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr_)};
        stateArray.add(state_0);
        const Propagated propagatedModel_3 = {propagator_, stateArray};
        EXPECT_TRUE(propagatedModel != propagatedModel_3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, IsDefined)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        EXPECT_TRUE(propagatedModel.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, StreamOperator)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << propagatedModel << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Print)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(propagatedModel.print(std::cout, true));
        EXPECT_NO_THROW(propagatedModel.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getEpoch)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        EXPECT_EQ(propagatedModel.getEpoch(), defaultInstant_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getRevolutionNumberAtEpoch)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        EXPECT_EQ(propagatedModel.getRevolutionNumberAtEpoch(), 1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateStateAt)
{
    // Setup Propagated model and orbit
    const Propagated propagatedModel = {propagator_, defaultState_};

    const Orbit orbit = {propagatedModel, earthSpherical_};

    // Setup instants
    const Array<Instant> instantArray = {
        Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC),
        defaultInstant_,
        Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC)};

    // Test CalculateStateAt in propagatedModel and in orbit
    {
        testing::internal::CaptureStdout();

        for (const Instant& instant : instantArray)
        {
            EXPECT_EQ(orbit.getStateAt(instant), propagatedModel.calculateStateAt(instant));
        }

        EXPECT_TRUE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateStatesAt)
{
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

        referencePositionArray.add(
            {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()}
        );
        referenceVelocityArray.add(
            {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()}
        );
    }

    // Test cachedStateArray with one cachedState at the beginning of reference states
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 0;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        // Setup Propagated model and perform propagation
        const Propagated propagatedModel = {propagator_, state};

        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

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

    // Test cachedStateArray with one cachedState at the middle of reference states
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 30;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        // Setup Propagated model and perform propagation
        const Propagated propagatedModel = {propagator_, state};
        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

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

    // Test cachedStateArray with one cachedState at the end of reference states
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 60;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        // Setup Propagated model and perform propagation
        const Propagated propagatedModel = {propagator_, state};
        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

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

    // Test cachedStateArray with two cachedStates, one at the beginning and one at the end of reference states
    {
        // Choose states to put into cachedStateArray
        Array<State> cachedStateArray = Array<State>::Empty();
        size_t cachedStateReferenceIndex_1 = 0;
        const State state_1 = {
            instantArray[cachedStateReferenceIndex_1],
            Position::Meters({referencePositionArray[cachedStateReferenceIndex_1]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex_1]}, gcrfSPtr_)};
        cachedStateArray.add(state_1);
        size_t cachedStateReferenceIndex_2 = 60;
        const State state_2 = {
            instantArray[cachedStateReferenceIndex_2],
            Position::Meters({referencePositionArray[cachedStateReferenceIndex_2]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex_2]}, gcrfSPtr_)};
        cachedStateArray.add(state_2);

        // Setup Propagated model and perform propagation
        const Propagated propagatedModel = {propagator_, cachedStateArray};
        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

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

    // Test cachedStateArray with half of reference states already in cachedStateArray
    {
        // Choose states to put into cachedStateArray
        Array<State> cachedStateArray = Array<State>::Empty();
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            if (i % 2 == 0)
            {
                const State state = {
                    instantArray[i],
                    Position::Meters({referencePositionArray[i]}, gcrfSPtr_),
                    Velocity::MetersPerSecond({referenceVelocityArray[i]}, gcrfSPtr_)};
                cachedStateArray.add(state);
            }
        }

        // Setup Propagated model and perform propagation
        const Propagated propagatedModel = {propagator_, cachedStateArray};
        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

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

    // Test cachedStateArray with all reference states already in cachedStateArray (no propagation should be needed)
    {
        // Choose states to put into cachedStateArray
        Array<State> cachedStateArray = Array<State>::Empty();
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            const State state = {
                instantArray[i],
                Position::Meters({referencePositionArray[i]}, gcrfSPtr_),
                Velocity::MetersPerSecond({referenceVelocityArray[i]}, gcrfSPtr_)};
            cachedStateArray.add(state);
        }

        // Setup Propagated model and perform propagation
        const Propagated propagatedModel = {propagator_, cachedStateArray};
        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

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

    // Test cachedStateArray with two duplicate States
    {
        // Choose state to put into cachedStateArray
        Array<State> cachedStateArray = Array<State>::Empty();
        size_t cachedStateReferenceIndex = 0;
        const State state = {
            instantArray[cachedStateReferenceIndex],
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};
        cachedStateArray.add({state, state});

        // Setup Propagated model and perform propagation
        const Propagated propagatedModel = {propagator_, cachedStateArray};

        ASSERT_EQ(propagatedModel.accessCachedStateArray().getSize(), 1);
        ASSERT_EQ(propagatedModel.accessCachedStateArray()[0], state);

        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

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

    // Test cachedStateArray with two cachedStates, unsorted
    {
        // Choose states to put into cachedStateArray
        Array<State> cachedStateArray = Array<State>::Empty();
        size_t cachedStateReferenceIndex_1 = 0;
        const State state_1 = {
            instantArray[cachedStateReferenceIndex_1],
            Position::Meters({referencePositionArray[cachedStateReferenceIndex_1]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex_1]}, gcrfSPtr_)};
        size_t cachedStateReferenceIndex_2 = 60;
        const State state_2 = {
            instantArray[cachedStateReferenceIndex_2],
            Position::Meters({referencePositionArray[cachedStateReferenceIndex_2]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex_2]}, gcrfSPtr_)};
        cachedStateArray.add(state_2);
        cachedStateArray.add(state_1);

        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[0].getInstant());

        const Propagated propagatedModel = {propagator_, cachedStateArray};

        EXPECT_LT(
            propagatedModel.accessCachedStateArray()[0].getInstant(),
            propagatedModel.accessCachedStateArray()[1].getInstant()
        );

        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].accessPosition();
            const Velocity velocity = propagatedStateArray[i].accessVelocity();

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateRevolutionNumberAt)
{
    // Satellite system setup

    // Test basic positive and negative revolution numbers
    {
        // Setup Propagated model and orbit
        const Propagated propagatedModel = {propagator_, defaultState_};

        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Calculate revolution number at instant
        const Instant instant = defaultInstant_;
        const Instant instant_before1 = Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC);
        const Instant instant_after1 = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);
        const Instant instant_after2 = Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC);

        // Check revolution numbers for propagated model
        EXPECT_EQ(1, propagatedModel.calculateRevolutionNumberAt(instant));
        EXPECT_EQ(-2, propagatedModel.calculateRevolutionNumberAt(instant_before1));
        EXPECT_EQ(1, propagatedModel.calculateRevolutionNumberAt(instant_after1));
        EXPECT_EQ(2, propagatedModel.calculateRevolutionNumberAt(instant_after2));

        // Check revolution numbers for orbit
        EXPECT_EQ(1, orbit.getRevolutionNumberAt(instant));
        EXPECT_EQ(-2, orbit.getRevolutionNumberAt(instant_before1));
        EXPECT_EQ(1, orbit.getRevolutionNumberAt(instant_after1));
        EXPECT_EQ(2, orbit.getRevolutionNumberAt(instant_after2));
    }

    // Test accuracy of calculateRevolutionNumber at by checking that it returns results accurate to the force model it
    // is using
    {
        const Shared<Celestial> earthEGM2008_70 = std::make_shared<Celestial>(Earth::EGM2008(70, 70));
        // Create environment
        const Shared<Dynamics> EGM2008Dynamics =
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM2008_70));

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2021, 1, 2, 0, 0, 0), Scale::UTC);

        const State state = {startInstant, defaultPosition_, defaultVelocity_};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_twobody = {propagator_, state};
        const Propagated propagatedModel_fullgrav = {{defaultNumericalSolver_, {EGM2008Dynamics}}, state};

        // Calculate gravitational parameter
        const Derived gravitationalParameter = EarthGravitationalModel::Spherical.gravitationalParameter_;
        const Derived::Unit GravitationalParameterSIUnit =
            Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);
        const Real gravitationalParameter_SI = gravitationalParameter.in(GravitationalParameterSIUnit);

        // Get position and velocity coordinates from starting state
        Position currentPosition = state.getPosition();
        Velocity currentVelocity = state.getVelocity();
        Vector3d currentPositionCoordinates = currentPosition.inUnit(Position::Unit::Meter).accessCoordinates();
        Vector3d currentVelocityCoordinates =
            currentVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

        // Calculate orbital period
        const double semiMajorAxis =
            -gravitationalParameter_SI * currentPositionCoordinates.norm() /
            (currentPositionCoordinates.norm() * std::pow(currentVelocityCoordinates.norm(), 2) -
             2.0 * gravitationalParameter_SI);
        const Duration currentOrbitalPeriod =
            Duration::Seconds(Real::TwoPi() * std::sqrt(std::pow(semiMajorAxis, 3) / gravitationalParameter_SI));

        // Calculate revolution number at instant
        const int propagationRevolutions = 5;
        const Instant endInstant = startInstant - currentOrbitalPeriod * propagationRevolutions;

        const Integer revolutionNumber_twobody = propagatedModel_twobody.calculateRevolutionNumberAt(endInstant);
        const Integer revolutionNumber_fullgrav = propagatedModel_fullgrav.calculateRevolutionNumberAt(endInstant);

        // Check that revolution numbers for two body are exact, for full grav are 1 higher due to losses (which are
        // gains in negative time), and that they are not equal
        EXPECT_EQ(-propagationRevolutions - 1, revolutionNumber_twobody);
        EXPECT_EQ(-propagationRevolutions, revolutionNumber_fullgrav);

        EXPECT_FALSE(revolutionNumber_twobody == revolutionNumber_fullgrav);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, AccessCachedStateArray)
{
    // Test cachedStateArray sorting during construction and accessing (not during propagation)
    {
        // Test valid and chronological state array input

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_};
        stateArray.add(state_1);
        const State state_2 = {defaultInstant_, defaultPosition_, defaultVelocity_};
        stateArray.add(state_2);
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_};
        stateArray.add(state_3);

        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, stateArray};

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }

    {
        // Test sorting of non chronological but valid state array

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_};
        stateArray.add(state_3);
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_};
        stateArray.add(state_1);
        const State state_2 = {defaultInstant_, defaultPosition_, defaultVelocity_};
        stateArray.add(state_2);

        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, stateArray};

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, SetCachedStateArray)
{
    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime(2021, 1, 2, 0, 0, 0), Scale::UTC);
    const State state = {startInstant, defaultPosition_, defaultVelocity_};

    // Propagated model setup
    Propagated propagatedModel = {propagator_, state};

    // Test cachedStateArray sorting during construction and accessing
    {
        // Test valid and chronological state array input

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_};
        stateArray.add(state_1);
        const State state_2 = {defaultInstant_, defaultPosition_, defaultVelocity_};
        stateArray.add(state_2);
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_};
        stateArray.add(state_3);

        // Manually set cachedStateArray
        propagatedModel.setCachedStateArray(stateArray);

        // Check propagatedModel is properly defined
        EXPECT_TRUE(propagatedModel.isDefined());

        // Access cachedStateArray and verify
        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }

    {
        // Test sorting of non chronological + duplicate but valid state array

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_};
        stateArray.add(state_3);
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_};
        stateArray.add(state_1);
        const State state_2 = {defaultInstant_, defaultPosition_, defaultVelocity_};
        stateArray.add(state_2);
        stateArray.add(state_2);

        // Manually set cachedStateArray
        propagatedModel.setCachedStateArray(stateArray);

        // Check propagatedModel is properly defined
        EXPECT_TRUE(propagatedModel.isDefined());

        // Access cachedStateArray and verify
        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }

    {
        // Test setting empty cachedStateArray

        EXPECT_THROW(
            {
                try
                {
                    propagatedModel.setCachedStateArray(Array<State>::Empty());
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(e.what(), String("{Propagated} is undefined."));
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

/* VALIDATION TESTS */
/* Force model validation tests */
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody)
{
    const Shared<const Frame> itrfSPtr = Frame::ITRF();

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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {propagator_, state};
        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-3, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referencePositionArray_ITRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_ITRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC));

            referencePositionArray_GCRF.add(
                {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()}
            );
            referenceVelocityArray_GCRF.add(
                {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()}
            );

            referencePositionArray_ITRF.add(
                {referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()}
            );
            referenceVelocityArray_ITRF.add(
                {referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()}
            );
        }

        // Current state and instant setup
        const Instant startInstant = instantArray[0];

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {propagator_, state};
        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-7, positionError_GCRF);
            ASSERT_GT(2e-10, velocityError_GCRF);

            // ITRF Compare
            const Position position_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).accessPosition();
            const Velocity velocity_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).accessVelocity();

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
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "ITRF Position error is: " << positionError_ITRF << "m" << std::endl;
            // std::cout << "ITRF Velocity error is: " << velocityError_ITRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_EGM2008)
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
    Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

        referencePositionArray_GCRF.add(
            1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
        );
        referenceVelocityArray_GCRF.add(
            1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
        );
    }

    // EGM2008 100x100 perturbation only vs STK EGM2008
    {
        const Shared<Celestial> earthEGM2008_100 = std::make_shared<Celestial>(Earth::EGM2008(100, 100));
        Array<Shared<Dynamics>> EGM2008Dynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM2008_100))};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {{defaultNumericalSolver_, {EGM2008Dynamics}}, state};
        const Orbit orbit = {propagatedModel, earthEGM2008_100};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(1.5e-1, positionError_GCRF);
            ASSERT_GT(1.5e-4, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM2008 360x360, 100x100, 70x70, 45x45 perturbation only self comparison
    {
        const Shared<Celestial> earthEGM2008_360 = std::make_shared<Celestial>(Earth::EGM2008(360, 360));
        const Shared<Celestial> earthEGM2008_100 = std::make_shared<Celestial>(Earth::EGM2008(100, 100));
        const Shared<Celestial> earthEGM2008_70 = std::make_shared<Celestial>(Earth::EGM2008(70, 70));
        const Shared<Celestial> earthEGM2008_45 = std::make_shared<Celestial>(Earth::EGM2008(45, 45));
        // Create environment
        const Array<Shared<Dynamics>> EGM2008Dynamics_360 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM2008_360))};
        const Array<Shared<Dynamics>> EGM2008Dynamics_100 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM2008_100))};
        const Array<Shared<Dynamics>> EGM2008Dynamics_70 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM2008_70))};
        const Array<Shared<Dynamics>> EGM2008Dynamics_45 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM2008_45))};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_360 = {{defaultNumericalSolver_, EGM2008Dynamics_360}, state};
        const Propagated propagatedModel_100 = {{defaultNumericalSolver_, EGM2008Dynamics_100}, state};
        const Propagated propagatedModel_70 = {{defaultNumericalSolver_, EGM2008Dynamics_70}, state};
        const Propagated propagatedModel_45 = {{defaultNumericalSolver_, EGM2008Dynamics_45}, state};

        const Orbit orbit_360 = {propagatedModel_360, earthEGM2008_360};
        const Orbit orbit_100 = {propagatedModel_100, earthEGM2008_100};
        const Orbit orbit_70 = {propagatedModel_70, earthEGM2008_70};
        const Orbit orbit_45 = {propagatedModel_45, earthEGM2008_45};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = orbit_360.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_100 = orbit_100.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_70 = orbit_70.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_45 = orbit_45.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF_360 = propagatedStateArray_360[i].accessPosition();
            const Velocity velocity_GCRF_360 = propagatedStateArray_360[i].accessVelocity();
            const Position position_GCRF_100 = propagatedStateArray_100[i].accessPosition();
            const Velocity velocity_GCRF_100 = propagatedStateArray_100[i].accessVelocity();
            const Position position_GCRF_70 = propagatedStateArray_70[i].accessPosition();
            const Velocity velocity_GCRF_70 = propagatedStateArray_70[i].accessVelocity();
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition();
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity();

            // 360 vs 100
            const double positionError_GCRF_360_100 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_100.accessCoordinates()).norm();
            const double velocityError_GCRF_360_100 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_100.accessCoordinates()).norm();
            ASSERT_GT(2e-3, positionError_GCRF_360_100);
            ASSERT_GT(2e-6, velocityError_GCRF_360_100);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 100 Position error is: " << positionError_GCRF_360_100 << "m" << std::endl;
            // std::cout << "360 vs 100 Velocity error is: " << velocityError_GCRF_360_100 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 70
            const double positionError_GCRF_360_70 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_70.accessCoordinates()).norm();
            const double velocityError_GCRF_360_70 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_70.accessCoordinates()).norm();
            ASSERT_GT(5e-2, positionError_GCRF_360_70);
            ASSERT_GT(5e-5, velocityError_GCRF_360_70);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 70 Position error is: " << positionError_GCRF_360_70 << "m" << std::endl;
            // std::cout << "360 vs 70 Velocity error is: " << velocityError_GCRF_360_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionError_GCRF_360_45 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm();
            const double velocityError_GCRF_360_45 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm();
            ASSERT_GT(5e-1, positionError_GCRF_360_45);
            ASSERT_GT(5e-4, velocityError_GCRF_360_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 45 Position error is: " << positionError_GCRF_360_45 << "m" << std::endl;
            // std::cout << "360 vs 45 Velocity error is: " << velocityError_GCRF_360_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_EGM96)
{
    // EGM96 360x360 perturbation only vs GMAT
    {
        const Shared<Celestial> earthEGM96_360 = std::make_shared<Celestial>(Earth::EGM96(360, 360));
        Array<Shared<Dynamics>> EGM96Dynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM96_360))};

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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {{defaultNumericalSolver_, EGM96Dynamics}, state};
        const Orbit orbit = {propagatedModel, earthEGM96_360};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(9e-2, positionError_GCRF);
            ASSERT_GT(9e-5, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM96 70x70 perturbation only vs STK WGS84EGM96
    {
        const Shared<Celestial> earthEGM96_70 = std::make_shared<Celestial>(Earth::EGM96(70, 70));
        Array<Shared<Dynamics>> EGM96Dynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM96_70))};

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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {{defaultNumericalSolver_, {EGM96Dynamics}}, state};
        const Orbit orbit = {propagatedModel, earthEGM96_70};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(1.5e-4, positionError_GCRF);
            ASSERT_GT(1.5e-7, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        const Shared<Celestial> earthEGM96_360 = std::make_shared<Celestial>(Earth::EGM96(360, 360));
        const Shared<Celestial> earthEGM96_180 = std::make_shared<Celestial>(Earth::EGM96(180, 180));
        const Shared<Celestial> earthEGM96_90 = std::make_shared<Celestial>(Earth::EGM96(90, 90));
        const Shared<Celestial> earthEGM96_45 = std::make_shared<Celestial>(Earth::EGM96(45, 45));

        // Create environment
        const Array<Shared<Dynamics>> EGM96Dynamics_360 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM96_360))};
        const Array<Shared<Dynamics>> EGM96Dynamics_180 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM96_180))};
        const Array<Shared<Dynamics>> EGM96Dynamics_90 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM96_90))};
        const Array<Shared<Dynamics>> EGM96Dynamics_45 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM96_45))};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_360 = {{defaultNumericalSolver_, EGM96Dynamics_360}, state};
        const Propagated propagatedModel_180 = {{defaultNumericalSolver_, EGM96Dynamics_180}, state};
        const Propagated propagatedModel_90 = {{defaultNumericalSolver_, EGM96Dynamics_90}, state};
        const Propagated propagatedModel_45 = {{defaultNumericalSolver_, EGM96Dynamics_45}, state};

        const Orbit orbit_360 = {propagatedModel_360, earthEGM96_360};
        const Orbit orbit_180 = {propagatedModel_180, earthEGM96_180};
        const Orbit orbit_90 = {propagatedModel_90, earthEGM96_90};
        const Orbit orbit_45 = {propagatedModel_45, earthEGM96_45};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = orbit_360.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_180 = orbit_180.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_90 = orbit_90.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_45 = orbit_45.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF_360 = propagatedStateArray_360[i].accessPosition();
            const Velocity velocity_GCRF_360 = propagatedStateArray_360[i].accessVelocity();
            const Position position_GCRF_180 = propagatedStateArray_180[i].accessPosition();
            const Velocity velocity_GCRF_180 = propagatedStateArray_180[i].accessVelocity();
            const Position position_GCRF_90 = propagatedStateArray_90[i].accessPosition();
            const Velocity velocity_GCRF_90 = propagatedStateArray_90[i].accessVelocity();
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition();
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity();

            // 360 vs 180
            const double positionError_GCRF_360_180 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_180.accessCoordinates()).norm();
            const double velocityError_GCRF_360_180 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_180.accessCoordinates()).norm();
            ASSERT_GT(9e-7, positionError_GCRF_360_180);
            ASSERT_GT(9e-10, velocityError_GCRF_360_180);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 180 Position error is: " << positionError_GCRF_360_180 << "m" << std::endl;
            // std::cout << "360 vs 180 Velocity error is: " << velocityError_GCRF_360_180 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 90
            const double positionError_GCRF_360_90 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_90.accessCoordinates()).norm();
            const double velocityError_GCRF_360_90 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_90.accessCoordinates()).norm();
            ASSERT_GT(7e-3, positionError_GCRF_360_90);
            ASSERT_GT(7e-6, velocityError_GCRF_360_90);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 90 Position error is: " << positionError_GCRF_360_90 << "m" << std::endl;
            // std::cout << "360 vs 90 Velocity error is: " << velocityError_GCRF_360_90 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionError_GCRF_360_45 =
                (position_GCRF_360.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm();
            const double velocityError_GCRF_360_45 =
                (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm();
            ASSERT_GT(5e-1, positionError_GCRF_360_45);
            ASSERT_GT(5e-4, velocityError_GCRF_360_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 45 Position error is: " << positionError_GCRF_360_45 << "m" << std::endl;
            // std::cout << "360 vs 45 Velocity error is: " << velocityError_GCRF_360_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_EGM84)
{
    // EGM84 70x70 perturbation only vs STK EGM84
    {
        const Shared<Celestial> earthEGM84_70 = std::make_shared<Celestial>(Earth::EGM84(70, 70));
        const Array<Shared<Dynamics>> EGM84Dynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM84_70))};

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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {{defaultNumericalSolver_, EGM84Dynamics}, state};
        const Orbit orbit = {propagatedModel, earthEGM84_70};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(7e-5, positionError_GCRF);
            ASSERT_GT(7e-8, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        const Shared<Celestial> earthEGM84_180 = std::make_shared<Celestial>(Earth::EGM84(180, 180));
        const Shared<Celestial> earthEGM84_70 = std::make_shared<Celestial>(Earth::EGM84(70, 70));
        const Shared<Celestial> earthEGM84_45 = std::make_shared<Celestial>(Earth::EGM84(45, 45));

        const Array<Shared<Dynamics>> EGM84Dynamics_180 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM84_180))};
        const Array<Shared<Dynamics>> EGM84Dynamics_70 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM84_70))};
        const Array<Shared<Dynamics>> EGM84Dynamics_45 = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM84_45))};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_180 = {{defaultNumericalSolver_, EGM84Dynamics_180}, state};
        const Propagated propagatedModel_70 = {{defaultNumericalSolver_, EGM84Dynamics_70}, state};
        const Propagated propagatedModel_45 = {{defaultNumericalSolver_, EGM84Dynamics_45}, state};

        const Orbit orbit_180 = {propagatedModel_180, earthEGM84_180};
        const Orbit orbit_70 = {propagatedModel_70, earthEGM84_70};
        const Orbit orbit_45 = {propagatedModel_45, earthEGM84_45};

        // Propagate all states
        const Array<State> propagatedStateArray_180 = orbit_180.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_70 = orbit_70.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_45 = orbit_45.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF_180 = propagatedStateArray_180[i].accessPosition();
            const Velocity velocity_GCRF_180 = propagatedStateArray_180[i].accessVelocity();
            const Position position_GCRF_70 = propagatedStateArray_70[i].accessPosition();
            const Velocity velocity_GCRF_70 = propagatedStateArray_70[i].accessVelocity();
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition();
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity();

            // 180 vs 70
            const double positionError_GCRF_180_70 =
                (position_GCRF_180.accessCoordinates() - position_GCRF_70.accessCoordinates()).norm();
            const double velocityError_GCRF_180_70 =
                (velocity_GCRF_180.accessCoordinates() - velocity_GCRF_70.accessCoordinates()).norm();
            ASSERT_GT(6e-2, positionError_GCRF_180_70);
            ASSERT_GT(6e-5, velocityError_GCRF_180_70);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 70 Position error is: " << positionError_GCRF_180_70 << "m" << std::endl;
            // std::cout << "180 vs 70 Velocity error is: " << velocityError_GCRF_180_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 180 vs 45
            const double positionError_GCRF_180_45 =
                (position_GCRF_180.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm();
            const double velocityError_GCRF_180_45 =
                (velocity_GCRF_180.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm();
            ASSERT_GT(3e-1, positionError_GCRF_180_45);
            ASSERT_GT(3e-4, velocityError_GCRF_180_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 45 Position error is: " << positionError_GCRF_180_45 << "m" << std::endl;
            // std::cout << "180 vs 45 Velocity error is: " << velocityError_GCRF_180_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_Sun_Moon)
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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> moonSpherical_ = std::make_shared<Celestial>(Moon::Spherical());
        const Shared<Celestial> sunSpherical_ = std::make_shared<Celestial>(Sun::Spherical());
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<CentralBodyGravity>(earthSpherical_),
            std::make_shared<ThirdBodyGravity>(moonSpherical_),
            std::make_shared<ThirdBodyGravity>(sunSpherical_)};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {{defaultNumericalSolver_, dynamics}, state};
        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-3, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_Sun)
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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> sunSpherical_ = std::make_shared<Celestial>(Sun::Spherical());
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<CentralBodyGravity>(earthSpherical_), std::make_shared<ThirdBodyGravity>(sunSpherical_)};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {{defaultNumericalSolver_, dynamics}, state};
        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-3, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_Moon)
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
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> moonSpherical_ = std::make_shared<Celestial>(Moon::Spherical());
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<CentralBodyGravity>(earthSpherical_), std::make_shared<ThirdBodyGravity>(moonSpherical_)};
        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {{defaultNumericalSolver_, dynamics}, state};
        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*gcrfSPtr_, *position_GCRF.accessFrame());
            ASSERT_EQ(*gcrfSPtr_, *velocity_GCRF.accessFrame());

            ASSERT_GT(2e-3, positionError_GCRF);
            ASSERT_GT(2e-6, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated,
    PropAccuracy_ExtendedDuration
)  // These tests take a long time and are for local testing only (not to be run on
   // every push to git)
{
    GTEST_SKIP() << "Skipping: Manual test";
    // EGM96 perturbation
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_EGM96_2month_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArray_GCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArray_GCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, Frame::GCRF()),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, Frame::GCRF())};

        const Shared<Celestial> earthEGM96_360 = std::make_shared<Celestial>(Earth::EGM96(360, 360));
        const Array<Shared<Dynamics>> EGM96Dynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM96_360))};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {{defaultNumericalSolver_, EGM96Dynamics}, state};
        const Orbit orbit = {propagatedModel, earthEGM96_360};

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition();
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity();

            const double positionError_GCRF =
                (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm();
            const double velocityError_GCRF =
                (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1e+3, positionError_GCRF);
            ASSERT_GT(1e+0, velocityError_GCRF);

            // Results console output
            // std::cout << state_GCRF.getInstant() << std::endl ;
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

/* Propagation Interval validation test */
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_IntervalSelfComparison)
{
    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

    const size_t propagationSpanHours = 24;
    const size_t propagationIntervalSeconds = 30;

    Array<Instant> instantArray = Array<Instant>::Empty();
    for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds; i += propagationIntervalSeconds)
    {
        instantArray.add(startInstant + Duration::Seconds(i));
    }

    const State state = {startInstant, defaultPosition_, defaultVelocity_};

    // Test that calculateStatesAt returns the same answer if states are requested at short or long intervals
    {
        // Setup Propagated model and orbit
        const Propagated propagatedModel = {propagator_, state};

        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Propagate all states at short interval
        const Array<State> propagatedStateArray_short = orbit.getStatesAt(instantArray);

        // Propagate all states at long interval
        Array<Instant> instantArray_longInterval = Array<Instant>::Empty();
        const size_t indexIncrement = 10;
        for (size_t i = 0; i < instantArray.getSize(); i += indexIncrement)
        {
            instantArray_longInterval.add(instantArray[i]);
        }
        const Array<State> propagatedStateArray_long = orbit.getStatesAt(instantArray_longInterval);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Run once every X times
            if (i % indexIncrement == 0)
            {
                // GCRF Compare
                const Position position_short = propagatedStateArray_short[i].accessPosition();
                const Velocity velocity_short = propagatedStateArray_short[i].accessVelocity();
                const Position position_long = propagatedStateArray_long[i / indexIncrement].accessPosition();
                const Velocity velocity_long = propagatedStateArray_long[i / indexIncrement].accessVelocity();

                const double positionError_GCRF =
                    (position_short.accessCoordinates() - position_long.accessCoordinates()).norm();
                const double velocityError_GCRF =
                    (velocity_short.accessCoordinates() - velocity_long.accessCoordinates()).norm();

                ASSERT_EQ(*gcrfSPtr_, *position_short.accessFrame());
                ASSERT_EQ(*gcrfSPtr_, *velocity_short.accessFrame());
                ASSERT_EQ(*gcrfSPtr_, *position_long.accessFrame());
                ASSERT_EQ(*gcrfSPtr_, *velocity_long.accessFrame());

                ASSERT_GT(1.0e-7, positionError_GCRF);
                ASSERT_GT(1.0e-7, velocityError_GCRF);

                // Results console output

                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
                // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }
    }

    // Test that calculateStateAt returns almost the same answer if states are requested at short or long intervals
    {
        // Setup Propagated model and orbit
        const Propagated propagatedModel = {propagator_, state};

        const Orbit orbit = {propagatedModel, earthSpherical_};

        testing::internal::CaptureStdout();

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Propagated output generated
            const State state_short = orbit.getStateAt(instantArray[i]);

            // Run once every X times
            if (i % 10 == 0)
            {
                const State state_long = orbit.getStateAt(instantArray[i]);

                // const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;
                // std::cout << "Resulting state in test loop:" << state_GCRF << std::endl ;
                // std::cout << "Resulting cached state array in test loop:" << cachedStateArray << std::endl ;

                // GCRF Compare
                const Position position_short = state_short.accessPosition();
                const Velocity velocity_short = state_short.accessVelocity();
                const Position position_long = state_long.accessPosition();
                const Velocity velocity_long = state_long.accessVelocity();

                const double positionError_GCRF =
                    (position_short.accessCoordinates() - position_long.accessCoordinates()).norm();
                const double velocityError_GCRF =
                    (velocity_short.accessCoordinates() - velocity_long.accessCoordinates()).norm();

                ASSERT_GT(3e-8, positionError_GCRF);
                ASSERT_GT(3e-11, velocityError_GCRF);

                // Results console output

                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
                // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }

        EXPECT_TRUE(testing::internal::GetCapturedStdout().empty());
    }
}

/* Stepper validation test */
TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_StepperTypeSelfComparison
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
        const NumericalSolver numericalSolver_54 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        // Setup initial conditions
        const State state = {startInstant, defaultPosition_, defaultVelocity_};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_54 = {{numericalSolver_54, defaultDynamics_}, state};
        const Propagated propagatedModel_78 = {propagator_, state};

        const Orbit orbit_54 = {propagatedModel_54, earthSpherical_};
        const Orbit orbit_78 = {propagatedModel_78, earthSpherical_};

        // Propagate all states
        const Array<State> propagatedStateArray_54 = orbit_54.getStatesAt(instantArray);
        const Array<State> propagatedStateArray_78 = orbit_78.getStatesAt(instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position_54 = propagatedStateArray_54[i].accessPosition();
            const Velocity velocity_54 = propagatedStateArray_54[i].accessVelocity();
            const Position position_78 = propagatedStateArray_78[i].accessPosition();
            const Velocity velocity_78 = propagatedStateArray_78[i].accessVelocity();

            const double positionError_GCRF =
                (position_54.accessCoordinates() - position_78.accessCoordinates()).norm();
            const double velocityError_GCRF =
                (velocity_54.accessCoordinates() - velocity_78.accessCoordinates()).norm();

            ASSERT_GT(5e-7, positionError_GCRF);
            ASSERT_GT(5e-10, velocityError_GCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

/* Benmarking speed test */
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_SpeedTest)
{
    GTEST_SKIP() << "Skipping: Manual test";
    // Benchmark the performance of a particular configuration vs TwoBody
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24 * 7;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600); i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Create loop to run timing test multiple times in a row and average them
        const size_t timingTestSampleSize = 5;
        Array<double> timeArray_default = Array<double>::Empty();
        Array<double> timeArray_tested = Array<double>::Empty();

        const Shared<Celestial> earthEGM2008_70 = std::make_shared<Celestial>(Earth::EGM2008(70, 70));
        const Shared<Celestial> moonSpherical = std::make_shared<Celestial>(Moon::Spherical());
        const Shared<Celestial> sunSpherical = std::make_shared<Celestial>(Sun::Spherical());

        const Array<Shared<Dynamics>> testedDynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthEGM2008_70)),
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(moonSpherical)),
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(sunSpherical))};

        const Position position = Position::Meters({7000000.0, 0.0, 0.0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, Frame::GCRF());

        const Shared<Celestial> earthSpherical = std::make_shared<Celestial>(Earth::Spherical());
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<CentralBodyGravity>(CentralBodyGravity(earthSpherical))};

        for (size_t i = 0; i < timingTestSampleSize; i++)
        {
            // Setup initial conditions
            const State state = {startInstant, position, velocity};

            // Setup Propagated model and orbit
            const Propagated propagatedModel_default = {{defaultNumericalSolver_, dynamics}, state};
            const Propagated propagatedModel_tested = {{defaultNumericalSolver_, testedDynamics}, state};

            const Orbit orbit_default = {propagatedModel_default, earthSpherical};
            const Orbit orbit_tested = {propagatedModel_tested, earthEGM2008_70};

            // Propagate all default states
            auto startTimer_default = std::chrono::steady_clock::now();
            const Array<State> propagatedStateArray_default = orbit_default.getStatesAt(instantArray);
            auto endTimer_default = std::chrono::steady_clock::now();

            auto startTimer_tested = std::chrono::steady_clock::now();
            const Array<State> propagatedStateArray_tested = orbit_tested.getStatesAt(instantArray);
            auto endTimer_tested = std::chrono::steady_clock::now();

            std::chrono::duration<double> elapsed_seconds_default = endTimer_default - startTimer_default;
            timeArray_default.add(elapsed_seconds_default.count());

            std::chrono::duration<double> elapsed_seconds_tested = endTimer_tested - startTimer_tested;
            timeArray_tested.add(elapsed_seconds_tested.count());
        }

        // Remove first and last couple of elements due to timinig inconsitincies
        timeArray_default.erase(timeArray_default.begin(), timeArray_default.begin() + 2);
        timeArray_default.erase(timeArray_default.end() - 2, timeArray_default.end());

        timeArray_tested.erase(timeArray_tested.begin(), timeArray_tested.begin() + 2);
        timeArray_tested.erase(timeArray_tested.end() - 2, timeArray_tested.end());

        // const double timeAverage_default = std::accumulate(timeArray_default.begin(), timeArray_default.end(), 0.0) /
        // timeArray_default.getSize() ; std::cout << "timeAverage_default: " << timeAverage_default << std::endl ;
        // const double timeAverage_tested = std::accumulate(timeArray_tested.begin(), timeArray_tested.end(), 0.0) /
        // timeArray_tested.getSize() ; std::cout << "timeAverage_tested: " << timeAverage_tested << std::endl ;

        // std::cout << "Ratio of tested to default is: " << timeAverage_tested / timeAverage_default << std::endl ;
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropagationAtExactInstants)
{
    {
        Array<State> states = {
            {Instant::DateTime(DateTime::Parse("2023-01-30T18:30:00.184", DateTime::Format::ISO8601), Scale::UTC),
             Position::Meters({-5981016.371280898340, 2114677.607544674072, 2683244.080375305377}, gcrfSPtr_),
             Velocity::MetersPerSecond({3142.190382984703, 50.122115740761, 6934.877387038766}, gcrfSPtr_)},
            {Instant::DateTime(DateTime::Parse("2023-01-30T18:35:00.184", DateTime::Format::ISO8601), Scale::UTC),
             Position::Meters({-4730169.446157290600, 2014399.438757266616, 4579373.510992174037}, gcrfSPtr_),
             Velocity::MetersPerSecond({5120.144014790862, -712.422950530929, 5589.998515385998}, gcrfSPtr_)},
            {Instant::DateTime(DateTime::Parse("2023-01-30T18:40:00.184", DateTime::Format::ISO8601), Scale::UTC),
             Position::Meters({-2965004.866673604585, 1695085.379882899811, 5976267.045515080914}, gcrfSPtr_),
             Velocity::MetersPerSecond({6539.557709872774, -1396.675776577052, 3637.641862957897}, gcrfSPtr_)}};

        Propagated propagated(propagator_, states);

        State estimatedState = propagated.calculateStateAt(states[1].getInstant());

        ASSERT_EQ(estimatedState, states[1]);
    }
}
