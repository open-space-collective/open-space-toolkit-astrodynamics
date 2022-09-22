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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Propagated propagatedModel_x = { propagatedModel } ;
        EXPECT_TRUE(propagatedModel == propagatedModel_x) ;

        const NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Propagated propagatedModel_x = { propagatedModel } ;
        EXPECT_FALSE(propagatedModel != propagatedModel_x) ;

        const NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
            NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        // Check that warning is received
        EXPECT_ANY_THROW(Propagated(satelliteDynamics, numericalSolver, stateArray)) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* VALIDATION TESTS */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO add instant validation to all unit tests as well as positin and velocity

/* Force model validation tests */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_SingleInstant_GMAT )
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
        // Create environment
        const Instant instantJ2000 = Instant::J2000() ;
        const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::EGM96()) } ;

        const Environment customEnvironment = Environment(instantJ2000, objects) ;
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC) ;
        // const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-11-05 07:20:35.212.815.780"), Scale::UTC) ;

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        // Current state and instant setup
        Duration propDuration = Duration::Seconds(86400.000000628642738) ; //86400.000000628642738

        // Setup initial conditions
        std::vector<double> xStart(6);
        xStart[0] = -1514668.9408102570269; xStart[1] = -192084.12149140036718; xStart[2] = 6831711.4584368728174 ;
        xStart[3] = -6348.0791876050259859; xStart[4] = 3867.5824926981121621;  xStart[5] = -1297.1761044290490705 ;

        const State state = { startInstant, Position::Meters({ xStart[0], xStart[1], xStart[2] }, gcrfSPtr), Velocity::MetersPerSecond({ xStart[3], xStart[4], xStart[5] }, gcrfSPtr) };

        // Satellite system setup
        const Composite satelliteGeometry( Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } )) ;
        const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

        // Satellite dynamics setup
        SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, state } ;

        // Construct default numerical solver
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {satelliteDynamics , numericalSolver} ;
        const Orbit orbit = { propagatedModel, customEnvironment.accessCelestialObjectWithName("Earth") } ;

        const State state_GCRF = orbit.getStateAt(startInstant + propDuration) ;

        // Results check

        const Position position_GCRF = state_GCRF.accessPosition() ;
        const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

        // std::cout << position_GCRF << std::endl ;
        // std::cout << velocity_GCRF << std::endl ;

        const Vector3d propPosition_refGCRF = {5613532.4280540344262, -3480632.0957510802145, 2353554.2858703929596} ;
        const Vector3d propVelocity_refGCRF = {-2705.7090322756782719, 416.64789960092368792, 7027.4303603681875785} ;
        const Real statePositionError_GCRF = (position_GCRF.accessCoordinates() - propPosition_refGCRF).norm() ;
        const Real stateVelocityError_GCRF = (velocity_GCRF.accessCoordinates() - propVelocity_refGCRF).norm() ;


        // // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Position error is: " << statePositionError_GCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << stateVelocityError_GCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;


        // ASSERTS
        ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
        ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

        ASSERT_GT(5e-1, statePositionError_GCRF) ;
        ASSERT_GT(5e-4, stateVelocityError_GCRF) ;

        // ASSERT_EQ(3763, revolutionNumber) ;

    }

    // Repeat this test for all force models

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_CSV_Table_GMAT )
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
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_EGM96_24hr_120sInterval_run.csv")), Table::Format::CSV, true) ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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

            ASSERT_GT(5e-1, positionError_GCRF) ;
            ASSERT_GT(5e-4, velocityError_GCRF) ;


        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Position error is: " << positionError_GCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityError_GCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;

        }

    }

    // Repeat this test for all force models

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_CSV_Table_STK )
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

    // Two body perturbation
    {

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Orbit.csv")), Table::Format::CSV, true) ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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

            ASSERT_GT(1e-3, positionError_GCRF) ;
            ASSERT_GT(1e-6, velocityError_GCRF) ;

            // ITRF Compare
            const Position position_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).accessPosition() ;
            const Velocity velocity_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).accessVelocity() ;

            const double positionError_ITRF = (position_ITRF.accessCoordinates() - referencePositionArray_ITRF[i]).norm() ;
            const double velocityError_ITRF = (velocity_ITRF.accessCoordinates() - referenceVelocityArray_ITRF[i]).norm() ;

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame()) ;
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame()) ;

            ASSERT_GT(1e-1, positionError_ITRF) ;
            ASSERT_GT(1e-4, velocityError_ITRF) ;


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

    // Repeat this test for all force models

}

TEST (DISABLED_OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_ExtendedDuration_GMAT )
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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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

/* Propagation Interval validation tests */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_GMAT_ShortInterval )
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

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_TwoBodyGravity_24hr_shortInterval_run.csv")), Table::Format::CSV, true) ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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

            ASSERT_GT(5e-0, positionError_GCRF) ;
            ASSERT_GT(5e-3, velocityError_GCRF) ;


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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_GMAT_MediumInterval )
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

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_TwoBodyGravity_24hr_mediumInterval_run.csv")), Table::Format::CSV, true) ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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

            ASSERT_GT(5e-1, positionError_GCRF) ;
            ASSERT_GT(5e-4, velocityError_GCRF) ;


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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_GMAT_LongInterval )
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

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_TwoBodyGravity_24hr_longInterval_run.csv")), Table::Format::CSV, true) ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

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

            ASSERT_GT(5e-1, positionError_GCRF) ;
            ASSERT_GT(5e-4, velocityError_GCRF) ;


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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_TwoBody_IntervalSelfComparison ) // This test cannot be switched to the getStatesAt() paradigm
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

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_EGM96_24hr_30sInterval_run.csv")), Table::Format::CSV, true) ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

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

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // Propagated output generated
            const State state_short = orbit_short.getStateAt(instantArray[i]) ;

            // Run once every X times
            if (i%20 == 0)
            {
                const State state_long = orbit_long.getStateAt(instantArray[i]) ;

                // const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;
                // std::cout << "Resulting state in test loop:" << state_GCRF << std::endl ;
                // std::cout << "Resulting cached state array in test loop:" << cachedStateArray << std::endl ;

                // GCRF Compare
                const Position position_short = state_short.accessPosition() ;
                const Velocity velocity_short = state_short.accessVelocity() ;
                const Position position_long = state_long.accessPosition() ;
                const Velocity velocity_long = state_long.accessVelocity() ;

                const double positionError_GCRF = (position_short.accessCoordinates() - position_long.accessCoordinates()).norm() ;
                const double velocityError_GCRF = (velocity_short.accessCoordinates() - velocity_long.accessCoordinates()).norm() ;

                ASSERT_GT(5e-5, positionError_GCRF) ;
                ASSERT_GT(5e-8, velocityError_GCRF) ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropAccuracy_EGM96_IntervalSelfComparison )   // this test cannot be switched to the getStatesAt() paradigm
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

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_EGM96_24hr_30sInterval_run.csv")), Table::Format::CSV, true) ;

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
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

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

        // Validation loop
        for (int i = 0; i < (int)instantArray.getSize(); i++)
        {

            // Propagated output generated
            const State state_short = orbit_short.getStateAt(instantArray[i]) ;

            // Run once every X times
            if (i%4 == 0)
            {
                const State state_long = orbit_long.getStateAt(instantArray[i]) ;

                // const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray() ;
                // std::cout << "Resulting state in test loop:" << state_GCRF << std::endl ;
                // std::cout << "Resulting cached state array in test loop:" << cachedStateArray << std::endl ;

                // GCRF Compare
                const Position position_short = state_short.accessPosition() ;
                const Velocity velocity_short = state_short.accessVelocity() ;
                const Position position_long = state_long.accessPosition() ;
                const Velocity velocity_long = state_long.accessVelocity() ;

                const double positionError_GCRF = (position_short.accessCoordinates() - position_long.accessCoordinates()).norm() ;
                const double velocityError_GCRF = (velocity_short.accessCoordinates() - velocity_long.accessCoordinates()).norm() ;

                ASSERT_GT(5e-5, positionError_GCRF) ;
                ASSERT_GT(5e-8, velocityError_GCRF) ;

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

/* Stepper validation tests */
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

        // Reference data setup
        const Table referenceData = Table::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/GMAT_TwoBodyGravity_24hr_30sInterval_run.csv")), Table::Format::CSV, true) ;

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
        const NumericalSolver numericalSolver_54 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        const NumericalSolver numericalSolver_78 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        // Setup initial conditions
        const State state = { startInstant, Position::Meters({ referencePositionArray_GCRF[0] }, gcrfSPtr), Velocity::MetersPerSecond({ referenceVelocityArray_GCRF[0] }, gcrfSPtr) } ;

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

            ASSERT_GT(1e-4, positionError_GCRF) ;
            ASSERT_GT(1e-7, velocityError_GCRF) ;

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

/* Tests to add */
// - GMAT vs STK file similarity test
// - validation tests for other force models: EGM84,96,2008, WGS84 and for truncations of those force models

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
