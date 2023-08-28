/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;

class TestCondition : public EventCondition
{
   public:
    TestCondition(const String& aName, const Criteria& aCriteria)
        : EventCondition(aName, aCriteria)
    {
    }

    MOCK_METHOD(
        bool,
        isSatisfied,
        (const VectorXd& currentStateVector,
         const Real& currentTime,
         const VectorXd& previousStateVector,
         const Real& previousTime),
        (const, override)
    );
};

class OpenSpaceToolkit_Astrodynamics_EventCondition : public ::testing::Test
{
   protected:
    const EventCondition::Criteria defaultCriteria_ = EventCondition::Criteria::PositiveCrossing;
    const String defaultName_ = "Test";
    const TestCondition defaultCondition_ = {defaultName_, defaultCriteria_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, Constructor)
{
    {
        EXPECT_NO_THROW(TestCondition testCondition(defaultName_, defaultCriteria_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultCondition_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, getName)
{
    {
        EXPECT_TRUE(defaultCondition_.getName() == defaultName_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, getCriteria)
{
    {
        EXPECT_TRUE(defaultCondition_.getCriteria() == defaultCriteria_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, getComparator)
{
    {
        EXPECT_NO_THROW(defaultCondition_.getComparator());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, comparator)
{
    // Positive Crossing
    {
        TestCondition testCondition = {
            defaultName_,
            EventCondition::Criteria::PositiveCrossing,
        };

        EXPECT_TRUE(testCondition.getComparator()(5.0, -4.0));
        EXPECT_FALSE(testCondition.getComparator()(-4.0, 5.0));
        EXPECT_FALSE(testCondition.getComparator()(3.0, 5.0));
        EXPECT_FALSE(testCondition.getComparator()(5.0, 3.0));
    }

    // Negative Crossing
    {
        TestCondition testCondition = {
            defaultName_,
            EventCondition::Criteria::NegativeCrossing,
        };

        EXPECT_TRUE(testCondition.getComparator()(-4.0, 5.0));
        EXPECT_FALSE(testCondition.getComparator()(5.0, -4.0));
        EXPECT_FALSE(testCondition.getComparator()(3.0, 5.0));
        EXPECT_FALSE(testCondition.getComparator()(5.0, 3.0));
    }

    // Any Crossing
    {
        TestCondition testCondition = {
            defaultName_,
            EventCondition::Criteria::AnyCrossing,
        };

        EXPECT_TRUE(testCondition.getComparator()(-5.0, 4.0));
        EXPECT_TRUE(testCondition.getComparator()(5.0, -4.0));
        EXPECT_FALSE(testCondition.getComparator()(3.0, 5.0));
        EXPECT_FALSE(testCondition.getComparator()(5.0, 3.0));
    }

    // Strictly Positive (previous value doesn't matter)
    {
        TestCondition testCondition = {
            defaultName_,
            EventCondition::Criteria::StrictlyPositive,
        };

        EXPECT_TRUE(testCondition.getComparator()(5.0, 4.0));
        EXPECT_TRUE(testCondition.getComparator()(4.0, 5.0));
        EXPECT_FALSE(testCondition.getComparator()(-5.0, 5.0));
    }

    // Strictly Negative (previous value doesn't matter)
    {
        TestCondition testCondition = {
            defaultName_,
            EventCondition::Criteria::StrictlyNegative,
        };

        EXPECT_TRUE(testCondition.getComparator()(-5.0, 4.0));
        EXPECT_TRUE(testCondition.getComparator()(-4.0, 5.0));
        EXPECT_FALSE(testCondition.getComparator()(5.0, -5.0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, StringFromCriteria)
{
    {
        EXPECT_TRUE(
            EventCondition::StringFromCriteria(EventCondition::Criteria::PositiveCrossing) == "Positive Crossing"
        );
        EXPECT_TRUE(
            EventCondition::StringFromCriteria(EventCondition::Criteria::NegativeCrossing) == "Negative Crossing"
        );
        EXPECT_TRUE(EventCondition::StringFromCriteria(EventCondition::Criteria::AnyCrossing) == "Any Crossing");
        EXPECT_TRUE(
            EventCondition::StringFromCriteria(EventCondition::Criteria::StrictlyPositive) == "Strictly Positive"
        );
        EXPECT_TRUE(
            EventCondition::StringFromCriteria(EventCondition::Criteria::StrictlyNegative) == "Strictly Negative"
        );
        EXPECT_TRUE(EventCondition::StringFromCriteria(EventCondition::Criteria::Undefined) == "Undefined");
    }
}
