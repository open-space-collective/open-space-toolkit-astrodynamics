/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AttitudeQuaternion.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const AttitudeQuaternion defaultAttitudeQuaternion_ = AttitudeQuaternion(defaultName_);
    const Array<Shared<const CoordinateSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<AttitudeQuaternion>(defaultAttitudeQuaternion_)
    };
    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion, Constructor)
{
    {
        EXPECT_NO_THROW(AttitudeQuaternion("NAME"));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion, Getters)
{
    EXPECT_EQ(4, defaultAttitudeQuaternion_.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion, Add)
{
    {
        VectorXd firstCoordinates(4);
        firstCoordinates << 0.0, 0.0, 0.0, 1.0;
        VectorXd secondCoordinates(4);
        secondCoordinates << 0.0, 0.0, 0.0, 1.0;

        EXPECT_ANY_THROW(defaultAttitudeQuaternion_.add(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinateBroker_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion, Subtract)
{
    {
        VectorXd firstCoordinates(4);
        firstCoordinates << 0.0, 0.0, 0.0, 1.0;
        VectorXd secondCoordinates(4);
        secondCoordinates << 0.0, 0.0, 0.0, 1.0;

        EXPECT_ANY_THROW(defaultAttitudeQuaternion_.subtract(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinateBroker_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(4);
        fullCoordinatesVector << 0.0, 0.0, 0.0, 1.0;
        const Shared<const CoordinateBroker> brokerkSPtr = defaultCoordinateBroker_;

        // TBI: check actual values
        EXPECT_NO_THROW(
            defaultAttitudeQuaternion_.inFrame(instant, fullCoordinatesVector, fromFrame, toFrame, brokerkSPtr)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_AttitudeQuaternion, Default)
{
    {
        EXPECT_NO_THROW(AttitudeQuaternion::Default());
    }
}
