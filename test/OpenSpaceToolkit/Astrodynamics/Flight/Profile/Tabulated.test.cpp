/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Models/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::physics::time::Interval;

using ostk::astro::trajectory::State;
using ostk::astro::flight::profile::models::Tabulated;


class OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        {
            const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
            const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
            const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
            const Quaternion attitude = Quaternion::Unit();
            const VectorXd angularVelocity = {0.0, 0.0, 0.0};
            states_.add({instant, position, velocity, attitude, angularVelocity});
        }

        {
            const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC);
            const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
            const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
            const Quaternion attitude = Quaternion::Unit();
            const VectorXd angularVelocity = {0.0, 0.0, 0.0};
            states_.add({instant, position, velocity, attitude, angularVelocity});
        }

        {
            const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC);
            const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
            const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
            const Quaternion attitude = Quaternion::Unit();
            const VectorXd angularVelocity = {0.0, 0.0, 0.0};
            states_.add({instant, position, velocity, attitude, angularVelocity});
        }
    }
    Array<State> states_ = Array<State>::Empty();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, Constructor)
{
    {
        EXPECT_NO_THROW(Tabulated tabulated(states_););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, EqualToOperator)
{
    const Tabulated tabulated = {states_};

    {
        EXPECT_TRUE(tabulated == tabulated);
    }

    {
        const Array<State> statesSubset = {states_[0]};
        const Tabulated tabulatedSubset = {statesSubset};
        EXPECT_FALSE(tabulated == tabulatedSubset);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, NotEqualToOperator)
{
    const Tabulated tabulated = {states_};

    {
        EXPECT_FALSE(tabulated != tabulated);
    }

    {
        const Array<State> statesSubset = {states_[0]};
        const Tabulated tabulatedSubset = {statesSubset};
        EXPECT_TRUE(tabulated != tabulatedSubset);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, StreamOperator)
{
    {    
        const Tabulated tabulated = {states_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << tabulated << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, IsDefined)
{
    {
        const Tabulated tabulated = {states_};
        EXPECT_TRUE(tabulated.isDefined());
    }

    {
        const Tabulated tabulated = {};
        EXPECT_FALSE(tabulated.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, Getters)
{
    {
        const Tabulated tabulated = {states_};
        const Interval interval = tabulated.getInterval();
        EXPECT_EQ(interval.getStart(), states_.accessFirst().accessInstant());
        EXPECT_EQ(interval.getEnd(), states_.accessLast().accessInstant());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, CalculateStateAt)
{
    // TBM
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, getAxesAt)
{
    {
        const Tabulated tabulated = {states_};
        EXPECT_THROW(tabulated.getAxesAt(states_.accessInstant()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, getBodyFrame)
{
    {
        const Tabulated tabulated = {states_};
        EXPECT_THROW(tabulated.getBodyFrame("Body Frame Name"));
    }
}
