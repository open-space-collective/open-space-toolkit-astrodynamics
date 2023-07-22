/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Duration;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::DurationCondition;

class OpenSpaceToolkit_Astrodynamics_DurationCondition : public ::testing::Test
{
    void SetUp() override
    {
        defaultCondition_ = std::make_shared<DurationCondition>(defaultCriteria_, defaultDuration_);
    }

   protected:
    const DurationCondition::Criteria defaultCriteria_ = DurationCondition::Criteria::PositiveOnly;
    const Duration defaultDuration_ = Duration::Seconds(5.0);
    Shared<DurationCondition> defaultCondition_ = nullptr;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_DurationCondition, Constructor)
{
    {
        EXPECT_NO_THROW(DurationCondition durationCondition(defaultCriteria_, defaultDuration_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_DurationCondition, getDuration)
{
    {
        EXPECT_TRUE(defaultCondition_->getDuration() == defaultDuration_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_DurationCondition, evaluate)
{
    {
        EXPECT_TRUE(defaultCondition_->evaluate(VectorXd::Zero(6), defaultDuration_.inSeconds()) == 0.0);
    }

    {
        EXPECT_TRUE(defaultCondition_->evaluate(VectorXd::Zero(6), 10.0) == (10.0 - defaultDuration_.inSeconds()));
    }

    {
        EXPECT_FALSE(defaultCondition_->evaluate(VectorXd::Zero(6), 0.0) == (defaultDuration_.inSeconds()));
    }
}
