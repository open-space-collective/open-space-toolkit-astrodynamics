////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

#include <Library/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <Library/Physics/Time/Duration.hpp>
#include <Library/Physics/Units/Derived/Angle.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Length.hpp>

#include <Library/Mathematics/Objects/Vector.hpp>

#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Real.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, Constructor)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        EXPECT_NO_THROW(COE(semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly) ;) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, EqualToOperator)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_TRUE(coe == coe) ;

    }

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE firstCoe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const COE secondCoe = { semiMajorAxis, eccentricity, inclination, Angle::Degrees(21.0), aop, trueAnomaly } ;

        EXPECT_FALSE(firstCoe == secondCoe) ;

    }

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_FALSE(COE::Undefined() == COE::Undefined()) ;
        EXPECT_FALSE(coe == COE::Undefined()) ;
        EXPECT_FALSE(COE::Undefined() == coe) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, NotEqualToOperator)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_FALSE(coe != coe) ;

    }

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE firstCoe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const COE secondCoe = { semiMajorAxis, eccentricity, inclination, Angle::Degrees(21.0), aop, trueAnomaly } ;

        EXPECT_TRUE(firstCoe != secondCoe) ;

    }

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_TRUE(COE::Undefined() != COE::Undefined()) ;
        EXPECT_TRUE(coe != COE::Undefined()) ;
        EXPECT_TRUE(COE::Undefined() != coe) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, StreamOperator)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << coe << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, IsDefined)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_TRUE(coe.isDefined()) ;

    }

    {

        EXPECT_FALSE(COE::Undefined().isDefined()) ;
        
    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetSemiMajorAxis)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_EQ(semiMajorAxis, coe.getSemiMajorAxis()) ;

    }

    {

        EXPECT_ANY_THROW(COE::Undefined().getSemiMajorAxis()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetEccentricity)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_EQ(eccentricity, coe.getEccentricity()) ;

    }

    {

        EXPECT_ANY_THROW(COE::Undefined().getEccentricity()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetInclination)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_EQ(inclination, coe.getInclination()) ;

    }

    {

        EXPECT_ANY_THROW(COE::Undefined().getInclination()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetRaan)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_EQ(raan, coe.getRaan()) ;

    }

    {

        EXPECT_ANY_THROW(COE::Undefined().getRaan()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetAop)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_EQ(aop, coe.getAop()) ;

    }

    {

        EXPECT_ANY_THROW(COE::Undefined().getAop()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetTrueAnomaly)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.1 ;
        const Angle inclination = Angle::Degrees(10.0) ;
        const Angle raan = Angle::Degrees(20.0) ;
        const Angle aop = Angle::Degrees(30.0) ;
        const Angle trueAnomaly = Angle::Degrees(40.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        EXPECT_EQ(trueAnomaly, coe.getTrueAnomaly()) ;

    }

    {

        EXPECT_ANY_THROW(COE::Undefined().getTrueAnomaly()) ;

    }

}

// TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetMeanAnomaly)
// {

//     using library::core::types::Real ;

//     using library::math::obj::Vector3d ;

//     using library::physics::units::Length ;
//     using library::physics::units::Angle ;
//     using library::physics::units::Derived ;
//     using library::physics::time::Duration ;
//     using library::physics::coord::Position ;
//     using library::physics::coord::Velocity ;
//     using library::physics::env::obj::celest::Earth ;

//     using library::astro::trajectory::orbit::models::kepler::COE ;
    
//     {

//         FAIL() ;

//     }

// }

// TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetEccentricAnomaly)
// {

//     using library::core::types::Real ;

//     using library::math::obj::Vector3d ;

//     using library::physics::units::Length ;
//     using library::physics::units::Angle ;
//     using library::physics::units::Derived ;
//     using library::physics::time::Duration ;
//     using library::physics::coord::Position ;
//     using library::physics::coord::Velocity ;
//     using library::physics::env::obj::celest::Earth ;

//     using library::astro::trajectory::orbit::models::kepler::COE ;
    
//     {

//         FAIL() ;

//     }

// }

// TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetMeanMotion)
// {

//     using library::core::types::Real ;

//     using library::math::obj::Vector3d ;

//     using library::physics::units::Length ;
//     using library::physics::units::Angle ;
//     using library::physics::units::Derived ;
//     using library::physics::time::Duration ;
//     using library::physics::coord::Position ;
//     using library::physics::coord::Velocity ;
//     using library::physics::env::obj::celest::Earth ;

//     using library::astro::trajectory::orbit::models::kepler::COE ;
    
//     {

//         FAIL() ;

//     }

// }

// TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetOrbitalPeriod)
// {

//     using library::core::types::Real ;

//     using library::math::obj::Vector3d ;

//     using library::physics::units::Length ;
//     using library::physics::units::Angle ;
//     using library::physics::units::Derived ;
//     using library::physics::time::Duration ;
//     using library::physics::coord::Position ;
//     using library::physics::coord::Velocity ;
//     using library::physics::env::obj::celest::Earth ;

//     using library::astro::trajectory::orbit::models::kepler::COE ;
    
//     {

//         FAIL() ;

//     }

// }

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, GetCartesianState)
{

    using library::core::types::Real ;
    using library::core::types::String ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(0.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const COE::CartesianState cartesianState = coe.getCartesianState(Earth::GravitationalConstant, Frame::GCRF()) ;

        const Position& position = cartesianState.first ;
        const Velocity& velocity = cartesianState.second ;

        EXPECT_TRUE(position.getCoordinates().isNear(Vector3d(7000000.0, 0.0, 0.0), Real::Epsilon())) << position.getCoordinates() ;
        EXPECT_EQ(Position::Unit::Meter, position.getUnit()) ;
        EXPECT_EQ(Frame::GCRF(), position.accessFrame()) ;

        EXPECT_TRUE(velocity.getCoordinates().isNear(Vector3d(0.0, 7546.0532872678359, 0.0), Real::Epsilon())) << velocity.getCoordinates() ;
        EXPECT_EQ(Velocity::Unit::MeterPerSecond, velocity.getUnit()) ;
        EXPECT_EQ(Frame::GCRF(), velocity.accessFrame()) ;

    }

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.05 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(10.0) ;
        const Angle aop = Angle::Degrees(20.0) ;
        const Angle trueAnomaly = Angle::Degrees(30.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const COE::CartesianState cartesianState = coe.getCartesianState(Earth::GravitationalConstant, Frame::GCRF()) ;

        const Position& position = cartesianState.first ;
        const Velocity& velocity = cartesianState.second ;

        const Vector3d referencePosition = { 3607105.091490086168, 4317224.055878742598, 3625268.495929836296 } ;
        const Vector3d referenceVelocity = { -6467.082890311416, 2601.669219824156, 3685.141177179613 } ;

        const Real positionTolerance = 1e-8 ;
        const Real velocityTolerance = 1e-12 ;

        EXPECT_TRUE(position.getCoordinates().isNear(referencePosition, positionTolerance)) << String::Format("{} - {} = {} [m]", position.getCoordinates().toString(), referencePosition.toString(), (position.getCoordinates() - referencePosition).norm()) ;
        EXPECT_EQ(Position::Unit::Meter, position.getUnit()) ;
        EXPECT_EQ(Frame::GCRF(), position.accessFrame()) ;

        EXPECT_TRUE(velocity.getCoordinates().isNear(referenceVelocity, velocityTolerance)) << String::Format("{} - {} = {} [m/s]", velocity.getCoordinates().toString(), referenceVelocity.toString(), (velocity.getCoordinates() - referenceVelocity).norm()) ;
        EXPECT_EQ(Velocity::Unit::MeterPerSecond, velocity.getUnit()) ;
        EXPECT_EQ(Frame::GCRF(), velocity.accessFrame()) ;

    }

    {

        // [TBI] Add more tests

    }

    {

        EXPECT_ANY_THROW(COE::Undefined().getCartesianState(Earth::GravitationalConstant, Frame::GCRF())) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, Undefined)
{

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        EXPECT_NO_THROW(COE::Undefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, Cartesian)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(0.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE referenceCoe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const COE::CartesianState cartesianState = referenceCoe.getCartesianState(Earth::GravitationalConstant, Frame::GCRF()) ;

        const COE coe = COE::Cartesian(cartesianState, Earth::GravitationalConstant) ;

        EXPECT_TRUE(coe.getSemiMajorAxis().inMeters().isNear(referenceCoe.getSemiMajorAxis().inMeters(), 1e-6)) ;
        EXPECT_TRUE(coe.getEccentricity().isNear(referenceCoe.getEccentricity(), Real::Epsilon())) ;
        EXPECT_TRUE(coe.getInclination().inDegrees().isNear(referenceCoe.getInclination().inDegrees(), Real::Epsilon())) ;
        EXPECT_TRUE(coe.getRaan().inDegrees().isNear(referenceCoe.getRaan().inDegrees(), Real::Epsilon())) ;
        EXPECT_TRUE(coe.getAop().inDegrees().isNear(referenceCoe.getAop().inDegrees(), Real::Epsilon())) ;
        EXPECT_TRUE(coe.getTrueAnomaly().inDegrees().isNear(referenceCoe.getTrueAnomaly().inDegrees(), Real::Epsilon())) ;

    }

    {

        // const Array

        // [TBI] Add more tests

    }

    {

        EXPECT_ANY_THROW(COE::Cartesian(COE::CartesianState({ Position::Undefined(), Velocity::Undefined() }), Earth::GravitationalConstant)) ;

    }

}

// TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, EccentricAnomalyFromTrueAnomaly)
// {

//     using library::core::types::Real ;

//     using library::math::obj::Vector3d ;

//     using library::physics::units::Length ;
//     using library::physics::units::Angle ;
//     using library::physics::units::Derived ;
//     using library::physics::time::Duration ;
//     using library::physics::coord::Position ;
//     using library::physics::coord::Velocity ;
//     using library::physics::env::obj::celest::Earth ;

//     using library::astro::trajectory::orbit::models::kepler::COE ;
    
//     {

//         FAIL() ;

//     }

// }

// TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, TrueAnomalyFromEccentricAnomaly)
// {

//     using library::core::types::Real ;

//     using library::math::obj::Vector3d ;

//     using library::physics::units::Length ;
//     using library::physics::units::Angle ;
//     using library::physics::units::Derived ;
//     using library::physics::time::Duration ;
//     using library::physics::coord::Position ;
//     using library::physics::coord::Velocity ;
//     using library::physics::env::obj::celest::Earth ;

//     using library::astro::trajectory::orbit::models::kepler::COE ;
    
//     {

//         FAIL() ;

//     }

// }

// TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, MeanAnomalyFromEccentricAnomaly)
// {

//     using library::core::types::Real ;

//     using library::math::obj::Vector3d ;

//     using library::physics::units::Length ;
//     using library::physics::units::Angle ;
//     using library::physics::units::Derived ;
//     using library::physics::time::Duration ;
//     using library::physics::coord::Position ;
//     using library::physics::coord::Velocity ;
//     using library::physics::env::obj::celest::Earth ;

//     using library::astro::trajectory::orbit::models::kepler::COE ;
    
//     {

//         FAIL() ;

//     }

// }

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE, EccentricAnomalyFromMeanAnomaly)
{

    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::orbit::models::kepler::COE ;
    
    {

        const Angle meanAnomaly = Angle::Degrees(0.0) ;
        const Real eccentricity = 0.0 ;
        const Real tolerance = 1e-5 ;

        const Angle eccentricAnomaly = COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance) ;

        EXPECT_NEAR(eccentricAnomaly.inDegrees(), 0.0, 0.0) ;

    }
    
    {

        const Angle meanAnomaly = Angle::Radians(0.99262603391585447) ;
        const Real eccentricity = 0.05 ;
        const Real tolerance = 1e-8 ;

        const Angle eccentricAnomaly = COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance) ;

        EXPECT_NEAR(eccentricAnomaly.inRadians(), 1.0356353614863638, 1e-8) ;

    }

    {

        const Angle meanAnomaly = Angle::Degrees(0.0) ;
        const Real eccentricity = 0.0 ;
        const Real tolerance = 1e-5 ;

        EXPECT_ANY_THROW(COE::EccentricAnomalyFromMeanAnomaly(Angle::Undefined(), eccentricity, tolerance)) ;
        EXPECT_ANY_THROW(COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, Real::Undefined(), tolerance)) ;
        EXPECT_ANY_THROW(COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, Real::Undefined())) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////