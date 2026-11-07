/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_Access, Constructor)
{
    using ostk::core::type::String;

    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    // Complete access is valid if the TCA is after the AOS and the LOS is after the TCA
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        EXPECT_NO_THROW(Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation););
    }

    // Complete access is valid without a max elevation
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);

        EXPECT_NO_THROW(Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal););
        EXPECT_NO_THROW(Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, std::nullopt););
    }

    // Complete access is not valid if the TCA is before the AOS
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        EXPECT_THROW(
            try {
                Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    String::Format(
                        "TCA [{}] < AOS [{}]", timeOfClosestApproach.toString(), acquisitionOfSignal.toString()
                    ),
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Complete access is not valid if the LOS is before the TCA
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        EXPECT_THROW(
            try {
                Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    String::Format("LOS [{}] < TCA [{}]", lossOfSignal.toString(), timeOfClosestApproach.toString()),
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Partial access is valid even without a time of closest approach
    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const std::optional<Instant> timeOfClosestApproach = std::nullopt;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const std::optional<Angle> maxElevation = std::nullopt;

        EXPECT_NO_THROW(Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation););
    }

    // Complete access is not valid without a time of closest approach
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const std::optional<Instant> timeOfClosestApproach = std::nullopt;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);

        EXPECT_THROW(
            Access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal), ostk::core::error::RuntimeError
        );
    }

    // Partial access is not valid if the loss of signal is before the acquisition of signal
    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const std::optional<Instant> timeOfClosestApproach = std::nullopt;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const std::optional<Angle> maxElevation = std::nullopt;

        EXPECT_THROW(
            try {
                Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    String::Format("LOS [{}] < AOS [{}]", lossOfSignal.toString(), acquisitionOfSignal.toString()),
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, EqualToOperator)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_TRUE(access == access);
    }

    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_TRUE(access == access);
    }

    {
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access firstAccess = {
            Access::Type::Complete, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation
        };
        const Access secondAccess = {
            Access::Type::Partial, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation
        };

        EXPECT_FALSE(firstAccess == secondAccess);
    }

    {
        const Access::Type type = Access::Type::Complete;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access firstAccess = {
            type,
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
            timeOfClosestApproach,
            lossOfSignal,
            maxElevation
        };
        const Access secondAccess = {
            type,
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
            timeOfClosestApproach,
            lossOfSignal,
            maxElevation
        };

        EXPECT_FALSE(firstAccess == secondAccess);
    }

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_TRUE(access == access);
    }

    {
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);

        // A partial access without a time of closest approach is equal to itself, but not to one that has it
        const Access withoutTca = {Access::Type::Partial, acquisitionOfSignal, std::nullopt, lossOfSignal};
        const Access withTca = {
            Access::Type::Partial,
            acquisitionOfSignal,
            Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC),
            lossOfSignal
        };

        EXPECT_TRUE(withoutTca == withoutTca);
        EXPECT_FALSE(withoutTca == withTca);
        EXPECT_FALSE(withTca == withoutTca);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, NotEqualToOperator)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access != access);
    }

    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access != access);
    }

    {
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access firstAccess = {
            Access::Type::Complete, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation
        };
        const Access secondAccess = {
            Access::Type::Partial, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation
        };

        EXPECT_TRUE(firstAccess != secondAccess);
    }

    {
        const Access::Type type = Access::Type::Complete;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access firstAccess = {
            type,
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
            timeOfClosestApproach,
            lossOfSignal,
            maxElevation
        };
        const Access secondAccess = {
            type,
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
            timeOfClosestApproach,
            lossOfSignal,
            maxElevation
        };

        EXPECT_TRUE(firstAccess != secondAccess);
    }

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        const Access partialAccessWithoutTca = {Access::Type::Partial, acquisitionOfSignal, std::nullopt, lossOfSignal};

        EXPECT_FALSE(partialAccessWithoutTca != partialAccessWithoutTca);
        EXPECT_TRUE(partialAccessWithoutTca != access);
        EXPECT_TRUE(access != partialAccessWithoutTca);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, StreamOperator)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << access << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, IsComplete)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_TRUE(access.isComplete());
    }

    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access.isComplete());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, GetType)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_EQ(type, access.getType());
    }

    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_EQ(type, access.getType());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, GetAcquisitionOfSignal)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_EQ(acquisitionOfSignal, access.getAcquisitionOfSignal());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, GetTimeOfClosestApproach)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        ASSERT_TRUE(access.getTimeOfClosestApproach().has_value());
        EXPECT_EQ(timeOfClosestApproach, access.getTimeOfClosestApproach().value());
    }

    {
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);

        const Access access = {Access::Type::Partial, acquisitionOfSignal, std::nullopt, lossOfSignal};

        EXPECT_FALSE(access.getTimeOfClosestApproach().has_value());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, GetLossOfSignal)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_EQ(lossOfSignal, access.getLossOfSignal());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, GetInterval)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_EQ(Interval::Closed(acquisitionOfSignal, lossOfSignal), access.getInterval());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, GetDuration)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_EQ(Duration::Minutes(2.0), access.getDuration());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, GetMaxElevation)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        ASSERT_TRUE(access.getMaxElevation().has_value());
        EXPECT_EQ(Angle::Degrees(54.3), access.getMaxElevation().value());
    }

    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, std::nullopt};

        EXPECT_FALSE(access.getMaxElevation().has_value());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, StringFromType)
{
    using ostk::astrodynamics::Access;

    {
        EXPECT_EQ("Complete", Access::StringFromType(Access::Type::Complete));
        EXPECT_EQ("Partial", Access::StringFromType(Access::Type::Partial));
    }
}
