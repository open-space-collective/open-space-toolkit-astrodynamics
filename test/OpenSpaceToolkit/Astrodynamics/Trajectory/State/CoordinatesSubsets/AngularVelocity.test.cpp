/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/AngularVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::container::Array;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::AttitudeQuaternion;
using ostk::astro::trajectory::state::coordinatessubsets::AngularVelocity;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Shared<const AttitudeQuaternion> defaultAttitudeQuaternionSPtr_ = AttitudeQuaternion::Default();
    const AngularVelocity defaultAngularVelocity_ = AngularVelocity(defaultAttitudeQuaternionSPtr_, defaultName_);

    const Array<Shared<const CoordinatesSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<AngularVelocity>(defaultAngularVelocity_)
    };
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity, Constructor)
{
    {
        EXPECT_NO_THROW(AngularVelocity(defaultAttitudeQuaternionSPtr_, defaultName_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity, Getters)
{
    EXPECT_EQ(3, defaultAngularVelocity_.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity, Add)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> frame = Frame::GCRF();

        VectorXd allCoordinates_1(3);
        allCoordinates_1 << 1.0, 2.0, 3.0;
        VectorXd allCoordinates_2(3);
        allCoordinates_2 << 4.0, 5.0, 6.0;

        EXPECT_ANY_THROW(
            defaultAngularVelocity_.add(instant, allCoordinates_1, allCoordinates_2, frame, defaultCoordinatesBroker_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity, Subtract)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> frame = Frame::GCRF();
        VectorXd allCoordinates_1(3);
        allCoordinates_1 << 1.0, 2.0, 3.0;
        VectorXd allCoordinates_2(3);
        allCoordinates_2 << -4.0, -5.0, -6.0;

        EXPECT_ANY_THROW(defaultAngularVelocity_.subtract(
            instant, allCoordinates_1, allCoordinates_2, frame, defaultCoordinatesBroker_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(7);
        fullCoordinatesVector << 0.0, 0.0, 0.0, 1.0, 1.0, 2.0, 3.0;
        const Array<Shared<const CoordinatesSubset>> coordinateSubsets = {
            defaultAttitudeQuaternionSPtr_, std::make_shared<AngularVelocity>(defaultAngularVelocity_)
        };
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(coordinateSubsets);

        // TBI: check actual values
        EXPECT_NO_THROW(defaultAngularVelocity_.inFrame(instant, fullCoordinatesVector, fromFrame, toFrame, brokerSPtr)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AngularVelocity, Default)
{
    {
        EXPECT_NO_THROW(AngularVelocity::Default());
    }
}
