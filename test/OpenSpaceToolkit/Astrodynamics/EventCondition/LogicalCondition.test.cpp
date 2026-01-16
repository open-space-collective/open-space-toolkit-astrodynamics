/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Type/Unique.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::type::Unique;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::physics::unit::Angle;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::AngularCondition;
using ostk::astrodynamics::eventcondition::BooleanCondition;
using ostk::astrodynamics::eventcondition::LogicalCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition : public ::testing::Test
{
   protected:
    const String defaultName_ = "Test Logical Condition";
    const LogicalCondition::Type defaultType_ = LogicalCondition::Type::And;
    Shared<BooleanCondition> alwaysTrueBooleanCondition_ = std::make_shared<BooleanCondition>(BooleanCondition(
        "Always True",
        BooleanCondition::Criterion::StrictlyPositive,
        []([[maybe_unused]] const State& aState) -> bool
        {
            return true;
        }
    ));
    Shared<BooleanCondition> alwaysFalseBooleanCondition_ = std::make_shared<BooleanCondition>(BooleanCondition(
        "Always False",
        BooleanCondition::Criterion::StrictlyPositive,
        []([[maybe_unused]] const State& aState) -> bool
        {
            return false;
        }
    ));
    const Array<Shared<EventCondition>> defaultEventConditions_ = {
        alwaysTrueBooleanCondition_, alwaysFalseBooleanCondition_
    };
    LogicalCondition defaultLogicalCondition_ = LogicalCondition(defaultName_, defaultType_, defaultEventConditions_);
    const Instant defaultInstant_ = Instant::J2000();
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(CoordinateBroker(Array<Shared<const CoordinateSubset>>::Empty()));
    const VectorXd defaultCoordinates_;
    const State defaultState_ = State(defaultInstant_, defaultCoordinates_, defaultFrame_, defaultCoordinateBroker_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Constructor)
{
    {
        EXPECT_NO_THROW(LogicalCondition(defaultName_, defaultType_, defaultEventConditions_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Getters)
{
    {
        EXPECT_EQ(defaultLogicalCondition_.getEventConditions().getSize(), defaultEventConditions_.getSize());
    }

    {
        EXPECT_EQ(defaultLogicalCondition_.getType(), defaultType_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, UpdateTarget)
{
    {
        LogicalCondition logicalCondition = LogicalCondition(
            defaultName_,
            defaultType_,
            {
                std::make_shared<RealCondition>(
                    RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Seconds(30.0))
                ),
            }
        );
        EXPECT_NO_THROW(logicalCondition.updateTarget(defaultState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultLogicalCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultLogicalCondition_.print(std::cout, false));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, And)
{
    {
        const Array<Shared<EventCondition>> eventConditions = {alwaysTrueBooleanCondition_};

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {alwaysFalseBooleanCondition_};

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysTrueBooleanCondition_, alwaysFalseBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysTrueBooleanCondition_, alwaysTrueBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysFalseBooleanCondition_, alwaysFalseBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Or)
{
    {
        const Array<Shared<EventCondition>> eventConditions = {alwaysTrueBooleanCondition_};

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {alwaysFalseBooleanCondition_};

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysTrueBooleanCondition_, alwaysFalseBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysTrueBooleanCondition_, alwaysTrueBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysFalseBooleanCondition_, alwaysFalseBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultState_, defaultState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Clone)
{
    EXPECT_NO_THROW({ Unique<LogicalCondition> clonedCondition(defaultLogicalCondition_.clone()); });
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Evaluate)
{
    {
        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, defaultEventConditions_);

        EXPECT_EQ(logicalCondition.evaluate(defaultState_), -1.0);
    }

    {
        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, defaultEventConditions_);

        EXPECT_EQ(logicalCondition.evaluate(defaultState_), 1.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Evaluate_MixedCriteria)
{
    // Test that LogicalCondition::evaluate() properly normalizes sub-conditions
    // with different satisfaction conventions (positive vs negative when satisfied)

    // Create a RealCondition with StrictlyPositive criterion
    // For StrictlyPositive: positive evaluate = satisfied
    const Shared<RealCondition> strictlyPositiveCondition = std::make_shared<RealCondition>(
        RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(5.0))
    );

    // Create a RealCondition with NegativeCrossing criterion
    // For NegativeCrossing: negative evaluate = satisfied (value is below target)
    const Shared<RealCondition> negativeCrossingCondition = std::make_shared<RealCondition>(
        RealCondition::DurationCondition(RealCondition::Criterion::NegativeCrossing, Duration::Seconds(10.0))
    );

    // Verify the evaluateNegativeWhenSatisfied flag
    EXPECT_FALSE(strictlyPositiveCondition->evaluateNegativeWhenSatisfied());
    EXPECT_TRUE(negativeCrossingCondition->evaluateNegativeWhenSatisfied());

    // Test OR LogicalCondition with mixed criteria
    // At time = 7s: strictlyPositive evaluates to 7-5=2 (positive, satisfied)
    //               negativeCrossing evaluates to 7-10=-3 (negative, satisfied after normalization)
    // With normalization: max(2, -(-3)) = max(2, 3) = 3
    {
        const State stateAt7s = State(
            Instant::J2000() + Duration::Seconds(7.0), defaultCoordinates_, defaultFrame_, defaultCoordinateBroker_
        );

        const LogicalCondition orCondition = LogicalCondition(
            "Or Condition", LogicalCondition::Type::Or, {strictlyPositiveCondition, negativeCrossingCondition}
        );

        const Real evaluateResult = orCondition.evaluate(stateAt7s);
        // Should be positive because OR is satisfied (at least one condition is satisfied)
        EXPECT_GT(evaluateResult, 0.0);
    }

    // Test AND LogicalCondition with mixed criteria
    // At time = 3s: strictlyPositive evaluates to 3-5=-2 (negative, not satisfied)
    //               negativeCrossing evaluates to 3-10=-7 (negative, satisfied after normalization gives +7)
    // With normalization: min(-2, 7) = -2
    {
        const State stateAt3s = State(
            Instant::J2000() + Duration::Seconds(3.0), defaultCoordinates_, defaultFrame_, defaultCoordinateBroker_
        );

        const LogicalCondition andCondition = LogicalCondition(
            "And Condition", LogicalCondition::Type::And, {strictlyPositiveCondition, negativeCrossingCondition}
        );

        const Real evaluateResult = andCondition.evaluate(stateAt3s);
        // Should be negative because AND is not satisfied (first condition is not satisfied)
        EXPECT_LT(evaluateResult, 0.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Evaluate_WithAngularCondition)
{
    const Array<Shared<const CoordinateSubset>> angularSubsets = {
        std::make_shared<CoordinateSubset>(CoordinateSubset("ANGLE", 1))
    };
    const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), angularSubsets);
    const auto buildState = [&stateBuilder](const Duration& aDuration, const Angle& angle) -> State
    {
        VectorXd coordinates(1);
        coordinates << angle.inRadians();
        return stateBuilder.build(Instant::J2000() + aDuration, coordinates);
    };

    const Shared<AngularCondition> angularNegativeCrossing = std::make_shared<AngularCondition>(
        "Angular Negative Crossing",
        AngularCondition::Criterion::NegativeCrossing,
        [](const State& aState) -> Real
        {
            return aState.accessCoordinates()[0];
        },
        Angle::Radians(3.0)
    );

    const Shared<RealCondition> realStrictlyPositive = std::make_shared<RealCondition>(
        RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(1.0))
    );

    EXPECT_TRUE(angularNegativeCrossing->evaluateNegativeWhenSatisfied());
    EXPECT_FALSE(realStrictlyPositive->evaluateNegativeWhenSatisfied());

    const LogicalCondition orCondition = LogicalCondition(
        "Or with Angular", LogicalCondition::Type::Or, {angularNegativeCrossing, realStrictlyPositive}
    );

    // Test case 1: Both conditions effectively satisfied
    // At angle = 1.0 rad (below target 3.0): angular evaluate() = 1.0 - 3.0 = -2.0
    //   For NegativeCrossing, below target means "crossed" → normalized: -(-2.0) = 2.0
    // At time = 2.0s: duration evaluate() = 2.0 - 1.0 = 1.0 (satisfied)
    // OR: max(2.0, 1.0) = 2.0
    {
        const State state = buildState(Duration::Seconds(2.0), Angle::Radians(1.0));
        const Real evaluateResult = orCondition.evaluate(state);
        EXPECT_DOUBLE_EQ(evaluateResult, 2.0);
    }

    // Test case 2: Angular condition satisfied (below target), duration not satisfied
    // At angle = 1.5 rad (below target 3.0): angular evaluate() = 1.5 - 3.0 = -1.5
    //   For NegativeCrossing, below target means "crossed" → normalized: -(-1.5) = 1.5
    // At time = 0.5s: duration evaluate() = 0.5 - 1.0 = -0.5 (not satisfied)
    // OR: max(1.5, -0.5) = 1.5
    {
        const State state = buildState(Duration::Seconds(0.5), Angle::Radians(1.5));
        const Real evaluateResult = orCondition.evaluate(state);
        EXPECT_DOUBLE_EQ(evaluateResult, 1.5);
    }

    // Test case 3: Neither condition satisfied
    // At angle = 3.5 rad (above target 3.0): angular evaluate() = 3.5 - 3.0 = 0.5
    //   For NegativeCrossing, above target means "not yet crossed" → normalized: -(0.5) = -0.5
    // At time = 0.5s: duration evaluate() = 0.5 - 1.0 = -0.5 (not satisfied)
    // OR: max(-0.5, -0.5) = -0.5
    {
        const State state = buildState(Duration::Seconds(0.5), Angle::Radians(3.5));
        const Real evaluateResult = orCondition.evaluate(state);
        EXPECT_DOUBLE_EQ(evaluateResult, -0.5);
    }
}
