/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

using ostk::physics::time::Interval;
using ostk::physics::time::Instant;
using ostk::physics::time::DateTime;
using ostk::physics::time::Scale;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::flight::profile::model::Tabulated;

class OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        {
            const Instant instant = Instant::DateTime(DateTime(2024, 1, 29, 0, 0, 0), Scale::UTC);
            const Position position =
                Position::Meters({755972.142139276024, -3390511.949699319433, 5955672.751532567665}, Frame::GCRF());
            const Velocity velocity =
                Velocity::MetersPerSecond({-563.764594800880, -6619.592151780337, -3685.668514834143}, Frame::GCRF());
            const Quaternion attitude = {
                -0.638160707740, -0.163520830523, 0.726693549038, 0.194751982966, Quaternion::Format::XYZS
            };
            const Vector3d angularVelocity = {0.0, 0.0, 0.0};
            states_.add({instant, position, velocity, attitude, angularVelocity, Frame::GCRF()});
        }

        {
            const Instant instant = Instant::DateTime(DateTime(2024, 1, 29, 0, 0, 30), Scale::UTC);
            const Position position =
                Position::Meters({738650.069144404028, -3587215.226667015813, 5841866.765430007130}, Frame::GCRF());
            const Velocity velocity =
                Velocity::MetersPerSecond({-590.931279522597, -6492.770161294917, -3900.690931064452}, Frame::GCRF());
            const Quaternion attitude = {
                -0.635371915567, -0.174049521049, 0.723374654124, 0.206732684649, Quaternion::Format::XYZS
            };
            const Vector3d angularVelocity = {0.0, 0.0, 0.0};
            states_.add({instant, position, velocity, attitude, angularVelocity, Frame::GCRF()});
        }

        tabulated_ = {states_};
    }

    Array<State> states_ = Array<State>::Empty();
    Tabulated tabulated_ = {states_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, Constructor)
{
    {
        EXPECT_NO_THROW(Tabulated tabulated(states_););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, EqualToOperator)
{
    {
        EXPECT_TRUE(tabulated_ == tabulated_);
    }

    {
        const Array<State> statesSubset = {states_[0]};
        const Tabulated tabulatedSubset = {statesSubset};
        EXPECT_FALSE(tabulated_ == tabulatedSubset);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, NotEqualToOperator)
{
    {
        EXPECT_FALSE(tabulated_ != tabulated_);
    }

    {
        const Array<State> statesSubset = {states_[0]};
        const Tabulated tabulatedSubset = {statesSubset};
        EXPECT_TRUE(tabulated_ != tabulatedSubset);
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

    {
        const Tabulated tabulated = {{}};
        EXPECT_FALSE(tabulated.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, Getters)
{
    {
        const Interval interval = tabulated_.getInterval();
        EXPECT_EQ(interval.getStart(), states_.accessFirst().accessInstant());
        EXPECT_EQ(interval.getEnd(), states_.accessLast().accessInstant());
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
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, getAxesAt)
{
    // undefined
    {
        const Tabulated undefinedTabulated = {{}};
        EXPECT_THROW(
            undefinedTabulated.getAxesAt(states_.accessFirst().accessInstant()), ostk::core::error::runtime::Undefined
        );
    }

    // instant undefined
    {
        const Instant instant = Instant::Undefined();
        EXPECT_THROW(tabulated_.getAxesAt(instant), ostk::core::error::runtime::Undefined);
    }

    // not yet implemented
    {
        EXPECT_THROW(
            tabulated_.getAxesAt(states_.accessFirst().accessInstant()), ostk::core::error::runtime::ToBeImplemented
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated, getBodyFrame)
{
    // instant undefined
    {
        const String frameName = String::Empty();
        EXPECT_THROW(tabulated_.getBodyFrame(frameName), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_THROW(tabulated_.getBodyFrame("Body Frame Name"), ostk::core::error::runtime::ToBeImplemented);
    }
}
