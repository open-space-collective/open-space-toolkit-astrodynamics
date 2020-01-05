////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Constructor)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        EXPECT_NO_THROW(TLE(firstLine, secondLine)) ;

    }

    {

        const String satelliteName = "ISS (ZARYA)" ;
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        EXPECT_NO_THROW(TLE(satelliteName, firstLine, secondLine)) ;

    }

    {

        const String firstLine = "" ;
        const String secondLine = "" ;

        EXPECT_NO_THROW(TLE(firstLine, secondLine)) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, EqualToOperator)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        EXPECT_TRUE(TLE(firstLine, secondLine) == TLE(firstLine, secondLine)) ;

    }

    {

        const String satelliteName = "ISS (ZARYA)" ;
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        EXPECT_FALSE(TLE(satelliteName, firstLine, secondLine) == TLE(firstLine, secondLine)) ;

    }

    {

        EXPECT_FALSE(TLE::Undefined() == TLE::Undefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, NotEqualToOperator)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        EXPECT_FALSE(TLE(firstLine, secondLine) != TLE(firstLine, secondLine)) ;

    }

    {

        const String satelliteName = "ISS (ZARYA)" ;
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        EXPECT_TRUE(TLE(satelliteName, firstLine, secondLine) != TLE(firstLine, secondLine)) ;

    }

    {

        EXPECT_TRUE(TLE::Undefined() != TLE::Undefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, StreamOperator)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String satelliteName = "ISS (ZARYA)" ;
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(satelliteName, firstLine, secondLine) ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << tle << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, GetSatelliteNumber)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(25544, tle.getSatelliteNumber()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getSatelliteNumber()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getClassification)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ("U", tle.getClassification()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getClassification()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getInternationalDesignator)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ("98067A", tle.getInternationalDesignator()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getInternationalDesignator()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEpoch)
{

    using ostk::core::types::String ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::DateTime ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC), tle.getEpoch()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getEpoch()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotionFirstTimeDerivativeDividedByTwo)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getMeanMotionFirstTimeDerivativeDividedByTwo()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotionSecondTimeDerivativeDividedBySix)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getMeanMotionSecondTimeDerivativeDividedBySix()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getBStarDragTerm)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getBStarDragTerm()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEphemerisType)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(0, tle.getEphemerisType()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getEphemerisType()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getElementSetNumber)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(292, tle.getElementSetNumber()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getElementSetNumber()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getFirstLineChecksum)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(7, tle.getFirstLineChecksum()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getFirstLineChecksum()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getInclination)
{

    using ostk::core::types::String ;

    using ostk::physics::units::Angle ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getInclination()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getRaan)
{

    using ostk::core::types::String ;

    using ostk::physics::units::Angle ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getRaan()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEccentricity)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(0.0006703, tle.getEccentricity()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getEccentricity()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getAop)
{

    using ostk::core::types::String ;

    using ostk::physics::units::Angle ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getAop()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanAnomaly)
{

    using ostk::core::types::String ;

    using ostk::physics::units::Angle ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getMeanAnomaly()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotion)
{

    using ostk::core::types::String ;

    using ostk::physics::units::Time ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)), tle.getMeanMotion()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getMeanMotion()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getRevolutionNumberAtEpoch)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getRevolutionNumberAtEpoch()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Undefined)
{

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        EXPECT_NO_THROW(TLE::Undefined()) ;
        EXPECT_FALSE(TLE::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, CanParse)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        EXPECT_TRUE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537")) ;
        EXPECT_TRUE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537")) ;
        EXPECT_TRUE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\n")) ;
        EXPECT_TRUE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\n\n")) ;
        EXPECT_TRUE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\r\n")) ;
        EXPECT_TRUE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\r\n\r\n")) ;

    }

    {

        EXPECT_FALSE(TLE::CanParse("")) ;
        EXPECT_FALSE(TLE::CanParse("abc")) ;
        EXPECT_FALSE(TLE::CanParse("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")) ;
        EXPECT_FALSE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537")) ;
        EXPECT_FALSE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2928\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537")) ;
        EXPECT_FALSE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563536")) ;
        EXPECT_FALSE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  292\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537")) ;
        EXPECT_FALSE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.7212539156353")) ;
        EXPECT_FALSE(TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  292a\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.7212539156353b")) ;

    }

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        EXPECT_TRUE(TLE::CanParse(firstLine, secondLine)) ;

    }

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\n" ;

        EXPECT_TRUE(TLE::CanParse(firstLine, secondLine)) ;

    }

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r\n" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\r\n" ;

        EXPECT_TRUE(TLE::CanParse(firstLine, secondLine)) ;

    }

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n0" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\n" ;

        EXPECT_FALSE(TLE::CanParse(firstLine, secondLine)) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Parse)
{

    using ostk::core::types::String ;

    using ostk::physics::units::Time ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const TLE tle = TLE::Parse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537") ;

        EXPECT_TRUE(tle.isDefined()) ;

        EXPECT_EQ(25544, tle.getSatelliteNumber()) ;
        EXPECT_EQ("U", tle.getClassification()) ;
        EXPECT_EQ("98067A", tle.getInternationalDesignator()) ;
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo()) ;
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix()) ;
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm()) ;
        EXPECT_EQ(0, tle.getEphemerisType()) ;
        EXPECT_EQ(292, tle.getElementSetNumber()) ;
        EXPECT_EQ(7, tle.getFirstLineChecksum()) ;
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination()) ;
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan()) ;
        EXPECT_EQ(0.0006703, tle.getEccentricity()) ;
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop()) ;
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly()) ;
        EXPECT_EQ(Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)), tle.getMeanMotion()) ;
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

    }

    {

        const TLE tle = TLE::Parse("ISS (ZARYA)\n1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537") ;

        EXPECT_TRUE(tle.isDefined()) ;

        EXPECT_EQ(25544, tle.getSatelliteNumber()) ;
        EXPECT_EQ("U", tle.getClassification()) ;
        EXPECT_EQ("98067A", tle.getInternationalDesignator()) ;
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo()) ;
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix()) ;
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm()) ;
        EXPECT_EQ(0, tle.getEphemerisType()) ;
        EXPECT_EQ(292, tle.getElementSetNumber()) ;
        EXPECT_EQ(7, tle.getFirstLineChecksum()) ;
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination()) ;
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan()) ;
        EXPECT_EQ(0.0006703, tle.getEccentricity()) ;
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop()) ;
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly()) ;
        EXPECT_EQ(Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)), tle.getMeanMotion()) ;
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

    }

    {

        const TLE tle = TLE::Parse("ISS (ZARYA)\n1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\n") ;

        EXPECT_TRUE(tle.isDefined()) ;

        EXPECT_EQ(25544, tle.getSatelliteNumber()) ;
        EXPECT_EQ("U", tle.getClassification()) ;
        EXPECT_EQ("98067A", tle.getInternationalDesignator()) ;
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo()) ;
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix()) ;
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm()) ;
        EXPECT_EQ(0, tle.getEphemerisType()) ;
        EXPECT_EQ(292, tle.getElementSetNumber()) ;
        EXPECT_EQ(7, tle.getFirstLineChecksum()) ;
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination()) ;
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan()) ;
        EXPECT_EQ(0.0006703, tle.getEccentricity()) ;
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop()) ;
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly()) ;
        EXPECT_EQ(Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)), tle.getMeanMotion()) ;
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Parse("")) ;
        EXPECT_ANY_THROW(TLE::Parse("Hello World!")) ;
        EXPECT_ANY_THROW(TLE::Parse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2926\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537")) ;
        EXPECT_ANY_THROW(TLE::Parse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563538")) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Load)
{

    using ostk::core::types::String ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::physics::units::Time ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const TLE tle = TLE::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/A.tle"))) ;

        EXPECT_TRUE(tle.isDefined()) ;

        EXPECT_EQ(25544, tle.getSatelliteNumber()) ;
        EXPECT_EQ("U", tle.getClassification()) ;
        EXPECT_EQ("98067A", tle.getInternationalDesignator()) ;
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo()) ;
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix()) ;
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm()) ;
        EXPECT_EQ(0, tle.getEphemerisType()) ;
        EXPECT_EQ(292, tle.getElementSetNumber()) ;
        EXPECT_EQ(7, tle.getFirstLineChecksum()) ;
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination()) ;
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan()) ;
        EXPECT_EQ(0.0006703, tle.getEccentricity()) ;
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop()) ;
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly()) ;
        EXPECT_EQ(Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)), tle.getMeanMotion()) ;
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

    }

    {

        const TLE tle = TLE::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/B.tle"))) ;

        EXPECT_TRUE(tle.isDefined()) ;

        EXPECT_EQ(25544, tle.getSatelliteNumber()) ;
        EXPECT_EQ("U", tle.getClassification()) ;
        EXPECT_EQ("98067A", tle.getInternationalDesignator()) ;
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo()) ;
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix()) ;
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm()) ;
        EXPECT_EQ(0, tle.getEphemerisType()) ;
        EXPECT_EQ(292, tle.getElementSetNumber()) ;
        EXPECT_EQ(7, tle.getFirstLineChecksum()) ;
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination()) ;
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan()) ;
        EXPECT_EQ(0.0006703, tle.getEccentricity()) ;
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop()) ;
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly()) ;
        EXPECT_EQ(Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)), tle.getMeanMotion()) ;
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

    }

    {

        const TLE tle = TLE::Load(File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/C.tle"))) ;

        EXPECT_TRUE(tle.isDefined()) ;

        EXPECT_EQ(25544, tle.getSatelliteNumber()) ;
        EXPECT_EQ("U", tle.getClassification()) ;
        EXPECT_EQ("98067A", tle.getInternationalDesignator()) ;
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo()) ;
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix()) ;
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm()) ;
        EXPECT_EQ(0, tle.getEphemerisType()) ;
        EXPECT_EQ(292, tle.getElementSetNumber()) ;
        EXPECT_EQ(7, tle.getFirstLineChecksum()) ;
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination()) ;
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan()) ;
        EXPECT_EQ(0.0006703, tle.getEccentricity()) ;
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop()) ;
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly()) ;
        EXPECT_EQ(Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)), tle.getMeanMotion()) ;
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Load(File::Undefined())) ;
        EXPECT_ANY_THROW(TLE::Load(File::Path(Path::Parse("/path/to/satellite.tle")))) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
