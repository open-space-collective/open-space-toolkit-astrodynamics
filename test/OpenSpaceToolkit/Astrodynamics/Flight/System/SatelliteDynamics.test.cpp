////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/SatelliteSystem.test.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteDynamics.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <boost/numeric/odeint.hpp>
#include <typeinfo>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, Constructor)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Satellite dynamics setup
        EXPECT_NO_THROW(SatelliteDynamics( defaultEnvironment, satelliteSystem, state )) ;
        EXPECT_NO_THROW(SatelliteDynamics( defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocity )) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, EqualToOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) } ;

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;
        const SatelliteDynamics satelliteDynamics_0 = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        EXPECT_TRUE(satelliteDynamics == satelliteDynamics_0) ;

        // Test for different satellite system
        const SatelliteSystem satelliteSystem_1 = { Mass {100.0, Mass::Unit::Kilogram } } ;
        const SatelliteDynamics satelliteDynamics_1 = { defaultEnvironment, satelliteSystem_1, state, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        EXPECT_FALSE(satelliteDynamics == satelliteDynamics_1) ;

        // Test for different state
        const State state_2 = { startInstant, Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) } ;
        const SatelliteDynamics satelliteDynamics_2 = { defaultEnvironment, satelliteSystem, state_2, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        EXPECT_FALSE(satelliteDynamics == satelliteDynamics_2) ;

        // Test for different start instant
        const Instant startInstant_3 = Instant::DateTime(DateTime(2018, 2, 1, 0, 0, 0), Scale::UTC) ;
        const State state_3 = { startInstant_3, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) } ;
        const SatelliteDynamics satelliteDynamics_3 = { defaultEnvironment, satelliteSystem, state_3, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;
        EXPECT_FALSE(satelliteDynamics == satelliteDynamics_3) ;

        // Test for different state vector type
        const SatelliteDynamics satelliteDynamics_4 = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocitywithDragCoefficient } ;

        EXPECT_FALSE(satelliteDynamics == satelliteDynamics_4) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, NotEqualToOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) } ;

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;
        const SatelliteDynamics satelliteDynamics_0 = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        EXPECT_FALSE(satelliteDynamics != satelliteDynamics_0) ;

        // Test for different satellite system
        const SatelliteSystem satelliteSystem_1 = { Mass {100.0, Mass::Unit::Kilogram } } ;
        const SatelliteDynamics satelliteDynamics_1 = { defaultEnvironment, satelliteSystem_1, state, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        EXPECT_TRUE(satelliteDynamics != satelliteDynamics_1) ;

        // Test for different state
        const State state_2 = { startInstant, Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) } ;
        const SatelliteDynamics satelliteDynamics_2 = { defaultEnvironment, satelliteSystem, state_2, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        EXPECT_TRUE(satelliteDynamics != satelliteDynamics_2) ;

        // Test for different start instant
        const Instant startInstant_3 = Instant::DateTime(DateTime(2018, 2, 1, 0, 0, 0), Scale::UTC) ;
        const State state_3 = { startInstant_3, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) } ;
        const SatelliteDynamics satelliteDynamics_3 = { defaultEnvironment, satelliteSystem, state_3, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;
        EXPECT_TRUE(satelliteDynamics != satelliteDynamics_3) ;

        // Test for different perturbation types
        const SatelliteDynamics satelliteDynamics_4 = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocitywithDragCoefficient } ;

        EXPECT_TRUE(satelliteDynamics != satelliteDynamics_4) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, IsDefined)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {
        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        EXPECT_TRUE(satelliteDynamics.isDefined()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, StreamOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << satelliteDynamics << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;


    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, Print)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(satelliteDynamics.print(std::cout, true)) ;
        EXPECT_NO_THROW(satelliteDynamics.print(std::cout, false)) ;
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, GetType)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics_PV = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;
        EXPECT_EQ(satelliteDynamics_PV.getStateVectorDimension(), SatelliteDynamics::StateVectorDimension::PositionVelocity) ;

        const SatelliteDynamics satelliteDynamics_PVD = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocitywithDragCoefficient } ;
        EXPECT_EQ(satelliteDynamics_PVD.getStateVectorDimension(), SatelliteDynamics::StateVectorDimension::PositionVelocitywithDragCoefficient) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, GetState)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Orbital model setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;
        EXPECT_EQ(satelliteDynamics.getState(), state) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, SetState)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) } ;

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Orbital model setup with Celestial object
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        const State state_0 = { startInstant, Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) } ;

        satelliteDynamics.setState(state_0) ;
        EXPECT_EQ(satelliteDynamics.getState(), state_0) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, AccessDynamicalEquations)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::celest::Sun ;
    using ostk::physics::env::obj::celest::Moon ;
    using ostk::physics::env::obj::Celestial ;
    using ostk::physics::env::Object ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    using namespace boost::numeric::odeint ;

    SatelliteDynamics::StateVector Earth_ReferencePull(6) ;
    SatelliteDynamics::StateVector Moon_ReferencePull(6) ;
    SatelliteDynamics::StateVector Sun_ReferencePull(6) ;
    SatelliteDynamics::StateVector Earth_Sun_Moon_ReferencePull(6) ;

    // Earth only gravity
    {

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects =
        {
            std::make_shared<Earth>(Earth::Spherical())
        } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup, choose equinox as instant to make geometry simple
        /* Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction */
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC) ;

        const State startState = { startInstant, Position::Meters({ 7000000, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 0.0, 0.0 }, gcrfSPtr) } ;

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Satellite dynamics setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, startState, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector((size_t)satelliteDynamics.getStateVectorDimension()) ;
        const Vector3d startPositionCoordinates = (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates() ;
        const Vector3d startVelocityCoordinates = (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;
        startStateVector[0] = startPositionCoordinates[0]; startStateVector[1] = startPositionCoordinates[1]; startStateVector[2] = startPositionCoordinates[2] ;
        startStateVector[3] = startVelocityCoordinates[0]; startStateVector[4] = startVelocityCoordinates[1]; startStateVector[5] = startVelocityCoordinates[2] ;

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper ;
        stepper.do_step(satelliteDynamics.accessDynamicalEquations(), startStateVector, (0.0), 1.0) ;

        // Set reference pull values for the Earth
        Earth_ReferencePull[0] = 6.999995932647768e+06 ; Earth_ReferencePull[1] = -2.312964634635743e-17 ;  Earth_ReferencePull[2] = 0.000000000000000e+00 ;
        Earth_ReferencePull[3] = -8.134706038871020e+00 ; Earth_ReferencePull[4] = -4.625929269271485e-17 ;  Earth_ReferencePull[5] = 0.000000000000000e+00 ;

        EXPECT_GT(1e-9, startStateVector[0] - Earth_ReferencePull[0]) ;
        EXPECT_GT(1e-9, startStateVector[1] - Earth_ReferencePull[1]) ;
        EXPECT_GT(1e-9, startStateVector[2] - Earth_ReferencePull[2]) ;
        EXPECT_GT(1e-9, startStateVector[3] - Earth_ReferencePull[3]) ;
        EXPECT_GT(1e-9, startStateVector[4] - Earth_ReferencePull[4]) ;
        EXPECT_GT(1e-9, startStateVector[5] - Earth_ReferencePull[5]) ;

        // std::cout.precision(15) ;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield) ;


        // std::cout << startStateVector[0] << std::endl ;
        // std::cout << startStateVector[1] << std::endl ;
        // std::cout << startStateVector[2] << std::endl ;
        // std::cout << startStateVector[3] << std::endl ;
        // std::cout << startStateVector[4] << std::endl ;
        // std::cout << startStateVector[5] << std::endl ;

    }

    // Sun only gravity
    {

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects =
        {
            std::make_shared<Sun>(Sun::Default())
        } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup, choose equinox as instant to make geometry simple
        /* Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction */
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC) ;

        const State startState = { startInstant, Position::Meters({ 7000000, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 0.0, 0.0 }, gcrfSPtr) } ;

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Satellite dynamics setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, startState, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector((size_t)satelliteDynamics.getStateVectorDimension()) ;
        const Vector3d startPositionCoordinates = (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates() ;
        const Vector3d startVelocityCoordinates = (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;
        startStateVector[0] = startPositionCoordinates[0]; startStateVector[1] = startPositionCoordinates[1]; startStateVector[2] = startPositionCoordinates[2] ;
        startStateVector[3] = startVelocityCoordinates[0]; startStateVector[4] = startVelocityCoordinates[1]; startStateVector[5] = startVelocityCoordinates[2] ;

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper ;
        stepper.do_step(satelliteDynamics.accessDynamicalEquations(), startStateVector, (0.0), 1.0) ;

        // Set reference pull values for the Earth
        Sun_ReferencePull[0] = 7.000000002989579e06 ; Sun_ReferencePull[1] = -8.983629560302395e-06 ;  Sun_ReferencePull[2] = -3.903244968847363e-06 ;
        Sun_ReferencePull[3] = 5.979155765283343e-03 ; Sun_ReferencePull[4] = -1.796707558028250e-05 ;  Sun_ReferencePull[5] = - 7.806410365948309e-06 ;

        EXPECT_GT(1e-9, startStateVector[0] - Sun_ReferencePull[0]) ;
        EXPECT_GT(1e-9, startStateVector[1] - Sun_ReferencePull[1]) ;
        EXPECT_GT(1e-9, startStateVector[2] - Sun_ReferencePull[2]) ;
        EXPECT_GT(1e-9, startStateVector[3] - Sun_ReferencePull[3]) ;
        EXPECT_GT(1e-9, startStateVector[4] - Sun_ReferencePull[4]) ;
        EXPECT_GT(1e-9, startStateVector[5] - Sun_ReferencePull[5]) ;

        // std::cout.precision(15) ;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield) ;


        // std::cout << startStateVector[0] << std::endl ;
        // std::cout << startStateVector[1] << std::endl ;
        // std::cout << startStateVector[2] << std::endl ;
        // std::cout << startStateVector[3] << std::endl ;
        // std::cout << startStateVector[4] << std::endl ;
        // std::cout << startStateVector[5] << std::endl ;


    }

    // Moon only gravity
    {

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects =
        {
            std::make_shared<Moon>(Moon::Default())
        } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup, choose equinox as instant to make geometry simple
        /* Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction */
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC) ;

        const State startState = { startInstant, Position::Meters({ 7000000, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 0.0, 0.0 }, gcrfSPtr) } ;

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Satellite dynamics setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, startState, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector((size_t)satelliteDynamics.getStateVectorDimension()) ;
        const Vector3d startPositionCoordinates = (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates() ;
        const Vector3d startVelocityCoordinates = (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;
        startStateVector[0] = startPositionCoordinates[0]; startStateVector[1] = startPositionCoordinates[1]; startStateVector[2] = startPositionCoordinates[2] ;
        startStateVector[3] = startVelocityCoordinates[0]; startStateVector[4] = startVelocityCoordinates[1]; startStateVector[5] = startVelocityCoordinates[2] ;

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper ;
        stepper.do_step(satelliteDynamics.accessDynamicalEquations(), startStateVector, (0.0), 1.0) ;

        // Set reference pull values for the Earth
        Moon_ReferencePull[0] = 7.000000000003034e+06 ; Moon_ReferencePull[1] = 1.368411761367117e-05 ;  Moon_ReferencePull[2] = 6.040563015622355e-06 ;
        Moon_ReferencePull[3] = 6.066782131026325e-06 ; Moon_ReferencePull[4] = 2.736823736584779e-05 ;  Moon_ReferencePull[5] = 1.208112814387077e-05 ;

        EXPECT_GT(1e-9, startStateVector[0] - Moon_ReferencePull[0]) ;
        EXPECT_GT(1e-9, startStateVector[1] - Moon_ReferencePull[1]) ;
        EXPECT_GT(1e-9, startStateVector[2] - Moon_ReferencePull[2]) ;
        EXPECT_GT(1e-9, startStateVector[3] - Moon_ReferencePull[3]) ;
        EXPECT_GT(1e-9, startStateVector[4] - Moon_ReferencePull[4]) ;
        EXPECT_GT(1e-9, startStateVector[5] - Moon_ReferencePull[5]) ;

        // std::cout.precision(15) ;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield) ;


        // std::cout << startStateVector[0] << std::endl ;
        // std::cout << startStateVector[1] << std::endl ;
        // std::cout << startStateVector[2] << std::endl ;
        // std::cout << startStateVector[3] << std::endl ;
        // std::cout << startStateVector[4] << std::endl ;
        // std::cout << startStateVector[5] << std::endl ;


    }

    // Eart+Sun+Moon gravity
    {

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects =
        {
            std::make_shared<Moon>(Moon::Default()),
            std::make_shared<Sun>(Sun::Default()),
            std::make_shared<Earth>(Earth::Spherical())
        } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup, choose equinox as instant to make geometry simple
        /* Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction */
        const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC) ;

        const State startState = { startInstant, Position::Meters({ 7000000, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 0.0, 0.0 }, gcrfSPtr) } ;

        // Default satellite system being used
        const SatelliteSystem satelliteSystem = { } ;

        // Satellite dynamics setup with Celestial object
        const SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, startState, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        // Set up initial state vector
        SatelliteDynamics::StateVector startStateVector((size_t)satelliteDynamics.getStateVectorDimension()) ;
        const Vector3d startPositionCoordinates = (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates() ;
        const Vector3d startVelocityCoordinates = (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;
        startStateVector[0] = startPositionCoordinates[0]; startStateVector[1] = startPositionCoordinates[1]; startStateVector[2] = startPositionCoordinates[2] ;
        startStateVector[3] = startVelocityCoordinates[0]; startStateVector[4] = startVelocityCoordinates[1]; startStateVector[5] = startVelocityCoordinates[2] ;

        // Perform 1.0s integration step
        runge_kutta4<SatelliteDynamics::StateVector> stepper ;
        stepper.do_step(satelliteDynamics.accessDynamicalEquations(), startStateVector, (0.0), 1.0) ;

        // Set reference pull values for the Earth
        Earth_Sun_Moon_ReferencePull[0] = Earth_ReferencePull[0] + Moon_ReferencePull[0] + Sun_ReferencePull[0] ;
        Earth_Sun_Moon_ReferencePull[1] = Earth_ReferencePull[1] + Moon_ReferencePull[1] + Sun_ReferencePull[1] ;
        Earth_Sun_Moon_ReferencePull[2] = Earth_ReferencePull[2] + Moon_ReferencePull[2] + Sun_ReferencePull[2] ;
        Earth_Sun_Moon_ReferencePull[3] = Earth_ReferencePull[3] + Moon_ReferencePull[3] + Sun_ReferencePull[3] ;
        Earth_Sun_Moon_ReferencePull[4] = Earth_ReferencePull[4] + Moon_ReferencePull[4] + Sun_ReferencePull[4] ;
        Earth_Sun_Moon_ReferencePull[5] = Earth_ReferencePull[5] + Moon_ReferencePull[5] + Sun_ReferencePull[5] ;

        // [0] = 6.999995935640380e+06 ; Earth_Sun_Moon_ReferencePull[1] = 4.700487584518332e-06 ;  Earth_Sun_Moon_ReferencePull[2] = 2.137317833766671e-06 ;
        // Earth_Sun_Moon_ReferencePull[3] = -8.128720814005144 ; Earth_Sun_Moon_ReferencePull[4] = 9.401159910098908e-06 ;  Earth_Sun_Moon_ReferencePull[5] = 4.274716925865539e-06 ;

        EXPECT_GT(1e-8, startStateVector[0] - Earth_Sun_Moon_ReferencePull[0]) ;
        EXPECT_GT(1e-8, startStateVector[1] - Earth_Sun_Moon_ReferencePull[1]) ;
        EXPECT_GT(1e-8, startStateVector[2] - Earth_Sun_Moon_ReferencePull[2]) ;
        EXPECT_GT(1e-8, startStateVector[3] - Earth_Sun_Moon_ReferencePull[3]) ;
        EXPECT_GT(1e-8, startStateVector[4] - Earth_Sun_Moon_ReferencePull[4]) ;
        EXPECT_GT(1e-8, startStateVector[5] - Earth_Sun_Moon_ReferencePull[5]) ;

        // std::cout.precision(15) ;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield) ;

        // std::cout << Earth_Sun_Moon_ReferencePull[3] << std::endl ;
        // std::cout << startStateVector[0] << std::endl ;
        // std::cout << startStateVector[1] << std::endl ;
        // std::cout << startStateVector[2] << std::endl ;
        // std::cout << startStateVector[3] << std::endl ;
        // std::cout << startStateVector[4] << std::endl ;
        // std::cout << startStateVector[5] << std::endl ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics, StringFromType)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        EXPECT_TRUE(SatelliteDynamics::StringFromStateVectorDimension(SatelliteDynamics::StateVectorDimension::PositionVelocity) == "PositionVelocity") ;
        EXPECT_TRUE(SatelliteDynamics::StringFromStateVectorDimension(SatelliteDynamics::StateVectorDimension::PositionVelocitywithDragCoefficient) == "PositionVelocitywithDragCoefficient") ;

    }

}
