/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Size;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

class SimpleCoordinatesSubset : public CoordinatesSubset
{
   public:
    SimpleCoordinatesSubset(const String& aName, const Size& aSize)
        : CoordinatesSubset(aName, aSize)
    {
    }

    VectorXd add(
        [[maybe_unused]] const Instant& anInstant,
        const VectorXd& allCoordinates_1,
        const VectorXd& allCoordinates_2,
        [[maybe_unused]] const Shared<const Frame>& aFrame,
        [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const override
    {
        return allCoordinates_1 + allCoordinates_2;
    }

    VectorXd subtract(
        [[maybe_unused]] const Instant& anInstant,
        const VectorXd& allCoordinates_1,
        const VectorXd& allCoordinates_2,
        [[maybe_unused]] const Shared<const Frame>& aFrame,
        [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const override
    {
        return allCoordinates_1 - allCoordinates_2;
    }

    VectorXd inFrame(
        [[maybe_unused]] const Instant& anInstant,
        const VectorXd& allCoordinates,
        [[maybe_unused]] const Shared<const Frame>& aFrame,
        [[maybe_unused]] const Shared<const Frame>& toFrame,
        [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const override
    {
        return allCoordinates;
    }
};

class OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Size defaultSize_ = 1;
    const SimpleCoordinatesSubset defaultCoordinateSubset_ = SimpleCoordinatesSubset(defaultName_, defaultSize_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, Constructor)
{
    {
        EXPECT_NO_THROW(SimpleCoordinatesSubset("NAME", 1));
    }

    {
        EXPECT_ANY_THROW(SimpleCoordinatesSubset("", 0));
        EXPECT_ANY_THROW(SimpleCoordinatesSubset("NAME", 0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultCoordinateSubset_ == defaultCoordinateSubset_);
    }

    {
        EXPECT_FALSE(defaultCoordinateSubset_ == SimpleCoordinatesSubset("OTHER", 1));
        EXPECT_FALSE(defaultCoordinateSubset_ == SimpleCoordinatesSubset("NAME", 2));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultCoordinateSubset_ != defaultCoordinateSubset_);
    }

    {
        EXPECT_TRUE(defaultCoordinateSubset_ != SimpleCoordinatesSubset("OTHER", 1));
        EXPECT_TRUE(defaultCoordinateSubset_ != SimpleCoordinatesSubset("NAME", 2));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset, Getters)
{
    {
        EXPECT_EQ(defaultName_, defaultCoordinateSubset_.getName());
        EXPECT_EQ(defaultSize_, defaultCoordinateSubset_.getSize());
    }
}
