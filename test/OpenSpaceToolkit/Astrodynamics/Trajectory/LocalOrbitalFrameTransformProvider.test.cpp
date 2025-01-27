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

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider : public ::testing::Test
{
   protected:
    const LocalOrbitalFrameTransformProvider::Type type_ = LocalOrbitalFrameTransformProvider::Type::VNC;

    const Instant instant_ = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);
    const Vector3d position_ = {7000000.0, 0.0, 0.0};
    const Vector3d velocity_ = {0.0, 5335.865450622126, 5335.865450622126};

    const State state_ = {
        instant_,
        Position::Meters(position_, Frame::GCRF()),
        Velocity::MetersPerSecond(velocity_, Frame::GCRF()),
    };

    const Shared<const LocalOrbitalFrameTransformProvider> localOrbitalFrameTransformProvider_ =
        LocalOrbitalFrameTransformProvider::Construct(type_, state_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider, Constructer)
{
    {
        EXPECT_NO_THROW(LocalOrbitalFrameTransformProvider(Transform::Identity(instant_)));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider, Construct)
{
    {
        {
            EXPECT_NO_THROW(
                LocalOrbitalFrameTransformProvider::Construct(LocalOrbitalFrameTransformProvider::Type::VNC, state_)
            );
        }

        {
            EXPECT_NO_THROW(
                LocalOrbitalFrameTransformProvider::Construct(LocalOrbitalFrameTransformProvider::Type::NED, state_)
            );
        }

        {
            EXPECT_NO_THROW(
                LocalOrbitalFrameTransformProvider::Construct(LocalOrbitalFrameTransformProvider::Type::LVLH, state_)
            );
        }

        {
            EXPECT_ANY_THROW(
                LocalOrbitalFrameTransformProvider::Construct(LocalOrbitalFrameTransformProvider::Type::LVLHGD, state_)
            );  // ToBeImplemented
        }

        {
            EXPECT_NO_THROW(
                LocalOrbitalFrameTransformProvider::Construct(LocalOrbitalFrameTransformProvider::Type::QSW, state_)
            );
        }

        {
            EXPECT_NO_THROW(
                LocalOrbitalFrameTransformProvider::Construct(LocalOrbitalFrameTransformProvider::Type::TNW, state_)
            );
        }

        {
            EXPECT_ANY_THROW(
                LocalOrbitalFrameTransformProvider::Construct(
                    LocalOrbitalFrameTransformProvider::Type::Undefined, state_
                )
            );
        }

        {
            EXPECT_THROW(
                LocalOrbitalFrameTransformProvider::Construct(LocalOrbitalFrameTransformProvider::Type::Custom, state_),
                ostk::core::error::RuntimeError
            );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider, IsDefined)
{
    {
        EXPECT_TRUE(localOrbitalFrameTransformProvider_->isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider, GetTransformAt)
{
    {
        Transform transform = localOrbitalFrameTransformProvider_->getTransformAt(instant_);

        EXPECT_TRUE(transform.isDefined());

        EXPECT_ANY_THROW(localOrbitalFrameTransformProvider_->getTransformAt(Instant::J2000()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameTransformProvider, GetTransformGenerator)
{
    {
        {
            const auto transformGenerator = LocalOrbitalFrameTransformProvider::GetTransformGenerator(
                LocalOrbitalFrameTransformProvider::Type::VNC
            );

            const Transform transform = transformGenerator(state_);

            EXPECT_TRUE(transform.isDefined());
        }

        {
            const auto transformGenerator = LocalOrbitalFrameTransformProvider::GetTransformGenerator(
                LocalOrbitalFrameTransformProvider::Type::NED
            );

            const Transform transform = transformGenerator(state_);

            EXPECT_TRUE(transform.isDefined());
        }

        {
            const auto transformGenerator = LocalOrbitalFrameTransformProvider::GetTransformGenerator(
                LocalOrbitalFrameTransformProvider::Type::LVLH
            );

            const Transform transform = transformGenerator(state_);

            EXPECT_TRUE(transform.isDefined());
        }

        {
            const auto transformGenerator = LocalOrbitalFrameTransformProvider::GetTransformGenerator(
                LocalOrbitalFrameTransformProvider::Type::QSW
            );

            const Transform transform = transformGenerator(state_);

            EXPECT_TRUE(transform.isDefined());
        }

        {
            const auto transformGenerator = LocalOrbitalFrameTransformProvider::GetTransformGenerator(
                LocalOrbitalFrameTransformProvider::Type::TNW
            );

            const Transform transform = transformGenerator(state_);

            EXPECT_TRUE(transform.isDefined());
        }
    }
}
