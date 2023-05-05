// Copyright © Loft Orbital Solutions Inc.

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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
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

using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::dynamics::SatelliteDynamics;
using ostk::astro::trajectory::State;

using ostk::astro::trajectory::Propagator;

/* UNIT TESTS */
class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        this->environment_ = Environment::Default();
    }

    Environment environment_ = Environment::Undefined();
    NumericalSolver numericalSolver_ = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};
    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Constructor)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
    const SatelliteDynamics satelliteDynamics = {environment_, satelliteSystem};

    // Constructor without putting into orbit object
    {
        EXPECT_NO_THROW(Propagator(satelliteDynamics, numericalSolver_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, EqualToOperator)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
    const SatelliteDynamics satelliteDynamics = {environment_, satelliteSystem};

    {
        const Propagator propagator = {satelliteDynamics, numericalSolver_};
        const Propagator propagator_x = {propagator};
        EXPECT_TRUE(propagator == propagator_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        const Propagator propagator_1 = {satelliteDynamics, numericalSolver_1};
        EXPECT_FALSE(propagator == propagator_1);

        const SatelliteSystem satelliteSystem_2 = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
        const SatelliteDynamics satelliteDynamics_2 = {environment_, satelliteSystem_2};
        const Propagator propagator_2 = {satelliteDynamics_2, numericalSolver_1};
        EXPECT_FALSE(propagator == propagator_2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, NotEqualToOperator)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
    const SatelliteDynamics satelliteDynamics = {environment_, satelliteSystem};

    {
        const Propagator propagator = {satelliteDynamics, numericalSolver_};
        const Propagator propagator_x = {propagator};
        EXPECT_FALSE(propagator != propagator_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        const Propagator propagator_1 = {satelliteDynamics, numericalSolver_1};
        EXPECT_TRUE(propagator != propagator_1);

        const SatelliteSystem satelliteSystem_2 = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
        const SatelliteDynamics satelliteDynamics_2 = {environment_, satelliteSystem};
        const Propagator propagator_2 = {satelliteDynamics_2, numericalSolver_1};
        EXPECT_TRUE(propagator != propagator_2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, IsDefined)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
    const SatelliteDynamics satelliteDynamics = {environment_, satelliteSystem};

    {
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        EXPECT_TRUE(propagator.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, StreamOperator)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
    const SatelliteDynamics satelliteDynamics = {environment_, satelliteSystem};

    {
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << propagator << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Print)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};
    const SatelliteDynamics satelliteDynamics = {environment_, satelliteSystem};

    {
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(propagator.print(std::cout, true));
        EXPECT_NO_THROW(propagator.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, CalculateStateAt)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

    // Create environment
    const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

    const Environment customEnvironment = Environment(Instant::J2000(), objects);

    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

    // Setup Propagator model and orbit
    const Propagator propagator = {satelliteDynamics, numericalSolver_};

    // Setup instants
    const Array<Instant> instantArray = {
        Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC)};

    for (const Instant& instant : instantArray)
    {
        EXPECT_NO_THROW(propagator.calculateStateAt(state, instant));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, CalculateStatesAt)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

    // Test exception for unsorted instant array
    {
        // Create environment
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

        const Environment customEnvironment = Environment(Instant::J2000(), objects);

        // Current state and instant setup
        const State state = {
            Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        // Setup instants
        Array<Instant> instantArray = Array<Instant>::Empty();
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC));
        instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC));

        EXPECT_ANY_THROW(propagator.calculateStatesAt(state, instantArray));
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

        referencePositionArray.add(
            {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()}
        );
        referenceVelocityArray.add(
            {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()}
        );
    }

    // Create environment
    const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

    const Environment customEnvironment = Environment(Instant::J2000(), objects);

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

    // Setup Propagator model and perform propagation
    const Propagator propagator = {satelliteDynamics, numericalSolver_};

    // Test forward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 0;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

    // Test forward + backward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 30;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

    // Test backward propagation
    {
        // Choose state to put into cachedStateArray
        size_t cachedStateReferenceIndex = 60;
        const Instant startInstant = instantArray[cachedStateReferenceIndex];
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray[cachedStateReferenceIndex]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[cachedStateReferenceIndex]}, gcrfSPtr_)};

        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

/* VALIDATION TESTS */
/* Force model validation tests */
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

    // Create environment
    const Instant instantJ2000 = Instant::J2000();
    const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

    const Environment customEnvironment = Environment(instantJ2000, objects);

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

    // Setup Propagator model and orbit
    const Propagator propagator = {satelliteDynamics, numericalSolver_};

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

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

        const Shared<const Frame> itrfSPtr = Frame::ITRF();

        // Current state and instant setup
        const Instant startInstant = instantArray[0];

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

    // Create environment
    const Instant instantJ2000 = Instant::J2000();
    const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::EGM2008(100, 100))};

    const Environment customEnvironment = Environment(instantJ2000, objects);

    // Satellite dynamics setup
    SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

    // Setup Propagator model and orbit
    const Propagator propagator = {satelliteDynamics, numericalSolver_};

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

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

        // Create environment
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

        // Setup Propagator model and orbit
        const Propagator propagator_360 = {satelliteDynamics_360, numericalSolver_};
        const Propagator propagator_100 = {satelliteDynamics_100, numericalSolver_};
        const Propagator propagator_70 = {satelliteDynamics_70, numericalSolver_};
        const Propagator propagator_45 = {satelliteDynamics_45, numericalSolver_};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = propagator_360.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_100 = propagator_100.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_70 = propagator_70.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_EGM96)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

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

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArray_GCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray_GCRF[0]}, gcrfSPtr_)};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

        // Setup Propagator model and orbit
        const Propagator propagator_360 = {satelliteDynamics_360, numericalSolver_};
        const Propagator propagator_180 = {satelliteDynamics_180, numericalSolver_};
        const Propagator propagator_90 = {satelliteDynamics_90, numericalSolver_};
        const Propagator propagator_45 = {satelliteDynamics_45, numericalSolver_};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = propagator_360.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_180 = propagator_180.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_90 = propagator_90.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_EGM84)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

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

        // Setup Propagator model and orbit
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

        // Setup Propagator model and orbit
        const Propagator propagator_180 = {satelliteDynamics_180, numericalSolver_};
        const Propagator propagator_70 = {satelliteDynamics_70, numericalSolver_};
        const Propagator propagator_45 = {satelliteDynamics_45, numericalSolver_};

        // Propagate all states
        const Array<State> propagatedStateArray_180 = propagator_180.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_70 = propagator_70.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Sun_Moon)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

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

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Sun)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

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

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_Moon)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

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

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator = {satelliteDynamics, numericalSolver_};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

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

/* Propagation Interval validation test */
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_IntervalSelfComparison)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator_short = {satelliteDynamics, numericalSolver_};
        const Propagator propagator_long = {satelliteDynamics, numericalSolver_};

        // Propagate all states at short interval
        const Array<State> propagatedStateArray_short = propagator_short.calculateStatesAt(state, instantArray);

        // Propagate all states at long interval
        Array<Instant> instantArray_longInterval = Array<Instant>::Empty();
        const size_t indexIncrement = 10;
        for (size_t i = 0; i < instantArray.getSize(); i += indexIncrement)
        {
            instantArray_longInterval.add(instantArray[i]);
        }
        const Array<State> propagatedStateArray_long =
            propagator_long.calculateStatesAt(state, instantArray_longInterval);

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

        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)};

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator_short = {satelliteDynamics, numericalSolver_};
        const Propagator propagator_long = {satelliteDynamics, numericalSolver_};

        testing::internal::CaptureStdout();

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Propagator output generated
            const State state_short = propagator_short.calculateStateAt(state, instantArray[i]);

            // Run once every X times
            if (i % 10 == 0)
            {
                const State state_long = propagator_long.calculateStateAt(state, instantArray[i]);

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
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, PropAccuracy_TwoBody_StepperTypeSelfComparison
)
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));
    const SatelliteSystem satelliteSystem = {
        Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

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

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};

        // Setup Propagator model and orbit
        const Propagator propagator_54 = {satelliteDynamics, numericalSolver_54};
        const Propagator propagator_78 = {satelliteDynamics, numericalSolver_78};

        // Propagate all states
        const Array<State> propagatedStateArray_54 = propagator_54.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_78 = propagator_78.calculateStatesAt(state, instantArray);

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
