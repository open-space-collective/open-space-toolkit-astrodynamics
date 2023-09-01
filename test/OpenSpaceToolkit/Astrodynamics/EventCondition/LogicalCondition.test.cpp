/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::LogicalCondition;
using ostk::astro::eventcondition::BooleanCondition;

class OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition : public ::testing::Test
{
   protected:
    const String defaultName_ = "Test Logical Condition";
    const LogicalCondition::Type defaultType_ = LogicalCondition::Type::And;
    Shared<BooleanCondition> alwaysTrueBooleanCondition_ = std::make_shared<BooleanCondition>(BooleanCondition(
        "Always True",
        BooleanCondition::Criterion::StrictlyPositive,
        []([[maybe_unused]] const VectorXd& aStateVector, [[maybe_unused]] const Real& aTime) -> Real
        {
            return true;
        }
    ));
    Shared<BooleanCondition> alwaysFalseBooleanCondition_ = std::make_shared<BooleanCondition>(BooleanCondition(
        "Always False",
        BooleanCondition::Criterion::StrictlyPositive,
        []([[maybe_unused]] const VectorXd& aStateVector, [[maybe_unused]] const Real& aTime) -> Real
        {
            return false;
        }
    ));
    const Array<Shared<EventCondition>> defaultEventConditions_ = {
        alwaysTrueBooleanCondition_, alwaysFalseBooleanCondition_
    };
    const LogicalCondition defaultLogicalCondition_ =
        LogicalCondition(defaultName_, defaultType_, defaultEventConditions_);
    const VectorXd defaultStateVector_;
    const Real defaultTime_ = 0.0;
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

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {alwaysFalseBooleanCondition_};

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_)
        );
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysTrueBooleanCondition_, alwaysFalseBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_)
        );
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysTrueBooleanCondition_, alwaysTrueBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysFalseBooleanCondition_, alwaysFalseBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::And, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition, Or)
{
    {
        const Array<Shared<EventCondition>> eventConditions = {alwaysTrueBooleanCondition_};

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {alwaysFalseBooleanCondition_};

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_)
        );
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysTrueBooleanCondition_, alwaysFalseBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysTrueBooleanCondition_, alwaysTrueBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_TRUE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_));
    }

    {
        const Array<Shared<EventCondition>> eventConditions = {
            alwaysFalseBooleanCondition_, alwaysFalseBooleanCondition_
        };

        const LogicalCondition logicalCondition =
            LogicalCondition(defaultName_, LogicalCondition::Type::Or, eventConditions);

        EXPECT_FALSE(logicalCondition.isSatisfied(defaultStateVector_, defaultTime_, defaultStateVector_, defaultTime_)
        );
    }
}
