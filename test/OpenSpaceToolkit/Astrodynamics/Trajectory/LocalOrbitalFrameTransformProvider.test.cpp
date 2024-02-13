/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

#include <Global.test.hpp>

using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::physics::coordinate::Transform;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;

class OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider : public ::testing::Test
{
   protected:
    const LocalOrbitalFrameTransformProvider::Type type_ = LocalOrbitalFrameTransformProvider::Type::VNC;

    const Instant instant_ = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);
    const Vector3d position_ = {7000000.0, 0.0, 0.0};
    const Vector3d velocity_ = {0.0, 5335.865450622126, 5335.865450622126};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider, Construct)
{
    {
        EXPECT_NO_THROW(LocalOrbitalFrameTransformProvider::Construct(
            LocalOrbitalFrameTransformProvider::Type::VNC, instant_, position_, velocity_
        ));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameTransformProvider::Construct(
            LocalOrbitalFrameTransformProvider::Type::NED, instant_, position_, velocity_
        ));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameTransformProvider::Construct(
            LocalOrbitalFrameTransformProvider::Type::LVLH, instant_, position_, velocity_
        ));
    }

    {
        EXPECT_ANY_THROW(LocalOrbitalFrameTransformProvider::Construct(
            LocalOrbitalFrameTransformProvider::Type::LVLHGD, instant_, position_, velocity_
        ));  // ToBeImplemented
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameTransformProvider::Construct(
            LocalOrbitalFrameTransformProvider::Type::QSW, instant_, position_, velocity_
        ));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameTransformProvider::Construct(
            LocalOrbitalFrameTransformProvider::Type::TNW, instant_, position_, velocity_
        ));
    }

    {
        EXPECT_ANY_THROW(LocalOrbitalFrameTransformProvider::Construct(
            LocalOrbitalFrameTransformProvider::Type::Undefined, instant_, position_, velocity_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider, IsDefined)
{
    {
        Shared<const LocalOrbitalFrameTransformProvider> localOrbitalFrameTransformProvider =
            LocalOrbitalFrameTransformProvider::Construct(type_, instant_, position_, velocity_);

        EXPECT_TRUE(localOrbitalFrameTransformProvider->isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider, GetTransformAt)
{
    {
        Shared<const LocalOrbitalFrameTransformProvider> localOrbitalFrameTransformProvider =
            LocalOrbitalFrameTransformProvider::Construct(type_, instant_, position_, velocity_);

        Transform transform = localOrbitalFrameTransformProvider->getTransformAt(instant_);

        EXPECT_TRUE(transform.isDefined());

        EXPECT_ANY_THROW(localOrbitalFrameTransformProvider->getTransformAt(Instant::J2000()));
    }
}
