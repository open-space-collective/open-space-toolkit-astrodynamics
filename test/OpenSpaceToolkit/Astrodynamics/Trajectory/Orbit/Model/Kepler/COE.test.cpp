/// Apache License 2.0

#include <cmath>
#include <limits>

#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::environment::object::celestial::Celestial;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::time::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanShort;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        coe_ = {
            defaultSemiMajorAxis_,
            defaultEccentricity_,
            defaultInclination_,
            defaultRaan_,
            defaultAop_,
            defaultTrueAnomaly_,
        };
    }

    const Length defaultSemiMajorAxis_ = Length::Kilometers(7000.0);
    const double defaultEccentricity_ = 0.1;
    const Angle defaultInclination_ = Angle::Degrees(10.0);
    const Angle defaultRaan_ = Angle::Degrees(20.0);
    const Angle defaultAop_ = Angle::Degrees(30.0);
    const Angle defaultTrueAnomaly_ = Angle::Degrees(40.0);

    COE coe_ = COE::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, Constructor)
{
    {
        EXPECT_NO_THROW(COE(defaultSemiMajorAxis_,
                            defaultEccentricity_,
                            defaultInclination_,
                            defaultRaan_,
                            defaultAop_,
                            defaultTrueAnomaly_););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, EqualToOperator)
{
    {
        EXPECT_TRUE(coe_ == coe_);
    }

    {
        const COE secondCoe = {
            defaultSemiMajorAxis_,
            defaultEccentricity_,
            defaultInclination_,
            Angle::Degrees(21.0),
            defaultAop_,
            defaultTrueAnomaly_,
        };

        EXPECT_FALSE(coe_ == secondCoe);
    }

    {
        EXPECT_FALSE(COE::Undefined() == COE::Undefined());
        EXPECT_FALSE(coe_ == COE::Undefined());
        EXPECT_FALSE(COE::Undefined() == coe_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, NotEqualToOperator)
{
    {
        EXPECT_FALSE(coe_ != coe_);
    }

    {
        const COE secondCoe = {
            defaultSemiMajorAxis_,
            defaultEccentricity_,
            defaultInclination_,
            Angle::Degrees(21.0),
            defaultAop_,
            defaultTrueAnomaly_
        };

        EXPECT_TRUE(coe_ != secondCoe);
    }

    {
        EXPECT_TRUE(COE::Undefined() != COE::Undefined());
        EXPECT_TRUE(coe_ != COE::Undefined());
        EXPECT_TRUE(COE::Undefined() != coe_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << coe_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, IsDefined)
{
    {
        EXPECT_TRUE(coe_.isDefined());
    }

    {
        EXPECT_FALSE(COE::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetSemiMajorAxis)
{
    {
        EXPECT_EQ(defaultSemiMajorAxis_, coe_.getSemiMajorAxis());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getSemiMajorAxis());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetEccentricity)
{
    {
        EXPECT_EQ(defaultEccentricity_, coe_.getEccentricity());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getEccentricity());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetInclination)
{
    {
        EXPECT_EQ(defaultInclination_, coe_.getInclination());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getInclination());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetRaan)
{
    {
        EXPECT_EQ(defaultRaan_, coe_.getRaan());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getRaan());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetAop)
{
    {
        EXPECT_EQ(defaultAop_, coe_.getAop());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getAop());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetTrueAnomaly)
{
    {
        EXPECT_EQ(defaultTrueAnomaly_, coe_.getTrueAnomaly());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getTrueAnomaly());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetPeriapsisRadius)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 - defaultEccentricity_), coe_.getPeriapsisRadius().inMeters()
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getPeriapsisRadius());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetApoapsisRadius)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 + defaultEccentricity_), coe_.getApoapsisRadius().inMeters()
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getApoapsisRadius());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetSemiLatusRectum)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 - std::pow(defaultEccentricity_, 2)),
            coe_.getSemiLatusRectum().inMeters()
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getSemiLatusRectum());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetRadialDistance)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 - std::pow(defaultEccentricity_, 2)) /
                (1.0 + defaultEccentricity_ * std::cos(defaultTrueAnomaly_.inRadians())),
            coe_.getRadialDistance().inMeters()
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getRadialDistance());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetAngularMomentum)
{
    {
        const Derived angularMomentum = coe_.getAngularMomentum(Earth::EGM2008.gravitationalParameter_);

        EXPECT_DOUBLE_EQ(
            std::sqrt(
                Earth::EGM2008.gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared()) *
                coe_.getSemiLatusRectum().inMeters()
            ),
            angularMomentum.in(angularMomentum.getUnit())
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getAngularMomentum(Earth::EGM2008.gravitationalParameter_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeMeanLTAN)
{
    {
        const Instant instant = Instant::DateTime(DateTime::Parse("2024-01-01T01:34:36.978"), Scale::UTC);

        const Time startMeanLTAN = Time::Parse("11:59:55.403");
        const Angle startRaan = Angle::Degrees(279.823758664135426);

        for (Size i = 0; i < 24; ++i)
        {
            const Angle raan = startRaan + Angle::Degrees(15.0 * i);
            const Time expectedMeanLTAN = Time::Hours(std::fmod(startMeanLTAN.getTotalFloatingHours() + 1.0 * i, 24.0));
            {
                const Time meanLTAN = COE::ComputeMeanLTAN(raan, instant);

                EXPECT_EQ(meanLTAN.getHour(), expectedMeanLTAN.getHour());
                EXPECT_NEAR(meanLTAN.getMinute(), expectedMeanLTAN.getMinute(), 1.0);
                EXPECT_NEAR(meanLTAN.getSecond(), expectedMeanLTAN.getSecond(), 15.0);
            }

            {
                const Time meanLTAN = COE::ComputeMeanLTAN(raan, instant, Sun::Default());

                EXPECT_EQ(meanLTAN.getHour(), expectedMeanLTAN.getHour());
                EXPECT_NEAR(meanLTAN.getMinute(), expectedMeanLTAN.getMinute(), 1.0);
                EXPECT_NEAR(meanLTAN.getSecond(), expectedMeanLTAN.getSecond(), 15.0);
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeLTAN)
{
    {
        const Instant instant = Instant::DateTime(DateTime::Parse("2024-01-01T01:34:36.978"), Scale::UTC);

        const Time startLTAN = Time::Parse("11:56:50.133");
        const Angle startRaan = Angle::Degrees(279.823758664135426);

        for (Size i = 0; i < 24; ++i)
        {
            const Angle raan = startRaan + Angle::Degrees(15.0 * i);
            const Time expectedLTAN = Time::Hours(std::fmod(startLTAN.getTotalFloatingHours() + 1.0 * i, 24.0));

            {
                const Time LTAN = COE::ComputeLTAN(raan, instant);

                EXPECT_EQ(LTAN.getHour(), expectedLTAN.getHour());
                EXPECT_NEAR(LTAN.getMinute(), expectedLTAN.getMinute(), 1.0);
                EXPECT_NEAR(LTAN.getSecond(), expectedLTAN.getSecond(), 15.0);
            }

            {
                const Time LTAN = COE::ComputeLTAN(raan, instant, Sun::Default());

                EXPECT_EQ(LTAN.getHour(), expectedLTAN.getHour());
                EXPECT_NEAR(LTAN.getMinute(), expectedLTAN.getMinute(), 1.0);
                EXPECT_NEAR(LTAN.getSecond(), expectedLTAN.getSecond(), 15.0);
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeMeanLTDN)
{
    {
        const Instant instant = Instant::DateTime(DateTime::Parse("2024-01-01T01:34:36.978"), Scale::UTC);

        const Time startMeanLTDN = Time::Parse("23:59:55.403");
        const Angle startRaan = Angle::Degrees(279.823758664135426);

        for (Size i = 0; i < 24; ++i)
        {
            const Angle raan = startRaan + Angle::Degrees(15.0 * i);
            const Time expectedMeanLTDN = Time::Hours(std::fmod(startMeanLTDN.getTotalFloatingHours() + 1.0 * i, 24.0));

            {
                const Time meanLTDN = COE::ComputeMeanLTDN(raan, instant);

                EXPECT_EQ(meanLTDN.getHour(), expectedMeanLTDN.getHour());
                EXPECT_NEAR(meanLTDN.getMinute(), expectedMeanLTDN.getMinute(), 1.0);
                EXPECT_NEAR(meanLTDN.getSecond(), expectedMeanLTDN.getSecond(), 15.0);
            }

            {
                const Time meanLTDN = COE::ComputeMeanLTDN(raan, instant, Sun::Default());

                EXPECT_EQ(meanLTDN.getHour(), expectedMeanLTDN.getHour());
                EXPECT_NEAR(meanLTDN.getMinute(), expectedMeanLTDN.getMinute(), 1.0);
                EXPECT_NEAR(meanLTDN.getSecond(), expectedMeanLTDN.getSecond(), 15.0);
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeLTDN)
{
    {
        const Instant instant = Instant::DateTime(DateTime::Parse("2024-01-01T01:34:36.978"), Scale::UTC);

        const Time startLTDN = Time::Parse("23:56:50.133");
        const Angle startRaan = Angle::Degrees(279.823758664135426);

        for (Size i = 0; i < 24; ++i)
        {
            const Angle raan = startRaan + Angle::Degrees(15.0 * i);
            const Time expectedLTDN = Time::Hours(std::fmod(startLTDN.getTotalFloatingHours() + 1.0 * i, 24.0));

            {
                const Time LTDN = COE::ComputeLTDN(raan, instant);

                EXPECT_EQ(LTDN.getHour(), expectedLTDN.getHour());
                EXPECT_NEAR(LTDN.getMinute(), expectedLTDN.getMinute(), 1.0);
                EXPECT_NEAR(LTDN.getSecond(), expectedLTDN.getSecond(), 15.0);
            }

            {
                const Time LTDN = COE::ComputeLTDN(raan, instant, Sun::Default());

                EXPECT_EQ(LTDN.getHour(), expectedLTDN.getHour());
                EXPECT_NEAR(LTDN.getMinute(), expectedLTDN.getMinute(), 1.0);
                EXPECT_NEAR(LTDN.getSecond(), expectedLTDN.getSecond(), 15.0);
            }
        }
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetMeanAnomaly)
// {

//     using ostk::core::type::Real;

//     using ostk::mathematics::object::Vector3d;

//     using ostk::physics::unit::Length;
//     using ostk::physics::unit::Angle;
//     using ostk::physics::unit::Derived;
//     using ostk::physics::time::Duration;
//     using ostk::physics::coordinate::Position;
//     using ostk::physics::coordinate::Velocity;
//     using ostk::physics::environment::object::celestial::Earth;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

//     {

//         FAIL();

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetEccentricAnomaly)
// {

//     using ostk::core::type::Real;

//     using ostk::mathematics::object::Vector3d;

//     using ostk::physics::unit::Length;
//     using ostk::physics::unit::Angle;
//     using ostk::physics::unit::Derived;
//     using ostk::physics::time::Duration;
//     using ostk::physics::coordinate::Position;
//     using ostk::physics::coordinate::Velocity;
//     using ostk::physics::environment::object::celestial::Earth;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

//     {

//         FAIL();

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetMeanMotion)
// {

//     using ostk::core::type::Real;

//     using ostk::mathematics::object::Vector3d;

//     using ostk::physics::unit::Length;
//     using ostk::physics::unit::Angle;
//     using ostk::physics::unit::Derived;
//     using ostk::physics::time::Duration;
//     using ostk::physics::coordinate::Position;
//     using ostk::physics::coordinate::Velocity;
//     using ostk::physics::environment::object::celestial::Earth;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

//     {

//         FAIL();

//     }

// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetArgumentOfLatitude)
{
    {
        EXPECT_EQ(defaultAop_ + defaultTrueAnomaly_, coe_.getArgumentOfLatitude());
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getArgumentOfLatitude());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetNodalPrecessionRate)
{
    {
        const COE coe = {
            Earth::EGM2008.equatorialRadius_ + Length::Kilometers(800.0),
            0.0,
            Angle::Degrees(56.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
        };
        const Derived nodalPrecessionRate = coe.getNodalPrecessionRate(
            Earth::EGM2008.gravitationalParameter_, Length::Meters(6.378137e6), 1.08262668e-3
        );

        EXPECT_NEAR(nodalPrecessionRate.in(Derived::Unit::RadianPerSecond()), -7.44e-7, 1e-6);
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetOrbitalPeriod)
// {

//     using ostk::core::type::Real;

//     using ostk::mathematics::object::Vector3d;

//     using ostk::physics::unit::Length;
//     using ostk::physics::unit::Angle;
//     using ostk::physics::unit::Derived;
//     using ostk::physics::time::Duration;
//     using ostk::physics::coordinate::Position;
//     using ostk::physics::coordinate::Velocity;
//     using ostk::physics::environment::object::celestial::Earth;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

//     {

//         FAIL();

//     }

// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetCartesianState)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const COE::CartesianState cartesianState =
            coe.getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());

        const Position& position = cartesianState.first;
        const Velocity& velocity = cartesianState.second;

        EXPECT_TRUE(position.getCoordinates().isNear(Vector3d(7000000.0, 0.0, 0.0), 1e-15))
            << position.getCoordinates();
        EXPECT_EQ(Position::Unit::Meter, position.getUnit());
        EXPECT_EQ(Frame::GCRF(), position.getFrame());

        EXPECT_TRUE(velocity.getCoordinates().isNear(Vector3d(0.0, 7546.0532872678359, 0.0), 1e-15))
            << velocity.getCoordinates();
        EXPECT_EQ(Velocity::Unit::MeterPerSecond, velocity.getUnit());
        EXPECT_EQ(Frame::GCRF(), velocity.getFrame());
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 0.05;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(10.0);
        const Angle aop = Angle::Degrees(20.0);
        const Angle trueAnomaly = Angle::Degrees(30.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const COE::CartesianState cartesianState =
            coe.getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());

        const Position& position = cartesianState.first;
        const Velocity& velocity = cartesianState.second;

        const Vector3d referencePosition = {3607105.091490086168, 4317224.055878742598, 3625268.495929836296};
        const Vector3d referenceVelocity = {-6467.082890311416, 2601.669219824156, 3685.141177179613};

        const double positionTolerance = 1e-8;
        const double velocityTolerance = 1e-12;

        EXPECT_TRUE(position.getCoordinates().isNear(referencePosition, positionTolerance)) << String::Format(
            "{} - {} = {} [m]",
            position.getCoordinates().toString(),
            referencePosition.toString(),
            (position.getCoordinates() - referencePosition).norm()
        );
        EXPECT_EQ(Position::Unit::Meter, position.getUnit());
        EXPECT_EQ(Frame::GCRF(), position.getFrame());

        EXPECT_TRUE(velocity.getCoordinates().isNear(referenceVelocity, velocityTolerance)) << String::Format(
            "{} - {} = {} [m/s]",
            velocity.getCoordinates().toString(),
            referenceVelocity.toString(),
            (velocity.getCoordinates() - referenceVelocity).norm()
        );
        EXPECT_EQ(Velocity::Unit::MeterPerSecond, velocity.getUnit());
        EXPECT_EQ(Frame::GCRF(), velocity.getFrame());
    }

    {
        // [TBI] Add more tests
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputePeriapsisRadius)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 - defaultEccentricity_),
            COE::ComputePeriapsisRadius(defaultSemiMajorAxis_.inMeters(), defaultEccentricity_)
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            coe_.getPeriapsisRadius().inMeters(),
            COE::ComputePeriapsisRadius(defaultSemiMajorAxis_.inMeters(), defaultEccentricity_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeApoapsisRadius)
{
    {
        EXPECT_DOUBLE_EQ(
            defaultSemiMajorAxis_.inMeters() * (1.0 + defaultEccentricity_),
            COE::ComputeApoapsisRadius(defaultSemiMajorAxis_.inMeters(), defaultEccentricity_)
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            coe_.getApoapsisRadius().inMeters(),
            COE::ComputeApoapsisRadius(defaultSemiMajorAxis_.inMeters(), defaultEccentricity_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeMeanMotion)
{
    {
        const double semiMajorAxis_m = defaultSemiMajorAxis_.inMeters();
        const double mu = Earth::EGM2008.gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared());

        EXPECT_DOUBLE_EQ(
            std::sqrt(mu / (semiMajorAxis_m * semiMajorAxis_m * semiMajorAxis_m)),
            COE::ComputeMeanMotion(semiMajorAxis_m, Earth::EGM2008.gravitationalParameter_)
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            coe_.getMeanMotion(Earth::EGM2008.gravitationalParameter_).in(Derived::Unit::RadianPerSecond()),
            COE::ComputeMeanMotion(defaultSemiMajorAxis_.inMeters(), Earth::EGM2008.gravitationalParameter_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeOrbitalPeriod)
{
    {
        EXPECT_DOUBLE_EQ(
            2.0 * M_PI /
                COE::ComputeMeanMotion(defaultSemiMajorAxis_.inMeters(), Earth::EGM2008.gravitationalParameter_),
            COE::ComputeOrbitalPeriod(defaultSemiMajorAxis_.inMeters(), Earth::EGM2008.gravitationalParameter_)
        );
    }

    {
        // The instance getter round-trips through Duration (integer nanoseconds), so compare with a
        // tolerance well above sub-nanosecond rounding.
        EXPECT_NEAR(
            coe_.getOrbitalPeriod(Earth::EGM2008.gravitationalParameter_).inSeconds(),
            COE::ComputeOrbitalPeriod(defaultSemiMajorAxis_.inMeters(), Earth::EGM2008.gravitationalParameter_),
            1e-6
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeNodalPrecessionRate)
{
    {
        const COE coe = {
            Earth::EGM2008.equatorialRadius_ + Length::Kilometers(800.0),
            0.0,
            Angle::Degrees(56.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
        };

        EXPECT_DOUBLE_EQ(
            coe.getNodalPrecessionRate(
                   Earth::EGM2008.gravitationalParameter_, Length::Meters(6.378137e6), 1.08262668e-3
            )
                .in(Derived::Unit::RadianPerSecond()),
            COE::ComputeNodalPrecessionRate(
                (Earth::EGM2008.equatorialRadius_ + Length::Kilometers(800.0)).inMeters(),
                0.0,
                Angle::Degrees(56.0).inRadians(),
                Earth::EGM2008.gravitationalParameter_,
                6.378137e6,
                1.08262668e-3
            )
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeArgumentOfLatitudeAngularRate)
{
    const auto earthSPtr = Environment::Default().getCelestialObjectWithName("Earth");

    const Length semiMajorAxis = Length::Kilometers(590.0 + 6378.137);
    const double eccentricity = 0.001;
    const Angle inclination = Angle::Degrees(97.0);

    const double expectedRate = 0.0010840210523878169;

    {
        const double rate = COE::ComputeArgumentOfLatitudeAngularRate(
            semiMajorAxis.inMeters(),
            eccentricity,
            inclination.inRadians(),
            earthSPtr->getGravitationalParameter(),
            earthSPtr->getEquatorialRadius().inMeters(),
            earthSPtr->getJ2()
        );

        EXPECT_NEAR(rate, expectedRate, std::abs(expectedRate) * 1e-6);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetArgumentOfLatitudeAngularRate)
{
    const auto earthSPtr = Environment::Default().getCelestialObjectWithName("Earth");

    const Derived gravitationalParameter = earthSPtr->getGravitationalParameter();
    const Length equatorialRadius = earthSPtr->getEquatorialRadius();
    const double j2 = earthSPtr->getJ2();

    const COE coe = {
        Length::Kilometers(590.0 + 6378.137),
        0.001,
        Angle::Degrees(97.0),
        Angle::Zero(),
        Angle::Zero(),
        Angle::Zero(),
    };

    const double expectedRate = 0.0010840210523878169;

    {
        const Derived rate = coe.getArgumentOfLatitudeAngularRate(gravitationalParameter, equatorialRadius, j2);

        EXPECT_NEAR(rate.in(Derived::Unit::RadianPerSecond()), expectedRate, std::abs(expectedRate) * 1e-6);

        // The getter must match the static computation.
        EXPECT_DOUBLE_EQ(
            rate.in(Derived::Unit::RadianPerSecond()),
            COE::ComputeArgumentOfLatitudeAngularRate(
                coe.getSemiMajorAxis().inMeters(),
                coe.getEccentricity(),
                coe.getInclination().inRadians(),
                gravitationalParameter,
                equatorialRadius.inMeters(),
                j2
            )
        );
    }

    {
        EXPECT_ANY_THROW(COE::Undefined().getArgumentOfLatitudeAngularRate(gravitationalParameter, equatorialRadius, j2)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GetVector)
{
    {
        const Vector6d coeVector = {
            defaultSemiMajorAxis_.inMeters(),
            defaultEccentricity_,
            defaultInclination_.inRadians(),
            defaultRaan_.inRadians(),
            defaultAop_.inRadians(),
            defaultTrueAnomaly_.inRadians(),
        };

        EXPECT_TRUE(((coeVector - coe_.getSIVector(COE::AnomalyType::True)).sum() < 1e-15));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, Undefined)
{
    {
        EXPECT_NO_THROW(COE::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, Cartesian)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE referenceCoe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const COE::CartesianState cartesianState =
            referenceCoe.getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());

        const COE coe = COE::Cartesian(cartesianState, Earth::EGM2008.gravitationalParameter_);

        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), referenceCoe.getSemiMajorAxis().inMeters(), 1e-6);
        EXPECT_TRUE(std::abs(coe.getEccentricity() - referenceCoe.getEccentricity()) <= 1e-15);
        EXPECT_NEAR(coe.getInclination().inDegrees(), referenceCoe.getInclination().inDegrees(), 1e-15);
        EXPECT_NEAR(coe.getRaan().inDegrees(), referenceCoe.getRaan().inDegrees(), 1e-15);
        EXPECT_NEAR(coe.getAop().inDegrees(), referenceCoe.getAop().inDegrees(), 1e-15);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), referenceCoe.getTrueAnomaly().inDegrees(), 1e-15);
    }

    // test edge case where true anomly is nan without std::clamp
    {
        // Non-circular, inclined
        {
            const COE::CartesianState cartesianState = {
                Position::Meters({6994047.553724454, 477490.53731528745, 416.6892069390969}, Frame::GCRF()),
                Velocity::MetersPerSecond({-516.3880965905739, 7563.794200060748, 6.600657312278296}, Frame::GCRF()),
            };

            const COE coe = COE::Cartesian(cartesianState, Earth::EGM2008.gravitationalParameter_);

            EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), 7087442.844884155, 1e-6);
            EXPECT_NEAR(coe.getEccentricity(), 0.010880490113017855, 1e-15);
            EXPECT_NEAR(coe.getInclination().inRadians(), 0.0008726646260034904, 1e-15);
            EXPECT_NEAR(coe.getRaan().inRadians(), 6.283185307179586, 1e-15);
            EXPECT_NEAR(coe.getAop().inRadians(), 0.06816523492423941, 1e-15);
            EXPECT_NEAR(coe.getTrueAnomaly().inRadians(), 0.0, 1e-15);
        }

        // Circular, inclined
        {
            const Length semiMajorAxis = Length::Kilometers(7000.0);
            const double eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(97.0);
            const Angle raan = Angle::Degrees(0.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(0.0);

            const COE referenceCoe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const COE::CartesianState cartesianState =
                referenceCoe.getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());

            const COE coe = COE::Cartesian(cartesianState, Earth::EGM2008.gravitationalParameter_);

            EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), referenceCoe.getSemiMajorAxis().inMeters(), 1e-6);
            EXPECT_TRUE(std::abs(coe.getEccentricity() - referenceCoe.getEccentricity()) <= 1e-15);
            EXPECT_NEAR(coe.getInclination().inDegrees(), referenceCoe.getInclination().inDegrees(), 1e-15);
            EXPECT_NEAR(coe.getRaan().inDegrees(), referenceCoe.getRaan().inDegrees(), 1e-15);
            EXPECT_NEAR(coe.getAop().inDegrees(), referenceCoe.getAop().inDegrees(), 1e-15);
            EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), referenceCoe.getTrueAnomaly().inDegrees(), 1e-15);
        }
    }

    {
        EXPECT_ANY_THROW(COE::Cartesian(
            COE::CartesianState({Position::Undefined(), Velocity::Undefined()}), Earth::EGM2008.gravitationalParameter_
        ));
    }

    {
        const Position position =
            Position::Meters({6596065.624114551, 2282234.953292401, -18030.93992064121}, Frame::GCRF());
        const Velocity velocity =
            Velocity::MetersPerSecond({345.4716519563907, -967.0404288726759, 7488.686029827369}, Frame::GCRF());
        const COE coe = COE::Cartesian({position, velocity}, Earth::EGM2008.gravitationalParameter_);

        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), 6983041.667517853, 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 0.001288522395593299, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), 97.80765762597238, 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 19.06529544678578, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 68.50632506660459, 1e-10);
        EXPECT_NEAR(coe.getMeanAnomaly().inDegrees(), 291.4817543658902, 1e-10);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, FromSIVector)
{
    {
        const Vector6d coeVector = {
            defaultSemiMajorAxis_.inMeters(),
            defaultEccentricity_,
            defaultInclination_.inRadians(),
            defaultRaan_.inRadians(),
            defaultAop_.inRadians(),
            defaultTrueAnomaly_.inRadians(),
        };

        const COE coeFromVector = COE::FromSIVector(coeVector, COE::AnomalyType::True);
        EXPECT_TRUE(coeFromVector == coe_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, FrozenOrbit)
{
    const Length semiMajorAxis = Length::Kilometers(8000);

    const Length re = Earth::EGM2008.equatorialRadius_;
    const double j2 = Earth::EGM2008.J2_;
    const double j3 = Earth::EGM2008.J3_;

    // Only provide SMA
    {
        const COE coe = COE::FrozenOrbit(semiMajorAxis, re, j2, j3);

        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), semiMajorAxis.inMeters(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 8.340085671757e-4, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), 63.4349, 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 90.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), 0.0, 1e-10);
    }

    // Provide inclination
    // @ref: https://www.mathworks.com/matlabcentral/fileexchange/39119-frozen-orbit-design?s_tid=FX_rc3_behav
    {
        const COE coe = COE::FrozenOrbit(semiMajorAxis, re, j2, j3, std::nullopt, Angle::Degrees(45.0));

        EXPECT_NEAR(coe.getEccentricity(), 6.594e-4, 1e-6);
        EXPECT_NEAR(coe.getAop().inDegrees(), 90.0, 1e-10);
    }

    // Provide critical AoP and eccentricity
    // @ref: https://www.mathworks.com/matlabcentral/fileexchange/39119-frozen-orbit-design?s_tid=FX_rc3_behav
    {
        const COE coe = COE::FrozenOrbit(
            semiMajorAxis, re, j2, j3, 6.5941377e-4, Angle::Undefined(), Angle::Degrees(0.0), Angle::Degrees(90.0)
        );

        EXPECT_NEAR(coe.getInclination().inDegrees(), 45.0, 1e-2);
    }

    // Provide AoP
    {
        const COE coe = COE::FrozenOrbit(
            semiMajorAxis,
            re,
            j2,
            j3,
            std::nullopt,
            Angle::Undefined(),
            Angle::Degrees(45.0),
            Angle::Degrees(45.0),
            Angle::Degrees(0.0)
        );

        EXPECT_NEAR(coe.getInclination().inDegrees(), 63.4349, 1e-10);
    }

    // Non-exact critical angle supplied
    {
        const Angle non_critical_aop = Angle::Degrees(80.0);
        const COE coe = COE::FrozenOrbit(
            semiMajorAxis, re, j2, j3, std::nullopt, Angle::Degrees(63.43490001), Angle::Degrees(0.0), non_critical_aop
        );

        EXPECT_NEAR(coe.getAop().inDegrees(), non_critical_aop.inDegrees(), 1e-10);
    }

    // Construct with Celestial
    {
        COE coe = COE::FrozenOrbit(semiMajorAxis, Environment::Default().getCelestialObjectWithName("Earth"));

        EXPECT_TRUE(coe.isDefined());
    }

    {
        {
            // No SMA provided
            {
                EXPECT_THROW(COE::FrozenOrbit(Length::Undefined(), re, j2, j3), ostk::core::error::runtime::Undefined);
            }

            // No equatorial radius provided
            {
                EXPECT_THROW(
                    COE::FrozenOrbit(semiMajorAxis, Length::Undefined(), j2, j3), ostk::core::error::runtime::Undefined
                );
            }

            // No J2 provided
            {
                {
                    EXPECT_THROW(COE::FrozenOrbit(semiMajorAxis, re, 0.0, j3), ostk::core::error::runtime::Undefined);
                }
            }

            // No J3 provided
            {
                {
                    EXPECT_THROW(COE::FrozenOrbit(semiMajorAxis, re, j2, 0.0), ostk::core::error::runtime::Undefined);
                }
            }

            // No RAAN provided
            {
                EXPECT_THROW(
                    COE::FrozenOrbit(semiMajorAxis, re, j2, j3, std::nullopt, Angle::Undefined(), Angle::Undefined()),
                    ostk::core::error::runtime::Undefined
                );
            }

            // No true anomaly provided
            {
                EXPECT_THROW(
                    COE::FrozenOrbit(
                        semiMajorAxis,
                        re,
                        j2,
                        j3,
                        std::nullopt,
                        Angle::Undefined(),
                        Angle::Degrees(0.0),
                        Angle::Undefined(),
                        Angle::Undefined()
                    ),
                    ostk::core::error::runtime::Undefined
                );
            }

            // Null Celestial provided
            {
                {
                    EXPECT_THROW(
                        COE::FrozenOrbit(semiMajorAxis, std::make_shared<const Celestial>(Celestial::Undefined())),
                        ostk::core::error::runtime::Undefined
                    );
                }

                {
                    EXPECT_THROW(COE::FrozenOrbit(semiMajorAxis, nullptr), ostk::core::error::runtime::Undefined);
                }
            }
        }

        // Excessively large eccentricity
        {
            EXPECT_THROW(COE::FrozenOrbit(semiMajorAxis, re, j2, j3, 0.1), ostk::core::error::RuntimeError);
        }

        // Inclination and eccentricity both provided
        {
            EXPECT_THROW(
                COE::FrozenOrbit(semiMajorAxis, re, j2, j3, 0.0001, Angle::Degrees(45.0)),
                ostk::core::error::RuntimeError
            );
        }

        // AoP and inclination both provided and both non-critical
        {
            EXPECT_THROW(
                COE::FrozenOrbit(
                    semiMajorAxis,
                    re,
                    j2,
                    j3,
                    std::nullopt,
                    Angle::Degrees(45.0),
                    Angle::Degrees(0.0),
                    Angle::Degrees(45.0)
                ),
                ostk::core::error::RuntimeError
            );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, SunSynchronous)
{
    {
        const Environment environment = Environment::Default();
        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC);
        const Length altitude = Length::Kilometers(500.0);
        const Length semiMajorAxis =
            Environment::Default().getCelestialObjectWithName("Earth")->getEquatorialRadius() + altitude;
        const Time localTimeAtDescendingNode = Time::Parse("12:00:00");

        // Convert descending node time to ascending node time (add 12 hours)
        const double ltdnHours = localTimeAtDescendingNode.getTotalFloatingHours();
        const double ltanHours = std::fmod(ltdnHours + 12.0, 24.0);
        const Time localTimeAtAscendingNode = Time::Hours(ltanHours);
        const Angle argumentOfLatitude = Angle::Degrees(50.0);

        const COE coe = COE::SunSynchronous(
            semiMajorAxis,
            localTimeAtAscendingNode,
            epoch,
            environment.getCelestialObjectWithName("Earth"),
            0.0,
            argumentOfLatitude
        );

        EXPECT_TRUE(coe.isDefined());
        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), semiMajorAxis.inMeters(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), argumentOfLatitude.inDegrees(), 1e-10);
        // Sun-synchronous inclination should be around 97-98 degrees for 500 km altitude
        EXPECT_GT(coe.getInclination().inDegrees(), 97.0);
        EXPECT_LT(coe.getInclination().inDegrees(), 99.0);
    }

    {
        EXPECT_THROW(
            COE::SunSynchronous(
                Length::Undefined(),
                Time::Parse("12:00:00"),
                Instant::J2000(),
                Environment::Default().getCelestialObjectWithName("Earth"),
                0.0,
                Angle::Zero()
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::SunSynchronous(
                Length::Kilometers(500.0),
                Time::Parse("12:00:00"),
                Instant::Undefined(),
                Environment::Default().getCelestialObjectWithName("Earth"),
                0.0,
                Angle::Zero()
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::SunSynchronous(
                Length::Kilometers(500.0), Time::Parse("12:00:00"), Instant::J2000(), nullptr, 0.0, Angle::Zero()
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::SunSynchronous(
                Length::Kilometers(500.0),
                Time::Parse("12:00:00"),
                Instant::J2000(),
                Environment::Default().getCelestialObjectWithName("Earth"),
                0.0,
                Angle::Undefined()
            ),
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GeoSynchronous)
{
    {
        const Environment environment = Environment::Default();
        const Instant epoch = Instant::J2000();
        const Angle inclination = Angle::Degrees(0.01);
        const Angle longitude = Angle::Degrees(0.0);

        const COE coe =
            COE::GeoSynchronous(epoch, inclination, longitude, environment.getCelestialObjectWithName("Earth"));

        EXPECT_TRUE(coe.isDefined());
        EXPECT_NEAR(coe.getInclination().inDegrees(), inclination.inDegrees(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 1e-15, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), 0.0, 1e-10);
        // Geosynchronous altitude is approximately 35786 km
        const Length expectedSemiMajorAxis =
            environment.getCelestialObjectWithName("Earth")->getEquatorialRadius() + Length::Meters(35786000.0);
        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), expectedSemiMajorAxis.inMeters(), 1e-3);
    }

    {
        EXPECT_THROW(
            COE::GeoSynchronous(
                Instant::Undefined(),
                Angle::Degrees(0.01),
                Angle::Degrees(0.0),
                Environment::Default().getCelestialObjectWithName("Earth")
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::GeoSynchronous(
                Instant::J2000(),
                Angle::Undefined(),
                Angle::Degrees(0.0),
                Environment::Default().getCelestialObjectWithName("Earth")
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::GeoSynchronous(
                Instant::J2000(),
                Angle::Degrees(0.01),
                Angle::Undefined(),
                Environment::Default().getCelestialObjectWithName("Earth")
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::GeoSynchronous(Instant::J2000(), Angle::Degrees(0.01), Angle::Degrees(0.0), nullptr),
            ostk::core::error::runtime::Undefined
        );

        // Test non-Earth celestial object
        EXPECT_THROW(
            COE::GeoSynchronous(
                Instant::J2000(),
                Angle::Degrees(0.01),
                Angle::Degrees(0.0),
                Environment::Default().getCelestialObjectWithName("Moon")
            ),
            ostk::core::error::runtime::ToBeImplemented
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeSunSynchronousInclination)
{
    {
        const Environment environment = Environment::Default();
        const Length altitude = Length::Kilometers(500.0);
        const Length semiMajorAxis = environment.getCelestialObjectWithName("Earth")->getEquatorialRadius() + altitude;

        const Angle inclination =
            COE::ComputeSunSynchronousInclination(semiMajorAxis, 0.0, environment.getCelestialObjectWithName("Earth"));

        EXPECT_TRUE(inclination.isDefined());
        // Sun-synchronous inclination should be around 97-98 degrees for 500 km altitude
        EXPECT_GT(inclination.inDegrees(), 97.0);
        EXPECT_LT(inclination.inDegrees(), 99.0);
    }

    // Data taken from https://github.com/JuliaSpace/SatelliteAnalysis.jl/blob/main/test/sun_syncrhonous_orbits.jl
    {
        const Environment environment = Environment::Default();
        const Length semiMajorAxis = Length::Meters(7130982.0);

        const Angle inclination =
            COE::ComputeSunSynchronousInclination(semiMajorAxis, 0.0, environment.getCelestialObjectWithName("Earth"));

        EXPECT_NEAR(inclination.inRadians(), 1.7175896973066611, 1e-5);
    }

    {
        const Environment environment = Environment::Default();
        const Length semiMajorAxis = Length::Meters(7130982.0);
        const double eccentricity = 0.001111;

        const Angle inclination = COE::ComputeSunSynchronousInclination(
            semiMajorAxis, eccentricity, environment.getCelestialObjectWithName("Earth")
        );

        EXPECT_NEAR(inclination.inRadians(), 1.7175893324980402, 1e-4);
    }

    {
        EXPECT_THROW(
            COE::ComputeSunSynchronousInclination(
                Length::Undefined(), 0.0, Environment::Default().getCelestialObjectWithName("Earth")
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::ComputeSunSynchronousInclination(Length::Kilometers(6971.0), 0.0, nullptr),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::ComputeSunSynchronousInclination(
                Length::Kilometers(6971.0), 0.0, std::make_shared<const Celestial>(Celestial::Undefined())
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::ComputeSunSynchronousInclination(
                Length::Kilometers(6971.0), 0.0, Environment::Default().getCelestialObjectWithName("Sun")
            ),
            ostk::core::error::runtime::ToBeImplemented
        );

        EXPECT_THROW(
            COE::ComputeSunSynchronousInclination(
                Length::Kilometers(6971.0), -0.5, Environment::Default().getCelestialObjectWithName("Earth")
            ),
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeRaanFromLTAN)
{
    const Environment environment = Environment::Default();

    // Data taken from https://github.com/JuliaSpace/SatelliteToolbox.jl/blob/master/test/time/raan.jl
    {
        const Instant epoch = Instant::DateTime(DateTime::Parse("2021-06-19 19:35:35"), Scale::UTC);
        const Time localTimeAtAscendingNode = Time::Parse("22:30:00");

        const Angle raan =
            COE::ComputeRaanFromLTAN(localTimeAtAscendingNode, epoch, environment.getCelestialObjectWithName("Earth"));

        EXPECT_TRUE(raan.isDefined());
        EXPECT_NEAR(raan.inRadians(), 4.289024646407403, 1e-2);  // Difference due to Sun position computation
    }

    {
        EXPECT_THROW(
            COE::ComputeRaanFromLTAN(
                Time::Parse("12:00:00"), Instant::Undefined(), environment.getCelestialObjectWithName("Earth")
            ),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::ComputeRaanFromLTAN(Time::Parse("12:00:00"), Instant::J2000(), nullptr),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::ComputeRaanFromLTAN(
                Time::Parse("12:00:00"), Instant::J2000(), environment.getCelestialObjectWithName("Moon")
            ),
            ostk::core::error::runtime::ToBeImplemented
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, Circular)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Angle inclination = Angle::Degrees(45.0);
        const Angle argumentOfLatitude = Angle::Degrees(30.0);

        const COE coe = COE::Circular(semiMajorAxis, inclination, argumentOfLatitude);

        EXPECT_TRUE(coe.isDefined());
        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), semiMajorAxis.inMeters(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), inclination.inDegrees(), 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), argumentOfLatitude.inDegrees(), 1e-10);
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Angle inclination = Angle::Degrees(45.0);

        const COE coe = COE::Circular(semiMajorAxis, inclination);

        EXPECT_TRUE(coe.isDefined());
        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), semiMajorAxis.inMeters(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), inclination.inDegrees(), 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), 0.0, 1e-10);
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);

        const COE coe = COE::Circular(semiMajorAxis);

        EXPECT_TRUE(coe.isDefined());
        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), semiMajorAxis.inMeters(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), 0.0, 1e-10);
    }

    {
        EXPECT_THROW(
            COE::Circular(Length::Undefined(), Angle::Degrees(45.0), Angle::Zero()),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::Circular(Length::Kilometers(7000.0), Angle::Undefined(), Angle::Zero()),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            COE::Circular(Length::Kilometers(7000.0), Angle::Degrees(45.0), Angle::Undefined()),
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, Equatorial)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 0.1;
        const Angle trueAnomaly = Angle::Degrees(45.0);

        const COE coe = COE::Equatorial(semiMajorAxis, eccentricity, trueAnomaly);

        EXPECT_TRUE(coe.isDefined());
        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), semiMajorAxis.inMeters(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), eccentricity, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), trueAnomaly.inDegrees(), 1e-10);
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 0.1;

        const COE coe = COE::Equatorial(semiMajorAxis, eccentricity);

        EXPECT_TRUE(coe.isDefined());
        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), semiMajorAxis.inMeters(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), eccentricity, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), 0.0, 1e-10);
    }

    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);

        const COE coe = COE::Equatorial(semiMajorAxis);

        EXPECT_TRUE(coe.isDefined());
        EXPECT_NEAR(coe.getSemiMajorAxis().inMeters(), semiMajorAxis.inMeters(), 1e-10);
        EXPECT_NEAR(coe.getEccentricity(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getInclination().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getRaan().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getAop().inDegrees(), 0.0, 1e-10);
        EXPECT_NEAR(coe.getTrueAnomaly().inDegrees(), 0.0, 1e-10);
    }

    {
        EXPECT_THROW(COE::Equatorial(Length::Undefined(), 0.1, Angle::Zero()), ostk::core::error::runtime::Undefined);

        EXPECT_THROW(
            COE::Equatorial(Length::Kilometers(7000.0), 0.1, Angle::Undefined()), ostk::core::error::runtime::Undefined
        );

        // Invalid eccentricity
        EXPECT_THROW(COE::Equatorial(Length::Kilometers(7000.0), -0.1, Angle::Zero()), ostk::core::error::RuntimeError);

        EXPECT_THROW(COE::Equatorial(Length::Kilometers(7000.0), 1.0, Angle::Zero()), ostk::core::error::RuntimeError);

        EXPECT_THROW(COE::Equatorial(Length::Kilometers(7000.0), 1.5, Angle::Zero()), ostk::core::error::RuntimeError);
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, EccentricAnomalyFromTrueAnomaly)
// {

//     using ostk::core::type::Real;

//     using ostk::mathematics::object::Vector3d;

//     using ostk::physics::unit::Length;
//     using ostk::physics::unit::Angle;
//     using ostk::physics::unit::Derived;
//     using ostk::physics::time::Duration;
//     using ostk::physics::coordinate::Position;
//     using ostk::physics::coordinate::Velocity;
//     using ostk::physics::environment::object::celestial::Earth;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

//     {

//         FAIL();

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, TrueAnomalyFromEccentricAnomaly)
// {

//     using ostk::core::type::Real;

//     using ostk::mathematics::object::Vector3d;

//     using ostk::physics::unit::Length;
//     using ostk::physics::unit::Angle;
//     using ostk::physics::unit::Derived;
//     using ostk::physics::time::Duration;
//     using ostk::physics::coordinate::Position;
//     using ostk::physics::coordinate::Velocity;
//     using ostk::physics::environment::object::celestial::Earth;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

//     {

//         FAIL();

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, MeanAnomalyFromEccentricAnomaly)
// {

//     using ostk::core::type::Real;

//     using ostk::mathematics::object::Vector3d;

//     using ostk::physics::unit::Length;
//     using ostk::physics::unit::Angle;
//     using ostk::physics::unit::Derived;
//     using ostk::physics::time::Duration;
//     using ostk::physics::coordinate::Position;
//     using ostk::physics::coordinate::Velocity;
//     using ostk::physics::environment::object::celestial::Earth;

//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

//     {

//         FAIL();

//     }

// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, EccentricAnomalyFromMeanAnomaly)
{
    {
        const Angle meanAnomaly = Angle::Degrees(0.0);
        const double eccentricity = 0.0;
        const double tolerance = 1e-5;

        const Angle eccentricAnomaly = COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance);

        EXPECT_NEAR(eccentricAnomaly.inDegrees(), 0.0, 0.0);
    }

    {
        const Angle meanAnomaly = Angle::Radians(0.99262603391585447);
        const double eccentricity = 0.05;
        const double tolerance = 1e-8;

        const Angle eccentricAnomaly = COE::EccentricAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance);

        EXPECT_NEAR(eccentricAnomaly.inRadians(), 1.0356353614863638, 1e-8);
    }

    {
        const Angle meanAnomaly = Angle::Degrees(0.0);
        const double eccentricity = 0.0;
        const double tolerance = 1e-5;

        EXPECT_ANY_THROW(COE::EccentricAnomalyFromMeanAnomaly(Angle::Undefined(), eccentricity, tolerance));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, TrueAnomalyFromMeanAnomaly)
{
    {
        const Angle meanAnomaly = Angle::Radians(5.08731632317414);
        const double eccentricity = 0.001288522395593299;
        const double tolerance = 1e-15;

        const Angle trueAnomaly = COE::TrueAnomalyFromMeanAnomaly(meanAnomaly, eccentricity, tolerance);

        EXPECT_NEAR(trueAnomaly.inRadians(), 5.084916881853494, tolerance);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, StringFromElement)
{
    const Array<Tuple<COE::Element, String>> testCases = {
        {COE::Element::SemiMajorAxis, "SemiMajorAxis"},
        {COE::Element::Eccentricity, "Eccentricity"},
        {COE::Element::Inclination, "Inclination"},
        {COE::Element::Aop, "Aop"},
        {COE::Element::Raan, "Raan"},
        {COE::Element::TrueAnomaly, "TrueAnomaly"},
        {COE::Element::MeanAnomaly, "MeanAnomaly"},
        {COE::Element::EccentricAnomaly, "EccentricAnomaly"},
        {COE::Element::ArgumentOfLatitude, "ArgumentOfLatitude"},
    };

    for (const auto& testCase : testCases)
    {
        const COE::Element element = std::get<0>(testCase);
        const String& expectedString = std::get<1>(testCase);

        EXPECT_EQ(COE::StringFromElement(element), expectedString);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ConvertAnomaly)
{
    {
        EXPECT_DOUBLE_EQ(
            COE::FromSIVector({7.0e6, 0.0, 0.0, 0.0, 0.0, 0.0}, COE::AnomalyType::True).getTrueAnomaly().inDegrees(),
            0.0
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            COE::FromSIVector({7.0e6, 0.0, 0.0, 0.0, 0.0, 0.0}, COE::AnomalyType::Mean).getMeanAnomaly().inDegrees(),
            0.0
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            COE::FromSIVector({7.0e6, 0.0, 0.0, 0.0, 0.0, 0.0}, COE::AnomalyType::Eccentric)
                .getEccentricAnomaly()
                .inDegrees(),
            0.0
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, LocalTimeOfNodes_Frame)
{
    // Frames whose Z axis is the true pole (CIRF) differ from GCRF by precession and nutation, so the Sun right
    // ascension, and hence the local time of the nodes, depends on the frame the RAAN is measured in.

    const Instant instant = Instant::DateTime(DateTime::Parse("2026-01-01T00:00:00"), Scale::UTC);
    const Angle raan = Angle::Degrees(100.0);

    const auto computeSunRightAscension = [&instant](const Shared<const Frame>& aFrameSPtr) -> double
    {
        const Vector3d sun = Sun::Default().getPositionIn(aFrameSPtr, instant).getCoordinates();

        return std::atan2(sun.y(), sun.x());
    };

    const auto toLocalTimeHours = [&raan](double aSunAngle) -> double
    {
        return std::fmod((std::fmod(raan.inRadians() - aSunAngle, 2.0 * M_PI) * 12.0 / M_PI) + 12.0, 24.0);
    };

    // Default frame is GCRF
    {
        EXPECT_DOUBLE_EQ(
            COE::ComputeLTAN(raan, instant).getTotalFloatingHours(),
            COE::ComputeLTAN(raan, instant, Sun::Default(), Frame::GCRF()).getTotalFloatingHours()
        );
        EXPECT_DOUBLE_EQ(
            COE::ComputeLTDN(raan, instant).getTotalFloatingHours(),
            COE::ComputeLTDN(raan, instant, Sun::Default(), Frame::GCRF()).getTotalFloatingHours()
        );
        EXPECT_DOUBLE_EQ(
            COE::ComputeMeanLTAN(raan, instant).getTotalFloatingHours(),
            COE::ComputeMeanLTAN(raan, instant, Sun::Default(), Frame::GCRF()).getTotalFloatingHours()
        );
        EXPECT_DOUBLE_EQ(
            COE::ComputeMeanLTDN(raan, instant).getTotalFloatingHours(),
            COE::ComputeMeanLTDN(raan, instant, Sun::Default(), Frame::GCRF()).getTotalFloatingHours()
        );
    }

    // The Sun right ascension is evaluated in the provided frame
    {
        const Shared<const Frame> cirfSPtr = Frame::CIRF();

        const double expectedLTAN = toLocalTimeHours(computeSunRightAscension(cirfSPtr));

        EXPECT_NEAR(
            COE::ComputeLTAN(raan, instant, Sun::Default(), cirfSPtr).getTotalFloatingHours(), expectedLTAN, 1e-9
        );
        EXPECT_NEAR(
            COE::ComputeLTDN(raan, instant, Sun::Default(), cirfSPtr).getTotalFloatingHours(),
            std::fmod(expectedLTAN + 12.0, 24.0),
            1e-9
        );

        const double gcrfLTAN = COE::ComputeLTAN(raan, instant).getTotalFloatingHours();
        const double cirfLTAN = COE::ComputeLTAN(raan, instant, Sun::Default(), cirfSPtr).getTotalFloatingHours();

        // The frames are not equivalent: precession since J2000 shifts the Sun right ascension by several tens of
        // seconds of time
        EXPECT_GT(std::abs(gcrfLTAN - cirfLTAN), 5.0 / 3600.0);
        EXPECT_LT(std::abs(gcrfLTAN - cirfLTAN), 0.1);

        const double gcrfMeanLTAN = COE::ComputeMeanLTAN(raan, instant).getTotalFloatingHours();
        const double cirfMeanLTAN =
            COE::ComputeMeanLTAN(raan, instant, Sun::Default(), cirfSPtr).getTotalFloatingHours();

        EXPECT_NEAR(cirfMeanLTAN - gcrfMeanLTAN, cirfLTAN - gcrfLTAN, 1e-9);
    }

    // Undefined frame
    {
        EXPECT_ANY_THROW(COE::ComputeLTAN(raan, instant, Sun::Default(), nullptr));
        EXPECT_ANY_THROW(COE::ComputeLTDN(raan, instant, Sun::Default(), nullptr));
        EXPECT_ANY_THROW(COE::ComputeMeanLTAN(raan, instant, Sun::Default(), nullptr));
        EXPECT_ANY_THROW(COE::ComputeMeanLTDN(raan, instant, Sun::Default(), nullptr));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, ComputeRaanFromLTAN_Frame)
{
    const Shared<const Celestial> earthSPtr = Environment::Default().accessCelestialObjectWithName("Earth");
    const Instant epoch = Instant::DateTime(DateTime::Parse("2026-01-01T00:00:00"), Scale::UTC);
    const Time ltan = Time::Parse("10:30:00");

    const Shared<const Frame> cirfSPtr = Frame::CIRF();

    // Default frame is GCRF
    {
        EXPECT_DOUBLE_EQ(
            COE::ComputeRaanFromLTAN(ltan, epoch, earthSPtr).inRadians(),
            COE::ComputeRaanFromLTAN(ltan, epoch, earthSPtr, Sun::Default(), Frame::GCRF()).inRadians()
        );
    }

    // Round trip in a non-default frame
    {
        const Angle raan = COE::ComputeRaanFromLTAN(ltan, epoch, earthSPtr, Sun::Default(), cirfSPtr);

        EXPECT_NEAR(
            COE::ComputeMeanLTAN(raan, epoch, Sun::Default(), cirfSPtr).getTotalFloatingHours(),
            ltan.getTotalFloatingHours(),
            1e-9
        );

        // The same RAAN does not give the same local time when interpreted in GCRF
        EXPECT_GT(
            std::abs(COE::ComputeMeanLTAN(raan, epoch).getTotalFloatingHours() - ltan.getTotalFloatingHours()),
            5.0 / 3600.0
        );
    }

    // Sun-synchronous COE carries the RAAN of the requested frame
    {
        const COE coe =
            COE::SunSynchronous(Length::Kilometers(6878.0), ltan, epoch, earthSPtr, 0.0, Angle::Zero(), cirfSPtr);

        EXPECT_NEAR(
            coe.getRaan().inRadians(),
            COE::ComputeRaanFromLTAN(ltan, epoch, earthSPtr, Sun::Default(), cirfSPtr).inRadians(),
            1e-12
        );
    }

    // Undefined frame
    {
        EXPECT_ANY_THROW(COE::ComputeRaanFromLTAN(ltan, epoch, earthSPtr, Sun::Default(), nullptr));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE, GeoSynchronous_Frame)
{
    const Shared<const Celestial> earthSPtr = Environment::Default().accessCelestialObjectWithName("Earth");
    const Instant epoch = Instant::DateTime(DateTime::Parse("2026-01-01T00:00:00"), Scale::UTC);
    const Angle longitude = Angle::Degrees(30.0);

    const Shared<const Frame> cirfSPtr = Frame::CIRF();

    // Default frame is GCRF
    {
        EXPECT_EQ(
            COE::GeoSynchronous(epoch, Angle::Degrees(0.01), longitude, earthSPtr),
            COE::GeoSynchronous(epoch, Angle::Degrees(0.01), longitude, earthSPtr, Frame::GCRF())
        );
    }

    // The RAAN points to the requested longitude, measured in the requested frame
    {
        const COE coe = COE::GeoSynchronous(epoch, Angle::Degrees(0.01), longitude, earthSPtr, cirfSPtr);

        const Position nodePosition = Position::Meters(
            {std::cos(longitude.inRadians()), std::sin(longitude.inRadians()), 0.0}, earthSPtr->accessFrame()
        );
        const Vector3d nodeCIRF = nodePosition.inFrame(cirfSPtr, epoch).getCoordinates();

        EXPECT_NEAR(coe.getRaan().inRadians(), std::atan2(nodeCIRF.y(), nodeCIRF.x()), 1e-12);

        const COE coeGCRF = COE::GeoSynchronous(epoch, Angle::Degrees(0.01), longitude, earthSPtr);

        // CIRF and GCRF share (almost) the same X axis, so the difference for an equatorial node is small but
        // well above numerical noise (~1e-4 deg from the tilt of the pole)
        EXPECT_GT(std::abs(coe.getRaan().inDegrees() - coeGCRF.getRaan().inDegrees()), 1e-5);
    }

    // Undefined frame
    {
        EXPECT_ANY_THROW(COE::GeoSynchronous(epoch, Angle::Degrees(0.01), longitude, earthSPtr, nullptr));
    }
}
