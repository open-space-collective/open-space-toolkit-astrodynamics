/// Apache License 2.0

#include <sstream>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Constructor)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        EXPECT_NO_THROW(TLE(firstLine, secondLine));
    }

    {
        const String satelliteName = "ISS (ZARYA)";
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        EXPECT_NO_THROW(TLE(satelliteName, firstLine, secondLine));
    }

    {
        const String firstLine = "";
        const String secondLine = "";

        EXPECT_NO_THROW(TLE(firstLine, secondLine));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, EqualToOperator)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        EXPECT_TRUE(TLE(firstLine, secondLine) == TLE(firstLine, secondLine));
    }

    {
        const String satelliteName = "ISS (ZARYA)";
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        EXPECT_FALSE(TLE(satelliteName, firstLine, secondLine) == TLE(firstLine, secondLine));
    }

    {
        EXPECT_FALSE(TLE::Undefined() == TLE::Undefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, NotEqualToOperator)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        EXPECT_FALSE(TLE(firstLine, secondLine) != TLE(firstLine, secondLine));
    }

    {
        const String satelliteName = "ISS (ZARYA)";
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        EXPECT_TRUE(TLE(satelliteName, firstLine, secondLine) != TLE(firstLine, secondLine));
    }

    {
        EXPECT_TRUE(TLE::Undefined() != TLE::Undefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, StreamOperator)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String satelliteName = "ISS (ZARYA)";
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(satelliteName, firstLine, secondLine);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << tle << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, GetSatelliteNumber)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(25544, tle.getSatelliteNumber());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getSatelliteNumber());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getClassification)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ("U", tle.getClassification());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getClassification());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getInternationalDesignator)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ("98067A", tle.getInternationalDesignator());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getInternationalDesignator());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEpoch)
{
    using ostk::core::type::String;

    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC), tle.getEpoch());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getEpoch());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotionFirstTimeDerivativeDividedByTwo)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getMeanMotionFirstTimeDerivativeDividedByTwo());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotionSecondTimeDerivativeDividedBySix)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getMeanMotionSecondTimeDerivativeDividedBySix());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getBStarDragTerm)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getBStarDragTerm());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEphemerisType)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(0, tle.getEphemerisType());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getEphemerisType());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getElementSetNumber)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(292, tle.getElementSetNumber());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getElementSetNumber());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getFirstLineChecksum)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(7, tle.getFirstLineChecksum());
    }

    {
        const String firstLine = "1 43020U 98067NH  20013.27161098  .00059259  00000-0  26371-3 0  9991";
        const String secondLine = "2 43020  51.6414 326.1073 0001865 145.5478 214.5646 15.83723373122744";

        const TLE tle {firstLine, secondLine};

        EXPECT_EQ(1, tle.getFirstLineChecksum());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getFirstLineChecksum());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getInclination)
{
    using ostk::core::type::String;

    using ostk::physics::units::Angle;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getInclination());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getRaan)
{
    using ostk::core::type::String;

    using ostk::physics::units::Angle;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getRaan());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getEccentricity)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(0.0006703, tle.getEccentricity());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getEccentricity());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getAop)
{
    using ostk::core::type::String;

    using ostk::physics::units::Angle;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getAop());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanAnomaly)
{
    using ostk::core::type::String;

    using ostk::physics::units::Angle;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getMeanAnomaly());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getMeanMotion)
{
    using ostk::core::type::String;

    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Time;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(
            Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
            tle.getMeanMotion()
        );
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getMeanMotion());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getRevolutionNumberAtEpoch)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle(firstLine, secondLine);

        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getRevolutionNumberAtEpoch());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, getSecondLineChecksum)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        const TLE tle {firstLine, secondLine};

        EXPECT_EQ(7, tle.getSecondLineChecksum());
    }

    {
        const String firstLine = "1 43020U 98067NH  20013.27161098  .00059259  00000-0  26371-3 0  9991";
        const String secondLine = "2 43020  51.6414 326.1073 0001865 145.5478 214.5646 15.83723373122744";

        const TLE tle {firstLine, secondLine};

        EXPECT_EQ(4, tle.getSecondLineChecksum());
    }

    {
        EXPECT_ANY_THROW(TLE::Undefined().getSecondLineChecksum());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, SetSatelliteNumber)
{
    using ostk::core::type::Integer;
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995";
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975  9992";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(99993, tle.getSatelliteNumber());

        const Integer updatedSatelliteNumber = 99995;

        tle.setSatelliteNumber(updatedSatelliteNumber);

        EXPECT_EQ(99995, tle.getSatelliteNumber());

        const String newFirstLine = "1 99995U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99997";
        const String newSecondLine = "2 99995 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975  9994";

        EXPECT_EQ(newFirstLine, tle.getFirstLine());
        EXPECT_EQ(newSecondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1     0U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99996";
        const String secondLine = "2     0 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009993";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(0, tle.getSatelliteNumber());

        const Integer updatedSatelliteNumber = 99995;

        tle.setSatelliteNumber(updatedSatelliteNumber);

        EXPECT_EQ(99995, tle.getSatelliteNumber());

        const String newFirstLine = "1 99995U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99997";
        const String newSecondLine = "2 99995 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009994";

        EXPECT_EQ(newFirstLine, tle.getFirstLine());
        EXPECT_EQ(newSecondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995";
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(99993, tle.getSatelliteNumber());

        const Integer updatedSatelliteNumber = 400;

        tle.setSatelliteNumber(updatedSatelliteNumber);

        EXPECT_EQ(400, tle.getSatelliteNumber());

        const String newFirstLine = "1   400U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99990";
        const String newSecondLine = "2   400 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009997";

        EXPECT_EQ(newFirstLine, tle.getFirstLine());
        EXPECT_EQ(newSecondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995";
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992";

        TLE tle(firstLine, secondLine);

        EXPECT_ANY_THROW(tle.setSatelliteNumber(100001));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, SetEpoch)
{
    using ostk::core::type::String;

    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        TLE tle(firstLine, secondLine);

        const Instant epoch = Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC);

        EXPECT_EQ(epoch, tle.getEpoch());

        tle.setEpoch(epoch);

        EXPECT_EQ(epoch, tle.getEpoch());
        EXPECT_EQ(firstLine, tle.getFirstLine());
        EXPECT_EQ(secondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC), tle.getEpoch());

        const Instant newEpoch = Instant::DateTime(DateTime::Parse("2020-09-20 20:25:30.103.142"), Scale::UTC);

        tle.setEpoch(newEpoch);

        ASSERT_GT(0.1, Duration::Between(newEpoch, tle.getEpoch()).inSeconds());

        const String newFirstLine = "1 25544U 98067A   20264.85104286 -.00002182  00000-0 -11606-4 0  2927";

        EXPECT_EQ(newFirstLine, tle.getFirstLine());
        EXPECT_EQ(secondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC), tle.getEpoch());

        const Instant newEpoch = Instant::DateTime(DateTime::Parse("1987-02-20 20:25:30.103.142"), Scale::UTC);

        tle.setEpoch(newEpoch);

        ASSERT_GT(0.1, Duration::Between(newEpoch, tle.getEpoch()).inSeconds());

        const String newFirstLine = "1 25544U 98067A   87051.85104286 -.00002182  00000-0 -11606-4 0  2924";

        EXPECT_EQ(newFirstLine, tle.getFirstLine());
        EXPECT_EQ(secondLine, tle.getSecondLine());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, SetRevolutionNumberAtEpoch)
{
    using ostk::core::type::Integer;
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995";
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(999, tle.getRevolutionNumberAtEpoch());

        const Integer updatedRevolutionNumberAtEpoch = 10;

        tle.setRevolutionNumberAtEpoch(updatedRevolutionNumberAtEpoch);

        EXPECT_EQ(10, tle.getRevolutionNumberAtEpoch());

        const String newSecondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975   106";

        EXPECT_EQ(firstLine, tle.getFirstLine());
        EXPECT_EQ(newSecondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995";
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975  9992";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(999, tle.getRevolutionNumberAtEpoch());

        const Integer updatedRevolutionNumberAtEpoch = 56353;

        tle.setRevolutionNumberAtEpoch(updatedRevolutionNumberAtEpoch);

        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());

        const String newSecondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975563537";

        EXPECT_EQ(firstLine, tle.getFirstLine());
        EXPECT_EQ(newSecondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995";
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(999, tle.getRevolutionNumberAtEpoch());

        const Integer updatedRevolutionNumberAtEpoch = 56353;

        tle.setRevolutionNumberAtEpoch(updatedRevolutionNumberAtEpoch);

        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());

        const String newSecondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975563537";

        EXPECT_EQ(firstLine, tle.getFirstLine());
        EXPECT_EQ(newSecondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995";
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975  9992";

        TLE tle(firstLine, secondLine);

        EXPECT_EQ(999, tle.getRevolutionNumberAtEpoch());

        const Integer updatedRevolutionNumberAtEpoch = 6353;

        tle.setRevolutionNumberAtEpoch(updatedRevolutionNumberAtEpoch);

        EXPECT_EQ(6353, tle.getRevolutionNumberAtEpoch());

        const String newSecondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975 63532";

        EXPECT_EQ(firstLine, tle.getFirstLine());
        EXPECT_EQ(newSecondLine, tle.getSecondLine());
    }

    {
        const String firstLine = "1 99993U 21990ZZZ 21182.62513889  .00000763  00000-0  42347-4 0 99995";
        const String secondLine = "2 99993 097.5132 311.4037 0016005 231.4378 006.3908 15.13696975009992";

        TLE tle(firstLine, secondLine);

        EXPECT_ANY_THROW(tle.setRevolutionNumberAtEpoch(459289));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Undefined)
{
    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        EXPECT_NO_THROW(TLE::Undefined());
        EXPECT_FALSE(TLE::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, CanParse)
{
    using ostk::core::filesystem::File;
    using ostk::core::filesystem::Path;
    using ostk::core::type::Integer;
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        EXPECT_TRUE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537")
        );
        EXPECT_TRUE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537")
        );
        EXPECT_TRUE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537\n")
        );
        EXPECT_TRUE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537\n\n")
        );
        EXPECT_TRUE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537\r\n")
        );
        EXPECT_TRUE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537\r\n\r\n")
        );
    }

    {
        EXPECT_FALSE(TLE::CanParse(""));
        EXPECT_FALSE(TLE::CanParse("abc"));
        EXPECT_FALSE(
            TLE::CanParse("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
        );
        EXPECT_FALSE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537")
        );
        EXPECT_FALSE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2928\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537")
        );
        EXPECT_FALSE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563536")
        );
        EXPECT_FALSE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  292\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.72125391563537")
        );
        EXPECT_FALSE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.7212539156353")
        );
        EXPECT_FALSE(
            TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  292a\n2 25544  51.6416 "
                          "247.4627 0006703 130.5360 325.0288 15.7212539156353b")
        );
    }

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        EXPECT_TRUE(TLE::CanParse(firstLine, secondLine));
    }

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\n";

        EXPECT_TRUE(TLE::CanParse(firstLine, secondLine));
    }

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\r\n";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\r\n";

        EXPECT_TRUE(TLE::CanParse(firstLine, secondLine));
    }

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n0";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537\n";

        EXPECT_FALSE(TLE::CanParse(firstLine, secondLine));
    }

    {
        const File activeTlesFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/active.txt")
        );

        const String allTles = activeTlesFile.getContents();

        std::istringstream allTlesStream {allTles};

        String satelliteName = String::Empty();
        String firstLine = String::Empty();
        String secondLine = String::Empty();

        Integer count = 0;

        String line;

        while (std::getline(allTlesStream, line))
        {
            if (satelliteName.isEmpty())
            {
                satelliteName = line;
            }
            else if (firstLine.isEmpty())
            {
                firstLine = line;
            }
            else
            {
                secondLine = line;

                EXPECT_TRUE(TLE::CanParse(firstLine, secondLine));

                satelliteName = String::Empty();
                firstLine = String::Empty();
                secondLine = String::Empty();

                count++;
            }
        }

        EXPECT_EQ(2458, count);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Parse)
{
    using ostk::core::filesystem::File;
    using ostk::core::filesystem::Path;
    using ostk::core::type::Integer;
    using ostk::core::type::String;

    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Time;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const TLE tle = TLE::Parse(
            "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 247.4627 0006703 "
            "130.5360 325.0288 15.72125391563537"
        );

        EXPECT_TRUE(tle.isDefined());

        EXPECT_EQ(25544, tle.getSatelliteNumber());
        EXPECT_EQ("U", tle.getClassification());
        EXPECT_EQ("98067A", tle.getInternationalDesignator());
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo());
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix());
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm());
        EXPECT_EQ(0, tle.getEphemerisType());
        EXPECT_EQ(292, tle.getElementSetNumber());
        EXPECT_EQ(7, tle.getFirstLineChecksum());
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination());
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan());
        EXPECT_EQ(0.0006703, tle.getEccentricity());
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop());
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly());
        EXPECT_EQ(
            Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
            tle.getMeanMotion()
        );
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());
        EXPECT_EQ(7, tle.getSecondLineChecksum());
    }

    {
        const TLE tle = TLE::Parse(
            "ISS (ZARYA)\n1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 "
            "247.4627 0006703 130.5360 325.0288 15.72125391563537"
        );

        EXPECT_TRUE(tle.isDefined());

        EXPECT_EQ(25544, tle.getSatelliteNumber());
        EXPECT_EQ("U", tle.getClassification());
        EXPECT_EQ("98067A", tle.getInternationalDesignator());
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo());
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix());
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm());
        EXPECT_EQ(0, tle.getEphemerisType());
        EXPECT_EQ(292, tle.getElementSetNumber());
        EXPECT_EQ(7, tle.getFirstLineChecksum());
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination());
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan());
        EXPECT_EQ(0.0006703, tle.getEccentricity());
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop());
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly());
        EXPECT_EQ(
            Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
            tle.getMeanMotion()
        );
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());
        EXPECT_EQ(7, tle.getSecondLineChecksum());
    }

    {
        const TLE tle = TLE::Parse(
            "ISS (ZARYA)\n1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 "
            "247.4627 0006703 130.5360 325.0288 15.72125391563537\n"
        );

        EXPECT_TRUE(tle.isDefined());

        EXPECT_EQ(25544, tle.getSatelliteNumber());
        EXPECT_EQ("U", tle.getClassification());
        EXPECT_EQ("98067A", tle.getInternationalDesignator());
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo());
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix());
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm());
        EXPECT_EQ(0, tle.getEphemerisType());
        EXPECT_EQ(292, tle.getElementSetNumber());
        EXPECT_EQ(7, tle.getFirstLineChecksum());
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination());
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan());
        EXPECT_EQ(0.0006703, tle.getEccentricity());
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop());
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly());
        EXPECT_EQ(
            Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
            tle.getMeanMotion()
        );
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());
    }

    {
        EXPECT_ANY_THROW(TLE::Parse(""));
        EXPECT_ANY_THROW(TLE::Parse("Hello World!"));
        EXPECT_ANY_THROW(
            TLE::Parse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2926\n2 25544  51.6416 "
                       "247.4627 0006703 130.5360 325.0288 15.72125391563537")
        );
        EXPECT_ANY_THROW(
            TLE::Parse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n2 25544  51.6416 "
                       "247.4627 0006703 130.5360 325.0288 15.72125391563538")
        );
    }

    {
        const File activeTlesFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/active.txt")
        );

        const String allTles = activeTlesFile.getContents();

        std::istringstream allTlesStream {allTles};

        String satelliteName = String::Empty();
        String firstLine = String::Empty();
        String secondLine = String::Empty();

        Integer count = 0;

        String line;

        while (std::getline(allTlesStream, line))
        {
            if (satelliteName.isEmpty())
            {
                satelliteName = line;
            }
            else if (firstLine.isEmpty())
            {
                firstLine = line;
            }
            else
            {
                secondLine = line;

                const String tleString = String::Format("{}\n{}\n{}\n", satelliteName, firstLine, secondLine);

                const TLE tle = TLE::Parse(tleString);

                EXPECT_EQ(satelliteName, tle.getSatelliteName());
                EXPECT_EQ(firstLine, tle.getFirstLine());
                EXPECT_EQ(secondLine, tle.getSecondLine());

                satelliteName = String::Empty();
                firstLine = String::Empty();
                secondLine = String::Empty();

                count++;
            }
        }

        EXPECT_EQ(2458, count);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Load)
{
    using ostk::core::filesystem::File;
    using ostk::core::filesystem::Path;
    using ostk::core::type::String;

    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Time;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const TLE tle = TLE::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/A.tle"))
        );

        EXPECT_TRUE(tle.isDefined());

        EXPECT_EQ(25544, tle.getSatelliteNumber());
        EXPECT_EQ("U", tle.getClassification());
        EXPECT_EQ("98067A", tle.getInternationalDesignator());
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo());
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix());
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm());
        EXPECT_EQ(0, tle.getEphemerisType());
        EXPECT_EQ(292, tle.getElementSetNumber());
        EXPECT_EQ(7, tle.getFirstLineChecksum());
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination());
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan());
        EXPECT_EQ(0.0006703, tle.getEccentricity());
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop());
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly());
        EXPECT_EQ(
            Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
            tle.getMeanMotion()
        );
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());
    }

    {
        const TLE tle = TLE::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/B.tle"))
        );

        EXPECT_TRUE(tle.isDefined());

        EXPECT_EQ(25544, tle.getSatelliteNumber());
        EXPECT_EQ("U", tle.getClassification());
        EXPECT_EQ("98067A", tle.getInternationalDesignator());
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo());
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix());
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm());
        EXPECT_EQ(0, tle.getEphemerisType());
        EXPECT_EQ(292, tle.getElementSetNumber());
        EXPECT_EQ(7, tle.getFirstLineChecksum());
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination());
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan());
        EXPECT_EQ(0.0006703, tle.getEccentricity());
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop());
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly());
        EXPECT_EQ(
            Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
            tle.getMeanMotion()
        );
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());
        EXPECT_EQ(7, tle.getSecondLineChecksum());
    }

    {
        const TLE tle = TLE::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE/C.tle"))
        );

        EXPECT_TRUE(tle.isDefined());

        EXPECT_EQ(25544, tle.getSatelliteNumber());
        EXPECT_EQ("U", tle.getClassification());
        EXPECT_EQ("98067A", tle.getInternationalDesignator());
        EXPECT_EQ(-0.00002182, tle.getMeanMotionFirstTimeDerivativeDividedByTwo());
        EXPECT_EQ(0.0, tle.getMeanMotionSecondTimeDerivativeDividedBySix());
        EXPECT_EQ(-0.11606e-4, tle.getBStarDragTerm());
        EXPECT_EQ(0, tle.getEphemerisType());
        EXPECT_EQ(292, tle.getElementSetNumber());
        EXPECT_EQ(7, tle.getFirstLineChecksum());
        EXPECT_EQ(Angle::Degrees(51.6416), tle.getInclination());
        EXPECT_EQ(Angle::Degrees(247.4627), tle.getRaan());
        EXPECT_EQ(0.0006703, tle.getEccentricity());
        EXPECT_EQ(Angle::Degrees(130.5360), tle.getAop());
        EXPECT_EQ(Angle::Degrees(325.0288), tle.getMeanAnomaly());
        EXPECT_EQ(
            Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
            tle.getMeanMotion()
        );
        EXPECT_EQ(56353, tle.getRevolutionNumberAtEpoch());
        EXPECT_EQ(7, tle.getSecondLineChecksum());
    }

    {
        EXPECT_ANY_THROW(TLE::Load(File::Undefined()));
        EXPECT_ANY_THROW(TLE::Load(File::Path(Path::Parse("/path/to/satellite.tle"))));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, Construct)
{
    using ostk::core::type::Integer;
    using ostk::core::type::Real;
    using ostk::core::type::String;

    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Time;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String satelliteName = "Satellite";
        const Integer satelliteNumber = 25544;
        const String classification = "U";
        const String internationalDesignator = "98067A";
        const Instant epoch = Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104.192"), Scale::UTC);
        const Real meanMotionFirstTimeDerivativeDividedByTwo = -0.00002182;
        const Real meanMotionSecondTimeDerivativeDividedBySix = 0.0;
        const Real bStarDragTerm = -0.11606e-4;
        const Integer ephemerisType = 0;
        const Integer elementSetNumber = 292;
        const Angle inclination = Angle::Degrees(51.6416);
        const Angle raan = Angle::Degrees(247.4627);
        const Real eccentricity = 0.0006703;
        const Angle aop = Angle::Degrees(130.5360);
        const Angle meanAnomaly = Angle::Degrees(325.0288);
        const Derived meanMotion =
            Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day));
        const Integer revolutionNumberAtEpoch = 56353;

        {
            const TLE tle = TLE::Construct(
                satelliteName,
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("Satellite", tle.getSatelliteName());
            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                0,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1     0U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2     0  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                "A",
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544A 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                "03067BT",
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 03067BT  08264.51782528 -.00002182  00000-0 -11606-4 0  2923", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                Instant::DateTime(DateTime::Parse("2022-01-01 00:00:00"), Scale::UTC),
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   22001.00000000 -.00002182  00000-0 -11606-4 0  2924", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                0.0,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528  .00000000  00000-0 -11606-4 0  2923", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                0.1,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528  .10000000  00000-0 -11606-4 0  2924", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                -0.1,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.10000000  00000-0 -11606-4 0  2925", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                0.123456789,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528  .12345679  00000-0 -11606-4 0  2920", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                0.1,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  10000-0 -11606-4 0  2928", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                -0.1,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182 -10000-0 -11606-4 0  2929", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                0.57410e-4,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  57410-4 -11606-4 0  2928", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                -0.57410e-4,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182 -57410-4 -11606-4 0  2929", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                -0.57410e+4,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182 -57410+4 -11606-4 0  2928", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                0.0,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0  00000-0 0  2928", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                0.1,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0  10000-0 0  2929", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                -0.1,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -10000-0 0  2920", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                0,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0    04", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                99999,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0999999", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                Angle::Degrees(0.0),
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544   0.0000 247.4627 0006703 130.5360 325.0288 15.72125391563534", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                Angle::Degrees(359.999911111),
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544 359.9999 247.4627 0006703 130.5360 325.0288 15.72125391563537", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                Angle::Degrees(-45.0),
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544 315.0000 247.4627 0006703 130.5360 325.0288 15.72125391563533", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                Angle::Degrees(0.0),
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416   0.0000 0006703 130.5360 325.0288 15.72125391563535", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                Angle::Degrees(359.999911111),
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 359.9999 0006703 130.5360 325.0288 15.72125391563538", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                Angle::Degrees(-45.0),
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 315.0000 0006703 130.5360 325.0288 15.72125391563534", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                0.0,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0000000 130.5360 325.0288 15.72125391563531", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                0.1,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 1000000 130.5360 325.0288 15.72125391563532", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                0.123456789,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 1234568 130.5360 325.0288 15.72125391563530", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                0.9999999,
                aop,
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 9999999 130.5360 325.0288 15.72125391563534", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                Angle::Degrees(0.0),
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703   0.0000 325.0288 15.72125391563539", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                Angle::Degrees(359.999911111),
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 359.9999 325.0288 15.72125391563532", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                Angle::Degrees(-45.0),
                meanAnomaly,
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 315.0000 325.0288 15.72125391563538", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                Angle::Degrees(0.0),
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360   0.0000 15.72125391563539", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                Angle::Degrees(359.999911111),
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 359.9999 15.72125391563532", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                Angle::Degrees(-45.0),
                meanMotion,
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 315.0000 15.72125391563538", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                Derived(0.0, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288  0.00000000563531", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                Derived(99.99999999, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
                revolutionNumberAtEpoch
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 99.99999999563531", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                0
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391    05", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                internationalDesignator,
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                99999
            );

            EXPECT_EQ("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391999990", tle.getSecondLine());
        }

        {
            const TLE tle = TLE::Construct(
                satelliteNumber,
                classification,
                "22001YAM",
                epoch,
                meanMotionFirstTimeDerivativeDividedByTwo,
                meanMotionSecondTimeDerivativeDividedBySix,
                bStarDragTerm,
                ephemerisType,
                elementSetNumber,
                inclination,
                raan,
                eccentricity,
                aop,
                meanAnomaly,
                meanMotion,
                99999
            );

            EXPECT_EQ("1 25544U 22001YAM 08264.51782528 -.00002182  00000-0 -11606-4 0  2922", tle.getFirstLine());
            EXPECT_EQ("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391999990", tle.getSecondLine());
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE, GenerateChecksum)
{
    using ostk::core::type::String;

    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        const String firstLine = "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
        const String secondLine = "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";

        EXPECT_EQ(7, TLE::GenerateChecksum(firstLine));
        EXPECT_EQ(7, TLE::GenerateChecksum(secondLine));

        EXPECT_EQ(7, TLE::GenerateChecksum(firstLine.getSubstring(0, 68).trim() + "0"));
        EXPECT_EQ(7, TLE::GenerateChecksum(secondLine.getSubstring(0, 68).trim() + "9"));
    }

    {
        const String firstLine = "";

        EXPECT_ANY_THROW(TLE::GenerateChecksum(firstLine));
    }
}
