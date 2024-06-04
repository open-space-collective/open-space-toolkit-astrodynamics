/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Shared<const CartesianPosition> defaultCartesianPositionSPtr_ = CartesianPosition::Default();
    const CartesianVelocity defaultCartesianVelocity_ = CartesianVelocity(defaultCartesianPositionSPtr_, defaultName_);

    const Array<Shared<const CoordinateSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<CartesianVelocity>(defaultCartesianVelocity_)
    };
    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity, Constructor)
{
    {
        EXPECT_NO_THROW(CartesianVelocity(defaultCartesianPositionSPtr_, defaultName_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity, Getters)
{
    EXPECT_EQ(3, defaultCartesianVelocity_.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity, Add)
{
    {
        VectorXd allCoordinates_1(3);
        allCoordinates_1 << 1.0, 2.0, 3.0;
        VectorXd allCoordinates_2(3);
        allCoordinates_2 << 4.0, 5.0, 6.0;

        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        const VectorXd actual = defaultCartesianVelocity_.add(
            Instant::Undefined(), allCoordinates_1, allCoordinates_2, Frame::Undefined(), defaultCoordinateBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity, Subtract)
{
    {
        VectorXd allCoordinates_1(3);
        allCoordinates_1 << 1.0, 2.0, 3.0;
        VectorXd allCoordinates_2(3);
        allCoordinates_2 << -4.0, -5.0, -6.0;

        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        const VectorXd actual = defaultCartesianVelocity_.subtract(
            Instant::Undefined(), allCoordinates_1, allCoordinates_2, Frame::Undefined(), defaultCoordinateBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(6);
        fullCoordinatesVector << 1.0e6, 2.0e6, 3.0e5, 4.0e3, -5.0e3, 6.0e3;
        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            defaultCartesianPositionSPtr_, std::make_shared<CartesianVelocity>(defaultCartesianVelocity_)
        };
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(coordinateSubsets);

        Vector3d expected = Velocity::MetersPerSecond({4.0e3, -5.0e3, 6.0e3}, fromFrame)
                                .inFrame(Position::Meters({1.0e6, 2.0e6, 3.0e5}, fromFrame), toFrame, instant)
                                .getCoordinates();

        const VectorXd actual =
            defaultCartesianVelocity_.inFrame(instant, fullCoordinatesVector, fromFrame, toFrame, brokerSPtr);

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity, Default)
{
    {
        EXPECT_NO_THROW(CartesianVelocity::Default());
    }
}
