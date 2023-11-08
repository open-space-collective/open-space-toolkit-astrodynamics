/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Instant;
using ostk::physics::units::Length;
using ostk::physics::units::Angle;

using ostk::astro::EventCondition;
using ostk::astro::trajectory::State;

class TestCondition : public EventCondition
{
   public:
    TestCondition(
        const String& aName, const std::function<Real(const State&)>& anEvaluator, const EventCondition::Target& aTarget
    )
        : EventCondition(aName, anEvaluator, aTarget)
    {
    }

    MOCK_METHOD(bool, isSatisfied, (const State& currentState, const State& previousState), (const, override));
};

class OpenSpaceToolkit_Astrodynamics_EventCondition : public ::testing::Test
{
   protected:
    const String defaultName_ = "Test Event Condition";
    const EventCondition::Target defaultTarget_ = {0.0, EventCondition::Target::Type::Absolute};
    TestCondition defaultCondition_ = {
        defaultName_,
        []([[maybe_unused]] const State& aState) -> Real
        {
            return 0.0;
        },
        defaultTarget_
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, EventConditionTarget_Constructor)
{
    {
        EXPECT_NO_THROW(EventCondition::Target(0.0));
        EXPECT_NO_THROW(EventCondition::Target(0.0, EventCondition::Target::Type::Relative));
    }

    {
        EXPECT_NO_THROW(EventCondition::Target(Length::Meters(0.0)));
        EXPECT_NO_THROW(EventCondition::Target(Length::Meters(0.0), EventCondition::Target::Type::Relative));
    }

    {
        EXPECT_NO_THROW(EventCondition::Target(Angle::Degrees(0.0)));
        EXPECT_NO_THROW(EventCondition::Target(Angle::Degrees(0.0), EventCondition::Target::Type::Relative));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, EventConditionTarget_Equals)
{
    {
        const EventCondition::Target target = {0.0, EventCondition::Target::Type::Absolute};
        EXPECT_TRUE(target == target);
    }

    {
        const EventCondition::Target target = {Real::Undefined(), EventCondition::Target::Type::Absolute};
        EXPECT_FALSE(target == target);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, EventConditionTarget_NotEquals)
{
    {
        const EventCondition::Target target = {0.0, EventCondition::Target::Type::Absolute};
        EXPECT_FALSE(target != target);
    }

    {
        const EventCondition::Target target = {Real::Undefined(), EventCondition::Target::Type::Absolute};
        EXPECT_TRUE(target != target);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, EventConditionTarget_isDefined)
{
    {
        const EventCondition::Target target = {0.0, EventCondition::Target::Type::Absolute};
        EXPECT_TRUE(target.isDefined());
    }

    {
        const EventCondition::Target target = {Real::Undefined(), EventCondition::Target::Type::Absolute};
        EXPECT_FALSE(target.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, EventConditionTarget_StringFromType)
{
    {
        EXPECT_EQ(EventCondition::Target::StringFromType(EventCondition::Target::Type::Absolute), "Absolute");
        EXPECT_EQ(EventCondition::Target::StringFromType(EventCondition::Target::Type::Relative), "Relative");
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, Constructor)
{
    {
        EXPECT_NO_THROW(TestCondition testCondition(defaultName_, nullptr, defaultTarget_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultCondition_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, GetName)
{
    {
        EXPECT_TRUE(defaultCondition_.getName() == defaultName_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, GetEvaluator)
{
    {
        EXPECT_NO_THROW(defaultCondition_.getEvaluator());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, GetTarget)
{
    {
        EXPECT_EQ(defaultCondition_.getTarget(), defaultTarget_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, UpdateTarget)
{
    const State state = {
        Instant::J2000(),
        Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::GCRF()),
    };

    {
        defaultCondition_.updateTarget(state);
        EXPECT_FALSE(defaultCondition_.getTarget().valueOffset == 0.0);
    }

    {
        TestCondition eventCondition = {"name", nullptr, {0.0, EventCondition::Target::Type::Relative}};

        EXPECT_THROW(eventCondition.updateTarget(state), ostk::core::error::runtime::Undefined);
    }

    {
        TestCondition eventCondition = {
            "name",
            []([[maybe_unused]] const State& aState) -> Real
            {
                return 5.0;
            },
            {0.0, EventCondition::Target::Type::Relative}
        };
        EXPECT_NO_THROW(eventCondition.updateTarget(state));
    }
}
