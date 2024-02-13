/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinatesBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatessubset::CartesianPosition;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const CartesianPosition defaultCartesianPosition_ = CartesianPosition(defaultName_);
    const Array<Shared<const CoordinateSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<CartesianPosition>(defaultCartesianPosition_)
    };
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition, Constructor)
{
    {
        EXPECT_NO_THROW(CartesianPosition("NAME"));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition, Getters)
{
    EXPECT_EQ(3, defaultCartesianPosition_.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition, Add)
{
    {
        VectorXd firstCoordinates(3);
        firstCoordinates << 1.0, 2.0, 3.0;
        VectorXd secondCoordinates(3);
        secondCoordinates << 4.0, 5.0, 6.0;

        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        const VectorXd actual = defaultCartesianPosition_.add(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinatesBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition, Subtract)
{
    {
        VectorXd firstCoordinates(3);
        firstCoordinates << 1.0, 2.0, 3.0;
        VectorXd secondCoordinates(3);
        secondCoordinates << -4.0, -5.0, -6.0;

        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        const VectorXd actual = defaultCartesianPosition_.subtract(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinatesBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(3);
        fullCoordinatesVector << 1.0e7, -1e7, 5e6;
        const Shared<const CoordinatesBroker> brokerkSPtr = defaultCoordinatesBroker_;

        Vector3d expected = Position::Meters({1.0e7, -1e7, 5e6}, fromFrame).inFrame(toFrame, instant).getCoordinates();

        const VectorXd actual =
            defaultCartesianPosition_.inFrame(instant, fullCoordinatesVector, fromFrame, toFrame, brokerkSPtr);

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition, Default)
{
    {
        EXPECT_NO_THROW(CartesianPosition::Default());
    }
}
