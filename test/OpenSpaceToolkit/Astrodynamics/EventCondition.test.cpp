/// Apache License 2.0

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

    virtual Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override
    {
        (void)aStateVector;
        (void)aTime;
        return 5.0;
    }
};

class OpenSpaceToolkit_Astrodynamics_EventCondition : public ::testing::Test
{
   protected:
    const EventCondition::Criteria defaultCriteria_ = EventCondition::Criteria::PositiveCrossing;
    const String defaultName_ = "Test";
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
        TestCondition testCondition(defaultName_, defaultCriteria_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << testCondition << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, Print)
{
    {
        TestCondition testCondition(defaultName_, defaultCriteria_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(testCondition.print(std::cout, true));
        EXPECT_NO_THROW(testCondition.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, getName)
{
    {
        TestCondition testCondition(defaultName_, defaultCriteria_);

        EXPECT_TRUE(testCondition.getName() == defaultName_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, getCriteria)
{
    {
        TestCondition testCondition(defaultName_, defaultCriteria_);

        EXPECT_TRUE(testCondition.getCriteria() == defaultCriteria_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, isSatisfied)
{
    // Positive Crossing
    {
        TestCondition testCondition(defaultName_, EventCondition::Criteria::PositiveCrossing);

        EXPECT_TRUE(testCondition.isSatisfied(5.0, -4.0));
        EXPECT_FALSE(testCondition.isSatisfied(-4.0, 5.0));
        EXPECT_FALSE(testCondition.isSatisfied(3.0, 5.0));
        EXPECT_FALSE(testCondition.isSatisfied(5.0, 3.0));
    }

    // Negative Crossing
    {
        TestCondition testCondition(defaultName_, EventCondition::Criteria::NegativeCrossing);

        EXPECT_TRUE(testCondition.isSatisfied(-4.0, 5.0));
        EXPECT_FALSE(testCondition.isSatisfied(5.0, -4.0));
        EXPECT_FALSE(testCondition.isSatisfied(3.0, 5.0));
        EXPECT_FALSE(testCondition.isSatisfied(5.0, 3.0));
    }

    // Any Crossing
    {
        TestCondition testCondition(defaultName_, EventCondition::Criteria::AnyCrossing);

        EXPECT_TRUE(testCondition.isSatisfied(-5.0, 4.0));
        EXPECT_TRUE(testCondition.isSatisfied(-5.0, 4.0));
        EXPECT_FALSE(testCondition.isSatisfied(3.0, 5.0));
        EXPECT_FALSE(testCondition.isSatisfied(5.0, 3.0));
    }

    // Strictly Positive (previous value doesn't matter)
    {
        TestCondition testCondition(defaultName_, EventCondition::Criteria::StrictlyPositive);

        EXPECT_TRUE(testCondition.isSatisfied(5.0, 4.0));
        EXPECT_TRUE(testCondition.isSatisfied(4.0, 5.0));
        EXPECT_FALSE(testCondition.isSatisfied(-5.0, 5.0));
    }

    // Strictly Negative (previous value doesn't matter)
    {
        TestCondition testCondition(defaultName_, EventCondition::Criteria::StrictlyNegative);

        EXPECT_TRUE(testCondition.isSatisfied(-5.0, 4.0));
        EXPECT_TRUE(testCondition.isSatisfied(-4.0, 5.0));
        EXPECT_FALSE(testCondition.isSatisfied(5.0, -5.0));
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
