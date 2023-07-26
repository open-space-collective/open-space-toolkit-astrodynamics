/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, Constructor)
{
    {
        EXPECT_NO_THROW(CoordinatesSubset("NAME", 1));
    }

    {
        EXPECT_ANY_THROW(CoordinatesSubset("", 0));
        EXPECT_ANY_THROW(CoordinatesSubset("NAME", 0));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, EqualToOperator)
{
    {
        EXPECT_TRUE(CoordinatesSubset("NAME", 1) == CoordinatesSubset("NAME", 1));
    }

    {
        EXPECT_FALSE(CoordinatesSubset("NAME", 1) == CoordinatesSubset("OTHER", 1));
        EXPECT_FALSE(CoordinatesSubset("NAME", 1) == CoordinatesSubset("NAME", 2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, NotEqualToOperator)
{
    {
        EXPECT_FALSE(CoordinatesSubset("NAME", 1) != CoordinatesSubset("NAME", 1));
    }

    {
        EXPECT_TRUE(CoordinatesSubset("NAME", 1) != CoordinatesSubset("OTHER", 1));
        EXPECT_TRUE(CoordinatesSubset("NAME", 1) != CoordinatesSubset("NAME", 2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, Getters)
{
    {
        const CoordinatesSubset coordinatesSubset = CoordinatesSubset("NAME", 1);

        EXPECT_EQ("NAME", coordinatesSubset.getName());
        EXPECT_EQ(1, coordinatesSubset.getSize());
    }
}
