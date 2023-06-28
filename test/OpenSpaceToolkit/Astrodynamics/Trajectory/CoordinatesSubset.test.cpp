/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

#include <Global.test.hpp>

using ostk::core::types::Integer;

using ostk::astro::trajectory::CoordinatesSubset;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, Constructor)
{
    {
        EXPECT_NO_THROW(CoordinatesSubset("ID", 1));
    }

    {
        EXPECT_NO_THROW(CoordinatesSubset("ID", 10));
    }

    {
        EXPECT_ANY_THROW(CoordinatesSubset("ID", 0));
    }

    {
        EXPECT_ANY_THROW(CoordinatesSubset("ID", -1));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, IsDefined)
{
    {
        EXPECT_TRUE(CoordinatesSubset("ID", 1).isDefined());
    }

    {
        EXPECT_TRUE(CoordinatesSubset("ID", 10).isDefined());
    }

    {
        EXPECT_FALSE(CoordinatesSubset("", 1).isDefined());
    }

    {
        EXPECT_FALSE(CoordinatesSubset("ID", Integer::Undefined()).isDefined());
    }

    {
        EXPECT_FALSE(CoordinatesSubset::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, EqualToOperator)
{
    {
        EXPECT_TRUE(CoordinatesSubset("ID", 1) == CoordinatesSubset("ID", 2));
    }

    {
        EXPECT_FALSE(CoordinatesSubset("ID_1", 1) == CoordinatesSubset("ID_2", 1));
    }

    {
        EXPECT_FALSE(CoordinatesSubset("ID", 1) == CoordinatesSubset::Undefined());
    }

    {
        EXPECT_FALSE(CoordinatesSubset::Undefined() == CoordinatesSubset("ID", 1));
    }

    {
        EXPECT_FALSE(CoordinatesSubset::Undefined() == CoordinatesSubset::Undefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, NotEqualToOperator)
{
    {
        EXPECT_FALSE(CoordinatesSubset("ID", 1) != CoordinatesSubset("ID", 2));
    }

    {
        EXPECT_TRUE(CoordinatesSubset("ID_1", 1) != CoordinatesSubset("ID_2", 1));
    }

    {
        EXPECT_TRUE(CoordinatesSubset("ID", 1) != CoordinatesSubset::Undefined());
    }

    {
        EXPECT_TRUE(CoordinatesSubset::Undefined() != CoordinatesSubset("ID", 1));
    }

    {
        EXPECT_TRUE(CoordinatesSubset::Undefined() != CoordinatesSubset::Undefined());
    }
}
