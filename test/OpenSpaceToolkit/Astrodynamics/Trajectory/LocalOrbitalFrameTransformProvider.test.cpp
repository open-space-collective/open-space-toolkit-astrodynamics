/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

#include <Global.test.hpp>

using ostk::core::type::Shared;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Transform;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

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

    // custom transform
    {
        const auto aTransformGenerator = [](const Instant& anInstant,
                                            const Vector3d& aPositionCoordinates,
                                            const Vector3d& aVelocityCoordinates) -> Transform
        {
            const Vector3d transformPosition = -aPositionCoordinates;
            const Vector3d transformVelocity = -aVelocityCoordinates;
            const Vector3d xAxis = aVelocityCoordinates.normalized();
            const Vector3d yAxis = aPositionCoordinates.cross(aVelocityCoordinates).normalized();
            const Vector3d zAxis = xAxis.cross(yAxis);
            const Quaternion transformOrientation =
                Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();
            const Vector3d transformAngularVelocity = {0.0, 0.0, 0.0};

            return {
                anInstant,
                transformPosition,
                transformVelocity,
                transformOrientation,
                transformAngularVelocity,
                Transform::Type::Passive
            };
        };

        const Shared<const LocalOrbitalFrameTransformProvider> localOrbitalFrameTransformProvider =
            LocalOrbitalFrameTransformProvider::Construct(
                LocalOrbitalFrameTransformProvider::Type::VNC, instant_, position_, velocity_
            );

        const Shared<const LocalOrbitalFrameTransformProvider> localOrbitalFrameTransformProviderCustom =
            LocalOrbitalFrameTransformProvider::Construct(instant_, position_, velocity_, aTransformGenerator);

        EXPECT_EQ(
            localOrbitalFrameTransformProvider->getTransformAt(instant_),
            localOrbitalFrameTransformProviderCustom->getTransformAt(instant_)
        );
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
