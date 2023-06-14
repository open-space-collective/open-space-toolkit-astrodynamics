/// Apache License 2.0

#include <boost/numeric/odeint.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, Constructor)
{
    using ostk::core::ctnr::Array;
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
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;
    using ostk::astro::trajectory::State;

    using namespace boost::numeric::odeint;

    // Constructor
    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup
        EXPECT_NO_THROW(SatelliteDynamics satellitedynamics(defaultEnvironment, satelliteSystem));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, CopyConstructor)
{
    using ostk::core::ctnr::Array;
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
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;
    using ostk::astro::trajectory::State;

    using namespace boost::numeric::odeint;

    // Copy constructor
    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup
        const SatelliteDynamics satellitedynamics = {defaultEnvironment, satelliteSystem};

        EXPECT_NO_THROW(SatelliteDynamics satellitedynamicsCopy(satellitedynamics));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, EqualToOperator)
{
    using ostk::core::ctnr::Array;
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
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;
    using ostk::astro::trajectory::State;

    using namespace boost::numeric::odeint;

    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = {defaultEnvironment, satelliteSystem};
        const SatelliteDynamics satelliteDynamics_0 = {defaultEnvironment, satelliteSystem};

        EXPECT_TRUE(satelliteDynamics == satelliteDynamics_0);

        // Test for different satellite system
        const Composite satelliteGeometry_1(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem_1 = {
            Mass(90.0, Mass::Unit::Kilogram), satelliteGeometry_1, Matrix3d::Identity(), 0.8, 2.2};
        const SatelliteDynamics satelliteDynamics_1 = {defaultEnvironment, satelliteSystem_1};

        EXPECT_FALSE(satelliteDynamics == satelliteDynamics_1);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, NotEqualToOperator)
{
    using ostk::core::ctnr::Array;
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
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;
    using ostk::astro::trajectory::State;

    using namespace boost::numeric::odeint;

    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = {defaultEnvironment, satelliteSystem};
        const SatelliteDynamics satelliteDynamics_0 = {defaultEnvironment, satelliteSystem};

        EXPECT_FALSE(satelliteDynamics != satelliteDynamics_0);

        // Test for different satellite system
        const Composite satelliteGeometry_1(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem_1 = {
            Mass(90.0, Mass::Unit::Kilogram), satelliteGeometry_1, Matrix3d::Identity(), 0.8, 2.2};
        const SatelliteDynamics satelliteDynamics_1 = {defaultEnvironment, satelliteSystem_1};

        EXPECT_TRUE(satelliteDynamics != satelliteDynamics_1);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, IsDefined)
{
    using ostk::core::ctnr::Array;
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
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;
    using ostk::astro::trajectory::State;

    using namespace boost::numeric::odeint;

    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = {defaultEnvironment, satelliteSystem};

        EXPECT_TRUE(satelliteDynamics.isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, StreamOperator)
{
    using ostk::core::ctnr::Array;
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
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;
    using ostk::astro::trajectory::State;

    using namespace boost::numeric::odeint;

    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = {defaultEnvironment, satelliteSystem};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << satelliteDynamics << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, Print)
{
    using ostk::core::ctnr::Array;
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
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;
    using ostk::astro::trajectory::State;

    using namespace boost::numeric::odeint;

    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = {defaultEnvironment, satelliteSystem};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(satelliteDynamics.print(std::cout, true));
        EXPECT_NO_THROW(satelliteDynamics.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, GetSetInstant)
{
    using ostk::core::ctnr::Array;
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
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Moon;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;

    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;
    using ostk::astro::trajectory::State;

    using namespace boost::numeric::odeint;

    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default();
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Current instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Orbital model setup with Celestial object
        SatelliteDynamics satelliteDynamics = {defaultEnvironment, satelliteSystem};
        satelliteDynamics.setInstant(instant);
        EXPECT_EQ(satelliteDynamics.getInstant(), instant);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_SatelliteDynamics, getDynamicalEquations)
{
    using ostk::core::types::Shared;
    using ostk::core::types::Real;
    using ostk::core::ctnr::Array;
    using ostk::core::types::String;
    using ostk::core::types::Integer;

    using ostk::math::obj::Matrix3d;
    using ostk::math::obj::Vector3d;
    using ostk::math::geom::d3::objects::Cuboid;
    using ostk::math::geom::d3::objects::Composite;
    using ostk::math::geom::d3::objects::Point;

    using ostk::physics::units::Length;
    using ostk::physics::units::Mass;
    using ostk::physics::units::Derived;
    using ostk::physics::time::Scale;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Interval;
    using ostk::physics::time::DateTime;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::Environment;
    using ostk::physics::env::ephem::Analytical;
    using ostk::physics::env::Object;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::env::obj::celest::Sun;
    using ostk::physics::env::obj::celest::Moon;
    using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
    using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
    using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

    using ostk::astro::trajectory::State;
    using ostk::astro::flight::system::SatelliteSystem;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics;

    using namespace boost::numeric::odeint;

    SatelliteDynamics::StateVector Earth_ReferencePull(6);
    SatelliteDynamics::StateVector Earth_DragReference(6);
    SatelliteDynamics::StateVector Moon_ReferencePull(6);
    SatelliteDynamics::StateVector Sun_ReferencePull(6);
    SatelliteDynamics::StateVector Earth_Sun_Moon_ReferencePull(6);

    // Earth only gravity
    {
        // Create environment
        const Instant instantJ2000 = Instant::J2000();

        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Current state and instant setup, choose equinox as instant to make geometry simple
        // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

        const State startState = {
            startInstant,
            Position::Meters({7000000, 0.0, 0.0}, gcrfSPtr),
            Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)};

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        // Drag coefficient to 0.0 to neglect atmospheric drag
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

        // Satellite dynamics setup with Celestial object
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};
        satelliteDynamics.setInstant(startInstant);

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector(6);
        const Vector3d startPositionCoordinates =
            (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates();
        const Vector3d startVelocityCoordinates =
            (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();
        startStateVector[0] = startPositionCoordinates[0];
        startStateVector[1] = startPositionCoordinates[1];
        startStateVector[2] = startPositionCoordinates[2];
        startStateVector[3] = startVelocityCoordinates[0];
        startStateVector[4] = startVelocityCoordinates[1];
        startStateVector[5] = startVelocityCoordinates[2];

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper;
        stepper.do_step(satelliteDynamics.getDynamicalEquations(), startStateVector, (0.0), 1.0);

        // Set reference pull values for the Earth
        Earth_ReferencePull[0] = 6.999995932647768e+06;
        Earth_ReferencePull[1] = -2.312964634635743e-17;
        Earth_ReferencePull[2] = 0.000000000000000e+00;
        Earth_ReferencePull[3] = -8.134706038871020e+00;
        Earth_ReferencePull[4] = -4.625929269271485e-17;
        Earth_ReferencePull[5] = 0.000000000000000e+00;

        EXPECT_GT(1e-15, startStateVector[0] - Earth_ReferencePull[0]);
        EXPECT_GT(1e-15, startStateVector[1] - Earth_ReferencePull[1]);
        EXPECT_GT(1e-15, startStateVector[2] - Earth_ReferencePull[2]);
        EXPECT_GT(1e-15, startStateVector[3] - Earth_ReferencePull[3]);
        EXPECT_GT(1e-15, startStateVector[4] - Earth_ReferencePull[4]);
        EXPECT_GT(1e-15, startStateVector[5] - Earth_ReferencePull[5]);
    }

    // Sun only gravity
    {
        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Sun>(Sun::Default())};

        const Environment customEnvironment = Environment(instantJ2000, objects);
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Current state and instant setup, choose equinox as instant to make geometry simple
        // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

        const State startState = {
            startInstant,
            Position::Meters({7000000, 0.0, 0.0}, gcrfSPtr),
            Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)};

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        // Drag coefficient to 0.0 to neglect atmospheric drag
        const SatelliteSystem satelliteSystem = {
            Mass(90.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

        // Satellite dynamics setup with Celestial object
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};
        satelliteDynamics.setInstant(startInstant);

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector(6);
        const Vector3d startPositionCoordinates =
            (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates();
        const Vector3d startVelocityCoordinates =
            (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();
        startStateVector[0] = startPositionCoordinates[0];
        startStateVector[1] = startPositionCoordinates[1];
        startStateVector[2] = startPositionCoordinates[2];
        startStateVector[3] = startVelocityCoordinates[0];
        startStateVector[4] = startVelocityCoordinates[1];
        startStateVector[5] = startVelocityCoordinates[2];

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper;
        stepper.do_step(satelliteDynamics.getDynamicalEquations(), startStateVector, (0.0), 1.0);

        // Set reference pull values for the Earth
        Sun_ReferencePull[0] = 7.000000000000282e+06;
        Sun_ReferencePull[1] = -1.266173652819505e-09;
        Sun_ReferencePull[2] = -5.501324277544413e-10;
        Sun_ReferencePull[3] = 5.618209329643997e-07;
        Sun_ReferencePull[4] = -2.532321435973975e-09;
        Sun_ReferencePull[5] = -1.100253640019350e-09;

        EXPECT_GT(1e-15, startStateVector[0] - Sun_ReferencePull[0]);
        EXPECT_GT(1e-15, startStateVector[1] - Sun_ReferencePull[1]);
        EXPECT_GT(1e-15, startStateVector[2] - Sun_ReferencePull[2]);
        EXPECT_GT(1e-15, startStateVector[3] - Sun_ReferencePull[3]);
        EXPECT_GT(1e-15, startStateVector[4] - Sun_ReferencePull[4]);
        EXPECT_GT(1e-15, startStateVector[5] - Sun_ReferencePull[5]);
    }

    // Moon only gravity
    {
        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Moon>(Moon::Default())};

        const Environment customEnvironment = Environment(instantJ2000, objects);
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Current state and instant setup, choose equinox as instant to make geometry simple
        // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

        const State startState = {
            startInstant,
            Position::Meters({7000000, 0.0, 0.0}, gcrfSPtr),
            Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)};

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup with Celestial object
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};
        satelliteDynamics.setInstant(startInstant);

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector(6);
        const Vector3d startPositionCoordinates =
            (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates();
        const Vector3d startVelocityCoordinates =
            (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();
        startStateVector[0] = startPositionCoordinates[0];
        startStateVector[1] = startPositionCoordinates[1];
        startStateVector[2] = startPositionCoordinates[2];
        startStateVector[3] = startVelocityCoordinates[0];
        startStateVector[4] = startVelocityCoordinates[1];
        startStateVector[5] = startVelocityCoordinates[2];

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper;
        stepper.do_step(satelliteDynamics.getDynamicalEquations(), startStateVector, (0.0), 1.0);

        // Set reference pull values for the Earth
        Moon_ReferencePull[0] = 6.999999999999768e+06;
        Moon_ReferencePull[1] = 1.474353635647267e-07;
        Moon_ReferencePull[2] = 6.508220913373722e-08;
        Moon_ReferencePull[3] = -4.620551958115301e-07;
        Moon_ReferencePull[4] = 2.948701962648114e-07;
        Moon_ReferencePull[5] = 1.301641965195380e-07;

        EXPECT_GT(1e-15, startStateVector[0] - Moon_ReferencePull[0]);
        EXPECT_GT(1e-15, startStateVector[1] - Moon_ReferencePull[1]);
        EXPECT_GT(1e-15, startStateVector[2] - Moon_ReferencePull[2]);
        EXPECT_GT(1e-15, startStateVector[3] - Moon_ReferencePull[3]);
        EXPECT_GT(1e-15, startStateVector[4] - Moon_ReferencePull[4]);
        EXPECT_GT(1e-15, startStateVector[5] - Moon_ReferencePull[5]);
    }

    // Eart+Sun+Moon gravity
    {
        // Create environment
        const Instant instantJ2000 = Instant::J2000();

        const Array<Shared<Object>> objects = {
            std::make_shared<Moon>(Moon::Default()),
            std::make_shared<Sun>(Sun::Default()),
            std::make_shared<Earth>(Earth::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Current state and instant setup, choose equinox as instant to make geometry simple
        // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

        const State startState = {
            startInstant,
            Position::Meters({7000000, 0.0, 0.0}, gcrfSPtr),
            Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)};

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        // Drag coefficient to 0.0 to neglect atmospheric drag
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 0.0};

        // Satellite dynamics setup with Celestial object
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};
        satelliteDynamics.setInstant(startInstant);

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector(6);
        const Vector3d startPositionCoordinates =
            (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates();
        const Vector3d startVelocityCoordinates =
            (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();
        startStateVector[0] = startPositionCoordinates[0];
        startStateVector[1] = startPositionCoordinates[1];
        startStateVector[2] = startPositionCoordinates[2];
        startStateVector[3] = startVelocityCoordinates[0];
        startStateVector[4] = startVelocityCoordinates[1];
        startStateVector[5] = startVelocityCoordinates[2];

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper;
        stepper.do_step(satelliteDynamics.getDynamicalEquations(), startStateVector, (0.0), 1.0);

        // Set reference pull values for the Earth
        Earth_Sun_Moon_ReferencePull[0] = Earth_ReferencePull[0] + Moon_ReferencePull[0] + Sun_ReferencePull[0];
        Earth_Sun_Moon_ReferencePull[1] = Earth_ReferencePull[1] + Moon_ReferencePull[1] + Sun_ReferencePull[1];
        Earth_Sun_Moon_ReferencePull[2] = Earth_ReferencePull[2] + Moon_ReferencePull[2] + Sun_ReferencePull[2];
        Earth_Sun_Moon_ReferencePull[3] = Earth_ReferencePull[3] + Moon_ReferencePull[3] + Sun_ReferencePull[3];
        Earth_Sun_Moon_ReferencePull[4] = Earth_ReferencePull[4] + Moon_ReferencePull[4] + Sun_ReferencePull[4];
        Earth_Sun_Moon_ReferencePull[5] = Earth_ReferencePull[5] + Moon_ReferencePull[5] + Sun_ReferencePull[5];

        // [0] = 6.999995935640380e+06 ; Earth_Sun_Moon_ReferencePull[1] = 4.700487584518332e-06 ;
        // Earth_Sun_Moon_ReferencePull[2] = 2.137317833766671e-06 ; Earth_Sun_Moon_ReferencePull[3] =
        // -8.128720814005144 ; Earth_Sun_Moon_ReferencePull[4] = 9.401159910098908e-06 ;
        // Earth_Sun_Moon_ReferencePull[5] = 4.274716925865539e-06 ;

        EXPECT_GT(5e-14, startStateVector[0] - Earth_Sun_Moon_ReferencePull[0]);
        EXPECT_GT(5e-14, startStateVector[1] - Earth_Sun_Moon_ReferencePull[1]);
        EXPECT_GT(5e-14, startStateVector[2] - Earth_Sun_Moon_ReferencePull[2]);
        EXPECT_GT(5e-14, startStateVector[3] - Earth_Sun_Moon_ReferencePull[3]);
        EXPECT_GT(5e-14, startStateVector[4] - Earth_Sun_Moon_ReferencePull[4]);
        EXPECT_GT(5e-14, startStateVector[5] - Earth_Sun_Moon_ReferencePull[5]);
    }

    // Earth only gravity + exponential Drag
    {
        // Comparison data generated using OREKIT

        // Setup environment
        const Instant instantJ2000 = Instant::J2000();

        // Custom Earth with added exponential atmosphere
        const Shared<Object> earth = std::make_shared<Earth>(
            Earth(
            Earth::Models::Spherical::GravitationalParameter,
            Earth::Models::Spherical::EquatorialRadius,
            Earth::Models::Spherical::Flattening,
            Earth::Models::Spherical::J2,
            Earth::Models::Spherical::J4,
            std::make_shared<Analytical>(Frame::ITRF()),
            EarthGravitationalModel::Type::Spherical,
            EarthMagneticModel::Type::Undefined,
            EarthAtmosphericModel::Type::Exponential,
            Instant::J2000()
            ));

        const Array<Shared<Object>> objects = {earth};

        const Environment customEnvironment = Environment(instantJ2000, objects);

        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Satellite shape does not matter for drag, since constant Cd & area are defined
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        // Satellite System
        // Cd = 2.1
        // Area = 1.0
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 1.0, 2.1};

        // Initial conditions
        const Instant startInstant = Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC);
        const Length semiMajorAxis = Earth::Models::Spherical::EquatorialRadius + Length::Kilometers(500);
        const double startingSpeedY = 7612.608170359118000;

        const State startState = {
            startInstant,
            Position::Meters({semiMajorAxis.inMeters(), 0.0, 0.0}, gcrfSPtr),
            Velocity::MetersPerSecond({0.0, startingSpeedY, 0.0}, gcrfSPtr)};

        // Satellite dynamics setup with Celestial object
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};
        satelliteDynamics.setInstant(startInstant);

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector(6);
        const Vector3d startPositionCoordinates =
            (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates();
        const Vector3d startVelocityCoordinates =
            (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();
        startStateVector[0] = startPositionCoordinates[0];
        startStateVector[1] = startPositionCoordinates[1];
        startStateVector[2] = startPositionCoordinates[2];
        startStateVector[3] = startVelocityCoordinates[0];
        startStateVector[4] = startVelocityCoordinates[1];
        startStateVector[5] = startVelocityCoordinates[2];

        // Check initial conditions precision
        EXPECT_NEAR(startStateVector[0], 6878137.0, 1e-15);
        EXPECT_NEAR(startStateVector[1], 0.0, 1e-15);
        EXPECT_NEAR(startStateVector[2], 0.0, 1e-15);
        EXPECT_NEAR(startStateVector[3], 0.0, 1e-15);
        EXPECT_NEAR(startStateVector[4], 7612.608170359118, 1e-15);
        EXPECT_NEAR(startStateVector[5], 0.0, 1e-15);

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper;
        stepper.do_step(satelliteDynamics.getDynamicalEquations(), startStateVector, (0.0), 1.0);

        // Reference values achieved in OREKit
        Earth_DragReference[0] = 6878132.787246078000000;
        Earth_DragReference[1] = 7612.606615971900000;
        Earth_DragReference[2] = -0.000000000000330;

        Earth_DragReference[3] = -8.425506982847088;
        Earth_DragReference[4] = 7612.603507382901000;
        Earth_DragReference[5] = -0.000000000000649;

        // OREKit has Z-axis change at the 13th decimal place, which is not realistic for this simple model.
        // Consider this the precision and compare at 12th decimal place.
        EXPECT_NEAR(startStateVector[0], Earth_DragReference[0], 1e-12);
        EXPECT_NEAR(startStateVector[1], Earth_DragReference[1], 1e-12);
        EXPECT_NEAR(startStateVector[2], Earth_DragReference[2], 1e-12);
        EXPECT_NEAR(startStateVector[3], Earth_DragReference[3], 1e-12);
        EXPECT_NEAR(startStateVector[4], Earth_DragReference[4], 1e-12);
        EXPECT_NEAR(startStateVector[5], Earth_DragReference[5], 1e-12);
    }

    // Minimum radius "re-entry" test
    {
        // Create environment
        const Instant instantJ2000 = Instant::J2000();
        const Array<Shared<Object>> objects = {std::make_shared<Earth>(Earth::Spherical())};

        const Environment customEnvironment = Environment(instantJ2000, objects);
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        // Current state and instant setup, choose equinox as instant to make geometry simple
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

        const State startState = {
            startInstant,
            Position::Meters({700000, 0.0, 0.0}, gcrfSPtr),
            Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)};

        // Default satellite system being used
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        const SatelliteSystem satelliteSystem = {
            Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2};

        // Satellite dynamics setup with Celestial object
        SatelliteDynamics satelliteDynamics = {customEnvironment, satelliteSystem};
        satelliteDynamics.setInstant(startInstant);

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector(6);
        const Vector3d startPositionCoordinates =
            (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates();
        const Vector3d startVelocityCoordinates =
            (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();
        startStateVector[0] = startPositionCoordinates[0];
        startStateVector[1] = startPositionCoordinates[1];
        startStateVector[2] = startPositionCoordinates[2];
        startStateVector[3] = startVelocityCoordinates[0];
        startStateVector[4] = startVelocityCoordinates[1];
        startStateVector[5] = startVelocityCoordinates[2];

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper;

        EXPECT_ANY_THROW(stepper.do_step(satelliteDynamics.getDynamicalEquations(), startStateVector, (0.0), 1.0));
    }

}
