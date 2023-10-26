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

using ostk::astro::EventCondition;
using ostk::astro::trajectory::State;

class TestCondition : public EventCondition
{
   public:
    TestCondition(
        const String& aName, const std::function<Real(const State&)>& anEvaluator, const bool& targetIsRelative
    )
        : EventCondition(aName, anEvaluator, 0.0, targetIsRelative)
    {
    }

    MOCK_METHOD(bool, isSatisfied, (const State& currentState, const State& previousState), (const, override));
};

class OpenSpaceToolkit_Astrodynamics_EventCondition : public ::testing::Test
{
   protected:
    const String defaultName_ = "Test Event Condition";
    TestCondition defaultCondition_ = {
        defaultName_,
        []([[maybe_unused]] const State& aState) -> Real
        {
            return 0.0;
        },
        false,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, Constructor)
{
    {
        EXPECT_NO_THROW(TestCondition testCondition(defaultName_, nullptr, true));
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
        EXPECT_DOUBLE_EQ(defaultCondition_.getTarget(), 0.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition, TargetIsRelative)
{
    {
        EXPECT_FALSE(defaultCondition_.targetIsRelative());
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
        EXPECT_THROW(defaultCondition_.updateTarget(state), ostk::core::error::RuntimeError);
    }

    {
        TestCondition eventCondition = {"name", nullptr, true};

        EXPECT_THROW(eventCondition.updateTarget(state), ostk::core::error::runtime::Undefined);
    }

    {
        TestCondition eventCondition = {
            "name",
            []([[maybe_unused]] const State& aState) -> Real
            {
                return 5.0;
            },
            true
        };
        EXPECT_NO_THROW(eventCondition.updateTarget(state));
        EXPECT_DOUBLE_EQ(eventCondition.getTarget(), 5.0);
    }
}
