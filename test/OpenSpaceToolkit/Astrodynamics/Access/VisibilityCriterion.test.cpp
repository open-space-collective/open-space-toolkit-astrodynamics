/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>

#include <Global.test.hpp>

using ostk::core::container::Map;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::physics::coordinate::spherical::AER;
using ostk::physics::Environment;
using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using ostk::mathematics::object::Interval;
using ostk::mathematics::object::Vector3d;

using ostk::astrodynamics::access::VisibilityCriterion;

const double deg2rad = M_PI / 180.0;

class OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion : public ::testing::Test
{
   protected:
    Environment defaultEnvironment_ = Environment::Default();
    const Shared<const Celestial> defaultEarthSPtr_ = defaultEnvironment_.accessCelestialObjectWithName("Earth");
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, Constructor)
{
    // Test FromAERInterval
    {
        const Interval<Real> azimuthInterval = Interval<Real>::Closed(0.0, 360.0);
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        EXPECT_NO_THROW(VisibilityCriterion visibilityCriterion =
                            VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval););
    }

    // Test FromAERMask
    {
        const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        EXPECT_NO_THROW(VisibilityCriterion visibilityCriterion =
                            VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval););
    }

    // Test FromLineOfSight
    {
        EXPECT_NO_THROW(VisibilityCriterion visibilityCriterion =
                            VisibilityCriterion::FromLineOfSight(defaultEnvironment_););
    }

    // Test FromElevationInterval
    {
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);

        EXPECT_NO_THROW(VisibilityCriterion visibilityCriterion =
                            VisibilityCriterion::FromElevationInterval(elevationInterval););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, Is)
{
    // AERInterval
    {
        const Interval<Real> azimuthInterval = Interval<Real>::Closed(0.0, 360.0);
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval);

        EXPECT_TRUE(visibilityCriterion.is<VisibilityCriterion::AERInterval>());
    }

    // AERMask
    {
        const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval);

        EXPECT_TRUE(visibilityCriterion.is<VisibilityCriterion::AERMask>());
    }

    // LineOfSight
    {
        const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

        EXPECT_TRUE(visibilityCriterion.is<VisibilityCriterion::LineOfSight>());
    }

    // ElevationInterval
    {
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);

        const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromElevationInterval(elevationInterval);

        EXPECT_TRUE(visibilityCriterion.is<VisibilityCriterion::ElevationInterval>());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, As)
{
    // AERInterval
    {
        const Interval<Real> azimuthInterval = Interval<Real>::Closed(0.0, 360.0);
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval);

        const auto aerInterval = visibilityCriterion.as<VisibilityCriterion::AERInterval>();
        ASSERT_TRUE(aerInterval.has_value());
        EXPECT_EQ(aerInterval.value().azimuth.getLowerBound(), azimuthInterval.getLowerBound() * deg2rad);
        EXPECT_EQ(aerInterval.value().azimuth.getUpperBound(), azimuthInterval.getUpperBound() * deg2rad);
        EXPECT_EQ(aerInterval.value().elevation.getLowerBound(), elevationInterval.getLowerBound() * deg2rad);
        EXPECT_EQ(aerInterval.value().elevation.getUpperBound(), elevationInterval.getUpperBound() * deg2rad);
        EXPECT_EQ(aerInterval.value().range, rangeInterval);
    }

    // AERMask
    {
        const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval);

        const auto aerMask = visibilityCriterion.as<VisibilityCriterion::AERMask>();
        ASSERT_TRUE(aerMask.has_value());
        for (const auto& [azimuth, elevation] : azimuthElevationMask)
        {
            EXPECT_EQ(aerMask.value().azimuthElevationMask.at(azimuth * deg2rad), elevation * deg2rad);
        }
        EXPECT_EQ(aerMask.value().range, rangeInterval);
    }

    // LineOfSight
    {
        const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

        const auto lineOfSight = visibilityCriterion.as<VisibilityCriterion::LineOfSight>();
        ASSERT_TRUE(lineOfSight.has_value());
        EXPECT_TRUE(lineOfSight.value().environment.isDefined());
    }

    // ElevationInterval
    {
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);

        const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromElevationInterval(elevationInterval);

        const auto elevationIntervalCriterion = visibilityCriterion.as<VisibilityCriterion::ElevationInterval>();
        ASSERT_TRUE(elevationIntervalCriterion.has_value());
        EXPECT_EQ(
            elevationIntervalCriterion.value().elevation.getLowerBound(), elevationInterval.getLowerBound() * deg2rad
        );
        EXPECT_EQ(
            elevationIntervalCriterion.value().elevation.getUpperBound(), elevationInterval.getUpperBound() * deg2rad
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, AERIntervalIsSatisfied)
{
    {
        const Interval<Real> azimuthInterval = Interval<Real>::Closed(0.0, 360.0);
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 500e3);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval);

        const auto aerInterval = visibilityCriterion.as<VisibilityCriterion::AERInterval>();
        ASSERT_TRUE(aerInterval.has_value());

        AER aer(Angle::Degrees(90.0), Angle::Degrees(22.5), Length::Meters(250e3));

        EXPECT_TRUE(aerInterval.value().isSatisfied(aer));

        AER aerOutside(Angle::Degrees(180.0), Angle::Degrees(-22.5), Length::Meters(250e3));

        EXPECT_FALSE(aerInterval.value().isSatisfied(aerOutside));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, AERMaskIsSatisfied)
{
    {
        Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {45.0, 15.0}, {90.0, 20.0}};
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval);

        const auto aerMask = visibilityCriterion.as<VisibilityCriterion::AERMask>();
        ASSERT_TRUE(aerMask.has_value());

        AER aer(Angle::Degrees(22.5), Angle::Degrees(20.0), Length::Meters(500e3));

        EXPECT_TRUE(aerMask.value().isSatisfied(aer));

        AER aerOutside(Angle::Degrees(22.5), Angle::Degrees(5.0), Length::Meters(500e3));

        EXPECT_FALSE(aerMask.value().isSatisfied(aerOutside));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, LineOfSightIsSatisfied)
{
    {
        const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

        const auto lineOfSight = visibilityCriterion.as<VisibilityCriterion::LineOfSight>();
        ASSERT_TRUE(lineOfSight.has_value());

        const Instant instant = Instant::J2000();
        const Vector3d fromPosition(7000e3, 0.0, 0.0);
        const Vector3d toPosition(0.0, 7000e3, 0.0);

        EXPECT_FALSE(lineOfSight.value().isSatisfied(instant, fromPosition, toPosition));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, ElevationIntervalIsSatisfied)
{
    {
        const Interval<Real> elevationInterval = Interval<Real>::Closed(10.0, 80.0);

        const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromElevationInterval(elevationInterval);

        const auto elevationIntervalCriterion = visibilityCriterion.as<VisibilityCriterion::ElevationInterval>();
        ASSERT_TRUE(elevationIntervalCriterion.has_value());

        const Real elevation = 45.0 * deg2rad;

        EXPECT_TRUE(elevationIntervalCriterion.value().isSatisfied(elevation));

        const Real elevationOutside = 5.0 * deg2rad;

        EXPECT_FALSE(elevationIntervalCriterion.value().isSatisfied(elevationOutside));
    }
}
