////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

#include <sstream>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Constructor)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, EqualToOperator)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, NotEqualToOperator)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, StreamOperator)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, GetSatelliteNumber)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getClassification)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getInternationalDesignator)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEpoch)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotionFirstTimeDerivativeDividedByTwo)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotionSecondTimeDerivativeDividedBySix)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getBStarDragTerm)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEphemerisType)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getElementSetNumber)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getFirstLineChecksum)
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

        const String firstLine = "1 43020U 98067NH  20013.27161098  .00059259  00000-0  26371-3 0  9991" ;
        const String secondLine = "2 43020  51.6414 326.1073 0001865 145.5478 214.5646 15.83723373122744" ;

        const TLE tle { firstLine, secondLine } ;

        EXPECT_EQ(1, tle.getFirstLineChecksum()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getFirstLineChecksum()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getInclination)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getRaan)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEccentricity)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getAop)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanAnomaly)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotion)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getRevolutionNumberAtEpoch)
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

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getSecondLineChecksum)
{

    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        const TLE tle { firstLine, secondLine } ;

        EXPECT_EQ(7, tle.getSecondLineChecksum()) ;

    }

    {

        const String firstLine = "1 43020U 98067NH  20013.27161098  .00059259  00000-0  26371-3 0  9991" ;
        const String secondLine = "2 43020  51.6414 326.1073 0001865 145.5478 214.5646 15.83723373122744" ;

        const TLE tle { firstLine, secondLine } ;

        EXPECT_EQ(4, tle.getSecondLineChecksum()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Undefined().getSecondLineChecksum()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, SetSatelliteNumber)
{

    using ostk::core::types::Integer ;
    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995" ;
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(99993, tle.getSatelliteNumber()) ;

        const Integer updatedSatelliteNumber = 99995 ;

        tle.setSatelliteNumber(updatedSatelliteNumber) ;

        EXPECT_EQ(99995, tle.getSatelliteNumber()) ;

        const String newFirstLine = "1 99995U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99997" ;
        const String newSecondLine = "2 99995 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009994" ;

        EXPECT_EQ(newFirstLine, tle.getFirstLine()) ;
        EXPECT_EQ(newSecondLine, tle.getSecondLine()) ;

    }

    {

        const String firstLine = "1     0U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99996" ;
        const String secondLine = "2     0 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009993" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(0, tle.getSatelliteNumber()) ;

        const Integer updatedSatelliteNumber = 99995 ;

        tle.setSatelliteNumber(updatedSatelliteNumber) ;

        EXPECT_EQ(99995, tle.getSatelliteNumber()) ;

        const String newFirstLine = "1 99995U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99997" ;
        const String newSecondLine = "2 99995 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009994" ;

        EXPECT_EQ(newFirstLine, tle.getFirstLine()) ;
        EXPECT_EQ(newSecondLine, tle.getSecondLine()) ;

    }

    {

        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995" ;
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(99993, tle.getSatelliteNumber()) ;

        const Integer updatedSatelliteNumber = 400 ;

        tle.setSatelliteNumber(updatedSatelliteNumber) ;

        EXPECT_EQ(400, tle.getSatelliteNumber()) ;

        const String newFirstLine = "1   400U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99990" ;
        const String newSecondLine = "2   400 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009997" ;

        EXPECT_EQ(newFirstLine, tle.getFirstLine()) ;
        EXPECT_EQ(newSecondLine, tle.getSecondLine()) ;

    }

    {

        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995" ;
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_ANY_THROW(tle.setSatelliteNumber(100001)) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, SetEpoch)
{

    using ostk::core::types::String ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::time::Duration ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        TLE tle(firstLine, secondLine) ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC) ;

        EXPECT_EQ(epoch, tle.getEpoch()) ;

        tle.setEpoch(epoch) ;

        EXPECT_EQ(epoch, tle.getEpoch()) ;
        EXPECT_EQ(firstLine, tle.getFirstLine()) ;
        EXPECT_EQ(secondLine, tle.getSecondLine()) ;

    }

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC), tle.getEpoch()) ;

        const Instant newEpoch = Instant::DateTime(DateTime::Parse("2020-09-20 20:25:30.103.142"), Scale::UTC) ;

        tle.setEpoch(newEpoch) ;

        ASSERT_GT(0.1, Duration::Between(newEpoch, tle.getEpoch()).inSeconds()) ;

        const String newFirstLine = "1 25544U 98067A   20264.85104286 -.00002182  00000-0 -11606-4 0  2927" ;

        EXPECT_EQ(newFirstLine, tle.getFirstLine()) ;
        EXPECT_EQ(secondLine, tle.getSecondLine()) ;

    }

    {

        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927" ;
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC), tle.getEpoch()) ;

        const Instant newEpoch = Instant::DateTime(DateTime::Parse("1987-02-20 20:25:30.103.142"), Scale::UTC) ;

        tle.setEpoch(newEpoch) ;

        ASSERT_GT(0.1, Duration::Between(newEpoch, tle.getEpoch()).inSeconds()) ;

        const String newFirstLine = "1 25544U 98067A   87051.85104286 -.00002182  00000-0 -11606-4 0  2924" ;

        EXPECT_EQ(newFirstLine, tle.getFirstLine()) ;
        EXPECT_EQ(secondLine, tle.getSecondLine()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, SetRevolutionNumberAtEpoch)
{

    using ostk::core::types::Integer ;
    using ostk::core::types::String ;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995" ;
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(999, tle.getRevolutionNumberAtEpoch()) ;

        const Integer updatedRevolutionNumberAtEpoch = 10 ;

        tle.setRevolutionNumberAtEpoch(updatedRevolutionNumberAtEpoch) ;

        EXPECT_EQ(10, tle.getRevolutionNumberAtEpoch()) ;

        const String newSecondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975000106" ;

        EXPECT_EQ(firstLine, tle.getFirstLine()) ;
        EXPECT_EQ(newSecondLine, tle.getSecondLine()) ;

    }

    {

        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995" ;
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(999, tle.getRevolutionNumberAtEpoch()) ;

        const Integer updatedRevolutionNumberAtEpoch = 56353 ;

        tle.setRevolutionNumberAtEpoch(updatedRevolutionNumberAtEpoch) ;

        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

        const String newSecondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975563537" ;

        EXPECT_EQ(firstLine, tle.getFirstLine()) ;
        EXPECT_EQ(newSecondLine, tle.getSecondLine()) ;

    }

    {

        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995" ;
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_EQ(999, tle.getRevolutionNumberAtEpoch()) ;

        const Integer updatedRevolutionNumberAtEpoch = 56353 ;

        tle.setRevolutionNumberAtEpoch(updatedRevolutionNumberAtEpoch) ;

        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch()) ;

        const String newSecondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975563537" ;

        EXPECT_EQ(firstLine, tle.getFirstLine()) ;
        EXPECT_EQ(newSecondLine, tle.getSecondLine()) ;

    }

    {

        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995" ;
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992" ;

        TLE tle(firstLine, secondLine) ;

        EXPECT_ANY_THROW(tle.setRevolutionNumberAtEpoch(459289)) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Undefined)
{

    using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

    {

        EXPECT_NO_THROW(TLE::Undefined()) ;
        EXPECT_FALSE(TLE::Undefined().isDefined()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, CanParse)
{

    using ostk::core::types::Integer ;
    using ostk::core::types::String ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

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

    {

        const File activeTlesFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/active.txt")) ;

        const String allTles = activeTlesFile.getContents() ;

        std::istringstream allTlesStream { allTles } ;

        String satelliteName = String::Empty() ;
        String firstLine = String::Empty() ;
        String secondLine = String::Empty() ;

        Integer count = 0 ;

        String line ;

        while (std::getline(allTlesStream, line))
        {

            if (satelliteName.isEmpty())
            {
                satelliteName = line ;
            }
            else if (firstLine.isEmpty())
            {
                firstLine = line ;
            }
            else
            {

                secondLine = line ;

                EXPECT_TRUE(TLE::CanParse(firstLine, secondLine)) ;

                satelliteName = String::Empty() ;
                firstLine = String::Empty() ;
                secondLine = String::Empty() ;

                count++ ;

            }

        }

        EXPECT_EQ(2458, count) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Parse)
{

    using ostk::core::types::Integer ;
    using ostk::core::types::String ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

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
        EXPECT_EQ(7, tle.getSecondLineChecksum()) ;

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
        EXPECT_EQ(7, tle.getSecondLineChecksum()) ;

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

    {

        const File activeTlesFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/active.txt")) ;

        const String allTles = activeTlesFile.getContents() ;

        std::istringstream allTlesStream { allTles } ;

        String satelliteName = String::Empty() ;
        String firstLine = String::Empty() ;
        String secondLine = String::Empty() ;

        Integer count = 0 ;

        String line ;

        while (std::getline(allTlesStream, line))
        {

            if (satelliteName.isEmpty())
            {
                satelliteName = line ;
            }
            else if (firstLine.isEmpty())
            {
                firstLine = line ;
            }
            else
            {

                secondLine = line ;

                const String tleString = String::Format("{}\n{}\n{}\n", satelliteName, firstLine, secondLine) ;

                const TLE tle = TLE::Parse(tleString) ;

                EXPECT_EQ(satelliteName, tle.getSatelliteName()) ;
                EXPECT_EQ(firstLine, tle.getFirstLine()) ;
                EXPECT_EQ(secondLine, tle.getSecondLine()) ;

                satelliteName = String::Empty() ;
                firstLine = String::Empty() ;
                secondLine = String::Empty() ;

                count++ ;

            }

        }

        EXPECT_EQ(2458, count) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Load)
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
        EXPECT_EQ(7, tle.getSecondLineChecksum()) ;

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
        EXPECT_EQ(7, tle.getSecondLineChecksum()) ;

    }

    {

        EXPECT_ANY_THROW(TLE::Load(File::Undefined())) ;
        EXPECT_ANY_THROW(TLE::Load(File::Path(Path::Parse("/path/to/satellite.tle")))) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
