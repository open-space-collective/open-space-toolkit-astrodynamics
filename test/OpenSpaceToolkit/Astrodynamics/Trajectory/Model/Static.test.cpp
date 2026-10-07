/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Static.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::trajectory::model::Static;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static : public ::testing::Test
{
   protected:
    const Position position_ = Position::Meters({7000000.0, 0.0, 0.0}, Frame::ITRF());
    const Instant instant_ = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static, Constructor)
{
    {
        EXPECT_NO_THROW(Static staticModel(position_));
    }

    {
        EXPECT_NO_THROW(Static staticModel(position_, Frame::GCRF()));
    }

    {
        EXPECT_THROW(
            Static staticModel(Position::Meters({7000000.0, 0.0, 0.0}, Frame::GCRF())),
            ostk::core::error::runtime::Wrong
        );
    }

    {
        EXPECT_THROW(
            Static staticModel(Position::Meters({7000000.0, 0.0, 0.0}, Frame::GCRF()), Frame::ITRF()),
            ostk::core::error::runtime::Wrong
        );
    }

    {
        EXPECT_THROW(Static staticModel(position_, nullptr), ostk::core::error::runtime::Undefined);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static, EqualToOperator)
{
    {
        EXPECT_TRUE(Static(position_) == Static(position_));
        EXPECT_TRUE(Static(position_) == Static(position_, Frame::ITRF()));
        EXPECT_TRUE(Static(position_, Frame::GCRF()) == Static(position_, Frame::GCRF()));
    }

    {
        EXPECT_FALSE(Static(position_) == Static(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF())));
        EXPECT_FALSE(Static(position_) == Static(position_, Frame::GCRF()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static, NotEqualToOperator)
{
    {
        EXPECT_FALSE(Static(position_) != Static(position_));
    }

    {
        EXPECT_TRUE(Static(position_) != Static(position_, Frame::GCRF()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static, IsDefined)
{
    {
        EXPECT_TRUE(Static(position_).isDefined());
        EXPECT_TRUE(Static(position_, Frame::GCRF()).isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static, GetFrame)
{
    {
        EXPECT_EQ(Static(position_).getFrame(), Frame::ITRF());
    }

    {
        EXPECT_EQ(Static(position_, Frame::GCRF()).getFrame(), Frame::GCRF());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static, CalculateStateAt)
{
    {
        EXPECT_THROW(Static(position_).calculateStateAt(Instant::Undefined()), ostk::core::error::runtime::Undefined);
    }

    {
        const State state = Static(position_).calculateStateAt(instant_);

        EXPECT_EQ(state.getInstant(), instant_);
        EXPECT_EQ(state.accessFrame(), Frame::ITRF());
        EXPECT_EQ(state.getPosition(), position_);
        EXPECT_EQ(state.getVelocity(), Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()));
    }

    {
        const State state = Static(position_, Frame::GCRF()).calculateStateAt(instant_);

        const State expectedState =
            State(instant_, position_, Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()))
                .inFrame(Frame::GCRF());

        EXPECT_EQ(state.getInstant(), instant_);
        EXPECT_EQ(state.accessFrame(), Frame::GCRF());
        EXPECT_EQ(state, expectedState);

        const State stateInITRF = state.inFrame(Frame::ITRF());

        EXPECT_TRUE(stateInITRF.getPosition().getCoordinates().isNear(position_.getCoordinates(), 1e-6));
        EXPECT_TRUE(stateInITRF.getVelocity().getCoordinates().isNear(Vector3d::Zero(), 1e-9));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(Static(position_, Frame::GCRF()).print(std::cout, true));
        EXPECT_NO_THROW(Static(position_).print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}
