/// Apache License 2.0

#include <gmock/gmock.h>

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
         const Shared<const Frame>& aFrameSPtr,
         const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr),
        (const, override)
    );

    MOCK_METHOD(
        VectorXd,
        subtract,
        (const Instant& anInstant,
         const VectorXd& aFullCoordinatesVector,
         const VectorXd& anotherFullCoordinatesVector,
         const Shared<const Frame>& aFrameSPtr,
         const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr),
        (const, override)
    );

    MOCK_METHOD(
        VectorXd,
        inFrame,
        (const Instant& anInstant,
         const VectorXd& aFullCoordinatesVector,
         const Shared<const Frame>& fromFrame,
         const Shared<const Frame>& toFrame,
         const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr),
        (const, override)
    );
};

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Size defaultSize_ = 1;
    const CoordinatesSubsetMock defaultCoordinateSubset_ = CoordinatesSubsetMock(defaultName_, defaultSize_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, Constructor)
{
    {
        EXPECT_NO_THROW(CoordinatesSubsetMock("NAME", 1));
    }

    {
        EXPECT_ANY_THROW(CoordinatesSubsetMock("", 0));
        EXPECT_ANY_THROW(CoordinatesSubsetMock("NAME", 0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultCoordinateSubset_ == defaultCoordinateSubset_);
    }

    {
        EXPECT_FALSE(defaultCoordinateSubset_ == CoordinatesSubsetMock("OTHER", 1));
        EXPECT_FALSE(defaultCoordinateSubset_ == CoordinatesSubsetMock("NAME", 2));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultCoordinateSubset_ != defaultCoordinateSubset_);
    }

    {
        EXPECT_TRUE(defaultCoordinateSubset_ != CoordinatesSubsetMock("OTHER", 1));
        EXPECT_TRUE(defaultCoordinateSubset_ != CoordinatesSubsetMock("NAME", 2));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset, Getters)
{
    {
        EXPECT_EQ(defaultName_, defaultCoordinateSubset_.getName());
        EXPECT_EQ(defaultSize_, defaultCoordinateSubset_.getSize());
    }
}
