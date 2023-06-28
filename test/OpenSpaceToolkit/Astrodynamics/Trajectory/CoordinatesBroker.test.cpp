/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>

#include <Global.test.hpp>

using ostk::core::types::Integer;
using ostk::core::types::String;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::CoordinatesSubset;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker, Constructor)
{
    {
        EXPECT_NO_THROW(CoordinatesBroker());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker, Operations)
{
    {
        CoordinatesSubset subset1 = CoordinatesSubset("S1", 1);
        CoordinatesSubset subset2 = CoordinatesSubset("S2", 2);
        CoordinatesSubset subset3 = CoordinatesSubset("S3", 3);
        CoordinatesBroker broker = CoordinatesBroker();

        EXPECT_EQ(0, broker.getNumberOfCoordinates());
        EXPECT_EQ(0, broker.getNumberOfSubsets());
        EXPECT_FALSE(broker.hasSubset(subset1));
        EXPECT_FALSE(broker.hasSubset(subset2));
        EXPECT_FALSE(broker.hasSubset(subset3));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset1));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset2));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset3));

        // Add subset 1
        EXPECT_EQ(0, broker.addSubset(subset1));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset1));
        EXPECT_FALSE(broker.hasSubset(subset2));
        EXPECT_FALSE(broker.hasSubset(subset3));
        EXPECT_EQ(0, broker.getSubsetIndex(subset1));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset2));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset3));

        // Add subset 2
        EXPECT_EQ(1, broker.addSubset(subset2));

        EXPECT_EQ(3, broker.getNumberOfCoordinates());
        EXPECT_EQ(2, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset1));
        EXPECT_TRUE(broker.hasSubset(subset2));
        EXPECT_FALSE(broker.hasSubset(subset3));
        EXPECT_EQ(0, broker.getSubsetIndex(subset1));
        EXPECT_EQ(1, broker.getSubsetIndex(subset2));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset3));

        // Add subset 3
        EXPECT_EQ(3, broker.addSubset(subset3));

        EXPECT_EQ(6, broker.getNumberOfCoordinates());
        EXPECT_EQ(3, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset1));
        EXPECT_TRUE(broker.hasSubset(subset2));
        EXPECT_TRUE(broker.hasSubset(subset3));
        EXPECT_EQ(0, broker.getSubsetIndex(subset1));
        EXPECT_EQ(1, broker.getSubsetIndex(subset2));
        EXPECT_EQ(3, broker.getSubsetIndex(subset3));
    }

    {
        CoordinatesSubset subset = CoordinatesSubset("S", 1);
        CoordinatesSubset duplicate = CoordinatesSubset("S", 1);
        CoordinatesBroker broker = CoordinatesBroker();

        // Add subset
        EXPECT_EQ(0, broker.addSubset(subset));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset));
        EXPECT_TRUE(broker.hasSubset(duplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset));
        EXPECT_EQ(0, broker.getSubsetIndex(duplicate));

        // Add subset again
        EXPECT_EQ(0, broker.addSubset(subset));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset));
        EXPECT_TRUE(broker.hasSubset(duplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset));
        EXPECT_EQ(0, broker.getSubsetIndex(duplicate));

        // Add duplicate
        EXPECT_EQ(0, broker.addSubset(duplicate));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset));
        EXPECT_TRUE(broker.hasSubset(duplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset));
        EXPECT_EQ(0, broker.getSubsetIndex(duplicate));
    }
}
