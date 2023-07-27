/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

class CoordinatesSubsetMock : public CoordinatesSubset
{
   public:
    CoordinatesSubsetMock(const String& aName, const Size& aSize)
        : CoordinatesSubset(aName, aSize) {};

    MOCK_METHOD(
        VectorXd,
        add,
        (const Instant& anInstant,
         const VectorXd& aFullCoordinatesVector,
         const VectorXd& anotherFullCoordinatesVector,
         const Shared<const Frame>& aFrame,
         const Shared<const CoordinatesBroker>& aCoordinatesBroker),
        (const, override)
    );

    MOCK_METHOD(
        VectorXd,
        subtract,
        (const Instant& anInstant,
         const VectorXd& aFullCoordinatesVector,
         const VectorXd& anotherFullCoordinatesVector,
         const Shared<const Frame>& aFrame,
         const Shared<const CoordinatesBroker>& aCoordinatesBroker),
        (const, override)
    );

    MOCK_METHOD(
        VectorXd,
        inFrame,
        (const Instant& anInstant,
         const VectorXd& allCoordinates,
         const Shared<const Frame>& fromFrame,
         const Shared<const Frame>& toFrame,
         const Shared<const CoordinatesBroker>& aCoordinatesBroker),
        (const, override)
    );
};

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker : public ::testing::Test
{
   protected:
    const Shared<CoordinatesSubsetMock> subset_1 = std::make_shared<CoordinatesSubsetMock>("S1", 1);
    const Shared<CoordinatesSubsetMock> subset_2 = std::make_shared<CoordinatesSubsetMock>("S2", 2);
    const Shared<CoordinatesSubsetMock> subset_3 = std::make_shared<CoordinatesSubsetMock>("S3", 3);
    const Shared<CoordinatesSubsetMock> subset_4 = std::make_shared<CoordinatesSubsetMock>("S4", 1);
    const Shared<CoordinatesSubsetMock> subsetDuplicate = std::make_shared<CoordinatesSubsetMock>("S1", 1);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker, Constructor)
{
    {
        EXPECT_NO_THROW(CoordinatesBroker());
    }

    {
        EXPECT_NO_THROW(CoordinatesBroker({subset_1, subset_2, subsetDuplicate}));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker, EqualToOperator)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker, NotEqualToOperator)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker, Accessors)
{
    {
        CoordinatesBroker broker = CoordinatesBroker();

        EXPECT_EQ(0, broker.accessSubsets().size());
    }

    {
        CoordinatesBroker broker = CoordinatesBroker({subset_1, subset_2, subsetDuplicate});

        EXPECT_EQ(2, broker.accessSubsets().size());
        EXPECT_EQ(subset_1, broker.accessSubsets()[0]);
        EXPECT_EQ(subset_2, broker.accessSubsets()[1]);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker, Getters)
{
    {
        CoordinatesBroker broker = CoordinatesBroker();

        EXPECT_EQ(0, broker.getNumberOfCoordinates());
        EXPECT_EQ(0, broker.getNumberOfSubsets());
        EXPECT_EQ(0, broker.getSubsets().size());
    }

    {
        CoordinatesBroker broker = CoordinatesBroker({subset_1, subset_2, subsetDuplicate});

        EXPECT_EQ(3, broker.getNumberOfCoordinates());
        EXPECT_EQ(2, broker.getNumberOfSubsets());
        EXPECT_EQ(2, broker.getSubsets().size());
        EXPECT_EQ(subset_1, broker.getSubsets()[0]);
        EXPECT_EQ(subset_2, broker.getSubsets()[1]);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker, Operations)
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
        EXPECT_TRUE(broker.hasSubset(subsetDuplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_EQ(0, broker.getSubsetIndex(subsetDuplicate));

        // Add subset again
        EXPECT_EQ(0, broker.addSubset(subset_1));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset_1));
        EXPECT_TRUE(broker.hasSubset(subsetDuplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_EQ(0, broker.getSubsetIndex(subsetDuplicate));

        // Add duplicate
        EXPECT_EQ(0, broker.addSubset(subsetDuplicate));

        EXPECT_EQ(1, broker.getNumberOfCoordinates());
        EXPECT_EQ(1, broker.getNumberOfSubsets());
        EXPECT_TRUE(broker.hasSubset(subset_1));
        EXPECT_TRUE(broker.hasSubset(subsetDuplicate));
        EXPECT_EQ(0, broker.getSubsetIndex(subset_1));
        EXPECT_EQ(0, broker.getSubsetIndex(subsetDuplicate));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker, ExtractCoordinates)
{
    {
        CoordinatesBroker broker = CoordinatesBroker();
        broker.addSubset(subset_1);
        broker.addSubset(subset_2);
        broker.addSubset(subset_3);

        VectorXd allCoordinates(6);
        allCoordinates << 0.0, 1.0, 2.0, 3.0, 4.0, 5.0;

        const VectorXd subset_1_coordinates = broker.extractCoordinates(allCoordinates, subset_1);
        EXPECT_EQ(1, subset_1_coordinates.size());
        EXPECT_EQ(0.0, subset_1_coordinates(0));

        const VectorXd subset_2_coordinates = broker.extractCoordinates(allCoordinates, subset_2);
        EXPECT_EQ(2, subset_2_coordinates.size());
        EXPECT_EQ(1.0, subset_2_coordinates(0));
        EXPECT_EQ(2.0, subset_2_coordinates(1));

        const VectorXd subset_3_coordinates = broker.extractCoordinates(allCoordinates, subset_3);
        EXPECT_EQ(3, subset_3_coordinates.size());
        EXPECT_EQ(3.0, subset_3_coordinates(0));
        EXPECT_EQ(4.0, subset_3_coordinates(1));
        EXPECT_EQ(5.0, subset_3_coordinates(2));

        EXPECT_ANY_THROW(broker.extractCoordinates(allCoordinates, subset_4));
    }
}