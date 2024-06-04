/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

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
    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(CoordinateBroker(Array<Shared<const CoordinateSubset>>::Empty()));

    const Array<Shared<const CoordinateSubset>> defaultSubsets_ = {
        std::make_shared<CoordinateSubset>(CoordinateSubset("REAL", 1))
    };
    const StateBuilder defaultStateBuilder_ = StateBuilder(defaultFrame_, defaultSubsets_);

    const State generateState(const Real& coordinate, const Instant& anInstant = Instant::J2000())
    {
        VectorXd coordinates(1);
        coordinates << coordinate;

        return defaultStateBuilder_.build(anInstant, coordinates);
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

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition, DurationCondition)
{
    {
        EXPECT_NO_THROW(
            RealCondition::DurationCondition(RealCondition::Criterion::PositiveCrossing, Duration::Hours(1.0))
        );
    }

    {
        RealCondition condition =
            RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(1.0));

        condition.updateTarget(generateState(0.0, Instant::J2000() + Duration::Minutes(1.0)));

        EXPECT_TRUE(condition.isSatisfied(
            generateState(0.0, Instant::J2000() + Duration::Minutes(2.1)), generateState(0.0, Instant::J2000())
        ));
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
