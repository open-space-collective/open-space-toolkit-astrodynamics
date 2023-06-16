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

#include <OpenSpaceToolkit/Physics/Environment.hpp>
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

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>
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

using ostk::physics::Environment;
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

using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::dynamics::SatelliteDynamics;
using ostk::astro::trajectory::Orbit;
using ostk::astro::trajectory::State;

using ostk::astro::trajectory::orbit::models::Propagated;

/* UNIT TESTS */
class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        this->defaultEnvironment_ = Environment::Default();

        this->defaultState_ = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
    }

    State defaultState_ = State::Undefined();
    Environment defaultEnvironment_ = Environment::Undefined();
    NumericalSolver defaultnumericalSolver_ = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};
    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Constructor)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
    const SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

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
        EXPECT_NO_THROW(Propagated(satelliteDynamics, defaultnumericalSolver_, defaultState_));
        EXPECT_NO_THROW(Propagated(satelliteDynamics, defaultnumericalSolver_, stateArray));
    }

    // Constructor with putting into orbit object
    {
        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};
        const Propagated propagatedModel_0 = {satelliteDynamics, defaultnumericalSolver_, stateArray};

        // Setup Orbit model
        EXPECT_NO_THROW(Orbit(propagatedModel, defaultEnvironment_.accessCelestialObjectWithName("Earth")));
        EXPECT_NO_THROW(Orbit(propagatedModel_0, defaultEnvironment_.accessCelestialObjectWithName("Earth")));
    }

    // Constructor with empty cachedStateArray
    {
        EXPECT_THROW(
            {
                try
                {
                    const Propagated propagatedModel =
                        Propagated(satelliteDynamics, defaultnumericalSolver_, Array<State>::Empty());
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
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};
        const Propagated propagatedModel_x = {propagatedModel};
        EXPECT_TRUE(propagatedModel == propagatedModel_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        const Propagated propagatedModel_1 = {satelliteDynamics, numericalSolver_1, defaultState_};
        EXPECT_FALSE(propagatedModel == propagatedModel_1);

        // State array setup, state Array construction with array of length one and same start
        Array<State> stateArray = Array<State>::Empty();
        stateArray.add(defaultState_);
        const Propagated propagatedModel_2 = {satelliteDynamics, defaultnumericalSolver_, stateArray};
        EXPECT_TRUE(propagatedModel == propagatedModel_2);

        // State array setup, state Array construction with array of length two and a different second state
        const Instant instant_0 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
        const State state_0 = {
            instant_0,
            Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr_)};
        stateArray.add(state_0);
        const Propagated propagatedModel_3 = {satelliteDynamics, defaultnumericalSolver_, stateArray};
        EXPECT_FALSE(propagatedModel == propagatedModel_3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, NotEqualToOperator)
{
    {
        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};
        const Propagated propagatedModel_x = {propagatedModel};
        EXPECT_FALSE(propagatedModel != propagatedModel_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        const Propagated propagatedModel_1 = {satelliteDynamics, numericalSolver_1, defaultState_};
        EXPECT_TRUE(propagatedModel != propagatedModel_1);

        // State array setup, state Array construction with array of length one and same start
        Array<State> stateArray = Array<State>::Empty();
        stateArray.add(defaultState_);
        const Propagated propagatedModel_2 = {satelliteDynamics, defaultnumericalSolver_, stateArray};
        EXPECT_FALSE(propagatedModel != propagatedModel_2);

        // State array setup, state Array construction with array of length two and a different second state
        const Instant instant_0 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
        const State state_0 = {
            instant_0,
            Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr_)};
        stateArray.add(state_0);
        const Propagated propagatedModel_3 = {satelliteDynamics, defaultnumericalSolver_, stateArray};
        EXPECT_TRUE(propagatedModel != propagatedModel_3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, IsDefined)
{
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};

        EXPECT_TRUE(propagatedModel.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, StreamOperator)
{
    {
        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << propagatedModel << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Print)
{
    {
        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(propagatedModel.print(std::cout, true));
        EXPECT_NO_THROW(propagatedModel.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getEpoch)
{
    {
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);

        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};

        EXPECT_EQ(propagatedModel.getEpoch(), startInstant);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getRevolutionNumberAtEpoch)
{
    {
        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};

        EXPECT_EQ(propagatedModel.getRevolutionNumberAtEpoch(), 1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateStateAt)
{
    // Create environment
    const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

    const Environment customEnvironment = Environment(Instant::J2000(), objects);

    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

    // Setup Propagated model and orbit
    const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};

    const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

    // Setup instants
    const Array<Instant> instantArray = {
        Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
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
    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    // Drag coefficient to 0.0 to neglect atmospheric drag
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

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

    // Create environment
    const Instant instantJ2000 = Instant::J2000();
    const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

    const Environment customEnvironment = Environment(instantJ2000, objects);

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

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

        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};

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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, cachedStateArray};
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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, cachedStateArray};
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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, cachedStateArray};
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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, cachedStateArray};

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

        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, cachedStateArray};

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
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    // Drag coefficient to 0.0 to neglect atmospheric drag
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

    // Test basic positive and negative revolution numbers
    {
        // Create environment
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

        const Environment customEnvironment = Environment(Instant::J2000(), objects);

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, defaultState_};

        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

        // Calculate revolution number at instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);
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
        // Create environment
        const Array<Shared<Object>> objects_twobody = {std::make_shared<Earth>(Earth::Spherical())};
        const Array<Shared<Object>> objects_fullgrav = {std::make_shared<Earth>(Earth::EGM2008(70, 70))};

        const Environment customEnvironment_twobody = Environment(Instant::J2000(), objects_twobody);
        const Environment customEnvironment_fullgrav = Environment(Instant::J2000(), objects_fullgrav);

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2021, 1, 2, 0, 0, 0), Scale::UTC);

        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics_twobody = {customEnvironment_twobody, satelliteSystem};
        SatelliteDynamics satelliteDynamics_fullgrav = {customEnvironment_fullgrav, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_twobody = {satelliteDynamics_twobody, defaultnumericalSolver_, state};
        const Propagated propagatedModel_fullgrav = {satelliteDynamics_fullgrav, defaultnumericalSolver_, state};

        // Calculate gravitational parameter
        const Derived gravitationalParameter = Earth::Models::Spherical::GravitationalParameter;
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
    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

    // Test cachedStateArray sorting during construction and accessing (not during propagation)
    {
        // Test valid and chronological state array input

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_1);
        const State state_2 = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_2);
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_3);

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, stateArray};

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
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_3);
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_1);
        const State state_2 = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_2);

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, stateArray};

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, SetCachedStateArray)
{
    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));  // TBI: Add fixtures later
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime(2021, 1, 2, 0, 0, 0), Scale::UTC);
    const State state = {
        startInstant,
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

    // Propagated model setup
    Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};

    // Test cachedStateArray sorting during construction and accessing
    {
        // Test valid and chronological state array input

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_1);
        const State state_2 = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_2);
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
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
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_3);
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
        stateArray.add(state_1);
        const State state_2 = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};
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
    // Create environment
    const Instant instantJ2000 = Instant::J2000();
    const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

    const Environment customEnvironment = Environment(instantJ2000, objects);

    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    // Drag coefficient to 0.0 to neglect atmospheric drag
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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
    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

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
        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::EGM2008(100, 100))};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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
        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects_360 = {std::make_shared<Earth>(Earth::EGM2008(360, 360))};
        const Array<Shared<Object>> objects_100 = {std::make_shared<Earth>(Earth::EGM2008(100, 100))};
        const Array<Shared<Object>> objects_70 = {std::make_shared<Earth>(Earth::EGM2008(70, 70))};
        const Array<Shared<Object>> objects_45 = {std::make_shared<Earth>(Earth::EGM2008(45, 45))};

        const Environment customEnvironment_360 = Environment(instantJ2000, objects_360);
        const Environment customEnvironment_100 = Environment(instantJ2000, objects_100);
        const Environment customEnvironment_70 = Environment(instantJ2000, objects_70);
        const Environment customEnvironment_45 = Environment(instantJ2000, objects_45);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics_360 = {customEnvironment_360, satelliteSystem};
        SatelliteDynamics satelliteDynamics_100 = {customEnvironment_100, satelliteSystem};
        SatelliteDynamics satelliteDynamics_70 = {customEnvironment_70, satelliteSystem};
        SatelliteDynamics satelliteDynamics_45 = {customEnvironment_45, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_360 = {satelliteDynamics_360, defaultnumericalSolver_, state};
        const Propagated propagatedModel_100 = {satelliteDynamics_100, defaultnumericalSolver_, state};
        const Propagated propagatedModel_70 = {satelliteDynamics_70, defaultnumericalSolver_, state};
        const Propagated propagatedModel_45 = {satelliteDynamics_45, defaultnumericalSolver_, state};

        const Orbit orbit_360 = {propagatedModel_360, customEnvironment_360.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_100 = {propagatedModel_100, customEnvironment_100.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_70 = {propagatedModel_70, customEnvironment_70.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_45 = {propagatedModel_45, customEnvironment_45.accessCelestialObjectWithName("Earth")};

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
    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::EGM96(360, 360))};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::EGM96(70, 70))};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects_360 = {std::make_shared<Earth>(Earth::EGM96(360, 360))};
        const Array<Shared<Object>> objects_180 = {std::make_shared<Earth>(Earth::EGM96(180, 180))};
        const Array<Shared<Object>> objects_90 = {std::make_shared<Earth>(Earth::EGM96(90, 90))};
        const Array<Shared<Object>> objects_45 = {std::make_shared<Earth>(Earth::EGM96(45, 45))};

        const Environment customEnvironment_360 = Environment(instantJ2000, objects_360);
        const Environment customEnvironment_180 = Environment(instantJ2000, objects_180);
        const Environment customEnvironment_90 = Environment(instantJ2000, objects_90);
        const Environment customEnvironment_45 = Environment(instantJ2000, objects_45);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics_360 = {customEnvironment_360, satelliteSystem};
        SatelliteDynamics satelliteDynamics_180 = {customEnvironment_180, satelliteSystem};
        SatelliteDynamics satelliteDynamics_90 = {customEnvironment_90, satelliteSystem};
        SatelliteDynamics satelliteDynamics_45 = {customEnvironment_45, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_360 = {satelliteDynamics_360, defaultnumericalSolver_, state};
        const Propagated propagatedModel_180 = {satelliteDynamics_180, defaultnumericalSolver_, state};
        const Propagated propagatedModel_90 = {satelliteDynamics_90, defaultnumericalSolver_, state};
        const Propagated propagatedModel_45 = {satelliteDynamics_45, defaultnumericalSolver_, state};

        const Orbit orbit_360 = {propagatedModel_360, customEnvironment_360.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_180 = {propagatedModel_180, customEnvironment_180.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_90 = {propagatedModel_90, customEnvironment_90.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_45 = {propagatedModel_45, customEnvironment_45.accessCelestialObjectWithName("Earth")};

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
    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::EGM84(70, 70))};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects_180 = {std::make_shared<Earth>(Earth::EGM84(180, 180))};
        const Array<Shared<Object>> objects_70 = {std::make_shared<Earth>(Earth::EGM84(70, 70))};
        const Array<Shared<Object>> objects_45 = {std::make_shared<Earth>(Earth::EGM84(45, 45))};

        const Environment customEnvironment_180 = Environment(instantJ2000, objects_180);
        const Environment customEnvironment_70 = Environment(instantJ2000, objects_70);
        const Environment customEnvironment_45 = Environment(instantJ2000, objects_45);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics_180 = {customEnvironment_180, satelliteSystem};
        SatelliteDynamics satelliteDynamics_70 = {customEnvironment_70, satelliteSystem};
        SatelliteDynamics satelliteDynamics_45 = {customEnvironment_45, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_180 = {satelliteDynamics_180, defaultnumericalSolver_, state};
        const Propagated propagatedModel_70 = {satelliteDynamics_70, defaultnumericalSolver_, state};
        const Propagated propagatedModel_45 = {satelliteDynamics_45, defaultnumericalSolver_, state};

        const Orbit orbit_180 = {propagatedModel_180, customEnvironment_180.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_70 = {propagatedModel_70, customEnvironment_70.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_45 = {propagatedModel_45, customEnvironment_45.accessCelestialObjectWithName("Earth")};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {
            std::make_shared<Earth>(Earth::Spherical()),
            std::make_shared<Moon>(Moon::Spherical()),
            std::make_shared<Sun>(Sun::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        // Drag coefficient to 0.0 to neglect atmospheric drag
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {
            std::make_shared<Earth>(Earth::Spherical()), std::make_shared<Sun>(Sun::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {
            std::make_shared<Earth>(Earth::Spherical()), std::make_shared<Moon>(Moon::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        // Drag coefficient to 0.0 to neglect atmospheric drag
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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

TEST(
    DISABLED_OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated,
    PropAccuracy_ExtendedDuration
)  // These tests take a long time and are for local testing only (not to be run on
   // every push to git)
{
    // EGM96 perturbation
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::EGM96())};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr)};

        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, state};
        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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
    // Satellite system setup
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

    // Create environment
    const Instant instantJ2000 = Instant::J2000();
    const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

    const Environment customEnvironment = Environment(instantJ2000, objects);

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

    const size_t propagationSpanHours = 24;
    const size_t propagationIntervalSeconds = 30;

    Array<Instant> instantArray = Array<Instant>::Empty();
    for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds; i += propagationIntervalSeconds)
    {
        instantArray.add(startInstant + Duration::Seconds(i));
    }

    const State state = {
        startInstant,
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

    // Test that calculateStatesAt returns the same answer if states are requested at short or long intervals
    {
        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};

        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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

                ASSERT_EQ(*Frame::GCRF(), *position_short.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *velocity_short.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *position_long.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *velocity_long.accessFrame());

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
        const Propagated propagatedModel = {satelliteDynamics, defaultnumericalSolver_, state};

        const Orbit orbit = {propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth")};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Construct default numerical solver
        const NumericalSolver numericalSolver_54 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};
        const NumericalSolver numericalSolver_78 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        // Satellite system setup
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_54 = {satelliteDynamics, numericalSolver_54, state};
        const Propagated propagatedModel_78 = {satelliteDynamics, numericalSolver_78, state};

        const Orbit orbit_54 = {propagatedModel_54, customEnvironment.accessCelestialObjectWithName("Earth")};
        const Orbit orbit_78 = {propagatedModel_78, customEnvironment.accessCelestialObjectWithName("Earth")};

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
TEST(DISABLED_OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_SpeedTest)
{
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

        for (size_t i = 0; i < timingTestSampleSize; i++)
        {
            // Create environment
            const Instant instantJ2000 = Instant::J2000();
            // Two body default
            const Array<Shared<Object>> objects_default = {std::make_shared<Earth>(Earth::Spherical())};
            // Environment being compared
            const Array<Shared<Object>> objects_tested = {
                std::make_shared<Earth>(Earth::EGM2008(70, 70)),
                std::make_shared<Moon>(Moon::Spherical()),
                std::make_shared<Sun>(Sun::Spherical())};

            const Environment customEnvironment_default = Environment(instantJ2000, objects_default);
            const Environment customEnvironment_tested = Environment(instantJ2000, objects_tested);
            const Shared<const Frame> gcrfSPtr = Frame::GCRF();

            // Construct default numerical solver
            NumericalSolver numericalSolver = {
                NumericalSolver::LogType::NoLog,
                NumericalSolver::StepperType::RungeKuttaFehlberg78,
                5.0,
                1.0e-15,
                1.0e-15};

            // Setup initial conditions
            const State state = {
                startInstant,
                Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr),
                Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr)};

            // Satellite system setup
            const Composite satelliteGeometry(Cuboid(
                {0.0, 0.0, 0.0},
                {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
                {1.0, 2.0, 3.0}
            ));
            const SatelliteSystem satelliteSystem = {
                Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics_default = {customEnvironment_default, satelliteSystem};
            SatelliteDynamics satelliteDynamics_tested = {customEnvironment_tested, satelliteSystem};

            // Setup Propagated model and orbit
            const Propagated propagatedModel_default = {satelliteDynamics_default, numericalSolver, state};
            const Propagated propagatedModel_tested = {satelliteDynamics_tested, numericalSolver, state};

            const Orbit orbit_default = {
                propagatedModel_default, customEnvironment_default.accessCelestialObjectWithName("Earth")};
            const Orbit orbit_tested = {
                propagatedModel_tested, customEnvironment_tested.accessCelestialObjectWithName("Earth")};

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

        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};
        const SatelliteDynamics satelliteDynamics = {defaultEnvironment_, satelliteSystem};

        Propagated propagated(satelliteDynamics, defaultnumericalSolver_, states);

        State estimatedState = propagated.calculateStateAt(states[1].getInstant());

        ASSERT_EQ(estimatedState, states[1]);
    }
}
