/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition, Constructor)
{
    {
        EXPECT_NO_THROW(CartesianPosition("NAME", 3));
    }

    {
        EXPECT_ANY_THROW(CartesianPosition("", 1));
    }

    {
        EXPECT_ANY_THROW(CartesianPosition("NAME", 0));
        EXPECT_ANY_THROW(CartesianPosition("NAME", 1));
        EXPECT_ANY_THROW(CartesianPosition("NAME", 2));
        EXPECT_ANY_THROW(CartesianPosition("NAME", 4));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition, EqualToOperator)
{
    {
        EXPECT_TRUE(CartesianPosition("NAME", 3) == CartesianPosition("NAME", 3));
    }

    {
        EXPECT_FALSE(CartesianPosition("NAME", 3) == CartesianPosition("OTHER", 3));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition, NotEqualToOperator)
{
    {
        EXPECT_FALSE(CartesianPosition("NAME", 3) != CartesianPosition("NAME", 3));
    }

    {
        EXPECT_TRUE(CartesianPosition("NAME", 3) != CartesianPosition("OTHER", 3));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition, Getters)
{
    const CartesianPosition cartesianPosition = CartesianPosition("NAME", 3);

    EXPECT_EQ("NAME", cartesianPosition.getName());
    EXPECT_EQ(3, cartesianPosition.getSize());
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition, Add)
{
    {
        const Shared<const CartesianPosition> cartesianPosition = CartesianPosition::ThreeDimensional();
        VectorXd allCoordinates_1(3);
        allCoordinates_1 << 1.0, 2.0, 3.0;
        VectorXd allCoordinates_2(3);
        allCoordinates_2 << 4.0, 5.0, 6.0;
        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        VectorXd actual = cartesianPosition->add(
            Instant::Undefined(),
            allCoordinates_1,
            allCoordinates_2,
            Frame::Undefined(),
            std::make_shared<CoordinatesBroker>(CoordinatesBroker({cartesianPosition}))
        );

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition, Subtract)
{
    {
        const Shared<const CartesianPosition> cartesianPosition = CartesianPosition::ThreeDimensional();
        VectorXd allCoordinates_1(3);
        allCoordinates_1 << 1.0, 2.0, 3.0;
        VectorXd allCoordinates_2(3);
        allCoordinates_2 << -4.0, -5.0, -6.0;
        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        VectorXd actual = cartesianPosition->subtract(
            Instant::Undefined(),
            allCoordinates_1,
            allCoordinates_2,
            Frame::Undefined(),
            std::make_shared<CoordinatesBroker>(CoordinatesBroker({cartesianPosition}))
        );

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> frame_1 = Frame::GCRF();
        const Shared<const Frame> frame_2 = Frame::TEME();
        const Shared<const CartesianPosition> cartesianPosition = CartesianPosition::ThreeDimensional();
        VectorXd allCoordinates(3);
        allCoordinates << 1.0e7, -1e7, 5e6;
        const Shared<const CoordinatesBroker> brokerkSPtr =
            std::make_shared<CoordinatesBroker>(CoordinatesBroker({cartesianPosition}));
        Vector3d expected = Position::Meters({1.0e7, -1e7, 5e6}, frame_1).inFrame(frame_2, instant).getCoordinates();

        VectorXd actual = cartesianPosition->inFrame(instant, allCoordinates, frame_1, frame_2, brokerkSPtr);

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_CartesianPosition, ThreeDimensional)
{
    {
        EXPECT_NO_THROW(CartesianPosition::ThreeDimensional());
    }

    {
        const Shared<const CartesianPosition> threeDimensional = CartesianPosition::ThreeDimensional();

        EXPECT_EQ(CartesianPosition::DEFAULT_NAME, threeDimensional->getName());
        EXPECT_EQ(3, threeDimensional->getSize());
    }
}
