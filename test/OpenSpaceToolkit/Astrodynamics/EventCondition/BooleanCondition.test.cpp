/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::object::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::eventcondition::BooleanCondition;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::StateBuilder;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

class OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition : public ::testing::Test
{
   protected:
    const BooleanCondition::Criterion defaultCriterion_ = BooleanCondition::Criterion::PositiveCrossing;
    const String defaultName_ = "Test Boolean Condition";
    const std::function<bool(const State&)> defaultEvaluator_ = [](const State& aState) -> Real
    {
        return aState.accessCoordinates()[0] > 0.0;
    };
    const bool defaultInverseFlag_ = true;
    const BooleanCondition defaultCondition_ = {
        "test",
        defaultCriterion_,
        defaultEvaluator_,
        defaultInverseFlag_,
    };
    const Instant defaultInstant_ = Instant::J2000();
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker(Array<Shared<const CoordinatesSubset>>::Empty()));

    const Array<Shared<const CoordinatesSubset>> defaultSubsets_ = {
        std::make_shared<CoordinatesSubset>(CoordinatesSubset("ANGLE", 1))
    };
    const StateBuilder defaultStateBuilder_ = StateBuilder(defaultFrame_, defaultSubsets_);

    const State generateState(const Real& coordinate)
    {
        VectorXd coordinates(1);
        coordinates << coordinate;

        return defaultStateBuilder_.build(defaultInstant_, coordinates);
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition, Constructor)
{
    {
        EXPECT_NO_THROW(BooleanCondition(defaultName_, defaultCriterion_, defaultEvaluator_));
    }
    {
        EXPECT_NO_THROW(BooleanCondition(defaultName_, defaultCriterion_, defaultEvaluator_, defaultInverseFlag_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition, Getters)
{
    {
        EXPECT_EQ(defaultCondition_.isInversed(), defaultInverseFlag_);
    }
    {
        BooleanCondition booleanCondition = BooleanCondition(defaultName_, defaultCriterion_, defaultEvaluator_);
        EXPECT_EQ(booleanCondition.isInversed(), false);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition, Evaluate)
{
    {
        EXPECT_EQ(defaultCondition_.evaluate(generateState(-1.0)), 1.0);
        EXPECT_EQ(defaultCondition_.evaluate(generateState(1.0)), -1.0);
    }

    {
        BooleanCondition booleanCondition =
            BooleanCondition(defaultName_, defaultCriterion_, defaultEvaluator_, !defaultInverseFlag_);

        EXPECT_EQ(defaultCondition_.evaluate(generateState(-1.0)), 1.0);
        EXPECT_EQ(defaultCondition_.evaluate(generateState(1.0)), -1.0);
    }
}
