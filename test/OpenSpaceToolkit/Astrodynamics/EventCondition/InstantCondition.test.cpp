/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::ctnr::Array;

using ostk::math::object::VectorXd;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::eventcondition::InstantCondition;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

class OpenSpaceToolkit_Astrodynamics_EventCondition_InstantCondition : public ::testing::Test
{
   protected:
    const InstantCondition::Criterion defaultCriterion_ = InstantCondition::Criterion::StrictlyPositive;
    const Instant defaultInstant_ = Instant::J2000();
    const VectorXd defaultCoordinates_;
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(Array<Shared<const CoordinatesSubset>>::Empty());
    const InstantCondition defaultCondition_ = InstantCondition(defaultCriterion_, defaultInstant_);

    const State generateState(const Instant& anInstant)
    {
        return State(anInstant, defaultCoordinates_, defaultFrame_, defaultCoordinatesBroker_);
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_InstantCondition, Constructor)
{
    {
        EXPECT_NO_THROW(InstantCondition instantCondition(defaultCriterion_, defaultInstant_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_InstantCondition, Getters)
{
    {
        EXPECT_TRUE(defaultCondition_.getInstant() == defaultInstant_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_InstantCondition, evaluate)
{
    {
        EXPECT_EQ(defaultCondition_.evaluate(generateState(defaultInstant_)), 0.0);
    }

    {
        EXPECT_EQ(defaultCondition_.evaluate(generateState(defaultInstant_ + Duration::Seconds(10))), 10.0);
    }

    {
        EXPECT_EQ(defaultCondition_.evaluate(generateState(defaultInstant_ - Duration::Hours(1))), -3600.0);
    }
}
