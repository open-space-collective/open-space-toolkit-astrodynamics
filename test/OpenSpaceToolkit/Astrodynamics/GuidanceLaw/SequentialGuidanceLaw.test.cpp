/// Apache License 2.0

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/SequentialGuidanceLaw.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::guidancelaw::SequentialGuidanceLaw;

class MockGuidanceLaw1 : public GuidanceLaw
{
   public:
    MockGuidanceLaw1(const String& aName)
        : GuidanceLaw(aName)
    {
    }

    Vector3d calculateThrustAccelerationAt(
        [[maybe_unused]] const Instant& anInstant,
        [[maybe_unused]] const Vector3d& aPositionCoordinates,
        [[maybe_unused]] const Vector3d& aVelocityCoordinates,
        [[maybe_unused]] const Real& aThrustDirection,
        [[maybe_unused]] const Shared<const Frame>& outputFrameSPtr
    ) const override
    {
        return Vector3d(1.0, 2.0, 3.0);
    }
};

class MockGuidanceLaw2 : public GuidanceLaw
{
   public:
    MockGuidanceLaw2(const String& aName)
        : GuidanceLaw(aName)
    {
    }

    Vector3d calculateThrustAccelerationAt(
        [[maybe_unused]] const Instant& anInstant,
        [[maybe_unused]] const Vector3d& aPositionCoordinates,
        [[maybe_unused]] const Vector3d& aVelocityCoordinates,
        [[maybe_unused]] const Real& aThrustDirection,
        [[maybe_unused]] const Shared<const Frame>& outputFrameSPtr
    ) const override
    {
        return Vector3d(4.0, 5.0, 6.0);
    }
};

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_SequentialGuidanceLaw : public ::testing::Test
{
   protected:
    const Interval interval1_ = Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Seconds(100.0));

    const Interval interval2_ =
        Interval::Closed(Instant::J2000() + Duration::Seconds(200.0), Instant::J2000() + Duration::Seconds(300.0));

    const Shared<GuidanceLaw> guidanceLaw1_ = std::make_shared<MockGuidanceLaw1>("My Guidance Law 1");
    const Shared<GuidanceLaw> guidanceLaw2_ = std::make_shared<MockGuidanceLaw2>("My Guidance Law 2");

    const SequentialGuidanceLaw sequentialGuidanceLaw_ =
        SequentialGuidanceLaw(Array<Tuple<Shared<GuidanceLaw>, Interval>>(
            {Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
             Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
        ));
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_SequentialGuidanceLaw, Constructor)
{
    // No arguments
    {
        EXPECT_NO_THROW(SequentialGuidanceLaw());
    }

    // With arguments
    {
        EXPECT_NO_THROW(SequentialGuidanceLaw(Array<Tuple<Shared<GuidanceLaw>, Interval>>(
            {Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
             Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
        )));
    }

    // Undefined interval
    {
        EXPECT_THROW(
            try {
                SequentialGuidanceLaw(Array<Tuple<Shared<GuidanceLaw>, Interval>>(
                    {Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw1_, Interval::Undefined())}
                ));
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Interval} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    // Null guidance law
    {
        EXPECT_THROW(
            try {
                SequentialGuidanceLaw(Array<Tuple<Shared<GuidanceLaw>, Interval>>(
                    {Tuple<Shared<GuidanceLaw>, Interval>(std::shared_ptr<GuidanceLaw>(nullptr), interval1_)}
                ));
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Guidance law cannot be null.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Intersecting intervals
    {
        EXPECT_THROW(
            try {
                SequentialGuidanceLaw(Array<Tuple<Shared<GuidanceLaw>, Interval>>(
                    {Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
                     Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw2_, interval1_)}
                ));
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("Interval intersects"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_SequentialGuidanceLaw, Getters)
{
    {
        const SequentialGuidanceLaw sequentialGuidanceLaw =
            SequentialGuidanceLaw(Array<Tuple<Shared<GuidanceLaw>, Interval>>(
                {Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
                 Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
            ));

        const auto expected = Array<Tuple<Shared<GuidanceLaw>, Interval>>(
            {Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
             Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
        );

        const auto actual = sequentialGuidanceLaw.getGuidanceLawsWithIntervals();

        EXPECT_EQ(actual.getSize(), expected.getSize());
        for (size_t i = 0; i < expected.getSize(); ++i)
        {
            EXPECT_EQ(std::get<0>(actual[i]), std::get<0>(expected[i]));
            EXPECT_EQ(std::get<1>(actual[i]), std::get<1>(expected[i]));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_SequentialGuidanceLaw, AddGuidanceLaw)
{
    // Adding guidance laws one by one
    {
        SequentialGuidanceLaw sequentialGuidanceLaw = SequentialGuidanceLaw();

        EXPECT_TRUE(sequentialGuidanceLaw.getGuidanceLawsWithIntervals().isEmpty());

        sequentialGuidanceLaw.addGuidanceLaw(guidanceLaw1_, interval1_);

        auto expected =
            Array<Tuple<Shared<GuidanceLaw>, Interval>>({Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw1_, interval1_)
            });
        auto actual = sequentialGuidanceLaw.getGuidanceLawsWithIntervals();
        EXPECT_EQ(actual.getSize(), expected.getSize());
        for (size_t i = 0; i < expected.getSize(); ++i)
        {
            EXPECT_EQ(std::get<0>(actual[i]), std::get<0>(expected[i]));
            EXPECT_EQ(std::get<1>(actual[i]), std::get<1>(expected[i]));
        }

        sequentialGuidanceLaw.addGuidanceLaw(guidanceLaw2_, interval2_);
        expected = Array<Tuple<Shared<GuidanceLaw>, Interval>>(
            {Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
             Tuple<Shared<GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
        );
        actual = sequentialGuidanceLaw.getGuidanceLawsWithIntervals();
        EXPECT_EQ(actual.getSize(), expected.getSize());
        for (size_t i = 0; i < expected.getSize(); ++i)
        {
            EXPECT_EQ(std::get<0>(actual[i]), std::get<0>(expected[i]));
            EXPECT_EQ(std::get<1>(actual[i]), std::get<1>(expected[i]));
        }
    }

    // Undefined interval
    {
        SequentialGuidanceLaw sequentialGuidanceLaw = SequentialGuidanceLaw();
        EXPECT_THROW(
            try {
                sequentialGuidanceLaw.addGuidanceLaw(guidanceLaw1_, Interval::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Interval} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    // Null guidance law
    {
        SequentialGuidanceLaw sequentialGuidanceLaw = SequentialGuidanceLaw();
        EXPECT_THROW(
            try {
                sequentialGuidanceLaw.addGuidanceLaw(std::shared_ptr<GuidanceLaw>(nullptr), interval1_);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Guidance law cannot be null.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
    // Intersecting intervals
    {
        SequentialGuidanceLaw sequentialGuidanceLaw = SequentialGuidanceLaw();
        sequentialGuidanceLaw.addGuidanceLaw(guidanceLaw1_, interval1_);

        EXPECT_THROW(
            try {
                sequentialGuidanceLaw.addGuidanceLaw(guidanceLaw2_, interval1_);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("Interval intersects"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_SequentialGuidanceLaw, CalculateThrustAccelerationAt)
{
    {
        struct TestCase
        {
            Instant instant;
            Vector3d expectedAcceleration;
        };

        const Array<TestCase> testCases = {
            {Instant::J2000() - Duration::Seconds(50.0), Vector3d(0.0, 0.0, 0.0)},
            {Instant::J2000() + Duration::Seconds(50.0), Vector3d(1.0, 2.0, 3.0)},
            {Instant::J2000() + Duration::Seconds(150.0), Vector3d(0.0, 0.0, 0.0)},
            {Instant::J2000() + Duration::Seconds(250.0), Vector3d(4.0, 5.0, 6.0)},
            {Instant::J2000() + Duration::Seconds(350.0), Vector3d(0.0, 0.0, 0.0)}
        };

        for (const auto& testCase : testCases)
        {
            const Vector3d acceleration = sequentialGuidanceLaw_.calculateThrustAccelerationAt(
                testCase.instant, Vector3d::Zero(), Vector3d::Zero(), 1.0, Frame::GCRF()
            );

            EXPECT_TRUE(acceleration.isNear(testCase.expectedAcceleration, 1e-15));
        }
    }
}
