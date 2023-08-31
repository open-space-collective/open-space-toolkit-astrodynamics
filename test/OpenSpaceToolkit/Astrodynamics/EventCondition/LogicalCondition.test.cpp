/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::LogicalCondition;
using ostk::astro::eventcondition::BooleanCondition;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesBroker;

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
    const LogicalCondition defaultLogicalCondition_ =
        LogicalCondition(defaultName_, defaultType_, defaultEventConditions_);
    const Instant defaultInstant_ = Instant::J2000();
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ = CoordinatesBroker({});
    const VectorXd defaultCoordinates_;
    const State defaultState_ = State(defaultInstant_, defaultCoordinates_, defaultFrame_, defaultCoordinatesBroker_);
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
