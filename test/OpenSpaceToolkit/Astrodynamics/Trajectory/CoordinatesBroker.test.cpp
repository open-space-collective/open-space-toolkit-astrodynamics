/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/Invariant.hpp>

#include <Global.test.hpp>

using ostk::math::obj::VectorXd;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::coordinatessubsets::Invariant;

static const Invariant subset_1 = Invariant("S1", 1);
static const Invariant subset_2 = Invariant("S2", 2);
static const Invariant subset_3 = Invariant("S3", 3);
static const Invariant subset_1_duplicate = Invariant("S1", 1);
static const Invariant subset_4 = Invariant("S4", 1);

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker, Constructor)
{
    {
        CoordinatesBroker broker = CoordinatesBroker();

        EXPECT_EQ(0, broker.getNumberOfCoordinates());
        EXPECT_EQ(0, broker.getNumberOfSubsets());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker, EqualToOperator)
{
    {
        CoordinatesBroker broker_1 = CoordinatesBroker();

        CoordinatesBroker broker_2 = CoordinatesBroker();

        EXPECT_TRUE(broker_1 == broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();
        broker_1.addSubset(subset_1);
        broker_1.addSubset(subset_2);

        CoordinatesBroker broker_2 = CoordinatesBroker();
        broker_2.addSubset(subset_1);
        broker_2.addSubset(subset_2);

        EXPECT_TRUE(broker_1 == broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();
        broker_1.addSubset(subset_1);
        broker_1.addSubset(subset_2);

        CoordinatesBroker broker_2 = CoordinatesBroker();
        broker_2.addSubset(subset_2);
        broker_2.addSubset(subset_1);

        EXPECT_FALSE(broker_1 == broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();
        broker_1.addSubset(subset_1);
        broker_1.addSubset(subset_2);

        CoordinatesBroker broker_2 = CoordinatesBroker();

        EXPECT_FALSE(broker_1 == broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();

        CoordinatesBroker broker_2 = CoordinatesBroker();
        broker_2.addSubset(subset_1);
        broker_2.addSubset(subset_2);

        EXPECT_FALSE(broker_1 == broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();
        broker_1.addSubset(subset_1);
        broker_1.addSubset(subset_2);

        CoordinatesBroker broker_2 = CoordinatesBroker();
        broker_2.addSubset(subset_1);

        EXPECT_FALSE(broker_1 == broker_2);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker, NotEqualToOperator)
{
    {
        CoordinatesBroker broker_1 = CoordinatesBroker();

        CoordinatesBroker broker_2 = CoordinatesBroker();

        EXPECT_FALSE(broker_1 != broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();
        broker_1.addSubset(subset_1);
        broker_1.addSubset(subset_2);

        CoordinatesBroker broker_2 = CoordinatesBroker();
        broker_2.addSubset(subset_1);
        broker_2.addSubset(subset_2);

        EXPECT_FALSE(broker_1 != broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();
        broker_1.addSubset(subset_1);
        broker_1.addSubset(subset_2);

        CoordinatesBroker broker_2 = CoordinatesBroker();
        broker_2.addSubset(subset_2);
        broker_2.addSubset(subset_1);

        EXPECT_TRUE(broker_1 != broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();
        broker_1.addSubset(subset_1);
        broker_1.addSubset(subset_2);

        CoordinatesBroker broker_2 = CoordinatesBroker();

        EXPECT_TRUE(broker_1 != broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();

        CoordinatesBroker broker_2 = CoordinatesBroker();
        broker_2.addSubset(subset_1);
        broker_2.addSubset(subset_2);

        EXPECT_TRUE(broker_1 != broker_2);
    }

    {
        CoordinatesBroker broker_1 = CoordinatesBroker();
        broker_1.addSubset(subset_1);
        broker_1.addSubset(subset_2);

        CoordinatesBroker broker_2 = CoordinatesBroker();
        broker_2.addSubset(subset_1);

        EXPECT_TRUE(broker_1 != broker_2);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker, Operations)
{
    {
        CoordinatesBroker broker = CoordinatesBroker();

        EXPECT_EQ(0, broker.getNumberOfCoordinates());
        EXPECT_EQ(0, broker.getNumberOfSubsets());
        EXPECT_FALSE(broker.hasSubset(subset_1));
        EXPECT_FALSE(broker.hasSubset(subset_2));
        EXPECT_FALSE(broker.hasSubset(subset_3));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset_1));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset_2));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset_3));

        // Add subset 1
        EXPECT_EQ(0, broker.addSubset(subset_1));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset_1));
        EXPECT_FALSE(broker.hasSubset(subset_2));
        EXPECT_FALSE(broker.hasSubset(subset_3));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset_2));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset_3));

        // Add subset 2
        EXPECT_EQ(1, broker.addSubset(subset_2));

        EXPECT_EQ(3, broker.getNumberOfCoordinates());
        EXPECT_EQ(2, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset_1));
        EXPECT_TRUE(broker.hasSubset(subset_2));
        EXPECT_FALSE(broker.hasSubset(subset_3));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_EQ(1, broker.getSubsetIndex(subset_2));
        EXPECT_ANY_THROW(broker.getSubsetIndex(subset_3));

        // Add subset 3
        EXPECT_EQ(3, broker.addSubset(subset_3));

        EXPECT_EQ(6, broker.getNumberOfCoordinates());
        EXPECT_EQ(3, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset_1));
        EXPECT_TRUE(broker.hasSubset(subset_2));
        EXPECT_TRUE(broker.hasSubset(subset_3));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_EQ(1, broker.getSubsetIndex(subset_2));
        EXPECT_EQ(3, broker.getSubsetIndex(subset_3));
    }

    {
        CoordinatesBroker broker = CoordinatesBroker();

        // Add subset
        EXPECT_EQ(0, broker.addSubset(subset_1));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset_1));
        EXPECT_TRUE(broker.hasSubset(subset_1_duplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1_duplicate));

        // Add subset again
        EXPECT_EQ(0, broker.addSubset(subset_1));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset_1));
        EXPECT_TRUE(broker.hasSubset(subset_1_duplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1_duplicate));

        // Add duplicate
        EXPECT_EQ(0, broker.addSubset(subset_1_duplicate));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset_1));
        EXPECT_TRUE(broker.hasSubset(subset_1_duplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1_duplicate));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker, Extract)
{
    {
        CoordinatesBroker broker = CoordinatesBroker();
        broker.addSubset(subset_1);
        broker.addSubset(subset_2);
        broker.addSubset(subset_3);

        VectorXd allCoordinates(6);
        allCoordinates << 0.0, 1.0, 2.0, 3.0, 4.0, 5.0;

        const VectorXd subset_1_coordinates = broker.extract(allCoordinates, subset_1);
        EXPECT_EQ(1, subset_1_coordinates.size());
        EXPECT_EQ(0.0, subset_1_coordinates(0));

        const VectorXd subset_2_coordinates = broker.extract(allCoordinates, subset_2);
        EXPECT_EQ(2, subset_2_coordinates.size());
        EXPECT_EQ(1.0, subset_2_coordinates(0));
        EXPECT_EQ(2.0, subset_2_coordinates(1));

        const VectorXd subset_3_coordinates = broker.extract(allCoordinates, subset_3);
        EXPECT_EQ(3, subset_3_coordinates.size());
        EXPECT_EQ(3.0, subset_3_coordinates(0));
        EXPECT_EQ(4.0, subset_3_coordinates(1));
        EXPECT_EQ(5.0, subset_3_coordinates(2));

        EXPECT_ANY_THROW(broker.extract(allCoordinates, subset_4));
    }
}
