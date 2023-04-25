////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/FullPropagator.test.cpp
/// @author         Vishwa Shah <vishwa@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Full/FullPropagator.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/FullSatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <numeric>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
using ostk::math::geom::d3::trf::rot::Quaternion ;

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

using ostk::astro::flight::profile::State ;
using ostk::astro::flight::system::SatelliteSystem ;
using ostk::astro::flight::system::dynamics::FullSatelliteDynamics ;
using ostk::astro::NumericalSolver ;

using ostk::astro::trajectory::FullPropagator ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* UNIT TESTS */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Full_Propagator : public ::testing::Test
{

    protected:

        void                    SetUp                                       ( ) override
        {

            this->environment_ = Environment::Default() ;

        }

        Environment environment_ = Environment::Undefined();
        NumericalSolver numericalSolver_ = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        const Shared<const Frame> gcrfSPtr_ = Frame::GCRF() ;

} ;

// TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Constructor)
// {
//     const Composite satelliteGeometry(Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 })) ;
//     const SatelliteSystem satelliteSystem = { Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;
//     const SatelliteDynamics satelliteDynamics = { environment_, satelliteSystem } ;

//     // Constructor without putting into orbit object
//     {
//         EXPECT_NO_THROW(Propagator( satelliteDynamics, numericalSolver_ )) ;
//     }

// }

// TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, EqualToOperator)
// {

//     const Composite satelliteGeometry(Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 })) ;
//     const SatelliteSystem satelliteSystem = { Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;
//     const SatelliteDynamics satelliteDynamics = { environment_, satelliteSystem } ;

//     {

//         const Propagator propagator = { satelliteDynamics, numericalSolver_ } ;
//         const Propagator propagator_x = { propagator } ;
//         EXPECT_TRUE(propagator == propagator_x) ;

//         const NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
//         const Propagator propagator_1 = { satelliteDynamics, numericalSolver_1 } ;
//         EXPECT_FALSE(propagator == propagator_1) ;

//         const SatelliteSystem satelliteSystem_2 = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;
//         const SatelliteDynamics satelliteDynamics_2 = { environment_, satelliteSystem_2 } ;
//         const Propagator propagator_2 = { satelliteDynamics_2, numericalSolver_1 } ;
//         EXPECT_FALSE(propagator == propagator_2) ;

//     }

// }

// TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, NotEqualToOperator)
// {

//     const Composite satelliteGeometry(Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 })) ;
//     const SatelliteSystem satelliteSystem = { Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;
//     const SatelliteDynamics satelliteDynamics = { environment_, satelliteSystem } ;

//     {

//         const Propagator propagator = { satelliteDynamics, numericalSolver_ } ;
//         const Propagator propagator_x = { propagator } ;
//         EXPECT_FALSE(propagator != propagator_x) ;

//         const NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
//         const Propagator propagator_1 = { satelliteDynamics, numericalSolver_1 } ;
//         EXPECT_TRUE(propagator != propagator_1) ;

//         const SatelliteSystem satelliteSystem_2 = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;
//         const SatelliteDynamics satelliteDynamics_2 = { environment_, satelliteSystem } ;
//         const Propagator propagator_2 = { satelliteDynamics_2, numericalSolver_1 } ;
//         EXPECT_TRUE(propagator != propagator_2) ;

//     }

// }

// TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, IsDefined)
// {

//     const Composite satelliteGeometry(Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 })) ;
//     const SatelliteSystem satelliteSystem = { Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;
//     const SatelliteDynamics satelliteDynamics = { environment_, satelliteSystem } ;

//     {

//         const Propagator propagator = { satelliteDynamics, numericalSolver_ } ;

//         EXPECT_TRUE(propagator.isDefined()) ;
//     }

// }

// TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, StreamOperator)
// {

//     const Composite satelliteGeometry(Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 })) ;
//     const SatelliteSystem satelliteSystem = { Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;
//     const SatelliteDynamics satelliteDynamics = { environment_, satelliteSystem } ;

//     {

//         const Propagator propagator = { satelliteDynamics, numericalSolver_ } ;

//         testing::internal::CaptureStdout() ;

//         EXPECT_NO_THROW(std::cout << propagator << std::endl) ;

//         EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;
//     }

// }

// TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Print)
// {

//     const Composite satelliteGeometry(Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 })) ;
//     const SatelliteSystem satelliteSystem = { Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;
//     const SatelliteDynamics satelliteDynamics = { environment_, satelliteSystem } ;

//     {

//         const Propagator propagator = { satelliteDynamics, numericalSolver_ } ;

//         testing::internal::CaptureStdout() ;

//         EXPECT_NO_THROW(propagator.print(std::cout, true)) ;
//         EXPECT_NO_THROW(propagator.print(std::cout, false)) ;
//         EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

//     }

// }

TEST_F (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Full_Propagator, CalculateStateAt)
{
    const Composite satelliteGeometry(Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 })) ;
    const SatelliteSystem satelliteSystem = { Mass(200.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

    // Create environment
    const Array<Shared<Object>> objects = { std::make_shared<Earth>(Earth::Spherical()) } ;

    const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
        Position::Meters({ 7000000.0, 0.0, 0.0 }, gcrfSPtr_),
        Velocity::MetersPerSecond({ 0.0, 5335.865450622126, 5335.865450622126 },gcrfSPtr_),
        Quaternion::XYZS(0.0, 0.0, 0.0, 1.0),
        Vector3d({ 0.0, 0.1, 0.0 }),
        gcrfSPtr_
    };

    // Satellite dynamics setup
    FullSatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem } ;

    // Setup Propagator model and orbit
    const FullPropagator propagator = { satelliteDynamics, numericalSolver_ } ;

    // Setup instants
    const Array<Instant> instantArray =
    {
        Instant::DateTime(DateTime(2018, 1, 1, 1, 0, 0), Scale::UTC),
        // Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
        // Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC),
        // Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC)
    } ;

    for (const Instant& instant : instantArray)
    {
        EXPECT_NO_THROW(propagator.calculateStateAt(state, instant)) ;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
