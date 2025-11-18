/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Transform.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Axes;
using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using CoordinateTransform = ostk::physics::coordinate::Transform;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;

using ostk::astrodynamics::flight::profile::model::Transform;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        this->orbit_ = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        this->transform_ = Transform::LocalOrbitalFramePointing(this->orbit_, Orbit::FrameType::VVLH);
    }

    Orbit orbit_ = Orbit::Undefined();
    Transform transform_ = Transform::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, Constructor)
{
    const auto dynamicProviderGenerator = [&](const Instant& anInstant) -> CoordinateTransform
    {
        const State state = this->orbit_.getStateAt(anInstant);

        const Position position_GCRF = state.getPosition();
        const Velocity velocity_GCRF = state.getVelocity();

        return CoordinateTransform::Active(
            anInstant,
            -position_GCRF.accessCoordinates(),
            -velocity_GCRF.accessCoordinates(),
            Quaternion::Unit(),
            Vector3d(0.0, 0.0, 0.0)  // TBM: Artificially set to 0 for now.
        );
    };

    EXPECT_NO_THROW(Transform transform(DynamicProvider(dynamicProviderGenerator), Frame::GCRF()));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << transform_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(transform_.print(std::cout, true));
    EXPECT_NO_THROW(transform_.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, IsDefined)
{
    {
        EXPECT_TRUE(transform_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, CalculateStateAt)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 29, 0, 0, 15), Scale::UTC);

        const State state = transform_.calculateStateAt(instant);

        EXPECT_EQ(state.getInstant(), instant);

        EXPECT_TRUE(state.accessFrame() == Frame::GCRF());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, GetAxesAt)
{
    // undefined
    {
        const Instant instant = Instant::J2000();
        const Transform undefinedTransform = Transform::Undefined();
        EXPECT_THROW(undefinedTransform.getAxesAt(instant), ostk::core::error::runtime::Undefined);
    }

    // instant undefined
    {
        const Instant instant = Instant::Undefined();
        EXPECT_THROW(transform_.getAxesAt(instant), ostk::core::error::runtime::Undefined);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 29, 0, 0, 15), Scale::UTC);
        const Axes axes = transform_.getAxesAt(instant);
        const State state = transform_.calculateStateAt(instant);
        const Quaternion q_B_GCRF = state.getAttitude();
        EXPECT_VECTORS_ALMOST_EQUAL(q_B_GCRF * axes.x(), Vector3d::X(), 1e-15);
        EXPECT_VECTORS_ALMOST_EQUAL(q_B_GCRF * axes.y(), Vector3d::Y(), 1e-15);
        EXPECT_VECTORS_ALMOST_EQUAL(q_B_GCRF * axes.z(), Vector3d::Z(), 1e-15);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, ConstructBodyFrame)
{
    // name undefined
    {
        const String frameName = String::Empty();
        EXPECT_THROW(transform_.constructBodyFrame(frameName), ostk::core::error::runtime::Undefined);
    }

    {
        const String frameName = "OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform.ConstructBodyFrame";

        // undefined model
        {
            const Transform undefinedTransform = Transform::Undefined();
            EXPECT_THROW(undefinedTransform.constructBodyFrame(frameName), ostk::core::error::runtime::Undefined);
        }

        {
            const Shared<const Frame> bodyFrame = transform_.constructBodyFrame(frameName);
            EXPECT_EQ(bodyFrame->getName(), frameName);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, Undefined)
{
    {
        EXPECT_FALSE(Transform::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, InertialPointing)
{
    {
        EXPECT_TRUE(Transform::InertialPointing(orbit_, Quaternion::Unit()).isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform, LocalOrbitalFramePointing)
{
    {
        EXPECT_TRUE(Transform::LocalOrbitalFramePointing(orbit_, Orbit::FrameType::VVLH).isDefined());
    }
}
