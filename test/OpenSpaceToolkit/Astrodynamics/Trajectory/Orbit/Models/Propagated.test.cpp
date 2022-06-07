////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.test.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
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

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* UNIT TESTS */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Constructor)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel_1 = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        const Propagated propagatedModel_2 = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                               Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None, Propagated::IntegrationStepperType::RungeKuttaCashKarp54, Propagated::IntegrationLogType::NoLog, 1.0e-15, 1.0e-15 } ;
        
        EXPECT_NO_THROW(propagatedModel_1) ; 
        EXPECT_NO_THROW(propagatedModel_2) ; 

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, EqualToOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state_AB = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel_A = { state_AB, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                               Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None, Propagated::IntegrationStepperType::RungeKuttaCashKarp54, Propagated::IntegrationLogType::NoLog, 1.0e-15, 1.0e-15 } ;

        const Propagated propagatedModel_B = { state_AB, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                               Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_TRUE(propagatedModel_A == propagatedModel_B) ;

        // Current state and instant setup
        const State state_CD = { startInstant, Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        const Propagated propagatedModel_C = { state_CD, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                               Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None, Propagated::IntegrationStepperType::RungeKuttaCashKarp54, Propagated::IntegrationLogType::NoLog, 1.0e-15, 1.0e-14 } ;

        const Propagated propagatedModel_D = { state_CD, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                               Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_FALSE(propagatedModel_C == propagatedModel_D) ;

        // Current state and instant setup
        const State state_EF = { startInstant, Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        const Propagated propagatedModel_E = { state_AB, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                               Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None, Propagated::IntegrationStepperType::RungeKuttaCashKarp54, Propagated::IntegrationLogType::NoLog, 1.0e-15, 1.0e-15 } ;

        const Propagated propagatedModel_F = { state_EF, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                               Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_FALSE(propagatedModel_E == propagatedModel_F) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, NotEqualToOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state_A = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel_A = { state_A, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_FALSE(propagatedModel_A != propagatedModel_A) ;

        // Current state and instant setup
        const State state_B = { startInstant, Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        const Propagated propagatedModel_B = { state_B, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_TRUE(propagatedModel_A != propagatedModel_B) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, IsDefined)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_TRUE(propagatedModel.isDefined()) ; 

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, StreamOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << propagatedModel << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Print)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(propagatedModel.print(std::cout, true)) ;
        EXPECT_NO_THROW(propagatedModel.print(std::cout, false)) ;
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getEpochANDgetRevolutionNumberAtEpoch)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::types::Integer ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_EQ(propagatedModel.getEpoch(),startInstant) ;

        EXPECT_EQ(propagatedModel.getRevolutionNumberAtEpoch(),1) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getGravitationalParameterANDgetEquatorialRadius)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::types::Integer ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;
    
    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_EQ(propagatedModel.getEquatorialRadius(),equatorialRadius) ;

        EXPECT_EQ(propagatedModel.getRevolutionNumberAtEpoch(), 1) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getType)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::types::Integer ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;
    
    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel_None = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_None.getGravitationalPerturbationType(),Propagated::GravitationalPerturbationType::None) ;

        const Propagated propagatedModel_J2 = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::J2, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_J2.getGravitationalPerturbationType(),Propagated::GravitationalPerturbationType::J2) ;

        const Propagated propagatedModel_TenByTen = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::TenByTen, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_TenByTen.getGravitationalPerturbationType(),Propagated::GravitationalPerturbationType::TenByTen) ;

        const Propagated propagatedModel_FourtyByFourty = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::FourtyByFourty, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_FourtyByFourty.getGravitationalPerturbationType(),Propagated::GravitationalPerturbationType::FourtyByFourty) ;

    }

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel_None = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_None.getAtmosphericPerturbationType(),Propagated::AtmosphericPerturbationType::None) ;

        const Propagated propagatedModel_Exponential = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::Exponential, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_Exponential.getAtmosphericPerturbationType(),Propagated::AtmosphericPerturbationType::Exponential) ;

        const Propagated propagatedModel_JacchiaRoberts = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::JacchiaRoberts, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_JacchiaRoberts.getAtmosphericPerturbationType(),Propagated::AtmosphericPerturbationType::JacchiaRoberts) ;

        const Propagated propagatedModel_NRLMISIS00 = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::NRLMISIS00, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_NRLMISIS00.getAtmosphericPerturbationType(),Propagated::AtmosphericPerturbationType::NRLMISIS00) ;
    }

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel_None = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_None.getThirdBodyPerturbationType(),Propagated::ThirdBodyPerturbationType::None) ;

        const Propagated propagatedModel_Luni = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::Luni } ;
        EXPECT_EQ(propagatedModel_Luni.getThirdBodyPerturbationType(),Propagated::ThirdBodyPerturbationType::Luni) ;

        const Propagated propagatedModel_Solar = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::Solar } ;
        EXPECT_EQ(propagatedModel_Solar.getThirdBodyPerturbationType(),Propagated::ThirdBodyPerturbationType::Solar) ;

        const Propagated propagatedModel_LuniSolar = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::LuniSolar } ;
        EXPECT_EQ(propagatedModel_LuniSolar.getThirdBodyPerturbationType(),Propagated::ThirdBodyPerturbationType::LuniSolar) ;
    }

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel_RungeKuttaCashKarp54 = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        EXPECT_EQ(propagatedModel_RungeKuttaCashKarp54.getIntegrationStepperType(),Propagated::IntegrationStepperType::RungeKuttaCashKarp54) ;

    }

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel_NoLog = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                                   Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None, Propagated::IntegrationStepperType::RungeKuttaCashKarp54, Propagated::IntegrationLogType::NoLog, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(propagatedModel_NoLog.getIntegrationLogType(),Propagated::IntegrationLogType::NoLog) ;

        const Propagated propagatedModel_LogConstant = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                                   Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None, Propagated::IntegrationStepperType::RungeKuttaCashKarp54, Propagated::IntegrationLogType::LogConstant, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(propagatedModel_LogConstant.getIntegrationLogType(),Propagated::IntegrationLogType::LogConstant) ;

        const Propagated propagatedModel_LogAdaptive = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, 
                                                   Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None, Propagated::IntegrationStepperType::RungeKuttaCashKarp54, Propagated::IntegrationLogType::LogAdaptive, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(propagatedModel_LogAdaptive.getIntegrationLogType(),Propagated::IntegrationLogType::LogAdaptive) ;

    }
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, calculateStateAtANDcalculateRevolutionNumberAt)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::types::Integer ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel = { state, startInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;

        EXPECT_TRUE(propagatedModel.calculateStateAt(startInstant) == state) ; 
        EXPECT_TRUE(propagatedModel.calculateRevolutionNumberAt(startInstant) == 1) ; 

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, StringFromPerturbationType)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::types::Integer ;
    using ostk::core::types::String ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {
        
        EXPECT_TRUE(Propagated::StringFromGravitationalPerturbationType(Propagated::GravitationalPerturbationType::None) == "None") ; 
        EXPECT_TRUE(Propagated::StringFromGravitationalPerturbationType(Propagated::GravitationalPerturbationType::J2) == "J2") ; 
        EXPECT_TRUE(Propagated::StringFromGravitationalPerturbationType(Propagated::GravitationalPerturbationType::TenByTen) == "TenByTen") ; 
        EXPECT_TRUE(Propagated::StringFromGravitationalPerturbationType(Propagated::GravitationalPerturbationType::FourtyByFourty) == "FourtyByFourty") ; 

    }

    {
        
        EXPECT_TRUE(Propagated::StringFromAtmosphericPerturbationType(Propagated::AtmosphericPerturbationType::None) == "None") ; 
        EXPECT_TRUE(Propagated::StringFromAtmosphericPerturbationType(Propagated::AtmosphericPerturbationType::Exponential) == "Exponential") ; 
        EXPECT_TRUE(Propagated::StringFromAtmosphericPerturbationType(Propagated::AtmosphericPerturbationType::JacchiaRoberts) == "JacchiaRoberts") ; 
        EXPECT_TRUE(Propagated::StringFromAtmosphericPerturbationType(Propagated::AtmosphericPerturbationType::NRLMISIS00) == "NRLMISIS00") ; 

    }

    {
        
        EXPECT_TRUE(Propagated::StringFromThirdBodyPerturbationType(Propagated::ThirdBodyPerturbationType::None) == "None") ; 
        EXPECT_TRUE(Propagated::StringFromThirdBodyPerturbationType(Propagated::ThirdBodyPerturbationType::Luni) == "Luni") ;
        EXPECT_TRUE(Propagated::StringFromThirdBodyPerturbationType(Propagated::ThirdBodyPerturbationType::Solar) == "Solar") ;
        EXPECT_TRUE(Propagated::StringFromThirdBodyPerturbationType(Propagated::ThirdBodyPerturbationType::LuniSolar) == "LuniSolar") ;

    }

    {
        
        EXPECT_TRUE(Propagated::StringFromIntegrationStepperType(Propagated::IntegrationStepperType::RungeKuttaCashKarp54) == "RungeKuttaCashKarp54") ; 

    }

    {
        
        EXPECT_TRUE(Propagated::StringFromIntegrationLogType(Propagated::IntegrationLogType::NoLog) == "NoLog") ; 
        EXPECT_TRUE(Propagated::StringFromIntegrationLogType(Propagated::IntegrationLogType::LogConstant) == "LogConstant") ; 
        EXPECT_TRUE(Propagated::StringFromIntegrationLogType(Propagated::IntegrationLogType::LogAdaptive) == "LogAdaptive") ; 
    }
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* VALIDATION TESTS */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropagationAccuracyTwoBody) // @BOSS will be modified to compare propagation accuracy using csv file produced by STK
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {        
        // Environment setup
        const Environment environment = Environment::Default() ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        
        // Current state and instant setup
        
        const Instant epochInstant = Instant::DateTime(DateTime::Parse("2021-06-30 17:26:46.742"), Scale::UTC) ; // Launch time
        
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2022-03-05 13:26:46.742"), Scale::UTC) ;

        Duration propDuration = Duration::Seconds(11345.0) ;

        std::vector<double> xStart(6);
        xStart[0] = -1260836.090852064; xStart[1] = 588280.4487875753; xStart[2] = 6745839.542136152; 
        xStart[3] = 7231.630244016249; xStart[4] = 2060.732438433088;  xStart[5] = 1167.816624957812;

        const Vector3d propPosition_refGCRF = {-1575658.863277743, 497462.0623771324, 6686886.954716642};
        const Vector3d propVelocity_refGCRF = {7155.923407265891, 2089.711281165372, 1526.340363065099};

        const State state = { startInstant, Position::Meters({ xStart[0], xStart[1], xStart[2] }, gcrfSPtr), Velocity::MetersPerSecond({ xStart[3], xStart[4], xStart[5] }, gcrfSPtr) };

        // Orbital model setup
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius ;

        const Propagated propagatedModel = { state, epochInstant, gravitationalParameter, equatorialRadius, Propagated::GravitationalPerturbationType::None, Propagated::AtmosphericPerturbationType::None, Propagated::ThirdBodyPerturbationType::None } ;
        
        // Orbit setup

        const Orbit orbit = { propagatedModel, environment.accessCelestialObjectWithName("Earth") } ;

        const State state_GCRF = orbit.getStateAt(startInstant + propDuration) ;
        
        const int revolutionNumber = orbit.getRevolutionNumberAt(startInstant + propDuration) ;
        
        // Results check 

        const Position position_GCRF = state_GCRF.accessPosition() ;
        const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

        const Real statePositionError_GCRF = (position_GCRF.accessCoordinates() - propPosition_refGCRF).norm() ;
        const Real stateVelocityError_GCRF = (velocity_GCRF.accessCoordinates() - propVelocity_refGCRF).norm() ;


        // Results console output

        std::cout << "**************************************" << std::endl;
        std::cout.setf(std::ios::scientific,std::ios::floatfield);
        std::cout << "Position error is: " << statePositionError_GCRF << "m" << std::endl;
        std::cout << "Velocity error is: " << stateVelocityError_GCRF <<  "m/s" << std::endl;
        std::cout.setf(std::ios::fixed,std::ios::floatfield);
        std::cout << "**************************************" << std::endl;
        

        // ASSERTS
        ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
        ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

        ASSERT_GT(1e-3, statePositionError_GCRF) ;
        ASSERT_GT(1e-6, stateVelocityError_GCRF) ;

        ASSERT_EQ(3763, revolutionNumber) ;

        // // Reference data setup

        // const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Orbit.csv")), Table::Format::CSV, true) ;

        // // Orbit test

        // for (const auto& referenceRow : referenceData)
        // {

        //     const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;

        //     const Vector3d propPosition_refGCRF = { referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal() } ;
        //     const Vector3d propVelocity_refGCRF = { referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal() } ;

        //     const Vector3d referencePosition_ITRF = { referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal() } ;
        //     const Vector3d referenceVelocity_ITRF = { referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal() } ;

        //     const Real referenceRevolutionNumber = referenceRow[13].accessReal() ;

        //     const State state_GCRF = orbit.getStateAt(instant) ;

        //     const Position position_GCRF = state_GCRF.accessPosition() ;
        //     const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

        //     ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
        //     ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

        //     ASSERT_GT(1e-3, (position_GCRF.accessCoordinates() - propPosition_refGCRF).norm()) ;
        //     ASSERT_GT(1e-6, (velocity_GCRF.accessCoordinates() - propVelocity_refGCRF).norm()) ;

        //     const Shared<const Frame> itrfFrame = Frame::ITRF() ;

        //     const State state_ITRF = state_GCRF.inFrame(itrfFrame) ;

        //     const Position position_ITRF = state_ITRF.accessPosition() ;
        //     const Velocity velocity_ITRF = state_ITRF.accessVelocity() ;

        //     ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame()) ;
        //     ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame()) ;

        //     ASSERT_GT(1e-1, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm()) ;
        //     ASSERT_GT(1e-0, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm()) ;

        //     ASSERT_EQ(referenceRevolutionNumber.floor(), orbit.getRevolutionNumberAt(instant)) ;

        // }


    }

}

