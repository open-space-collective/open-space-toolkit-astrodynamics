/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::eventcondition::DurationCondition;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

class OpenSpaceToolkit_Astrodynamics_EventCondition_DurationCondition : public ::testing::Test
{
   protected:
    const DurationCondition::Criterion defaultCriterion_ = DurationCondition::Criterion::StrictlyPositive;
    const Duration defaultDuration_ = Duration::Seconds(60.0);
    const Instant defaultReferenceInstant_ = Instant::J2000();
    const VectorXd defaultCoordinates_;
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(Array<Shared<const CoordinateSubset>>::Empty());
    const DurationCondition defaultCondition_ =
        DurationCondition(defaultCriterion_, defaultDuration_, defaultReferenceInstant_);

    const State generateState(const Instant& anInstant)
    {
        return State(anInstant, defaultCoordinates_, defaultFrame_, defaultCoordinateBroker_);
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_DurationCondition, Constructor)
{
    {
        EXPECT_NO_THROW(DurationCondition durationCondition(defaultCriterion_, defaultDuration_));
    }

    {
        EXPECT_NO_THROW(
            DurationCondition durationCondition(defaultCriterion_, defaultDuration_, defaultReferenceInstant_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_DurationCondition, GetDuration)
{
    {
        EXPECT_TRUE(defaultCondition_.getDuration() == defaultDuration_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_DurationCondition, GetReferenceInstant)
{
    {
        EXPECT_TRUE(defaultCondition_.getReferenceInstant() == defaultReferenceInstant_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_DurationCondition, Evaluate)
{
    // Test at reference instant: evaluation should be negative (duration not yet elapsed)
    {
        const Real evaluation = defaultCondition_.evaluate(generateState(defaultReferenceInstant_));
        EXPECT_DOUBLE_EQ(evaluation, -defaultDuration_.inSeconds());  // 0 - 60 = -60
    }

    // Test at target duration: evaluation should be zero
    {
        const Real evaluation = defaultCondition_.evaluate(generateState(defaultReferenceInstant_ + defaultDuration_));
        EXPECT_DOUBLE_EQ(evaluation, 0.0);  // 60 - 60 = 0
    }

    // Test after target duration: evaluation should be positive
    {
        const Real evaluation =
            defaultCondition_.evaluate(generateState(defaultReferenceInstant_ + Duration::Seconds(120.0)));
        EXPECT_DOUBLE_EQ(evaluation, 60.0);  // 120 - 60 = 60
    }

    // Test before reference instant: evaluation should be negative
    {
        const Real evaluation =
            defaultCondition_.evaluate(generateState(defaultReferenceInstant_ - Duration::Hours(1)));
        EXPECT_DOUBLE_EQ(evaluation, -3660.0);  // -3600 - 60 = -3660
    }

    {
        const DurationCondition durationCondition =
            DurationCondition(defaultCriterion_, Duration::Days(365.0 * 25.0), defaultReferenceInstant_);

        const Real evaluation = durationCondition.evaluate(
            generateState(defaultReferenceInstant_ + Duration::Days(365.0 * 25.0) - Duration::Seconds(1.0))
        );
        EXPECT_DOUBLE_EQ(evaluation, -1.0);  // -defaultDuration_ - 1.0 = -defaultDuration_ - 1.0
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_DurationCondition, UpdateTarget)
{
    DurationCondition durationCondition(defaultCriterion_, defaultDuration_, defaultReferenceInstant_);

    const Instant newReferenceInstant = defaultReferenceInstant_ + Duration::Hours(1.0);
    const State stateAtNewReference = generateState(newReferenceInstant);

    durationCondition.updateTarget(stateAtNewReference);

    EXPECT_TRUE(durationCondition.getReferenceInstant() == newReferenceInstant);

    // After update, evaluation at the new reference instant should be -60 (target duration not elapsed)
    EXPECT_DOUBLE_EQ(durationCondition.evaluate(stateAtNewReference), -60.0);

    // 60 seconds after the new reference should yield 0
    EXPECT_DOUBLE_EQ(durationCondition.evaluate(generateState(newReferenceInstant + Duration::Seconds(60.0))), 0.0);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_DurationCondition, IsSatisfied)
{
    // Test PositiveCrossing criterion
    {
        const DurationCondition durationCondition(
            DurationCondition::Criterion::PositiveCrossing, defaultDuration_, defaultReferenceInstant_
        );

        const State beforeState = generateState(defaultReferenceInstant_ + Duration::Seconds(59.0));
        const State afterState = generateState(defaultReferenceInstant_ + Duration::Seconds(61.0));

        EXPECT_TRUE(durationCondition.isSatisfied(afterState, beforeState));
        EXPECT_FALSE(durationCondition.isSatisfied(beforeState, afterState));
    }

    // Test StrictlyPositive criterion
    {
        const State beforeState = generateState(defaultReferenceInstant_ + Duration::Seconds(59.0));
        const State afterState = generateState(defaultReferenceInstant_ + Duration::Seconds(61.0));

        EXPECT_TRUE(defaultCondition_.isSatisfied(afterState, beforeState));
    }
}
