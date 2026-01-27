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
    // FromAERInterval
    {
        const Interval<Real> azimuthInterval = Interval<Real>::Closed(0.0, 360.0);
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        EXPECT_NO_THROW(VisibilityCriterion visibilityCriterion =
                            VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval););

        // Undefined

        {
            EXPECT_THROW(
                VisibilityCriterion::FromAERInterval(Interval<Real>::Undefined(), elevationInterval, rangeInterval),
                ostk::core::error::runtime::Undefined
            );
            EXPECT_THROW(
                VisibilityCriterion::FromAERInterval(azimuthInterval, Interval<Real>::Undefined(), rangeInterval),
                ostk::core::error::runtime::Undefined
            );
            EXPECT_THROW(
                VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, Interval<Real>::Undefined()),
                ostk::core::error::runtime::Undefined
            );
        }

        // incorrect bounds
        {
            {
                EXPECT_THROW(
                    VisibilityCriterion::FromAERInterval(
                        ostk::mathematics::object::Interval<Real>::Closed(-1.0, 350.0), elevationInterval, rangeInterval
                    ),
                    ostk::core::error::RuntimeError
                );
                EXPECT_THROW(
                    VisibilityCriterion::FromAERInterval(
                        ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.1), elevationInterval, rangeInterval
                    ),
                    ostk::core::error::RuntimeError
                );
            }
            {
                EXPECT_THROW(
                    VisibilityCriterion::FromAERInterval(
                        azimuthInterval, ostk::mathematics::object::Interval<Real>::Closed(-91.0, 0.0), rangeInterval
                    ),
                    ostk::core::error::RuntimeError
                );
                EXPECT_THROW(
                    VisibilityCriterion::FromAERInterval(
                        azimuthInterval, ostk::mathematics::object::Interval<Real>::Closed(-45.0, 91.0), rangeInterval
                    ),
                    ostk::core::error::RuntimeError
                );
            }
            {
                EXPECT_THROW(
                    VisibilityCriterion::FromAERInterval(
                        azimuthInterval, elevationInterval, ostk::mathematics::object::Interval<Real>::Closed(-1.0, 5.0)
                    ),
                    ostk::core::error::RuntimeError
                );
            }
        }
    }

    // FromAERMask
    {
        {
            const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
            const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

            EXPECT_NO_THROW(VisibilityCriterion visibilityCriterion =
                                VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval););
        }

        // Incorrect bounds
        {
            const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);
            // empty map
            {
                const Map<Real, Real> azimuthElevationMask = {};
                EXPECT_THROW(
                    VisibilityCriterion::FromAERMask(azimuthElevationMask, Interval<Real>::Undefined()),
                    ostk::core::error::runtime::Undefined
                );
            }

            // azimuth key < 0.0 for first key
            {
                const Map<Real, Real> azimuthElevationMask = {{-1.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
                EXPECT_THROW(
                    VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval),
                    ostk::core::error::RuntimeError
                );
            }

            // azimuth key > 360.0 for last key
            {
                const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {90.0, 15.0}, {361.0, 20.0}};
                EXPECT_THROW(
                    VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval),
                    ostk::core::error::RuntimeError
                );
            }

            // elevation greater than 90 for a value
            {
                const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 91.0}};
                EXPECT_THROW(
                    VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval),
                    ostk::core::error::RuntimeError
                );
            }

            // lower bound of range < 0.0
            {
                const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
                EXPECT_THROW(
                    VisibilityCriterion::FromAERMask(azimuthElevationMask, Interval<Real>::Closed(-1.0, 1e6)),
                    ostk::core::error::RuntimeError
                );
            }
        }
    }

    // FromLineOfSight
    {
        EXPECT_NO_THROW(VisibilityCriterion visibilityCriterion =
                            VisibilityCriterion::FromLineOfSight(defaultEnvironment_););
    }

    // FromElevationInterval
    {
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);

        EXPECT_NO_THROW(VisibilityCriterion visibilityCriterion =
                            VisibilityCriterion::FromElevationInterval(elevationInterval););

        // Incorrect bounds
        {
            EXPECT_THROW(
                VisibilityCriterion::FromElevationInterval(ostk::mathematics::object::Interval<Real>::Closed(-91.0, 0.0)
                ),
                ostk::core::error::RuntimeError
            );

            EXPECT_THROW(
                VisibilityCriterion::FromElevationInterval(
                    ostk::mathematics::object::Interval<Real>::Closed(-45.0, 91.0)
                ),
                ostk::core::error::RuntimeError
            );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, EqualToOperator)
{
    // AERInterval
    {
        const Interval<Real> azimuthInterval = Interval<Real>::Closed(0.0, 360.0);
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion1 =
            VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval);
        const VisibilityCriterion visibilityCriterion2 =
            VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval);

        EXPECT_TRUE(visibilityCriterion1 == visibilityCriterion2);
    }

    // AERMask
    {
        const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
        const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion1 =
            VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval);
        const VisibilityCriterion visibilityCriterion2 =
            VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval);

        EXPECT_TRUE(visibilityCriterion1 == visibilityCriterion2);
    }

    // LineOfSight
    {
        const VisibilityCriterion visibilityCriterion1 = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);
        const VisibilityCriterion visibilityCriterion2 = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

        EXPECT_TRUE(visibilityCriterion1 == visibilityCriterion2);
    }

    // ElevationInterval
    {
        const Interval<Real> elevationInterval = Interval<Real>::Closed(0.0, 90.0);

        const VisibilityCriterion visibilityCriterion1 = VisibilityCriterion::FromElevationInterval(elevationInterval);
        const VisibilityCriterion visibilityCriterion2 = VisibilityCriterion::FromElevationInterval(elevationInterval);

        EXPECT_TRUE(visibilityCriterion1 == visibilityCriterion2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion, NotEqualToOperator)
{
    // AERInterval
    {
        const Interval<Real> azimuthInterval1 = Interval<Real>::Closed(0.0, 360.0);
        const Interval<Real> elevationInterval1 = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> rangeInterval1 = Interval<Real>::Closed(0.0, 1e6);

        const Interval<Real> azimuthInterval2 = Interval<Real>::Closed(0.0, 360.0);
        const Interval<Real> elevationInterval2 = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> rangeInterval2 = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion1 =
            VisibilityCriterion::FromAERInterval(azimuthInterval1, elevationInterval1, rangeInterval1);
        const VisibilityCriterion visibilityCriterion2 =
            VisibilityCriterion::FromAERInterval(azimuthInterval2, elevationInterval2, rangeInterval2);

        EXPECT_FALSE(visibilityCriterion1 != visibilityCriterion2);
    }

    // AERMask
    {
        const Map<Real, Real> azimuthElevationMask1 = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
        const Interval<Real> rangeInterval1 = Interval<Real>::Closed(0.0, 1e6);

        const Map<Real, Real> azimuthElevationMask2 = {{0.0, 10.0}, {90.0, 15.0}, {180.0, 20.0}};
        const Interval<Real> rangeInterval2 = Interval<Real>::Closed(0.0, 1e6);

        const VisibilityCriterion visibilityCriterion1 =
            VisibilityCriterion::FromAERMask(azimuthElevationMask1, rangeInterval1);
        const VisibilityCriterion visibilityCriterion2 =
            VisibilityCriterion::FromAERMask(azimuthElevationMask2, rangeInterval2);

        EXPECT_FALSE(visibilityCriterion1 != visibilityCriterion2);
    }

    // LineOfSight
    {
        const VisibilityCriterion visibilityCriterion1 = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);
        const VisibilityCriterion visibilityCriterion2 = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

        EXPECT_FALSE(visibilityCriterion1 != visibilityCriterion2);
    }

    // ElevationInterval
    {
        const Interval<Real> elevationInterval1 = Interval<Real>::Closed(0.0, 90.0);
        const Interval<Real> elevationInterval2 = Interval<Real>::Closed(0.0, 90.0);

        const VisibilityCriterion visibilityCriterion1 = VisibilityCriterion::FromElevationInterval(elevationInterval1);
        const VisibilityCriterion visibilityCriterion2 = VisibilityCriterion::FromElevationInterval(elevationInterval2);

        EXPECT_FALSE(visibilityCriterion1 != visibilityCriterion2);
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

struct AERMaskIsSatisfiedTestCase
{
    Angle azimuth;
    Angle elevation;
    Length range;
    bool expectedResult;

    friend std::ostream& operator<<(std::ostream& os, const AERMaskIsSatisfiedTestCase& testCase)
    {
        os << "{ "
           << "Azimuth: " << testCase.azimuth.toString() << ", "
           << "Elevation: " << testCase.elevation.toString() << ", "
           << "Range: " << testCase.range.toString() << ", "
           << "Expected: " << (testCase.expectedResult ? "True" : "False") << " }";
        return os;
    }
};

class OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion_AERMaskIsSatisfied
    : public OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion,
      public ::testing::WithParamInterface<AERMaskIsSatisfiedTestCase>
{
};

TEST_P(OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion_AERMaskIsSatisfied, Test)
{
    const auto& param = GetParam();

    const Map<Real, Real> azimuthElevationMask = {{0.0, 10.0}, {45.0, 15.0}, {90.0, 20.0}};
    const Interval<Real> rangeInterval = Interval<Real>::Closed(0.0, 1e6);

    const VisibilityCriterion visibilityCriterion =
        VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeInterval);

    const auto aerMask = visibilityCriterion.as<VisibilityCriterion::AERMask>();
    ASSERT_TRUE(aerMask.has_value());

    EXPECT_EQ(
        aerMask.value().isSatisfied(param.azimuth.inRadians(), param.elevation.inRadians(), param.range.inMeters()),
        param.expectedResult
    );

    if (std::abs(param.elevation.inDegrees()) <= 90.0 && param.azimuth.inDegrees() <= 360.0)
    {
        const AER aer(param.azimuth, param.elevation, param.range);
        EXPECT_EQ(aerMask.value().isSatisfied(aer), param.expectedResult);
    }
}

INSTANTIATE_TEST_SUITE_P(
    AERMaskIsSatisfied,
    OpenSpaceToolkit_Astrodynamics_Access_VisibilityCriterion_AERMaskIsSatisfied,
    ::testing::Values(
        AERMaskIsSatisfiedTestCase {Angle::Degrees(22.5), Angle::Degrees(20.0), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(0.0), Angle::Degrees(10.1), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(0.1), Angle::Degrees(10.1), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(360.0), Angle::Degrees(10.1), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(360.1), Angle::Degrees(10.1), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(360.1), Angle::Degrees(91.0), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {
            Angle::Degrees(360.0 * (1.0 - std::numeric_limits<double>::epsilon())),
            Angle::Degrees(10.1),
            Length::Meters(500e3),
            true
        },
        AERMaskIsSatisfiedTestCase {Angle::Degrees(45.0), Angle::Degrees(15.1), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(90.0), Angle::Degrees(20.1), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(89.9), Angle::Degrees(20.1), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(90.1), Angle::Degrees(20.1), Length::Meters(500e3), true},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(22.5), Angle::Degrees(12.4), Length::Meters(500e3), false},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(0.0), Angle::Degrees(9.9), Length::Meters(500e3), false},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(0.1), Angle::Degrees(9.9), Length::Meters(500e3), false},
        AERMaskIsSatisfiedTestCase {
            Angle::Degrees(360.0 * (1.0 - std::numeric_limits<double>::epsilon())),
            Angle::Degrees(9.9),
            Length::Meters(500e3),
            false
        },
        AERMaskIsSatisfiedTestCase {Angle::Degrees(90.0), Angle::Degrees(19.9), Length::Meters(500e3), false},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(89.9), Angle::Degrees(19.9), Length::Meters(500e3), false},
        AERMaskIsSatisfiedTestCase {Angle::Degrees(90.1), Angle::Degrees(19.9), Length::Meters(500e3), false}
    )
);

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
    const Interval<Real> elevationInterval = Interval<Real>::Closed(10.0, 80.0);

    const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromElevationInterval(elevationInterval);

    const auto elevationIntervalCriterion = visibilityCriterion.as<VisibilityCriterion::ElevationInterval>();
    ASSERT_TRUE(elevationIntervalCriterion.has_value());

    {
        {
            const Real elevation = 45.0 * deg2rad;

            EXPECT_TRUE(elevationIntervalCriterion.value().isSatisfied(elevation));
        }

        {
            const Real elevationOutside = 5.0 * deg2rad;

            EXPECT_FALSE(elevationIntervalCriterion.value().isSatisfied(elevationOutside));
        }
    }

    {
        {
            const Angle elevation = Angle::Degrees(45.0);

            EXPECT_TRUE(elevationIntervalCriterion.value().isSatisfied(elevation));
        }

        {
            const Angle elevationOutside = Angle::Degrees(5.0);

            EXPECT_FALSE(elevationIntervalCriterion.value().isSatisfied(elevationOutside));
        }
    }
}
