/// Apache License 2.0

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/HeterogeneousGuidanceLaw.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::guidancelaw::HeterogeneousGuidanceLaw;

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

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_HeterogeneousGuidanceLaw : public ::testing::Test
{
   protected:
    const Interval interval1_ = Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Seconds(100.0));

    const Interval interval2_ =
        Interval::Closed(Instant::J2000() + Duration::Seconds(200.0), Instant::J2000() + Duration::Seconds(300.0));

    const Shared<const GuidanceLaw> guidanceLaw1_ = std::make_shared<MockGuidanceLaw1>("My Guidance Law 1");
    const Shared<const GuidanceLaw> guidanceLaw2_ = std::make_shared<MockGuidanceLaw2>("My Guidance Law 2");

    const HeterogeneousGuidanceLaw heterogeneousGuidanceLaw_ =
        HeterogeneousGuidanceLaw(Array<Pair<Shared<const GuidanceLaw>, Interval>>(
            {Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
             Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
        ));
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_HeterogeneousGuidanceLaw, Constructor)
{
    // No arguments
    {
        EXPECT_NO_THROW(HeterogeneousGuidanceLaw());
    }

    // With arguments
    {
        EXPECT_NO_THROW(HeterogeneousGuidanceLaw(Array<Pair<Shared<const GuidanceLaw>, Interval>>(
            {Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
             Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
        )));
    }

    // Undefined interval
    {
        EXPECT_THROW(
            try {
                HeterogeneousGuidanceLaw(Array<Pair<Shared<const GuidanceLaw>, Interval>>(
                    {Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw1_, Interval::Undefined())}
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
                HeterogeneousGuidanceLaw(Array<Pair<Shared<const GuidanceLaw>, Interval>>(
                    {Pair<Shared<const GuidanceLaw>, Interval>(std::shared_ptr<const GuidanceLaw>(nullptr), interval1_)}
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
                HeterogeneousGuidanceLaw(Array<Pair<Shared<const GuidanceLaw>, Interval>>(
                    {Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
                     Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw2_, interval1_)}
                ));
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("Interval intersects"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_HeterogeneousGuidanceLaw, Getters)
{
    {
        const HeterogeneousGuidanceLaw heterogeneousGuidanceLaw =
            HeterogeneousGuidanceLaw(Array<Pair<Shared<const GuidanceLaw>, Interval>>(
                {Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
                 Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
            ));

        const auto expected = Array<Pair<Shared<const GuidanceLaw>, Interval>>(
            {Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
             Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
        );

        const auto actual = heterogeneousGuidanceLaw.getGuidanceLawsWithIntervals();

        EXPECT_EQ(actual.getSize(), expected.getSize());
        for (size_t i = 0; i < expected.getSize(); ++i)
        {
            EXPECT_EQ(actual[i].first, expected[i].first);
            EXPECT_EQ(actual[i].second, expected[i].second);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_HeterogeneousGuidanceLaw, AddGuidanceLaw)
{
    // Adding guidance laws one by one
    {
        HeterogeneousGuidanceLaw heterogeneousGuidanceLaw = HeterogeneousGuidanceLaw();

        EXPECT_TRUE(heterogeneousGuidanceLaw.getGuidanceLawsWithIntervals().isEmpty());

        heterogeneousGuidanceLaw.addGuidanceLaw(guidanceLaw1_, interval1_);

        auto expected = Array<Pair<Shared<const GuidanceLaw>, Interval>>(
            {Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw1_, interval1_)}
        );
        auto actual = heterogeneousGuidanceLaw.getGuidanceLawsWithIntervals();
        EXPECT_EQ(actual.getSize(), expected.getSize());
        for (size_t i = 0; i < expected.getSize(); ++i)
        {
            EXPECT_EQ(actual[i].first, expected[i].first);
            EXPECT_EQ(actual[i].second, expected[i].second);
        }

        heterogeneousGuidanceLaw.addGuidanceLaw(guidanceLaw2_, interval2_);
        expected = Array<Pair<Shared<const GuidanceLaw>, Interval>>(
            {Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw1_, interval1_),
             Pair<Shared<const GuidanceLaw>, Interval>(guidanceLaw2_, interval2_)}
        );
        actual = heterogeneousGuidanceLaw.getGuidanceLawsWithIntervals();
        EXPECT_EQ(actual.getSize(), expected.getSize());
        for (size_t i = 0; i < expected.getSize(); ++i)
        {
            EXPECT_EQ(actual[i].first, expected[i].first);
            EXPECT_EQ(actual[i].second, expected[i].second);
        }
    }

    // Undefined interval
    {
        HeterogeneousGuidanceLaw heterogeneousGuidanceLaw = HeterogeneousGuidanceLaw();
        EXPECT_THROW(
            try {
                heterogeneousGuidanceLaw.addGuidanceLaw(guidanceLaw1_, Interval::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Interval} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    // Null guidance law
    {
        HeterogeneousGuidanceLaw heterogeneousGuidanceLaw = HeterogeneousGuidanceLaw();
        EXPECT_THROW(
            try {
                heterogeneousGuidanceLaw.addGuidanceLaw(std::shared_ptr<const GuidanceLaw>(nullptr), interval1_);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Guidance law cannot be null.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
    // Intersecting intervals
    {
        HeterogeneousGuidanceLaw heterogeneousGuidanceLaw = HeterogeneousGuidanceLaw();
        heterogeneousGuidanceLaw.addGuidanceLaw(guidanceLaw1_, interval1_);

        EXPECT_THROW(
            try {
                heterogeneousGuidanceLaw.addGuidanceLaw(guidanceLaw2_, interval1_);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("Interval intersects"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_HeterogeneousGuidanceLaw, CalculateThrustAccelerationAt)
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
            const Vector3d acceleration = heterogeneousGuidanceLaw_.calculateThrustAccelerationAt(
                testCase.instant, Vector3d::Zero(), Vector3d::Zero(), 1.0, Frame::GCRF()
            );

            EXPECT_TRUE(acceleration.isNear(testCase.expectedAcceleration, 1e-15));
        }
    }
}
