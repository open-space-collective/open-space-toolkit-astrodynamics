/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanEventCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::eventcondition::BooleanEventCondition;

class OpenSpaceToolkit_Astrodynamics_BooleanEventCondition : public ::testing::Test
{
    void SetUp() override
    {
        defaultStateVector_.resize(1);
        defaultStateVector_ << 0.5;
    }

   protected:
    const BooleanEventCondition::Criteria defaultCriteria_ = BooleanEventCondition::Criteria::PositiveCrossing;
    const std::function<bool(VectorXd, Real)> evaluator_ = [](const VectorXd& aStateVector,
                                                              [[maybe_unused]] const Real& aTime) -> Real
    {
        return aStateVector[0] > 0.0 && aStateVector[0] < 1.0;
    };
    const bool isInverse_ = false;
    const BooleanEventCondition defaultCondition_ = {
        "test",
        defaultCriteria_,
        evaluator_,
        isInverse_,
    };
    VectorXd defaultStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_BooleanEventCondition, Constructor)
{
    {
        EXPECT_NO_THROW(BooleanEventCondition condition("name", defaultCriteria_, evaluator_, isInverse_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_BooleanEventCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_BooleanEventCondition, isInversed)
{
    {
        EXPECT_TRUE(defaultCondition_.isInversed() == isInverse_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_BooleanEventCondition, Evaluate)
{
    {
        EXPECT_TRUE(defaultCondition_.evaluate(defaultStateVector_, 0.0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_BooleanEventCondition, IsSatisfied)
{
    VectorXd currentState(1);
    currentState << 0.5;
    VectorXd previousState(1);
    previousState << -0.5;

    {
        const BooleanEventCondition condition = {
            "test",
            defaultCriteria_,
            evaluator_,
            false,
        };

        EXPECT_TRUE(condition.isSatisfied(currentState, 0.0, previousState, 0.0));
        EXPECT_FALSE(condition.isSatisfied(previousState, 0.0, currentState, 0.0));
    }

    {
        const BooleanEventCondition condition = {
            "test",
            defaultCriteria_,
            evaluator_,
            true,
        };

        EXPECT_FALSE(condition.isSatisfied(currentState, 0.0, previousState, 0.0));
        EXPECT_TRUE(condition.isSatisfied(previousState, 0.0, currentState, 0.0));
    }
}
