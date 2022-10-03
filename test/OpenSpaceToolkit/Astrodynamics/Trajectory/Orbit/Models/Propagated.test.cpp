////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.test.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

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
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

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
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // Constructor without putting into orbit object
    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // State array setup
        Array<State> stateArray = Array<State>::Empty() ;
        const Instant instant_0 = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const State state_0 = { instant_0, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };
        stateArray.add(state_0) ;
        const Instant instant_1 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC) ;
        const State state_1 = { instant_1, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };
        stateArray.add(state_1) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state_0 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        EXPECT_NO_THROW(Propagated( satelliteDynamics , numericalSolver )) ;
        EXPECT_NO_THROW(Propagated( satelliteDynamics , numericalSolver, stateArray )) ;

    }

    // Constructor with putting into orbit object
    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // State array setup
        Array<State> stateArray = Array<State>::Empty() ;
        const Instant instant_0 = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const State state_0 = { instant_0, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };
        stateArray.add(state_0) ;
        const Instant instant_1 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC) ;
        const State state_1 = { instant_1, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };
        stateArray.add(state_1) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state_0 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Propagated propagatedModel_0 = {satelliteDynamics , numericalSolver, stateArray} ;

        // Setup Orbit model
        EXPECT_NO_THROW(Orbit( propagatedModel , defaultEnvironment.accessCelestialObjectWithName("Earth") )) ;
        EXPECT_NO_THROW(Orbit( propagatedModel_0 , defaultEnvironment.accessCelestialObjectWithName("Earth") )) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, EqualToOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Propagated propagatedModel_x = { propagatedModel } ;
        EXPECT_TRUE(propagatedModel == propagatedModel_x) ;

        const NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        const Propagated propagatedModel_1 = {satelliteDynamics , numericalSolver_1} ;
        EXPECT_FALSE(propagatedModel == propagatedModel_1) ;

        // State array setup, state Array construction with array of length one and same start
        Array<State> stateArray = Array<State>::Empty() ;
        stateArray.add(state) ;
        const Propagated propagatedModel_2 = {satelliteDynamics , numericalSolver, stateArray} ;
        EXPECT_TRUE(propagatedModel == propagatedModel_2) ;

        // State array setup, state Array construction with array of length two, duplicate state inputs
        stateArray.add(state) ;
        const Propagated propagatedModel_3 = {satelliteDynamics , numericalSolver, stateArray} ;
        EXPECT_TRUE(propagatedModel == propagatedModel_3) ;

        // State array setup, state Array construction with array of length two and a different second state
        const Instant instant_0 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC) ;
        const State state_0 = { instant_0, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };
        stateArray.add(state_0) ;
        const Propagated propagatedModel_4 = {satelliteDynamics , numericalSolver, stateArray} ;
        EXPECT_FALSE(propagatedModel == propagatedModel_4) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, NotEqualToOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Propagated propagatedModel_x = { propagatedModel } ;
        EXPECT_FALSE(propagatedModel != propagatedModel_x) ;

        const NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        const Propagated propagatedModel_1 = {satelliteDynamics , numericalSolver_1} ;
        EXPECT_TRUE(propagatedModel != propagatedModel_1) ;

        // State array setup, state Array construction with array of length one and same start
        Array<State> stateArray = Array<State>::Empty() ;
        stateArray.add(state) ;
        const Propagated propagatedModel_2 = {satelliteDynamics , numericalSolver, stateArray} ;
        EXPECT_FALSE(propagatedModel != propagatedModel_2) ;

        // State array setup, state Array construction with array of length two, duplicate state inputs
        stateArray.add(state) ;
        const Propagated propagatedModel_3 = {satelliteDynamics , numericalSolver, stateArray} ;
        EXPECT_FALSE(propagatedModel != propagatedModel_3) ;

        // State array setup, state Array construction with array of length two and a different second state
        const Instant instant_0 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC) ;
        const State state_0 = { instant_0, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };
        stateArray.add(state_0) ;
        const Propagated propagatedModel_4 = {satelliteDynamics , numericalSolver, stateArray} ;
        EXPECT_TRUE(propagatedModel != propagatedModel_4) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, IsDefined)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;

        EXPECT_TRUE(propagatedModel.isDefined()) ;
    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, StreamOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << propagatedModel << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;
    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Print)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(propagatedModel.print(std::cout, true)) ;
        EXPECT_NO_THROW(propagatedModel.print(std::cout, false)) ;
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getEpoch)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;

        EXPECT_EQ(propagatedModel.getEpoch(),startInstant) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getRevolutionNumberAtEpoch)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state = { startInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;

        EXPECT_EQ(propagatedModel.getRevolutionNumberAtEpoch(),1) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateStateAt)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // Test CalculateStateAt in propagatedModel and in orbit
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

        // Current state and instant setup
        const State state = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver} ;

        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Setup instants
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC) ;
        const Instant instant_before1 = Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC) ;
        const Instant instant_after1 = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC) ;
        const Instant instant_after2 = Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC) ;

        // Check getStateAt vs calculateStateAt
        EXPECT_EQ(orbit.getStateAt(instant), propagatedModel.calculateStateAt(instant)) ;
        EXPECT_EQ(orbit.getStateAt(instant_before1), propagatedModel.calculateStateAt(instant_before1)) ;
        EXPECT_EQ(orbit.getStateAt(instant_after1), propagatedModel.calculateStateAt(instant_after1)) ;
        EXPECT_EQ(orbit.getStateAt(instant_after2), propagatedModel.calculateStateAt(instant_after2)) ;

    }

    // Test that CalculateStateAt is adding propagated state to the correct position in cachedStateArray
    {
        // Test Instant before cachedStateArray
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 4, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        // Calculate state at instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 23, 0, 0), Scale::UTC) ;

        const State propagatedState = propagatedModel.calculateStateAt(instant) ;
        // const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;
        // const State propagatedState = orbit.getStateAt(instant) ;

        // std::cout << propagatedState << std::endl ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_LT(cachedStateArray[2].getInstant(),cachedStateArray[3].getInstant()) ;

        EXPECT_EQ(cachedStateArray[0].getInstant(), instant) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test Instant after cachedStateArray
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 4, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        // Calculate state at instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 4, 1, 0, 0), Scale::UTC) ;
        const State propagatedState = propagatedModel.calculateStateAt(instant) ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_LT(cachedStateArray[2].getInstant(),cachedStateArray[3].getInstant()) ;

        EXPECT_EQ(cachedStateArray[3].getInstant(), instant) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test Instant in between first and second elements, but closer to first in cachedStateArray
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 4, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        // Calculate state at instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC) ;
        const State propagatedState = propagatedModel.calculateStateAt(instant) ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;
        // std::cout << cachedStateArray << std::endl ;

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_LT(cachedStateArray[2].getInstant(),cachedStateArray[3].getInstant()) ;

        EXPECT_EQ(cachedStateArray[1].getInstant(), instant) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test Instant in between first and second elements, but closer to second in cachedStateArray
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 4, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        // Calculate state at instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 2, 23, 0, 0), Scale::UTC) ;
        const State propagatedState = propagatedModel.calculateStateAt(instant) ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;
        // std::cout << cachedStateArray << std::endl ;

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_LT(cachedStateArray[2].getInstant(),cachedStateArray[3].getInstant()) ;

        EXPECT_EQ(cachedStateArray[1].getInstant(), instant) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test Instant equal to one of the existing instants in cachedStateArray
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 4, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        // Calculate state at instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC) ;
        const State propagatedState = propagatedModel.calculateStateAt(instant) ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;
        // std::cout << cachedStateArray << std::endl ;

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;

        EXPECT_EQ(cachedStateArray.getSize(), 3) ;
        EXPECT_EQ(cachedStateArray[1].getInstant(), instant) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test Instant equal to one of the existing instants in cachedStateArray
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

        // Current state and instant setup
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver} ;

        // Calculate state at instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC) ;
        const State propagatedState = propagatedModel.calculateStateAt(instant) ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;
        // std::cout << cachedStateArray << std::endl ;

        // Check array
        EXPECT_EQ(cachedStateArray[0].getInstant(), instant) ;
        EXPECT_EQ(cachedStateArray.getSize(), 1) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateStatesAt)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // Test only instant sorting and insertion into cachedStateArray

        // Test correct handling of state array sorting and unsorting in propagatedModel and in Orbit
        {
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Create environment
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

            // Current state and instant setup
            const State state = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and orbit
            const Propagated propagatedModel = {satelliteDynamics, numericalSolver} ;

            const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

            // Setup instants
            Array<Instant> instantArray = Array<Instant>::Empty() ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC)) ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC)) ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC)) ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC)) ;

            Array<State> orbitPropagatedStateArray = orbit.getStatesAt(instantArray) ;
            Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            // Check getStatesAt vs calculateStatesAt
            EXPECT_EQ(orbitPropagatedStateArray, propagatedStateArray) ;

            // Check that the states are in the same order from the original instant array that was supplied
            EXPECT_EQ(instantArray[0], orbitPropagatedStateArray[0].getInstant()) ;
            EXPECT_EQ(instantArray[1], orbitPropagatedStateArray[1].getInstant()) ;
            EXPECT_EQ(instantArray[2], orbitPropagatedStateArray[2].getInstant()) ;
            EXPECT_EQ(instantArray[3], orbitPropagatedStateArray[3].getInstant()) ;

        }

        // Test interlocking of new states into cachedStateArray in terms of instants
        {
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Create environment
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Setup cachedStateArray
            Array<State> cachedStateArray = Array<State>::Empty() ;
            const State state_1 = { Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
            cachedStateArray.add(state_1) ;
            const State state_2 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
            cachedStateArray.add(state_2) ;
            const State state_3 = { Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
            cachedStateArray.add(state_3) ;

            // Setup instant Array
            Array<Instant> instantArray = Array<Instant>::Empty() ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 1, 23, 0, 0), Scale::UTC)) ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC)) ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 3, 0, 0), Scale::UTC)) ;

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and orbit
            const Propagated propagatedModel = {satelliteDynamics, numericalSolver, cachedStateArray} ;

            Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            Array<State> propagatedCachedStateArray = propagatedModel.accessCachedStateArray() ;

            // Check that the cachedStateArray have the states in chronological order and that there is one of each kind
            EXPECT_EQ(propagatedCachedStateArray.getSize(), instantArray.getSize() + cachedStateArray.getSize()) ;

            EXPECT_EQ(propagatedCachedStateArray[0].getInstant(), cachedStateArray[0].getInstant()) ;
            EXPECT_EQ(propagatedCachedStateArray[1].getInstant(), instantArray[0]) ;
            EXPECT_EQ(propagatedCachedStateArray[2].getInstant(), cachedStateArray[1].getInstant()) ;
            EXPECT_EQ(propagatedCachedStateArray[3].getInstant(), instantArray[1]) ;
            EXPECT_EQ(propagatedCachedStateArray[4].getInstant(), cachedStateArray[2].getInstant()) ;
            EXPECT_EQ(propagatedCachedStateArray[5].getInstant(), instantArray[2]) ;

        }

        // Test edge case: requesting of duplicate instants that also match an instant already in the catched state array
        {
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Create environment
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Setup cachedStateArray
            Array<State> cachedStateArray = Array<State>::Empty() ;
            const State state_1 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
            cachedStateArray.add(state_1) ;

            // Setup instant Array
            Array<Instant> instantArray = Array<Instant>::Empty() ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC)) ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC)) ;
            instantArray.add(Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC)) ;

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and orbit
            const Propagated propagatedModel = {satelliteDynamics, numericalSolver, cachedStateArray} ;

            Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            Array<State> propagatedCachedStateArray = propagatedModel.accessCachedStateArray() ;

            // Check that the propagatedStateArray has the requested instants even if they are duplicates
            EXPECT_EQ(propagatedStateArray.getSize(), 3 ) ;
            EXPECT_EQ(propagatedStateArray[0].getInstant(), instantArray[0]) ;
            EXPECT_EQ(propagatedStateArray[1].getInstant(), instantArray[1]) ;
            EXPECT_EQ(propagatedStateArray[2].getInstant(), instantArray[2]) ;

            // Check that the cachedStateArray have the states in chronological order and that there is one of each kind
            EXPECT_EQ(propagatedCachedStateArray.getSize(), 2 ) ;
            EXPECT_EQ(propagatedCachedStateArray[0].getInstant(), cachedStateArray[0].getInstant()) ;
            EXPECT_EQ(propagatedCachedStateArray[1].getInstant(), instantArray.back()) ;

        }

    // Test full state results against reference trajectory
        // Test cachedStateArray with one cachedState at the beginning of reference states
        {
            // Reference data setup
            const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/CalculateStatesAt_StateValidation.csv")), Table::Format::CSV, true) ;

            Array<Instant> instantArray = Array<Instant>::Empty() ;
            Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty() ;
            Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty() ;

            for (const auto& referenceRow : referenceData)
            {
                    instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC)) ;

                    referencePositionArray.add( {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()} ) ;
                    referenceVelocityArray.add( {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()} ) ;
            }

            // Create environment
            const Instant instantJ2000 = Instant::J2000() ;
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(instantJ2000, objects) ;
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Choose state to put into cachedStateArray
            size_t cachedStateReferenceIndex = 0 ;
            const Instant startInstant = instantArray[cachedStateReferenceIndex] ;
            const State state = { startInstant, Position::Meters({ referencePositionArray[cachedStateReferenceIndex] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray[cachedStateReferenceIndex] }, gcrfSPtr) } ;

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and perform propagation
            const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
            const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            // Since none of the reference states used are duplicates, the cached state array after propagation should be the same length as all the reference states
            EXPECT_EQ(propagatedModel.accessCachedStateArray().getSize(), instantArray.getSize()) ;

            // Validation loop
            for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
            {

                const Position position = propagatedStateArray[i].accessPosition() ;
                const Velocity velocity = propagatedStateArray[i].accessVelocity() ;

                const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm() ;
                const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm() ;

                ASSERT_GT(2e-7, positionError) ;
                ASSERT_GT(2e-10, velocityError) ;
                ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]) ;

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
            // Reference data setup
            const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/CalculateStatesAt_StateValidation.csv")), Table::Format::CSV, true) ;

            Array<Instant> instantArray = Array<Instant>::Empty() ;
            Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty() ;
            Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty() ;

            for (const auto& referenceRow : referenceData)
            {
                    instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC)) ;

                    referencePositionArray.add( {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()} ) ;
                    referenceVelocityArray.add( {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()} ) ;
            }

            // Create environment
            const Instant instantJ2000 = Instant::J2000() ;
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(instantJ2000, objects) ;
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Choose state to put into cachedStateArray
            size_t cachedStateReferenceIndex = 30 ;
            const Instant startInstant = instantArray[cachedStateReferenceIndex] ;
            const State state = { startInstant, Position::Meters({ referencePositionArray[cachedStateReferenceIndex] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray[cachedStateReferenceIndex] }, gcrfSPtr) } ;

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and perform propagation
            const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
            const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            // Since none of the reference states used are duplicates, the cached state array after propagation should be the same length as all the reference states
            EXPECT_EQ(propagatedModel.accessCachedStateArray().getSize(), instantArray.getSize()) ;

            // Validation loop
            for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
            {

                const Position position = propagatedStateArray[i].accessPosition() ;
                const Velocity velocity = propagatedStateArray[i].accessVelocity() ;

                const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm() ;
                const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm() ;

                ASSERT_GT(2e-7, positionError) ;
                ASSERT_GT(2e-10, velocityError) ;
                ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]) ;

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
            // Reference data setup
            const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/CalculateStatesAt_StateValidation.csv")), Table::Format::CSV, true) ;

            Array<Instant> instantArray = Array<Instant>::Empty() ;
            Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty() ;
            Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty() ;

            for (const auto& referenceRow : referenceData)
            {
                    instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC)) ;

                    referencePositionArray.add( {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()} ) ;
                    referenceVelocityArray.add( {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()} ) ;
            }

            // Create environment
            const Instant instantJ2000 = Instant::J2000() ;
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(instantJ2000, objects) ;
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Choose state to put into cachedStateArray
            size_t cachedStateReferenceIndex = 60 ;
            const Instant startInstant = instantArray[cachedStateReferenceIndex] ;
            const State state = { startInstant, Position::Meters({ referencePositionArray[cachedStateReferenceIndex] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray[cachedStateReferenceIndex] }, gcrfSPtr) } ;

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and perform propagation
            const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
            const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            // Since none of the reference states used are duplicates, the cached state array after propagation should be the same length as all the reference states
            EXPECT_EQ(propagatedModel.accessCachedStateArray().getSize(), instantArray.getSize()) ;

            // Validation loop
            for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
            {

                const Position position = propagatedStateArray[i].accessPosition() ;
                const Velocity velocity = propagatedStateArray[i].accessVelocity() ;

                const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm() ;
                const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm() ;

                ASSERT_GT(2e-7, positionError) ;
                ASSERT_GT(2e-10, velocityError) ;
                ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]) ;

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
            // Reference data setup
            const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/CalculateStatesAt_StateValidation.csv")), Table::Format::CSV, true) ;

            Array<Instant> instantArray = Array<Instant>::Empty() ;
            Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty() ;
            Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty() ;

            for (const auto& referenceRow : referenceData)
            {
                    instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC)) ;

                    referencePositionArray.add( {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()} ) ;
                    referenceVelocityArray.add( {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()} ) ;
            }

            // Create environment
            const Instant instantJ2000 = Instant::J2000() ;
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(instantJ2000, objects) ;
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Choose states to put into cachedStateArray
            Array<State> cachedStateArray = Array<State>::Empty() ;
            size_t cachedStateReferenceIndex_1 = 0 ;
            const State state_1 = { instantArray[cachedStateReferenceIndex_1], Position::Meters({ referencePositionArray[cachedStateReferenceIndex_1] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray[cachedStateReferenceIndex_1] }, gcrfSPtr) } ;
            cachedStateArray.add(state_1) ;
            size_t cachedStateReferenceIndex_2 = 60 ;
            const State state_2 = { instantArray[cachedStateReferenceIndex_2], Position::Meters({ referencePositionArray[cachedStateReferenceIndex_2] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray[cachedStateReferenceIndex_2] }, gcrfSPtr) } ;
            cachedStateArray.add(state_2) ;

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state_1 } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and perform propagation
            const Propagated propagatedModel = {satelliteDynamics , numericalSolver, cachedStateArray} ;
            const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            // Since none of the reference states used are duplicates, the cached state array after propagation should be the same length as all the reference states
            EXPECT_EQ(propagatedModel.accessCachedStateArray().getSize(), instantArray.getSize()) ;

            // Validation loop
            for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
            {

                const Position position = propagatedStateArray[i].accessPosition() ;
                const Velocity velocity = propagatedStateArray[i].accessVelocity() ;

                const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm() ;
                const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm() ;

                ASSERT_GT(2e-7, positionError) ;
                ASSERT_GT(2e-10, velocityError) ;
                ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]) ;

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
            // Reference data setup
            const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/CalculateStatesAt_StateValidation.csv")), Table::Format::CSV, true) ;

            Array<Instant> instantArray = Array<Instant>::Empty() ;
            Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty() ;
            Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty() ;

            for (const auto& referenceRow : referenceData)
            {
                    instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC)) ;

                    referencePositionArray.add( {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()} ) ;
                    referenceVelocityArray.add( {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()} ) ;
            }

            // Create environment
            const Instant instantJ2000 = Instant::J2000() ;
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(instantJ2000, objects) ;
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Choose states to put into cachedStateArray
            Array<State> cachedStateArray = Array<State>::Empty() ;
            for (size_t i = 0; i < instantArray.getSize(); i++)
            {
                if (i % 2 == 0)
                {
                    const State state = { instantArray[i], Position::Meters({ referencePositionArray[i] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray[i] }, gcrfSPtr) } ;
                    cachedStateArray.add(state) ;
                }
            }

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, cachedStateArray[0] } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and perform propagation
            const Propagated propagatedModel = {satelliteDynamics , numericalSolver, cachedStateArray} ;
            const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            // Since none of the reference states used are duplicates, the cached state array after propagation should be the same length as all the reference states
            EXPECT_EQ(propagatedModel.accessCachedStateArray().getSize(), instantArray.getSize()) ;

            // Validation loop
            for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
            {

                const Position position = propagatedStateArray[i].accessPosition() ;
                const Velocity velocity = propagatedStateArray[i].accessVelocity() ;

                const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm() ;
                const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm() ;

                ASSERT_GT(2e-7, positionError) ;
                ASSERT_GT(2e-10, velocityError) ;
                ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]) ;

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
            // Reference data setup
            const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/CalculateStatesAt_StateValidation.csv")), Table::Format::CSV, true) ;

            Array<Instant> instantArray = Array<Instant>::Empty() ;
            Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty() ;
            Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty() ;

            for (const auto& referenceRow : referenceData)
            {
                    instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC)) ;

                    referencePositionArray.add( {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()} ) ;
                    referenceVelocityArray.add( {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()} ) ;
            }

            // Create environment
            const Instant instantJ2000 = Instant::J2000() ;
            const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

            const Environment customEnvironment = Environment(instantJ2000, objects) ;
            const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            // Satellite system setup
            const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
            const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

            // Choose states to put into cachedStateArray
            Array<State> cachedStateArray = Array<State>::Empty() ;
            for (size_t i = 0; i < instantArray.getSize(); i++)
            {
                const State state = { instantArray[i], Position::Meters({ referencePositionArray[i] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray[i] }, gcrfSPtr) } ;
                cachedStateArray.add(state) ;
            }

            // Satellite dynamics setup
            SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, cachedStateArray[0] } ;

            // Construct default numerical solver
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

            // Setup Propagated model and perform propagation
            const Propagated propagatedModel = {satelliteDynamics , numericalSolver, cachedStateArray} ;
            const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray) ;

            // Since none of the reference states used are duplicates, the cached state array after propagation should be the same length as all the reference states
            EXPECT_EQ(propagatedModel.accessCachedStateArray().getSize(), instantArray.getSize()) ;

            // Validation loop
            for (size_t i = 0; i < propagatedStateArray.getSize(); i++)
            {

                const Position position = propagatedStateArray[i].accessPosition() ;
                const Velocity velocity = propagatedStateArray[i].accessVelocity() ;

                const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm() ;
                const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm() ;

                ASSERT_GT(2e-7, positionError) ;
                ASSERT_GT(2e-10, velocityError) ;
                ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]) ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateRevolutionNumberAt)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;
    // [TBI] add more stringent Rev Number tests
    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

        // Current state and instant setup
        const State state = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver} ;

        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Calculate revolution number at instant
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC) ;
        const Instant instant_before1 = Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC) ;
        const Instant instant_after1 = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC) ;
        const Instant instant_after2 = Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC) ;

        // Check revolution numbers for propagated model
        EXPECT_EQ(1, propagatedModel.calculateRevolutionNumberAt(instant)) ;
        EXPECT_EQ(-1, propagatedModel.calculateRevolutionNumberAt(instant_before1)) ;
        EXPECT_EQ(1, propagatedModel.calculateRevolutionNumberAt(instant_after1)) ;
        EXPECT_EQ(2, propagatedModel.calculateRevolutionNumberAt(instant_after2)) ;

        // Check revolution numbers for orbit
        EXPECT_EQ(1, orbit.getRevolutionNumberAt(instant)) ;
        EXPECT_EQ(-1, orbit.getRevolutionNumberAt(instant_before1)) ;
        EXPECT_EQ(1, orbit.getRevolutionNumberAt(instant_after1)) ;
        EXPECT_EQ(2, orbit.getRevolutionNumberAt(instant_after2)) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CachedStateArray)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // Test cachedStateArray sorting during construction and accessing (not during propagation)
    {
        // Test valid and chronological state array input
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_EQ(cachedStateArray.getSize(),3) ;
        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test valid and chronological state array input with a different state added as part of SatelliteDynamics
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        const State state_4 = { Instant::DateTime(DateTime(2018, 1, 4, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state_4 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_EQ(cachedStateArray.getSize(),4) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test sorting of non chronological but valid state array
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_LT(cachedStateArray[1].getInstant(),cachedStateArray[2].getInstant()) ;
        EXPECT_EQ(cachedStateArray.getSize(),3) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test duplicated non chronological state array input
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = state_1 ;
        stateArray.add(state_2) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics, numericalSolver, stateArray} ;

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;

        // Check array chronological order and that duplicate was removed
        EXPECT_LT(cachedStateArray[0].getInstant(),cachedStateArray[1].getInstant()) ;
        EXPECT_EQ(cachedStateArray.getSize(),2) ;

        EXPECT_NO_THROW(propagatedModel) ;

    }

    {
        // Test duplicated instants but not coordinates in state array input
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Create environment
        const Environment defaultEnvironment = Environment::Default() ;

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty() ;
        const State state_3 = { Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_3) ;
        const State state_1 = { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_1) ;
        const State state_2 = { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 8000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) };
        stateArray.add(state_2) ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { defaultEnvironment, satelliteSystem, state_1 } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Check that warning is received
        EXPECT_ANY_THROW(Propagated(satelliteDynamics, numericalSolver, stateArray)) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* VALIDATION TESTS */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Force model validation tests */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // Two body vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_TwoBody_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(2e-3, positionError_GCRF) ;
            ASSERT_GT(2e-6, velocityError_GCRF) ;


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
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_TwoBody_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referencePositionArray_ITRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_ITRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC)) ;

                referencePositionArray_GCRF.add( {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()} ) ;
                referenceVelocityArray_GCRF.add( {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()} ) ;

                referencePositionArray_ITRF.add( {referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()} ) ;
                referenceVelocityArray_ITRF.add( {referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()} ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;
        const Shared<const Frame> itrfSPtr = Frame::ITRF() ;

        // Current state and instant setup
        const Instant startInstant = instantArray[0] ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(2e-7, positionError_GCRF) ;
            ASSERT_GT(2e-10, velocityError_GCRF) ;

            // ITRF Compare
            const Position position_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).accessPosition() ;
            const Velocity velocity_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).accessVelocity() ;

            const double positionError_ITRF = (position_ITRF.accessCoordinates() - referencePositionArray_ITRF[i]).norm() ;
            const double velocityError_ITRF = (velocity_ITRF.accessCoordinates() - referenceVelocityArray_ITRF[i]).norm() ;

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame()) ;
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame()) ;

            ASSERT_GT(2e-2, positionError_ITRF) ;
            ASSERT_GT(2e-5, velocityError_ITRF) ;


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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_EGM2008 )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // EGM2008 100x100 perturbation only vs STK EGM2008
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_EGM2008_100x100_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::EGM2008(100, 100)) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(1.5e-1, positionError_GCRF) ;
            ASSERT_GT(1.5e-4, velocityError_GCRF) ;


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
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_EGM2008_100x100_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects_360 = { std::make_shared<Earth>(Earth::EGM2008(360, 360)) } ;
        const Array<Shared<Object>> objects_100 = { std::make_shared<Earth>(Earth::EGM2008(100, 100)) } ;
        const Array<Shared<Object>> objects_70 = { std::make_shared<Earth>(Earth::EGM2008(70, 70)) } ;
        const Array<Shared<Object>> objects_45 = { std::make_shared<Earth>(Earth::EGM2008(45, 45)) } ;

        const Environment customEnvironment_360 = Environment(instantJ2000, objects_360) ;
        const Environment customEnvironment_100 = Environment(instantJ2000, objects_100) ;
        const Environment customEnvironment_70 = Environment(instantJ2000, objects_70) ;
        const Environment customEnvironment_45 = Environment(instantJ2000, objects_45) ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics_360 = { customEnvironment_360, satelliteSystem, state } ;
        SatelliteDynamics satelliteDynamics_100 = { customEnvironment_100, satelliteSystem, state } ;
        SatelliteDynamics satelliteDynamics_70 = { customEnvironment_70, satelliteSystem, state } ;
        SatelliteDynamics satelliteDynamics_45 = { customEnvironment_45, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel_360 = {satelliteDynamics_360 , numericalSolver} ;
        const Propagated propagatedModel_100 = {satelliteDynamics_100 , numericalSolver} ;
        const Propagated propagatedModel_70 = {satelliteDynamics_70 , numericalSolver} ;
        const Propagated propagatedModel_45 = {satelliteDynamics_45 , numericalSolver} ;

        const Orbit orbit_360 = { propagatedModel_360, customEnvironment_360.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_100 = { propagatedModel_100, customEnvironment_100.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_70 = { propagatedModel_70, customEnvironment_70.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_45 = { propagatedModel_45, customEnvironment_45.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray_360 = orbit_360.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_100 = orbit_100.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_70 = orbit_70.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_45 = orbit_45.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF_360 = propagatedStateArray_360[i].accessPosition() ;
            const Velocity velocity_GCRF_360 = propagatedStateArray_360[i].accessVelocity() ;
            const Position position_GCRF_100 = propagatedStateArray_100[i].accessPosition() ;
            const Velocity velocity_GCRF_100 = propagatedStateArray_100[i].accessVelocity() ;
            const Position position_GCRF_70 = propagatedStateArray_70[i].accessPosition() ;
            const Velocity velocity_GCRF_70 = propagatedStateArray_70[i].accessVelocity() ;
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition() ;
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity() ;

            // 360 vs 100
            const double positionError_GCRF_360_100 = (position_GCRF_360.accessCoordinates() - position_GCRF_100.accessCoordinates()).norm() ;
            const double velocityError_GCRF_360_100 = (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_100.accessCoordinates()).norm() ;
            ASSERT_GT(2e-3, positionError_GCRF_360_100) ;
            ASSERT_GT(2e-6, velocityError_GCRF_360_100) ;

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 100 Position error is: " << positionError_GCRF_360_100 << "m" << std::endl;
            // std::cout << "360 vs 100 Velocity error is: " << velocityError_GCRF_360_100 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 70
            const double positionError_GCRF_360_70 = (position_GCRF_360.accessCoordinates() - position_GCRF_70.accessCoordinates()).norm() ;
            const double velocityError_GCRF_360_70 = (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_70.accessCoordinates()).norm() ;
            ASSERT_GT(5e-2, positionError_GCRF_360_70) ;
            ASSERT_GT(5e-5, velocityError_GCRF_360_70) ;

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 70 Position error is: " << positionError_GCRF_360_70 << "m" << std::endl;
            // std::cout << "360 vs 70 Velocity error is: " << velocityError_GCRF_360_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionError_GCRF_360_45 = (position_GCRF_360.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm() ;
            const double velocityError_GCRF_360_45 = (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm() ;
            ASSERT_GT(5e-1, positionError_GCRF_360_45) ;
            ASSERT_GT(5e-4, velocityError_GCRF_360_45) ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_EGM96 )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;


    // EGM96 360x360 perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_EGM96_360x360_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::EGM96(360, 360)) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(9e-2, positionError_GCRF) ;
            ASSERT_GT(9e-5, velocityError_GCRF) ;


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
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_WGS84EGM96_70x70_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::EGM96(70, 70)) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(1.5e-4, positionError_GCRF) ;
            ASSERT_GT(1.5e-7, velocityError_GCRF) ;


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
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_EGM96_360x360_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects_360 = { std::make_shared<Earth>(Earth::EGM96(360, 360)) } ;
        const Array<Shared<Object>> objects_180 = { std::make_shared<Earth>(Earth::EGM96(180, 180)) } ;
        const Array<Shared<Object>> objects_90 = { std::make_shared<Earth>(Earth::EGM96(90, 90)) } ;
        const Array<Shared<Object>> objects_45 = { std::make_shared<Earth>(Earth::EGM96(45, 45)) } ;

        const Environment customEnvironment_360 = Environment(instantJ2000, objects_360) ;
        const Environment customEnvironment_180 = Environment(instantJ2000, objects_180) ;
        const Environment customEnvironment_90 = Environment(instantJ2000, objects_90) ;
        const Environment customEnvironment_45 = Environment(instantJ2000, objects_45) ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics_360 = { customEnvironment_360, satelliteSystem, state } ;
        SatelliteDynamics satelliteDynamics_180 = { customEnvironment_180, satelliteSystem, state } ;
        SatelliteDynamics satelliteDynamics_90 = { customEnvironment_90, satelliteSystem, state } ;
        SatelliteDynamics satelliteDynamics_45 = { customEnvironment_45, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel_360 = {satelliteDynamics_360 , numericalSolver} ;
        const Propagated propagatedModel_180 = {satelliteDynamics_180 , numericalSolver} ;
        const Propagated propagatedModel_90 = {satelliteDynamics_90 , numericalSolver} ;
        const Propagated propagatedModel_45 = {satelliteDynamics_45 , numericalSolver} ;

        const Orbit orbit_360 = { propagatedModel_360, customEnvironment_360.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_180 = { propagatedModel_180, customEnvironment_180.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_90 = { propagatedModel_90, customEnvironment_90.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_45 = { propagatedModel_45, customEnvironment_45.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray_360 = orbit_360.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_180 = orbit_180.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_90 = orbit_90.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_45 = orbit_45.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF_360 = propagatedStateArray_360[i].accessPosition() ;
            const Velocity velocity_GCRF_360 = propagatedStateArray_360[i].accessVelocity() ;
            const Position position_GCRF_180 = propagatedStateArray_180[i].accessPosition() ;
            const Velocity velocity_GCRF_180 = propagatedStateArray_180[i].accessVelocity() ;
            const Position position_GCRF_90 = propagatedStateArray_90[i].accessPosition() ;
            const Velocity velocity_GCRF_90 = propagatedStateArray_90[i].accessVelocity() ;
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition() ;
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity() ;

            // 360 vs 180
            const double positionError_GCRF_360_180 = (position_GCRF_360.accessCoordinates() - position_GCRF_180.accessCoordinates()).norm() ;
            const double velocityError_GCRF_360_180 = (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_180.accessCoordinates()).norm() ;
            ASSERT_GT(9e-7, positionError_GCRF_360_180) ;
            ASSERT_GT(9e-10, velocityError_GCRF_360_180) ;

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 180 Position error is: " << positionError_GCRF_360_180 << "m" << std::endl;
            // std::cout << "360 vs 180 Velocity error is: " << velocityError_GCRF_360_180 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;


            // 360 vs 90
            const double positionError_GCRF_360_90 = (position_GCRF_360.accessCoordinates() - position_GCRF_90.accessCoordinates()).norm() ;
            const double velocityError_GCRF_360_90 = (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_90.accessCoordinates()).norm() ;
            ASSERT_GT(7e-3, positionError_GCRF_360_90) ;
            ASSERT_GT(7e-6, velocityError_GCRF_360_90) ;

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 90 Position error is: " << positionError_GCRF_360_90 << "m" << std::endl;
            // std::cout << "360 vs 90 Velocity error is: " << velocityError_GCRF_360_90 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;


            // 360 vs 45
            const double positionError_GCRF_360_45 = (position_GCRF_360.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm() ;
            const double velocityError_GCRF_360_45 = (velocity_GCRF_360.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm() ;
            ASSERT_GT(5e-1, positionError_GCRF_360_45) ;
            ASSERT_GT(5e-4, velocityError_GCRF_360_45) ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_EGM84 )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // EGM84 70x70 perturbation only vs STK EGM84
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_WGS84_70x70_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::EGM84(70, 70)) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(7e-5, positionError_GCRF) ;
            ASSERT_GT(7e-8, velocityError_GCRF) ;


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
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_WGS84_70x70_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects_180 = { std::make_shared<Earth>(Earth::EGM84(180, 180)) } ;
        const Array<Shared<Object>> objects_70 = { std::make_shared<Earth>(Earth::EGM84(70, 70)) } ;
        const Array<Shared<Object>> objects_45 = { std::make_shared<Earth>(Earth::EGM84(45, 45)) } ;

        const Environment customEnvironment_180 = Environment(instantJ2000, objects_180) ;
        const Environment customEnvironment_70 = Environment(instantJ2000, objects_70) ;
        const Environment customEnvironment_45 = Environment(instantJ2000, objects_45) ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics_180 = { customEnvironment_180, satelliteSystem, state } ;
        SatelliteDynamics satelliteDynamics_70 = { customEnvironment_70, satelliteSystem, state } ;
        SatelliteDynamics satelliteDynamics_45 = { customEnvironment_45, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel_180 = {satelliteDynamics_180 , numericalSolver} ;
        const Propagated propagatedModel_70 = {satelliteDynamics_70 , numericalSolver} ;
        const Propagated propagatedModel_45 = {satelliteDynamics_45 , numericalSolver} ;

        const Orbit orbit_180 = { propagatedModel_180, customEnvironment_180.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_70 = { propagatedModel_70, customEnvironment_70.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_45 = { propagatedModel_45, customEnvironment_45.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray_180 = orbit_180.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_70 = orbit_70.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_45 = orbit_45.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF_180 = propagatedStateArray_180[i].accessPosition() ;
            const Velocity velocity_GCRF_180 = propagatedStateArray_180[i].accessVelocity() ;
            const Position position_GCRF_70 = propagatedStateArray_70[i].accessPosition() ;
            const Velocity velocity_GCRF_70 = propagatedStateArray_70[i].accessVelocity() ;
            const Position position_GCRF_45 = propagatedStateArray_45[i].accessPosition() ;
            const Velocity velocity_GCRF_45 = propagatedStateArray_45[i].accessVelocity() ;

            // 180 vs 70
            const double positionError_GCRF_180_70 = (position_GCRF_180.accessCoordinates() - position_GCRF_70.accessCoordinates()).norm() ;
            const double velocityError_GCRF_180_70 = (velocity_GCRF_180.accessCoordinates() - velocity_GCRF_70.accessCoordinates()).norm() ;
            ASSERT_GT(6e-2, positionError_GCRF_180_70) ;
            ASSERT_GT(6e-5, velocityError_GCRF_180_70) ;

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 70 Position error is: " << positionError_GCRF_180_70 << "m" << std::endl;
            // std::cout << "180 vs 70 Velocity error is: " << velocityError_GCRF_180_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;


            // 180 vs 45
            const double positionError_GCRF_180_45 = (position_GCRF_180.accessCoordinates() - position_GCRF_45.accessCoordinates()).norm() ;
            const double velocityError_GCRF_180_45 = (velocity_GCRF_180.accessCoordinates() - velocity_GCRF_45.accessCoordinates()).norm() ;
            ASSERT_GT(3e-1, positionError_GCRF_180_45) ;
            ASSERT_GT(3e-4, velocityError_GCRF_180_45) ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_Sun_Moon )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // Sun+Moon perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_TwoBody_Sun_Moon_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects =
        {
            std::make_shared<Earth>(Earth::Spherical()),
            std::make_shared<Moon>(Moon::Spherical()),
            std::make_shared<Sun>(Sun::Spherical())
        } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(2e-3, positionError_GCRF) ;
            ASSERT_GT(2e-6, velocityError_GCRF) ;


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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_Sun )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // Sun perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_TwoBody_Sun_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects =
        {
            std::make_shared<Earth>(Earth::Spherical()),
            std::make_shared<Sun>(Sun::Spherical())
        } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(2e-3, positionError_GCRF) ;
            ASSERT_GT(2e-6, velocityError_GCRF) ;


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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_Moon )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // Moon perturbation only vs GMAT
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_TwoBody_Moon_2hr_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects =
        {
            std::make_shared<Earth>(Earth::Spherical()),
            std::make_shared<Moon>(Moon::Spherical())
        } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(2e-3, positionError_GCRF) ;
            ASSERT_GT(2e-6, velocityError_GCRF) ;


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

TEST (DISABLED_OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_ExtendedDuration_GMAT ) // These tests take a long time and are for local testing only (not to be run on every push to git)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    // EGM96 perturbation
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_EGM96_2month_run.csv")), Table::Format::CSV, true) ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        Array<Vector3d> referencePositionArray_GCRF = Array<Vector3d>::Empty() ;
        Array<Vector3d> referenceVelocityArray_GCRF = Array<Vector3d>::Empty() ;

        for (const auto& referenceRow : referenceData)
        {

                instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal())) ;

                referencePositionArray_GCRF.add(1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal()) ) ;
                referenceVelocityArray_GCRF.add(1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal()) ) ;

        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::EGM96()) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray = orbit.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_GCRF = propagatedStateArray[i].accessPosition() ;
            const Velocity velocity_GCRF = propagatedStateArray[i].accessVelocity() ;

            const double positionError_GCRF = (position_GCRF.accessCoordinates() - referencePositionArray_GCRF[i]).norm() ;
            const double velocityError_GCRF = (velocity_GCRF.accessCoordinates() - referenceVelocityArray_GCRF[i]).norm() ;

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

            ASSERT_GT(1e+3, positionError_GCRF) ;
            ASSERT_GT(1e+0, velocityError_GCRF) ;


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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_IntervalSelfComparison )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        const size_t propagationSpanHours = 24 ;
        const size_t propagationIntervalSeconds = 30 ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds; i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i)) ;
        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel_short = {satelliteDynamics , numericalSolver} ;
        const Propagated propagatedModel_long = {satelliteDynamics , numericalSolver} ;

        const Orbit orbit_short = { propagatedModel_short, customEnvironment.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_long = { propagatedModel_long, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states at short interval
        const Array<State> propagatedStateArray_short = orbit_short.getStatesAt(instantArray) ;

        // Propagate all states at long interval
        Array<Instant> instantArray_longInterval = Array<Instant>::Empty() ;
        const size_t indexIncrement = 10 ;
        for (int i = 0; i < (int)instantArray.getSize(); i += indexIncrement)
        {
            instantArray_longInterval.add(instantArray[i]) ;
        }
        const Array<State> propagatedStateArray_long = orbit_short.getStatesAt(instantArray_longInterval) ;

        // Validation loop
        for (size_t i = 0; i < (size_t)instantArray.getSize(); i++)
        {
            // Run once every X times
            if (i%indexIncrement == 0)
            {

                // GCRF Compare
                const Position position_short = propagatedStateArray_short[i].accessPosition() ;
                const Velocity velocity_short = propagatedStateArray_short[i].accessVelocity() ;
                const Position position_long = propagatedStateArray_long[i/indexIncrement].accessPosition() ;
                const Velocity velocity_long = propagatedStateArray_long[i/indexIncrement].accessVelocity() ;

                const double positionError_GCRF = (position_short.accessCoordinates() - position_long.accessCoordinates()).norm() ;
                const double velocityError_GCRF = (velocity_short.accessCoordinates() - velocity_long.accessCoordinates()).norm() ;

                ASSERT_EQ(*Frame::GCRF(), *position_short.accessFrame()) ;
                ASSERT_EQ(*Frame::GCRF(), *velocity_short.accessFrame()) ;
                ASSERT_EQ(*Frame::GCRF(), *position_long.accessFrame()) ;
                ASSERT_EQ(*Frame::GCRF(), *velocity_long.accessFrame()) ;

                ASSERT_GT(1.0e-30, positionError_GCRF) ;
                ASSERT_GT(1.0e-30, velocityError_GCRF) ;

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

}

/* Stepper validation test */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_StepperTypeSelfComparison )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

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

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;

    using ostk::astro::trajectory::orbit::models::Propagated ;

    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;

        const size_t propagationSpanHours = 24 ;
        const size_t propagationIntervalSeconds = 30 ;

        Array<Instant> instantArray = Array<Instant>::Empty() ;
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds; i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i)) ;
        }

        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Construct default numerical solver
        const NumericalSolver numericalSolver_54 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        const NumericalSolver numericalSolver_78 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 }, gcrfSPtr) } ;

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel_54 = {satelliteDynamics , numericalSolver_54} ;
        const Propagated propagatedModel_78 = {satelliteDynamics , numericalSolver_78} ;

        const Orbit orbit_54 = { propagatedModel_54, customEnvironment.accessCelestialObjectWithName("Earth") } ;
        const Orbit orbit_78 = { propagatedModel_78, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        // Propagate all states
        const Array<State> propagatedStateArray_54 = orbit_54.getStatesAt(instantArray) ;
        const Array<State> propagatedStateArray_78 = orbit_78.getStatesAt(instantArray) ;

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // GCRF Compare
            const Position position_54 = propagatedStateArray_54[i].accessPosition() ;
            const Velocity velocity_54 = propagatedStateArray_54[i].accessVelocity() ;
            const Position position_78 = propagatedStateArray_78[i].accessPosition() ;
            const Velocity velocity_78 = propagatedStateArray_78[i].accessVelocity() ;

            const double positionError_GCRF = (position_54.accessCoordinates() - position_78.accessCoordinates()).norm() ;
            const double velocityError_GCRF = (velocity_54.accessCoordinates() - velocity_78.accessCoordinates()).norm() ;

            ASSERT_GT(5e-7, positionError_GCRF) ;
            ASSERT_GT(5e-10, velocityError_GCRF) ;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
