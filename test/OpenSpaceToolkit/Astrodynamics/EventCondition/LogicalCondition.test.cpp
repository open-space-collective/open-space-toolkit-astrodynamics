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
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>

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

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::BooleanCondition;
using ostk::astrodynamics::eventcondition::LogicalCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

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

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Evaluate_And)
{
    // AND returns min of sub-condition evaluate() values
    // alwaysTrueBooleanCondition_ evaluates to 1.0 (true -> 1.0, target 0.0, so 1.0 - 0.0 = 1.0)
    // alwaysFalseBooleanCondition_ evaluates to -1.0 (false -> -1.0, target 0.0, so -1.0 - 0.0 = -1.0)

    {
        // AND(true, false) -> min(1.0, -1.0) = -1.0
        const LogicalCondition logicalCondition = LogicalCondition(
            defaultName_, LogicalCondition::Type::And, {alwaysTrueBooleanCondition_, alwaysFalseBooleanCondition_}
        );

        EXPECT_DOUBLE_EQ(logicalCondition.evaluate(defaultState_), -1.0);
    }

    {
        // AND(true, true) -> min(1.0, 1.0) = 1.0
        const LogicalCondition logicalCondition = LogicalCondition(
            defaultName_, LogicalCondition::Type::And, {alwaysTrueBooleanCondition_, alwaysTrueBooleanCondition_}
        );

        EXPECT_DOUBLE_EQ(logicalCondition.evaluate(defaultState_), 1.0);
    }

    {
        // AND(false, false) -> min(-1.0, -1.0) = -1.0
        const LogicalCondition logicalCondition = LogicalCondition(
            defaultName_, LogicalCondition::Type::And, {alwaysFalseBooleanCondition_, alwaysFalseBooleanCondition_}
        );

        EXPECT_DOUBLE_EQ(logicalCondition.evaluate(defaultState_), -1.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Evaluate_Or)
{
    // OR returns max of sub-condition evaluate() values

    {
        // OR(true, false) -> max(1.0, -1.0) = 1.0
        const LogicalCondition logicalCondition = LogicalCondition(
            defaultName_, LogicalCondition::Type::Or, {alwaysTrueBooleanCondition_, alwaysFalseBooleanCondition_}
        );

        EXPECT_DOUBLE_EQ(logicalCondition.evaluate(defaultState_), 1.0);
    }

    {
        // OR(true, true) -> max(1.0, 1.0) = 1.0
        const LogicalCondition logicalCondition = LogicalCondition(
            defaultName_, LogicalCondition::Type::Or, {alwaysTrueBooleanCondition_, alwaysTrueBooleanCondition_}
        );

        EXPECT_DOUBLE_EQ(logicalCondition.evaluate(defaultState_), 1.0);
    }

    {
        // OR(false, false) -> max(-1.0, -1.0) = -1.0
        const LogicalCondition logicalCondition = LogicalCondition(
            defaultName_, LogicalCondition::Type::Or, {alwaysFalseBooleanCondition_, alwaysFalseBooleanCondition_}
        );

        EXPECT_DOUBLE_EQ(logicalCondition.evaluate(defaultState_), -1.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Evaluate_WithRealConditions)
{
    // Test with RealConditions that have specific numeric values
    auto makeRealCondition = [](const Real& returnValue, const Real& target) -> Shared<RealCondition>
    {
        return std::make_shared<RealCondition>(RealCondition(
            "test",
            RealCondition::Criterion::AnyCrossing,
            [returnValue]([[maybe_unused]] const State& aState) -> Real
            {
                return returnValue;
            },
            target
        ));
    };

    {
        // AND(5.0 - 3.0, 2.0 - 4.0) = AND(2.0, -2.0) = min(2.0, -2.0) = -2.0
        const LogicalCondition logicalCondition = LogicalCondition(
            defaultName_, LogicalCondition::Type::And, {makeRealCondition(5.0, 3.0), makeRealCondition(2.0, 4.0)}
        );

        EXPECT_DOUBLE_EQ(logicalCondition.evaluate(defaultState_), -2.0);
    }

    {
        // OR(5.0 - 3.0, 2.0 - 4.0) = OR(2.0, -2.0) = max(2.0, -2.0) = 2.0
        const LogicalCondition logicalCondition = LogicalCondition(
            defaultName_, LogicalCondition::Type::Or, {makeRealCondition(5.0, 3.0), makeRealCondition(2.0, 4.0)}
        );

        EXPECT_DOUBLE_EQ(logicalCondition.evaluate(defaultState_), 2.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Clone)
{
    EXPECT_NO_THROW({ Unique<LogicalCondition> clonedCondition(defaultLogicalCondition_.clone()); });
}
