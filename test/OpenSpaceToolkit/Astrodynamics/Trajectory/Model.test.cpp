/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>

#include <Global.test.hpp>

using ostk::core::type::String;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::trajectory::Model;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Model : public ::testing::Test
{
   protected:
    const Interval interval_ = Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0));
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model, BeforeStartError)
{
    const Instant instant = interval_.accessStart() - Duration::Seconds(1.0);

    const Model::BeforeStartError error(instant, interval_);

    EXPECT_EQ(error.getInstant(), instant);
    EXPECT_EQ(error.getInterval(), interval_);
    EXPECT_EQ(
        error.getMessage(),
        String::Format(
            "Provided instant [{}] is before the start of the interval [{}, {}].",
            instant.toString(),
            interval_.accessStart().toString(),
            interval_.accessEnd().toString()
        )
    );

    // Catchable as an out-of-bounds error and as a generic OSTk runtime error, but distinct from an after-end error
    EXPECT_THROW(throw error, Model::BeforeStartError);
    EXPECT_THROW(throw error, Model::OutOfBoundsError);
    EXPECT_THROW(throw error, ostk::core::error::RuntimeError);

    EXPECT_EQ(dynamic_cast<const Model::AfterEndError*>(&error), nullptr);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model, AfterEndError)
{
    const Instant instant = interval_.accessEnd() + Duration::Seconds(1.0);

    const Model::AfterEndError error(instant, interval_);

    EXPECT_EQ(error.getInstant(), instant);
    EXPECT_EQ(error.getInterval(), interval_);
    EXPECT_EQ(
        error.getMessage(),
        String::Format(
            "Provided instant [{}] is after the end of the interval [{}, {}].",
            instant.toString(),
            interval_.accessStart().toString(),
            interval_.accessEnd().toString()
        )
    );

    // Catchable as an out-of-bounds error and as a generic OSTk runtime error, but distinct from a before-start error
    EXPECT_THROW(throw error, Model::AfterEndError);
    EXPECT_THROW(throw error, Model::OutOfBoundsError);
    EXPECT_THROW(throw error, ostk::core::error::RuntimeError);

    EXPECT_EQ(dynamic_cast<const Model::BeforeStartError*>(&error), nullptr);
}
