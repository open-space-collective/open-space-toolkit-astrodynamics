/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::CoordinatesSubset;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, Constructor)
{
    {
        EXPECT_NO_THROW(CoordinatesSubset("NAME", 1));
    }

    {
        EXPECT_ANY_THROW(CoordinatesSubset("", 0));
        EXPECT_ANY_THROW(CoordinatesSubset("NAME", 0));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, EqualToOperator)
{
    {
        EXPECT_TRUE(CoordinatesSubset("NAME", 1) == CoordinatesSubset("NAME", 1));
    }

    {
        EXPECT_FALSE(CoordinatesSubset("NAME", 1) == CoordinatesSubset("OTHER", 1));
        EXPECT_FALSE(CoordinatesSubset("NAME", 1) == CoordinatesSubset("NAME", 2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, NotEqualToOperator)
{
    {
        EXPECT_FALSE(CoordinatesSubset("NAME", 1) != CoordinatesSubset("NAME", 1));
    }

    {
        EXPECT_TRUE(CoordinatesSubset("NAME", 1) != CoordinatesSubset("OTHER", 1));
        EXPECT_TRUE(CoordinatesSubset("NAME", 1) != CoordinatesSubset("NAME", 2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, Getters)
{
    {
        const CoordinatesSubset coordinatesSubset = CoordinatesSubset("NAME", 1);

        EXPECT_EQ("NAME", coordinatesSubset.getName());
        EXPECT_EQ(1, coordinatesSubset.getSize());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, Add)
{
    {
        const CoordinatesSubset coordinatesSubset = CoordinatesSubset("NAME", 1);

        EXPECT_ANY_THROW(coordinatesSubset.add(Instant::Undefined(), VectorXd(0), VectorXd(0), nullptr, nullptr));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, Subtract)
{
    {
        const CoordinatesSubset coordinatesSubset = CoordinatesSubset("NAME", 1);

        EXPECT_ANY_THROW(coordinatesSubset.add(Instant::Undefined(), VectorXd(0), VectorXd(0), nullptr, nullptr));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, InFrame)
{
    {
        const CoordinatesSubset coordinatesSubset = CoordinatesSubset("NAME", 1);

        EXPECT_ANY_THROW(coordinatesSubset.inFrame(Instant::Undefined(), VectorXd(0), nullptr, nullptr, nullptr));
    }
}
