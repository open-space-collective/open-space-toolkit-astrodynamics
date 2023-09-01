/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::eventcondition::RealCondition;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

class OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition : public ::testing::Test
{
   protected:
    const RealCondition::Criterion defaultCriterion_ = RealCondition::Criterion::StrictlyPositive;
    const String defaultName_ = "Test Real Condition";
    const std::function<Real(const State&)> defaultEvaluator_ = [](const State& aState) -> Real
    {
        return aState.accessCoordinates()[0];
    };
    const Real defaultTarget_ = 1.0;
    const RealCondition defaultCondition_ = {defaultName_, defaultCriterion_, defaultEvaluator_, defaultTarget_};
    const Instant defaultInstant_ = Instant::J2000();
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker(Array<Shared<const CoordinatesSubset>>::Empty()));

    const State generateState(const Real& coordinate)
    {
        VectorXd coordinates;
        coordinates.resize(1);
        coordinates << coordinate;
        return State(defaultInstant_, coordinates, defaultFrame_, defaultCoordinatesBroker_);
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, Constructor)
{
    {
        EXPECT_NO_THROW(RealCondition(defaultName_, defaultCriterion_, defaultEvaluator_, defaultTarget_));
    }

    {
        EXPECT_NO_THROW(RealCondition(defaultName_, defaultCriterion_, defaultEvaluator_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultCondition_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, Getters)
{
    {
        EXPECT_TRUE(defaultCondition_.getName() == defaultName_);
        EXPECT_TRUE(defaultCondition_.getCriterion() == defaultCriterion_);
        EXPECT_NO_THROW(defaultCondition_.getEvaluator());  // Cannot compare equality for std::function
        EXPECT_TRUE(defaultCondition_.getTarget() == defaultTarget_);
    }

    {
        const RealCondition condition = RealCondition(defaultName_, defaultCriterion_, defaultEvaluator_);
        EXPECT_EQ(condition.getTarget(), 0.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, evaluate)
{
    {
        EXPECT_EQ(defaultCondition_.evaluate(generateState(-2.0)), -3.0);
        EXPECT_EQ(defaultCondition_.evaluate(generateState(0.0)), -1.0);
        EXPECT_EQ(defaultCondition_.evaluate(generateState(1.0)), 0.0);
        EXPECT_EQ(defaultCondition_.evaluate(generateState(2.0)), 1.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, isSatisfied)
{
    // Positive Crossing
    {
        RealCondition condition = {
            "name",
            RealCondition::Criterion::PositiveCrossing,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(generateState(2.0), generateState(0.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(0.0), generateState(2.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(0.0), generateState(-1.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(3.0), generateState(2.0)));
    }

    // Negative Crossing
    {
        RealCondition condition = {
            "name",
            RealCondition::Criterion::NegativeCrossing,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_FALSE(condition.isSatisfied(generateState(2.0), generateState(0.0)));
        EXPECT_TRUE(condition.isSatisfied(generateState(0.0), generateState(2.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(0.0), generateState(-1.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(3.0), generateState(2.0)));
    }

    // Any Crossing
    {
        RealCondition condition = {
            "name",
            RealCondition::Criterion::AnyCrossing,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(generateState(2.0), generateState(0.0)));
        EXPECT_TRUE(condition.isSatisfied(generateState(0.0), generateState(2.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(0.0), generateState(-1.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(3.0), generateState(2.0)));
    }

    // Strictly Positive (previous value doesn't matter)
    {
        RealCondition condition = {
            "name",
            RealCondition::Criterion::StrictlyPositive,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_TRUE(condition.isSatisfied(generateState(2.0), generateState(0.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(0.0), generateState(2.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(0.0), generateState(-1.0)));
        EXPECT_TRUE(condition.isSatisfied(generateState(3.0), generateState(2.0)));
    }

    // Strictly Negative (previous value doesn't matter)
    {
        RealCondition condition = {
            "name",
            RealCondition::Criterion::StrictlyNegative,
            defaultEvaluator_,
            defaultTarget_,
        };

        EXPECT_FALSE(condition.isSatisfied(generateState(2.0), generateState(0.0)));
        EXPECT_TRUE(condition.isSatisfied(generateState(0.0), generateState(2.0)));
        EXPECT_TRUE(condition.isSatisfied(generateState(0.0), generateState(-1.0)));
        EXPECT_FALSE(condition.isSatisfied(generateState(3.0), generateState(2.0)));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, StringFromCriterion)
{
    {
        EXPECT_TRUE(
            RealCondition::StringFromCriterion(RealCondition::Criterion::PositiveCrossing) == "Positive Crossing"
        );
        EXPECT_TRUE(
            RealCondition::StringFromCriterion(RealCondition::Criterion::NegativeCrossing) == "Negative Crossing"
        );
        EXPECT_TRUE(RealCondition::StringFromCriterion(RealCondition::Criterion::AnyCrossing) == "Any Crossing");
        EXPECT_TRUE(
            RealCondition::StringFromCriterion(RealCondition::Criterion::StrictlyPositive) == "Strictly Positive"
        );
        EXPECT_TRUE(
            RealCondition::StringFromCriterion(RealCondition::Criterion::StrictlyNegative) == "Strictly Negative"
        );
    }
}
