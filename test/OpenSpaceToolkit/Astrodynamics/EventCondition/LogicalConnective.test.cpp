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

// class EventConditionMock : public EventCondition
// {
//    public:
//     EventConditionMock(const String& aName, const EventCondition::Criteria& aCriteria)
//         : EventCondition(aName, aCriteria) {};

//     MOCK_METHOD(Real, evaluate, (const VectorXd& aStateVector, const Real& aTime), (const, override));
// };

class TestCondition : public EventCondition
{
   public:
    TestCondition()
        : EventCondition("First", EventCondition::Criteria::PositiveCrossing)
    {
    }

    virtual Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override
    {
        (void)aStateVector;
        return aTime;
    }
};

class OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective : public ::testing::Test
{
    void SetUp() override
    {
        logicalConnectiveCondition_ = {
            "Logical Connective Condition",
            {
                std::make_shared<TestCondition>(testCondition_),
            },
        };
    }

   protected:
    LogicalConnective logicalConnectiveCondition_ = {"Logical Connective Condition", {}};
    TestCondition testCondition_;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective, Evaluate)
{
    {
        EXPECT_ANY_THROW(logicalConnectiveCondition_.evaluate(VectorXd::Zero(2), 0.0));
    }
}

// TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective, SetValue)
// {
//     {
//         logicalConnectiveCondition_.setValue(VectorXd::Zero(2), 1.0);
//         for (const auto& eventCondition : logicalConnectiveCondition_.getEventConditions())
//         {
//             EXPECT_EQ(1.0, eventCondition->getValue());
//         }
//     }
// }
