/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;

using ostk::physics::coordinate::Transform;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameFactory, Accessors)
{
    {
        EXPECT_EQ(gcrfSPtr_, LOFFactorySPtr_->accessParentFrame());
    }

    {
        EXPECT_ANY_THROW(
            LocalOrbitalFrameFactory::Construct(LocalOrbitalFrameTransformProvider::Type::Undefined, gcrfSPtr_)
                ->accessParentFrame()
        );
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
