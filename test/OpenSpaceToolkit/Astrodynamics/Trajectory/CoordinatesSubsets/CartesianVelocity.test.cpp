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

static const CartesianPosition oneDimensionalCartesianPosition = CartesianPosition::OneDimensional();
static const CartesianPosition twoDimensionalCartesianPosition = CartesianPosition::TwoDimensional();
static const CartesianPosition threeDimensionalCartesianPosition = CartesianPosition::ThreeDimensional();

static const Instant instant = Instant::J2000();
static const Shared<const Frame> frame_1 = Frame::GCRF();
static const Shared<const Frame> frame_2 = Frame::TEME();

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, Constructor)
{
    {
        EXPECT_NO_THROW(CartesianVelocity("NAME", oneDimensionalCartesianPosition));
        EXPECT_NO_THROW(CartesianVelocity("NAME", twoDimensionalCartesianPosition));
        EXPECT_NO_THROW(CartesianVelocity("NAME", threeDimensionalCartesianPosition));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, EqualToOperator)
{
    {
        EXPECT_TRUE(
            CartesianVelocity("NAME", twoDimensionalCartesianPosition) ==
            CartesianVelocity("NAME", twoDimensionalCartesianPosition)
        );
    }

    {
        EXPECT_FALSE(
            CartesianVelocity("NAME", twoDimensionalCartesianPosition) ==
            CartesianVelocity("OTHER", twoDimensionalCartesianPosition)
        );
        EXPECT_FALSE(
            CartesianVelocity("NAME", oneDimensionalCartesianPosition) ==
            CartesianVelocity("NAME", twoDimensionalCartesianPosition)
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, NotEqualToOperator)
{
    {
        EXPECT_FALSE(
            CartesianVelocity("NAME", twoDimensionalCartesianPosition) !=
            CartesianVelocity("NAME", twoDimensionalCartesianPosition)
        );
    }

    {
        EXPECT_TRUE(
            CartesianVelocity("NAME", twoDimensionalCartesianPosition) !=
            CartesianVelocity("OTHER", twoDimensionalCartesianPosition)
        );
        EXPECT_TRUE(
            CartesianVelocity("NAME", oneDimensionalCartesianPosition) !=
            CartesianVelocity("NAME", twoDimensionalCartesianPosition)
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, Getters)
{
    const CartesianVelocity cartesianVelocity = CartesianVelocity("NAME", twoDimensionalCartesianPosition);

    EXPECT_EQ("NAME", cartesianVelocity.getName());
    EXPECT_EQ(2, cartesianVelocity.getSize());
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, FromPosition)
{
    {
        const CartesianVelocity oneDimensionalCartesianVelocity =
            CartesianVelocity::FromPosition(oneDimensionalCartesianPosition);

        EXPECT_EQ(CartesianVelocity::DEFAULT_NAME, oneDimensionalCartesianVelocity.getName());
        EXPECT_EQ(1, oneDimensionalCartesianVelocity.getSize());

        VectorXd allCoordinates(6);
        allCoordinates << 1.0e6, 4.0e3;
        CoordinatesBroker broker = CoordinatesBroker();
        broker.addSubset(oneDimensionalCartesianPosition);
        broker.addSubset(oneDimensionalCartesianVelocity);
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(broker);

        EXPECT_ANY_THROW(oneDimensionalCartesianVelocity.inFrame(instant, allCoordinates, frame_1, brokerSPtr, frame_2)
        );
    }

    {
        const CartesianVelocity twoDimensionalCartesianVelocity =
            CartesianVelocity::FromPosition(twoDimensionalCartesianPosition);

        EXPECT_EQ(CartesianVelocity::DEFAULT_NAME, twoDimensionalCartesianVelocity.getName());
        EXPECT_EQ(2, twoDimensionalCartesianVelocity.getSize());

        VectorXd allCoordinates(6);
        allCoordinates << 1.0e6, 2.0e6, 4.0e3, 5.0e3;
        CoordinatesBroker broker = CoordinatesBroker();
        broker.addSubset(twoDimensionalCartesianPosition);
        broker.addSubset(twoDimensionalCartesianVelocity);
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(broker);

        EXPECT_ANY_THROW(twoDimensionalCartesianVelocity.inFrame(instant, allCoordinates, frame_1, brokerSPtr, frame_2)
        );
    }

    {
        const CartesianVelocity threeDimensionalCartesianVelocity =
            CartesianVelocity::FromPosition(threeDimensionalCartesianPosition);

        EXPECT_EQ(CartesianVelocity::DEFAULT_NAME, threeDimensionalCartesianVelocity.getName());
        EXPECT_EQ(3, threeDimensionalCartesianVelocity.getSize());

        VectorXd allCoordinates(6);
        allCoordinates << 1.0e6, 2.0e6, 3.0e5, 4.0e3, -5.0e3, 6.0e3;
        CoordinatesBroker broker = CoordinatesBroker();
        broker.addSubset(threeDimensionalCartesianPosition);
        broker.addSubset(threeDimensionalCartesianVelocity);
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(broker);

        Vector3d expected = Velocity::MetersPerSecond({4.0e3, -5.0e3, 6.0e3}, frame_1)
                                .inFrame(Position::Meters({1.0e6, 2.0e6, 3.0e5}, frame_1), frame_2, instant)
                                .getCoordinates();
        VectorXd actual =
            threeDimensionalCartesianVelocity.inFrame(instant, allCoordinates, frame_1, brokerSPtr, frame_2);

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}
