/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalConnective.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::LogicalConnective;

class TestCondition : public EventCondition
{
   public:
    TestCondition()
        : EventCondition("name", EventCondition::Criteria::PositiveCrossing)
    {
    }

    MOCK_METHOD(
        bool,
        isSatisfied,
        (const VectorXd& currentStateVector,
         const Real& currentTime,
         const VectorXd& previousStateVector,
         const Real& previousTime),
        (const, override)
    );
};

class LogicalConnectiveMock : public LogicalConnective
{
   public:
    LogicalConnectiveMock(const std::vector<std::shared_ptr<EventCondition>>& aEventConditions)
        : LogicalConnective("name", aEventConditions)
    {
    }

    MOCK_METHOD(
        bool,
        isSatisfied,
        (const VectorXd& currentStateVector,
         const Real& currentTime,
         const VectorXd& previousStateVector,
         const Real& previousTime),
        (const, override)
    );
};

class OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective : public ::testing::Test
{
   protected:
    const Array<Shared<EventCondition>> defaultEventConditions_ = {
        std::make_shared<TestCondition>(),
    };
    const LogicalConnectiveMock logicalConnectiveCondition_ = {defaultEventConditions_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective, Constructor)
{
    {
        EXPECT_NO_THROW(LogicalConnectiveMock logicalConnective(defaultEventConditions_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective, GetEventConditions)
{
    {
        EXPECT_EQ(logicalConnectiveCondition_.getEventConditions().getSize(), defaultEventConditions_.getSize());
    }
}
