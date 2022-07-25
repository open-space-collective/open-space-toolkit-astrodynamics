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

#include <typeinfo>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* UNIT TESTS */
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
        const SatelliteDynamics satelliteDynamics_NoneAll = { defaultEnvironment, satelliteSystem, state, SatelliteDynamics::StateVectorDimension::PositionVelocity } ;

        // Repeat these tests to make sure you get the correct function pointer for each type of perturbation?

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* VALIDATION TESTS */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Can check the outputs of the derivatives and state transition matrices?
