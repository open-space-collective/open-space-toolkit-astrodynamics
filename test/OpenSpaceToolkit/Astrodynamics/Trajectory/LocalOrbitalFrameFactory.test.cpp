/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;
using ostk::math::geom::d3::trf::rot::Quaternion;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;

using ostk::physics::coord::Transform;

using ostk::astro::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;

class OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory : public ::testing::Test
{
   protected:
    const LocalOrbitalFrameTransformProvider::Type type_ = LocalOrbitalFrameTransformProvider::Type::VNC;
    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();
    Shared<const LocalOrbitalFrameFactory> LOFFactorySPtr_ = LocalOrbitalFrameFactory::VNC(gcrfSPtr_);

    const Instant instant_ = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);
    const Vector3d position_ = {7000000.0, 0.0, 0.0};
    const Vector3d velocity_ = {0.0, 5335.865450622126, 5335.865450622126};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory, Construct)
{
    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::Construct(LocalOrbitalFrameTransformProvider::Type::NED, gcrfSPtr_));
    }
    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::Construct(LocalOrbitalFrameTransformProvider::Type::LVLH, gcrfSPtr_));
    }
    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::Construct(LocalOrbitalFrameTransformProvider::Type::VVLH, gcrfSPtr_));
    }
    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::Construct(LocalOrbitalFrameTransformProvider::Type::QSW, gcrfSPtr_));
    }
    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::Construct(LocalOrbitalFrameTransformProvider::Type::TNW, gcrfSPtr_));
    }
    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::Construct(LocalOrbitalFrameTransformProvider::Type::VNC, gcrfSPtr_));
    }
    {
        EXPECT_NO_THROW(
            LocalOrbitalFrameFactory::Construct(LocalOrbitalFrameTransformProvider::Type::Undefined, gcrfSPtr_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory, Constructor)
{
    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::NED(gcrfSPtr_));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::LVLH(gcrfSPtr_));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::VVLH(gcrfSPtr_));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::QSW(gcrfSPtr_));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::TNW(gcrfSPtr_));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameFactory::VNC(gcrfSPtr_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory, GenerateFrame)
{
    {
        Shared<const Frame> localOrbitalFrame = LOFFactorySPtr_->generateFrame(instant_, position_, velocity_);

        Transform transform = localOrbitalFrame->getTransformTo(gcrfSPtr_, instant_);
        transform.accessOrientation();
    }

    {
        Shared<const Frame> localOrbitalFrame = LOFFactorySPtr_->generateFrame(instant_, position_, velocity_);

        EXPECT_ANY_THROW(localOrbitalFrame->getTransformTo(gcrfSPtr_, Instant::J2000()));
    }
}
