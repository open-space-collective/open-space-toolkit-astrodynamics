/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Size;
using ostk::core::ctnr::Array;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Size defaultSize_ = 1;
    const CoordinatesSubset defaultCoordinateSubset_ = CoordinatesSubset(defaultName_, defaultSize_);
    const CartesianPosition defaultCartesianPosition_ = CartesianPosition("pos");

    const Array<Shared<const CoordinatesSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<CoordinatesSubset>(defaultCoordinateSubset_),
        std::make_shared<CartesianPosition>(defaultCartesianPosition_)
    };
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, Constructor)
{
    {
        EXPECT_NO_THROW(CoordinatesSubset("NAME", 1));
    }

    {
        EXPECT_ANY_THROW(CoordinatesSubset("", 0));
        EXPECT_ANY_THROW(CoordinatesSubset("NAME", 0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultCoordinateSubset_ == defaultCoordinateSubset_);
    }

    {
        EXPECT_FALSE(defaultCoordinateSubset_ == CoordinatesSubset("OTHER", 1));
        EXPECT_FALSE(defaultCoordinateSubset_ == CoordinatesSubset("NAME", 2));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultCoordinateSubset_ != defaultCoordinateSubset_);
    }

    {
        EXPECT_TRUE(defaultCoordinateSubset_ != CoordinatesSubset("OTHER", 1));
        EXPECT_TRUE(defaultCoordinateSubset_ != CoordinatesSubset("NAME", 2));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, Getters)
{
    {
        EXPECT_EQ(defaultName_, defaultCoordinateSubset_.getName());
        EXPECT_EQ(defaultSize_, defaultCoordinateSubset_.getSize());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, Add)
{
    VectorXd input1(4);
    VectorXd input2(4);

    input1 << 1.0, 1.0, 2.0, 3.0;
    input2 << 2.0, 7.0, 3.0, 4.0;

    {
        VectorXd expected(1);
        VectorXd actual(1);

        expected << 3.0;

        actual = defaultCoordinateSubset_.add(
            Instant::Undefined(), input1, input2, Frame::Undefined(), defaultCoordinatesBroker_
        );

        EXPECT_EQ(expected, actual);
    }

    {
        VectorXd expected(3);
        VectorXd actual(3);

        expected << 8.0, 5.0, 7.0;

        actual = defaultCartesianPosition_.add(
            Instant::Undefined(), input1, input2, Frame::Undefined(), defaultCoordinatesBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, Subtract)
{
    VectorXd input1(4);
    VectorXd input2(4);

    input1 << 1.0, 1.0, 2.0, 3.0;
    input2 << 2.0, 7.0, 3.0, 4.0;

    {
        VectorXd expected(1);
        VectorXd actual(1);

        expected << -1.0;

        actual = defaultCoordinateSubset_.subtract(
            Instant::Undefined(), input1, input2, Frame::Undefined(), defaultCoordinatesBroker_
        );

        EXPECT_EQ(expected, actual);
    }

    {
        VectorXd expected(3);
        VectorXd actual(3);

        expected << -6.0, -1.0, -1.0;

        actual = defaultCartesianPosition_.subtract(
            Instant::Undefined(), input1, input2, Frame::Undefined(), defaultCoordinatesBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}
