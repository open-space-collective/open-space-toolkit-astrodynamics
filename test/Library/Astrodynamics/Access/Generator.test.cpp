////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Access/Generator.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Access/Generator.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>

#include <Library/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <Library/Physics/Coordinate/Spherical/LLA.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TEST (Library_Astrodynamics_Access_Generator, ComputeAccesses)
// {

//     using library::core::types::Real ;
//     using library::core::ctnr::Array ;

//     using library::physics::units::Length ;
//     using library::physics::units::Angle ;
//     using library::physics::units::Derived ;
//     using library::physics::time::Scale ;
//     using library::physics::time::Instant ;
//     using library::physics::time::Duration ;
//     using library::physics::time::DateTime ;
//     using library::physics::time::Interval ;
//     using library::physics::coord::spherical::LLA ;
//     using library::physics::coord::Position ;
//     using library::physics::coord::Frame ;
//     using library::physics::Environment ;
//     using library::physics::env::obj::celest::Earth ;

//     using library::astro::Trajectory ;
//     using library::astro::trajectory::Orbit ;
//     using library::astro::trajectory::State ;
//     using library::astro::trajectory::orbit::models::Kepler ;
//     using library::astro::trajectory::orbit::models::kepler::COE ;
//     using library::astro::Access ;
//     using library::astro::access::Generator ;

//     // {

//     //     const Environment environment = Environment::Default() ;

//     //     const Generator generator = { environment } ;

//     //     const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
//     //     const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 1, 2, 0, 0), Scale::UTC) ;

//     //     const Interval interval = Interval::Closed(startInstant, endInstant) ;

//     //     auto generateFirstOrbit = [&startInstant] () -> Orbit
//     //     {

//     //         const Length semiMajorAxis = Length::Kilometers(7000.0) ;
//     //         const Real eccentricity = 0.0 ;
//     //         const Angle inclination = Angle::Degrees(+45.0) ;
//     //         const Angle raan = Angle::Degrees(0.0) ;
//     //         const Angle aop = Angle::Degrees(0.0) ;
//     //         const Angle trueAnomaly = Angle::Degrees(0.0) ;

//     //         const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//     //         const Instant epoch = startInstant ;
//     //         const Derived gravitationalConstant = Earth::GravitationalConstant ;
//     //         const Length equatorialRadius = Earth::EquatorialRadius ;
//     //         const Real J2 = Earth::J2 ;

//     //         const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

//     //         const Orbit orbit = { keplerianModel } ;

//     //         return orbit ;

//     //     } ;

//     //     auto generateSecondOrbit = [&startInstant] () -> Orbit
//     //     {

//     //         const Length semiMajorAxis = Length::Kilometers(7000.0) ;
//     //         const Real eccentricity = 0.0 ;
//     //         const Angle inclination = Angle::Degrees(+45.0) ;
//     //         const Angle raan = Angle::Degrees(180.0) ;
//     //         const Angle aop = Angle::Degrees(0.0) ;
//     //         const Angle trueAnomaly = Angle::Degrees(180.0) ;

//     //         const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//     //         const Instant epoch = startInstant ;
//     //         const Derived gravitationalConstant = Earth::GravitationalConstant ;
//     //         const Length equatorialRadius = Earth::EquatorialRadius ;
//     //         const Real J2 = Earth::J2 ;

//     //         const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

//     //         const Orbit orbit = { keplerianModel } ;

//     //         return orbit ;

//     //     } ;

//     //     const Orbit fromOrbit = generateFirstOrbit() ;
//     //     const Orbit toOrbit = generateSecondOrbit() ;

//     //     const Array<Access> accesses = generator.computeAccesses(interval, fromOrbit, toOrbit) ;

//     //     for (const auto& access : accesses)
//     //     {
//     //         std::cout << access << std::endl ;
//     //     }

//     // }

//     {

//         const Environment environment = Environment::Default() ;

//         const Generator generator = { environment } ;

//         const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
//         const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 1, 6, 0, 0), Scale::UTC) ;

//         const Interval interval = Interval::Closed(startInstant, endInstant) ;

//         auto generateGroundStationTrajectory = [] () -> Trajectory
//         {

//             const LLA groundStationLla = { Angle::Degrees(0.0), Angle::Degrees(0.0), Length::Meters(0.0) } ;

//             const Position groundStationPosition = Position::Meters(groundStationLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF()) ;

//             return Trajectory::Position(groundStationPosition) ;

//         } ;

//         auto generateSatelliteOrbit = [&environment, &startInstant] () -> Orbit
//         {

//             const Length semiMajorAxis = Length::Kilometers(7000.0) ;
//             const Real eccentricity = 0.0 ;
//             const Angle inclination = Angle::Degrees(+45.0) ;
//             const Angle raan = Angle::Degrees(0.0) ;
//             const Angle aop = Angle::Degrees(0.0) ;
//             const Angle trueAnomaly = Angle::Degrees(0.0) ;

//             const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//             const Instant epoch = startInstant ;
//             const Derived gravitationalConstant = Earth::GravitationalConstant ;
//             const Length equatorialRadius = Earth::EquatorialRadius ;
//             const Real J2 = Earth::J2 ;

//             const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

//             const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

//             return orbit ;

//         } ;

//         const Trajectory groundStationTrajectory = generateGroundStationTrajectory() ;
//         const Orbit satelliteOrbit = generateSatelliteOrbit() ;

//         const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit) ;

//         for (const auto& access : accesses)
//         {
//             std::cout << access << std::endl ;
//         }

//     }

// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////