/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::eventcondition::RealCondition;

class OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition : public ::testing::Test
{
   protected:
    const RealCondition::Criteria defaultCriteria_ = RealCondition::Criteria::StrictlyPositive;
    const String defaultName_ = "Test Real Condition";
    const std::function<Real(const VectorXd&, const Real&)> defaultEvaluator_ =
        []([[maybe_unused]] const VectorXd& aStateVector, const Real& aTime) -> Real
    {
        return aTime;
    };
    const Real defaultTarget_ = 1.0;
    const RealCondition defaultCondition_ = {defaultName_, defaultCriteria_, defaultEvaluator_, defaultTarget_};
    const VectorXd defaultStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, Constructor)
{
    {
        EXPECT_NO_THROW(RealCondition(defaultName_, defaultCriteria_, defaultEvaluator_, defaultTarget_));
    }

    {
        EXPECT_NO_THROW(RealCondition(defaultName_, defaultCriteria_, defaultEvaluator_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultCondition_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, Getters)
{
    {
        EXPECT_TRUE(defaultCondition_.getName() == defaultName_);
        EXPECT_TRUE(defaultCondition_.getCriteria() == defaultCriteria_);
        EXPECT_NO_THROW(defaultCondition_.getEvaluator());  // Cannot compare equality for std::function
        EXPECT_TRUE(defaultCondition_.getTarget() == defaultTarget_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, evaluate)
{
    {
        EXPECT_EQ(defaultCondition_.evaluate(defaultStateVector_, -2.0), -3.0);
        EXPECT_EQ(defaultCondition_.evaluate(defaultStateVector_, 0.0), -1.0);
        EXPECT_EQ(defaultCondition_.evaluate(defaultStateVector_, 1.0), 0.0);
        EXPECT_EQ(defaultCondition_.evaluate(defaultStateVector_, 2.0), 1.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, isSatisfied)
{
    // Positive Crossing
    {
        RealCondition condition = {
            "name",
            RealCondition::Criteria::PositiveCrossing,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 2.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, -1.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 3.0, defaultStateVector_, 2.0));
    }

    // Negative Crossing
    {
        RealCondition condition = {
            "name",
            RealCondition::Criteria::NegativeCrossing,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 2.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, -1.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 3.0, defaultStateVector_, 2.0));
    }

    // Any Crossing
    {
        RealCondition condition = {
            "name",
            RealCondition::Criteria::AnyCrossing,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 2.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, -1.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 3.0, defaultStateVector_, 2.0));
    }

    // Strictly Positive (previous value doesn't matter)
    {
        RealCondition condition = {
            "name",
            RealCondition::Criteria::StrictlyPositive,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 2.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, -1.0));
        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 3.0, defaultStateVector_, 2.0));
    }

    // Strictly Negative (previous value doesn't matter)
    {
        RealCondition condition = {
            "name",
            RealCondition::Criteria::StrictlyNegative,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 2.0));
        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, -1.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 3.0, defaultStateVector_, 2.0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, StringFromCriteria)
{
    {
        EXPECT_TRUE(
            RealCondition::StringFromCriteria(RealCondition::Criteria::PositiveCrossing) == "Positive Crossing"
        );
        EXPECT_TRUE(
            RealCondition::StringFromCriteria(RealCondition::Criteria::NegativeCrossing) == "Negative Crossing"
        );
        EXPECT_TRUE(RealCondition::StringFromCriteria(RealCondition::Criteria::AnyCrossing) == "Any Crossing");
        EXPECT_TRUE(
            RealCondition::StringFromCriteria(RealCondition::Criteria::StrictlyPositive) == "Strictly Positive"
        );
        EXPECT_TRUE(
            RealCondition::StringFromCriteria(RealCondition::Criteria::StrictlyNegative) == "Strictly Negative"
        );
        EXPECT_TRUE(RealCondition::StringFromCriteria(RealCondition::Criteria::Undefined) == "Undefined");
    }
}
