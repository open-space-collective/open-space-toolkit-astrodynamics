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

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, Constructor)
{
    {
        EXPECT_NO_THROW(CartesianVelocity("NAME", CartesianPosition::ThreeDimensional()));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, EqualToOperator)
{
    {
        EXPECT_TRUE(
            CartesianVelocity("NAME", CartesianPosition::ThreeDimensional()) ==
            CartesianVelocity("NAME", CartesianPosition::ThreeDimensional())
        );
    }

    {
        EXPECT_FALSE(
            CartesianVelocity("NAME", CartesianPosition::ThreeDimensional()) ==
            CartesianVelocity("OTHER", CartesianPosition::ThreeDimensional())
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, NotEqualToOperator)
{
    {
        EXPECT_FALSE(
            CartesianVelocity("NAME", CartesianPosition::ThreeDimensional()) !=
            CartesianVelocity("NAME", CartesianPosition::ThreeDimensional())
        );
    }

    {
        EXPECT_TRUE(
            CartesianVelocity("NAME", CartesianPosition::ThreeDimensional()) !=
            CartesianVelocity("OTHER", CartesianPosition::ThreeDimensional())
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, Getters)
{
    const CartesianVelocity cartesianVelocity = CartesianVelocity("NAME", CartesianPosition::ThreeDimensional());

    EXPECT_EQ("NAME", cartesianVelocity.getName());
    EXPECT_EQ(3, cartesianVelocity.getSize());
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, Add)
{
    {
        const Shared<const CartesianVelocity> cartesianVelocity =
            CartesianVelocity::FromPosition(CartesianPosition::ThreeDimensional());

        VectorXd allCoordinates_1(3);
        allCoordinates_1 << 1.0, 2.0, 3.0;
        VectorXd allCoordinates_2(3);
        allCoordinates_2 << 4.0, 5.0, 6.0;
        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        VectorXd actual = cartesianVelocity->add(
            Instant::Undefined(),
            allCoordinates_1,
            allCoordinates_2,
            Frame::Undefined(),
            CoordinatesBroker::FromSubsets({cartesianVelocity})
        );

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, Subtract)
{
    {
        const Shared<const CartesianVelocity> cartesianVelocity =
            CartesianVelocity::FromPosition(CartesianPosition::ThreeDimensional());

        VectorXd allCoordinates_1(3);
        allCoordinates_1 << 1.0, 2.0, 3.0;
        VectorXd allCoordinates_2(3);
        allCoordinates_2 << -4.0, -5.0, -6.0;
        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        VectorXd actual = cartesianVelocity->subtract(
            Instant::Undefined(),
            allCoordinates_1,
            allCoordinates_2,
            Frame::Undefined(),
            CoordinatesBroker::FromSubsets({cartesianVelocity})
        );

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> frame_1 = Frame::GCRF();
        const Shared<const Frame> frame_2 = Frame::TEME();
        const Shared<const CartesianVelocity> cartesianVelocity =
            CartesianVelocity::FromPosition(CartesianPosition::ThreeDimensional());
        VectorXd allCoordinates(6);
        allCoordinates << 1.0e6, 2.0e6, 3.0e5, 4.0e3, -5.0e3, 6.0e3;
        const Shared<const CoordinatesBroker> brokerSPtr =
            CoordinatesBroker::FromSubsets({CartesianPosition::ThreeDimensional(), cartesianVelocity});

        Vector3d expected = Velocity::MetersPerSecond({4.0e3, -5.0e3, 6.0e3}, frame_1)
                                .inFrame(Position::Meters({1.0e6, 2.0e6, 3.0e5}, frame_1), frame_2, instant)
                                .getCoordinates();
        VectorXd actual = cartesianVelocity->inFrame(instant, allCoordinates, frame_1, frame_2, brokerSPtr);

        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(expected(0), actual(0));
        EXPECT_EQ(expected(1), actual(1));
        EXPECT_EQ(expected(2), actual(2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_CartesianVelocity, FromPosition)
{
    {
        EXPECT_NO_THROW(CartesianVelocity::FromPosition(CartesianPosition::ThreeDimensional()));
    }

    {
        const Shared<const CartesianVelocity> cartesianVelocity =
            CartesianVelocity::FromPosition(CartesianPosition::ThreeDimensional());

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
