/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

class OpenSpaceToolkit_Astrodynamics_EventCondition_InstantCondition : public ::testing::Test
{
   protected:
    const InstantCondition::Criterion defaultCriterion_ = InstantCondition::Criterion::StrictlyPositive;
    const Instant defaultInstant_ = Instant::J2000();
    const VectorXd defaultCoordinates_;
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(Array<Shared<const CoordinateSubset>>::Empty());
    const InstantCondition defaultCondition_ = InstantCondition(defaultCriterion_, defaultInstant_);

    const State generateState(const Instant& anInstant)
    {
        return State(anInstant, defaultCoordinates_, defaultFrame_, defaultCoordinateBroker_);
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
