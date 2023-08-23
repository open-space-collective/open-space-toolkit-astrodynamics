/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/Invariant.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::ctnr::Size;

using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::Invariant;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_Invariant : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Size defaultSize_ = 2;
    const Invariant defaultInvariant_ = Invariant(defaultName_, defaultSize_);
    const Array<Shared<const CoordinatesSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<Invariant>(defaultInvariant_)
    };
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_Invariant, Constructor)
{
    {
        EXPECT_NO_THROW(Invariant("NAME", 1));
    }

    {
        EXPECT_NO_THROW(Invariant("OTHER_NAME", 4));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_Invariant, Getters)
{
    EXPECT_EQ(2, defaultInvariant_.getSize());
    EXPECT_EQ("NAME", defaultInvariant_.getName());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_Invariant, Add)
{
    {
        VectorXd firstCoordinates(2);
        firstCoordinates << 1.0, 2.0;
        VectorXd secondCoordinates(2);
        secondCoordinates << 4.0, 5.0;

        VectorXd expected(2);
        expected << 5.0, 7.0;

        const VectorXd actual = defaultInvariant_.add(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinatesBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_Invariant, Subtract)
{
    {
        VectorXd firstCoordinates(2);
        firstCoordinates << 1.0, 2.0;
        VectorXd secondCoordinates(2);
        secondCoordinates << -4.0, -5.0;

        VectorXd expected(2);
        expected << 5.0, 7.0;

        const VectorXd actual = defaultInvariant_.subtract(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinatesBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_Invariant, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(3);
        fullCoordinatesVector << 1.0e7, -1e7, 5e6;
        const Shared<const CoordinatesBroker> brokerkSPtr = defaultCoordinatesBroker_;

        VectorXd expected = fullCoordinatesVector.segment(0, 2);

        const VectorXd actual =
            defaultInvariant_.inFrame(instant, fullCoordinatesVector, fromFrame, toFrame, brokerkSPtr);

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_Invariant, Mass)
{
    {
        EXPECT_NO_THROW(Invariant::Mass());
    }
}
