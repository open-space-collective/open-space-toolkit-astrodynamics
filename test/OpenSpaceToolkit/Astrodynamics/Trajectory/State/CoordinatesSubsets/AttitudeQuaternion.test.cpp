/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/AttitudeQuaternion.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::geometry::d3::transformation::rotation::Quaternion;
using ostk::math::object::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::AttitudeQuaternion;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const AttitudeQuaternion defaultAttitudeQuaternion_ = AttitudeQuaternion(defaultName_);
    const Array<Shared<const CoordinatesSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<AttitudeQuaternion>(defaultAttitudeQuaternion_)
    };
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion, Constructor)
{
    {
        EXPECT_NO_THROW(AttitudeQuaternion("NAME"));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion, Getters)
{
    EXPECT_EQ(4, defaultAttitudeQuaternion_.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion, Add)
{
    {
        VectorXd firstCoordinates(4);
        firstCoordinates << 0.0, 0.0, 0.0, 1.0;
        VectorXd secondCoordinates(4);
        secondCoordinates << 0.0, 0.0, 0.0, 1.0;

        EXPECT_ANY_THROW(defaultAttitudeQuaternion_.add(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinatesBroker_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion, Subtract)
{
    {
        VectorXd firstCoordinates(4);
        firstCoordinates << 0.0, 0.0, 0.0, 1.0;
        VectorXd secondCoordinates(4);
        secondCoordinates << 0.0, 0.0, 0.0, 1.0;

        EXPECT_ANY_THROW(defaultAttitudeQuaternion_.subtract(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinatesBroker_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(4);
        fullCoordinatesVector << 0.0, 0.0, 0.0, 1.0;
        const Shared<const CoordinatesBroker> brokerkSPtr = defaultCoordinatesBroker_;

        // TBI: check actual values
        EXPECT_NO_THROW(
            defaultAttitudeQuaternion_.inFrame(instant, fullCoordinatesVector, fromFrame, toFrame, brokerkSPtr)
        );
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion, ConversionToAndFromQuaternion
)
{
    {
        VectorXd coordinates(4);
        coordinates << 0.0, 0.0, 0.0, 1.0;

        const Quaternion quaternion = AttitudeQuaternion::coordinatesToQuaternion(coordinates);

        const VectorXd roundTripCoordinates = AttitudeQuaternion::quaterionToCoordinates(quaternion);

        EXPECT_EQ(coordinates, roundTripCoordinates);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion, Default)
{
    {
        EXPECT_NO_THROW(AttitudeQuaternion::Default());
    }
}
