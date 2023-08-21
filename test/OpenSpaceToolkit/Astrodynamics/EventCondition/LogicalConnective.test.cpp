/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalConnective.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::LogicalConnective;

class TestCondition : public EventCondition
{
   public:
    TestCondition(const String& aName)
        : EventCondition(aName, EventCondition::Criteria::PositiveCrossing, 0.0)
    {
    }

    virtual Real compute(const VectorXd& aStateVector, const Real& aTime) const override
    {
        (void)aStateVector;
        return aTime;
    }
};

class OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective : public ::testing::Test
{
   protected:
    const String defaultName_ = "test";
    const TestCondition testCondition_ = {defaultName_};
    const LogicalConnective logicalConnectiveCondition_ = {
        "Logical Connective Condition",
        {std::make_shared<TestCondition>(testCondition_)},
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective, Constructor)
{
    {
        EXPECT_NO_THROW(LogicalConnective logicalConnective(
            "Logical Connective Condition",
            {
                std::make_shared<TestCondition>(testCondition_),
            }
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective, GetEventConditions)
{
    {
        EXPECT_EQ(logicalConnectiveCondition_.getEventConditions().getSize(), 1);
        EXPECT_EQ(logicalConnectiveCondition_.getEventConditions()[0]->getName(), defaultName_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective, IsSatisfied)
{
    {
        EXPECT_ANY_THROW(logicalConnectiveCondition_.isSatisfied(0.0, 0.0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective, Compute)
{
    {
        EXPECT_ANY_THROW(logicalConnectiveCondition_.compute(VectorXd::Zero(2), 0.0));
    }
}
