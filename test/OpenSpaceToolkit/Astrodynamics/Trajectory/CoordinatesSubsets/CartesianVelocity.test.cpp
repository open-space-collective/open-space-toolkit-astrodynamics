/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::coordinatessubsets::CartesianVelocity;

static const Shared<const CartesianPosition> threeDimensionalCartesianPosition =
    std::make_shared<CartesianPosition>(CartesianPosition("CARTESIAN_POSITION", 3));

static const Instant instant = Instant::J2000();
static const Shared<const Frame> frame_1 = Frame::GCRF();
static const Shared<const Frame> frame_2 = Frame::TEME();

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, Constructor)
{
    {
        EXPECT_NO_THROW(CartesianVelocity("NAME", threeDimensionalCartesianPosition));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, EqualToOperator)
{
    {
        EXPECT_TRUE(
            CartesianVelocity("NAME", threeDimensionalCartesianPosition) ==
            CartesianVelocity("NAME", threeDimensionalCartesianPosition)
        );
    }

    {
        EXPECT_FALSE(
            CartesianVelocity("NAME", threeDimensionalCartesianPosition) ==
            CartesianVelocity("OTHER", threeDimensionalCartesianPosition)
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, NotEqualToOperator)
{
    {
        EXPECT_FALSE(
            CartesianVelocity("NAME", threeDimensionalCartesianPosition) !=
            CartesianVelocity("NAME", threeDimensionalCartesianPosition)
        );
    }

    {
        EXPECT_TRUE(
            CartesianVelocity("NAME", threeDimensionalCartesianPosition) !=
            CartesianVelocity("OTHER", threeDimensionalCartesianPosition)
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, Getters)
{
    const CartesianVelocity cartesianVelocity = CartesianVelocity("NAME", threeDimensionalCartesianPosition);

    EXPECT_EQ("NAME", cartesianVelocity.getName());
    EXPECT_EQ(3, cartesianVelocity.getSize());
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, InFrame)
{
    {
        const Shared<const CartesianVelocity> cartesianVelocity =
            std::make_shared<const CartesianVelocity>("CARTESIAN_VELOCITY", threeDimensionalCartesianPosition);

        VectorXd allCoordinates(6);
        allCoordinates << 1.0e6, 2.0e6, 3.0e5, 4.0e3, -5.0e3, 6.0e3;
        const Shared<const CoordinatesBroker> brokerSPtr =
            CoordinatesBroker::FromSubsets({threeDimensionalCartesianPosition, cartesianVelocity});

        Vector3d expected = Velocity::MetersPerSecond({4.0e3, -5.0e3, 6.0e3}, frame_1)
                                .inFrame(Position::Meters({1.0e6, 2.0e6, 3.0e5}, frame_1), frame_2, instant)
                                .getCoordinates();
        VectorXd actual = cartesianVelocity->inFrame(instant, allCoordinates, frame_1, brokerSPtr, frame_2);

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, FromPosition)
{
    {
        EXPECT_NO_THROW(CartesianVelocity::FromPosition(threeDimensionalCartesianPosition));
    }

    {
        const Shared<const CartesianVelocity> cartesianVelocity =
            CartesianVelocity::FromPosition(threeDimensionalCartesianPosition);

        EXPECT_EQ(CartesianVelocity::DEFAULT_NAME, cartesianVelocity->getName());
        EXPECT_EQ(3, cartesianVelocity->getSize());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, ThreeDimensional)
{
    {
        EXPECT_NO_THROW(CartesianVelocity::ThreeDimensional());
    }

    {
        const Shared<const CartesianVelocity> threeDimensional = CartesianVelocity::ThreeDimensional();

        EXPECT_EQ(CartesianVelocity::DEFAULT_NAME, threeDimensional->getName());
        EXPECT_EQ(3, threeDimensional->getSize());
    }
}
