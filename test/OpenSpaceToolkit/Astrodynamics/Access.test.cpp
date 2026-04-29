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

    // Partial access is valid if even if only the acquisition of signal and loss of signal are defined
    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::Undefined();
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Undefined();

        EXPECT_NO_THROW(Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation););
    }

    // Partial access is not valid if the loss of signal is before the acquisition of signal
    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::Undefined();
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Angle maxElevation = Angle::Undefined();

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

        EXPECT_FALSE(Access::Undefined() == Access::Undefined());
        EXPECT_FALSE(Access::Undefined() == access);
        EXPECT_FALSE(access == Access::Undefined());
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

        EXPECT_TRUE(Access::Undefined() != Access::Undefined());
        EXPECT_TRUE(Access::Undefined() != access);
        EXPECT_TRUE(access != Access::Undefined());
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

TEST(OpenSpaceToolkit_Astrodynamics_Access, IsDefined)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    // Complete access is defined if the acquisition of signal, time of closest approach, loss of signal and max
    // elevation are all defined
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_TRUE(access.isDefined());
    }

    // Complete access is not defined if the time of closest approach is undefined
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::Undefined();
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access.isDefined());
    }

    // Complete access is not defined if the loss of signal is undefined
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::Undefined();
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access.isDefined());
    }

    // Complete access is not defined if the acquisition of signal is undefined
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::Undefined();
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Degrees(54.3);

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access.isDefined());
    }

    // Complete access is not defined if the max elevation is undefined
    {
        const Access::Type type = Access::Type::Complete;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Undefined();

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access.isDefined());
    }

    // Partial access is defined even if only the acquisition of signal and loss of signal are defined
    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::Undefined();
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Undefined();

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_TRUE(access.isDefined());
    }

    // Partial access is not defined if the acquisition of signal is undefined
    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::Undefined();
        const Instant timeOfClosestApproach = Instant::Undefined();
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
        const Angle maxElevation = Angle::Undefined();

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access.isDefined());
    }

    // Partial access is not defined if the loss of signal is undefined
    {
        const Access::Type type = Access::Type::Partial;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant timeOfClosestApproach = Instant::Undefined();
        const Instant lossOfSignal = Instant::Undefined();
        const Angle maxElevation = Angle::Undefined();

        const Access access = {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};

        EXPECT_FALSE(access.isDefined());
    }

    {
        EXPECT_FALSE(Access::Undefined().isDefined());
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

    {
        EXPECT_ANY_THROW(Access::Undefined().isComplete());
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

    {
        EXPECT_ANY_THROW(Access::Undefined().getType());
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

    {
        EXPECT_ANY_THROW(Access::Undefined().getAcquisitionOfSignal());
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

        EXPECT_EQ(timeOfClosestApproach, access.getTimeOfClosestApproach());
    }

    {
        EXPECT_ANY_THROW(Access::Undefined().getTimeOfClosestApproach());
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

    {
        EXPECT_ANY_THROW(Access::Undefined().getLossOfSignal());
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

    {
        EXPECT_ANY_THROW(Access::Undefined().getInterval());
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

    {
        EXPECT_ANY_THROW(Access::Undefined().getDuration());
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

        EXPECT_EQ(Angle::Degrees(54.3), access.getMaxElevation());
    }

    {
        EXPECT_ANY_THROW(Access::Undefined().getMaxElevation());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, Undefined)
{
    using ostk::astrodynamics::Access;

    {
        EXPECT_NO_THROW(Access::Undefined());

        EXPECT_FALSE(Access::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access, StringFromType)
{
    using ostk::astrodynamics::Access;

    {
        EXPECT_EQ("Undefined", Access::StringFromType(Access::Type::Undefined));
        EXPECT_EQ("Complete", Access::StringFromType(Access::Type::Complete));
        EXPECT_EQ("Partial", Access::StringFromType(Access::Type::Partial));
    }
}
