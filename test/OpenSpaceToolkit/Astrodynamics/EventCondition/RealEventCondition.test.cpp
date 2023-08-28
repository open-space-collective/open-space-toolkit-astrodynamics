/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::eventcondition::RealEventCondition;

class OpenSpaceToolkit_Astrodynamics_RealEventCondition : public ::testing::Test
{
    void SetUp() override
    {
        defaultStateVector_.resize(1);
        defaultStateVector_ << 1.0;
    }

   protected:
    const RealEventCondition::Criteria defaultCriteria_ = RealEventCondition::Criteria::PositiveCrossing;
    const Real defaultTarget_ = 1.0;
    const std::function<Real(VectorXd, Real)> evaluator_ = []([[maybe_unused]] const VectorXd& aStateVector,
                                                              const Real& aTime) -> Real
    {
        return aTime;
    };
    const RealEventCondition defaultCondition_ = {
        "test",
        defaultCriteria_,
        evaluator_,
        defaultTarget_,
    };
    VectorXd defaultStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_RealEventCondition, Constructor)
{
    {
        EXPECT_NO_THROW(RealEventCondition condition("name", defaultCriteria_, evaluator_, defaultTarget_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RealEventCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RealEventCondition, getTarget)
{
    {
        EXPECT_TRUE(defaultCondition_.getTarget() == defaultTarget_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RealEventCondition, Evaluate)
{
    {
        const Real time = 5.0;
        EXPECT_TRUE(defaultCondition_.evaluate(defaultStateVector_, time) == (time - defaultTarget_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RealEventCondition, IsSatisfied)
{
    // Positive Crossing
    {
        RealEventCondition condition = {
            "name",
            RealEventCondition::Criteria::PositiveCrossing,
            evaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 2.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, -1.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 3.0, defaultStateVector_, 2.0));
    }

    // Negative Crossing
    {
        RealEventCondition condition = {
            "name",
            RealEventCondition::Criteria::NegativeCrossing,
            evaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 2.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, -1.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 3.0, defaultStateVector_, 2.0));
    }

    // Any Crossing
    {
        RealEventCondition condition = {
            "name",
            RealEventCondition::Criteria::AnyCrossing,
            evaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 2.0));
        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, -1.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 3.0, defaultStateVector_, 2.0));
    }

    // Strictly Positive (previous value doesn't matter)
    {
        RealEventCondition condition = {
            "name",
            RealEventCondition::Criteria::StrictlyPositive,
            evaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 0.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 0.0));
    }

    // Strictly Negative (previous value doesn't matter)
    {
        RealEventCondition condition = {
            "name",
            RealEventCondition::Criteria::StrictlyNegative,
            evaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(defaultStateVector_, 0.0, defaultStateVector_, 1.0));
        EXPECT_FALSE(condition.isSatisfied(defaultStateVector_, 2.0, defaultStateVector_, 1.0));
    }
}
