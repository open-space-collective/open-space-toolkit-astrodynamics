/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::eventcondition::BooleanCondition;

class OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition : public ::testing::Test
{
   protected:
    const BooleanCondition::Criterion defaultCriterion_ = BooleanCondition::Criterion::PositiveCrossing;
    const String defaultName_ = "Test Boolean Condition";
    const std::function<bool(VectorXd, Real)> defaultEvaluator_ = []([[maybe_unused]] const VectorXd& aStateVector,
                                                                     const Real& aTime) -> Real
    {
        return aTime > 0.0;
    };
    const bool defaultInverseFlag_ = true;
    const BooleanCondition defaultCondition_ = {
        "test",
        defaultCriterion_,
        defaultEvaluator_,
        defaultInverseFlag_,
    };
    VectorXd defaultStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition, Constructor)
{
    {
        EXPECT_NO_THROW(BooleanCondition(defaultName_, defaultCriterion_, defaultEvaluator_));
    }
    {
        EXPECT_NO_THROW(BooleanCondition(defaultName_, defaultCriterion_, defaultEvaluator_, defaultInverseFlag_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition, Getters)
{
    {
        EXPECT_EQ(defaultCondition_.isInversed(), defaultInverseFlag_);
    }
    {
        BooleanCondition booleanCondition = BooleanCondition(defaultName_, defaultCriterion_, defaultEvaluator_);
        EXPECT_EQ(booleanCondition.isInversed(), false);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition, Evaluate)
{
    {
        EXPECT_EQ(defaultCondition_.evaluate(defaultStateVector_, -1.0), 1.0);
        EXPECT_EQ(defaultCondition_.evaluate(defaultStateVector_, 1.0), -1.0);
    }

    {
        BooleanCondition booleanCondition =
            BooleanCondition(defaultName_, defaultCriterion_, defaultEvaluator_, !defaultInverseFlag_);

        EXPECT_EQ(defaultCondition_.evaluate(defaultStateVector_, -1.0), 1.0);
        EXPECT_EQ(defaultCondition_.evaluate(defaultStateVector_, 1.0), -1.0);
    }
}
