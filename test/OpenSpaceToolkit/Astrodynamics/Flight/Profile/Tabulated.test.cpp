/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::String;
using ostk::core::type::Shared;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Axes;
using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;

using ostk::astrodynamics::flight::profile::model::Tabulated;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated : public ::testing::Test
{
   protected:
    const Array<State> states_ = {
        State(
            Instant::DateTime(DateTime(2024, 1, 29, 0, 0, 0), Scale::UTC),
            Position::Meters({755972.142139276024, -3390511.949699319433, 5955672.751532567665}, Frame::GCRF()),
            Velocity::MetersPerSecond({-563.764594800880, -6619.592151780337, -3685.668514834143}, Frame::GCRF()),
            Quaternion(-0.638160707740, -0.163520830523, 0.726693549038, 0.194751982966, Quaternion::Format::XYZS),
            Vector3d(0.0, 0.0, 0.0),
            Frame::GCRF()
        ),
        State(
            Instant::DateTime(DateTime(2024, 1, 29, 0, 0, 30), Scale::UTC),
            Position::Meters({738650.069144404028, -3587215.226667015813, 5841866.765430007130}, Frame::GCRF()),
            Velocity::MetersPerSecond({-590.931279522597, -6492.770161294917, -3900.690931064452}, Frame::GCRF()),
            Quaternion(-0.635371915567, -0.174049521049, 0.723374654124, 0.206732684649, Quaternion::Format::XYZS),
            Vector3d(0.0, 0.0, 0.0),
            Frame::GCRF()
        )
    };
    const Tabulated tabulated_ = Tabulated(states_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, Constructor)
{
    {
        EXPECT_NO_THROW(Tabulated tabulated(states_););
    }

    {
        EXPECT_NO_THROW(Tabulated tabulated(states_, Interpolator::Type::Linear););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, EqualToOperator)
{
    {
        EXPECT_TRUE(tabulated_ == tabulated_);
    }

    {
        const Tabulated tabulated = {{states_[0], states_[1], states_[1]}};
        EXPECT_FALSE(tabulated_ == tabulated);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, NotEqualToOperator)
{
    {
        EXPECT_FALSE(tabulated_ != tabulated_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << tabulated_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(tabulated_.print(std::cout, true));
    EXPECT_NO_THROW(tabulated_.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, IsDefined)
{
    {
        EXPECT_TRUE(tabulated_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, Getters)
{
    {
        const Interval interval = tabulated_.getInterval();
        EXPECT_EQ(interval.getStart(), states_.accessFirst().accessInstant());
        EXPECT_EQ(interval.getEnd(), states_.accessLast().accessInstant());
    }

    {
        const Interpolator::Type interpolatorType = tabulated_.getInterpolatorType();
        EXPECT_EQ(interpolatorType, Interpolator::Type::Linear);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, CalculateStateAt)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 29, 0, 0, 15), Scale::UTC);

        const State state = tabulated_.calculateStateAt(instant);

        EXPECT_EQ(state.getInstant(), instant);

        const Vector3d positionCoordinates = {747311.105641840026, -3488863.588183167391, 5898769.758481287397};
        EXPECT_VECTORS_ALMOST_EQUAL(state.getPosition().accessCoordinates(), positionCoordinates, 1e-10);

        const Vector3d velocityCoordinates = {-577.347937161739, -6556.181156537627, -3793.179722949298};
        EXPECT_VECTORS_ALMOST_EQUAL(state.getVelocity().accessCoordinates(), velocityCoordinates, 1e-10);

        VectorXd attitudeVector(4);
        attitudeVector << -0.636788056981, -0.168790939736, 0.725058861220, 0.200749189082;
        EXPECT_VECTORS_ALMOST_EQUAL(state.getAttitude().toVector(Quaternion::Format::XYZS), attitudeVector, 1e-10);

        const Vector3d angularVelocity = {0.0, 0.0, 0.0};
        EXPECT_VECTORS_ALMOST_EQUAL(state.getAngularVelocity(), angularVelocity, 1e-10);

        EXPECT_TRUE(state.getPosition().accessFrame() == Frame::GCRF());
    }

    // state at the start of the interval
    {
        const State expectedState = states_.accessFirst();
        const Instant instant = expectedState.accessInstant();
        const State state = tabulated_.calculateStateAt(instant);

        EXPECT_EQ(state.getInstant(), instant);
        EXPECT_VECTORS_ALMOST_EQUAL(
            state.getPosition().getCoordinates(), expectedState.getPosition().getCoordinates(), 1e-15
        );
        EXPECT_VECTORS_ALMOST_EQUAL(
            state.getVelocity().getCoordinates(), expectedState.getVelocity().getCoordinates(), 1e-15
        );
        EXPECT_VECTORS_ALMOST_EQUAL(
            state.getAttitude().toVector(Quaternion::Format::XYZS),
            expectedState.getAttitude().toVector(Quaternion::Format::XYZS),
            1e-10
        );
        EXPECT_VECTORS_ALMOST_EQUAL(state.getAngularVelocity(), expectedState.getAngularVelocity(), 1e-15);
    }

    // state at the end of the interval
    {
        const State expectedState = states_.accessLast();
        const Instant instant = expectedState.accessInstant();
        const State state = tabulated_.calculateStateAt(instant);

        EXPECT_EQ(state.getInstant(), instant);
        EXPECT_VECTORS_ALMOST_EQUAL(
            state.getPosition().getCoordinates(), expectedState.getPosition().getCoordinates(), 1e-15
        );
        EXPECT_VECTORS_ALMOST_EQUAL(
            state.getVelocity().getCoordinates(), expectedState.getVelocity().getCoordinates(), 1e-15
        );
        EXPECT_VECTORS_ALMOST_EQUAL(
            state.getAttitude().toVector(Quaternion::Format::XYZS),
            expectedState.getAttitude().toVector(Quaternion::Format::XYZS),
            1e-10
        );
        EXPECT_VECTORS_ALMOST_EQUAL(state.getAngularVelocity(), expectedState.getAngularVelocity(), 1e-15);
    }

    // state outside the interval
    {
        const Instant instant = states_.accessLast().accessInstant() + Duration::Seconds(1.0);
        EXPECT_THROW(tabulated_.calculateStateAt(instant), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, GetAxesAt)
{
    // instant undefined
    {
        const Instant instant = Instant::Undefined();
        EXPECT_THROW(tabulated_.getAxesAt(instant), ostk::core::error::runtime::Undefined);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 29, 0, 0, 15), Scale::UTC);
        const Axes axes = tabulated_.getAxesAt(instant);
        const State state = tabulated_.calculateStateAt(instant);
        const Quaternion q_B_GCRF = state.getAttitude();
        EXPECT_VECTORS_ALMOST_EQUAL(q_B_GCRF * axes.x(), Vector3d::X(), 1e-15);
        EXPECT_VECTORS_ALMOST_EQUAL(q_B_GCRF * axes.y(), Vector3d::Y(), 1e-15);
        EXPECT_VECTORS_ALMOST_EQUAL(q_B_GCRF * axes.z(), Vector3d::Z(), 1e-15);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, GetBodyFrame)
{
    // name undefined
    {
        const String frameName = String::Empty();
        EXPECT_THROW(tabulated_.getBodyFrame(frameName), ostk::core::error::runtime::Undefined);
    }

    {
        const Shared<const Frame> bodyFrame = tabulated_.getBodyFrame("test");
        EXPECT_EQ(bodyFrame->getName(), "test");
        EXPECT_EQ(
            Frame::GCRF()->getTransformTo(bodyFrame, states_.accessFirst().accessInstant()).getTranslation(),
            -states_.accessFirst().getPosition().getCoordinates()
        );
        EXPECT_EQ(
            Frame::GCRF()->getTransformTo(bodyFrame, states_.accessFirst().accessInstant()).getVelocity(),
            -states_.accessFirst().getVelocity().getCoordinates()
        );
        EXPECT_EQ(
            Frame::GCRF()->getTransformTo(bodyFrame, states_.accessFirst().accessInstant()).getOrientation(),
            states_.accessFirst().getAttitude().normalize()
        );
        EXPECT_EQ(
            Frame::GCRF()->getTransformTo(bodyFrame, states_.accessFirst().accessInstant()).getAngularVelocity(),
            Vector3d::Zero()
        );
    }
}
