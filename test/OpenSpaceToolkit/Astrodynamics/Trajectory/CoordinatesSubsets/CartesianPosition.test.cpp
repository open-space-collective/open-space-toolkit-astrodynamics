/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianPosition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::coordinatessubsets::CartesianPosition;

static const Instant instant = Instant::J2000();
static const Shared<const Frame> frame_1 = Frame::GCRF();
static const Shared<const Frame> frame_2 = Frame::TEME();

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition, Constructor)
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

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition, EqualToOperator)
{
    {
        EXPECT_TRUE(CartesianPosition("NAME", 3) == CartesianPosition("NAME", 3));
    }

    {
        EXPECT_FALSE(CartesianPosition("NAME", 3) == CartesianPosition("OTHER", 3));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition, NotEqualToOperator)
{
    {
        EXPECT_FALSE(CartesianPosition("NAME", 3) != CartesianPosition("NAME", 3));
    }

    {
        EXPECT_TRUE(CartesianPosition("NAME", 3) != CartesianPosition("OTHER", 3));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition, Getters)
{
    const CartesianPosition cartesianPosition = CartesianPosition("NAME", 3);

    EXPECT_EQ("NAME", cartesianPosition.getName());
    EXPECT_EQ(3, cartesianPosition.getSize());
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition, InFrame)
{
    {
        const Shared<CartesianPosition> cartesianPosition =
            std::make_shared<CartesianPosition>(CartesianPosition("NAME", 3));

        VectorXd allCoordinates(3);
        allCoordinates << 1.0e7, -1e7, 5e6;
        const Shared<const CoordinatesBroker> broker = CoordinatesBroker::FromSubsets({cartesianPosition});

        Vector3d expected = Position::Meters({1.0e7, -1e7, 5e6}, frame_1).inFrame(frame_2, instant).getCoordinates();
        VectorXd actual = cartesianPosition->inFrame(instant, allCoordinates, frame_1, broker, frame_2);

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianPosition, ThreeDimensional)
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
