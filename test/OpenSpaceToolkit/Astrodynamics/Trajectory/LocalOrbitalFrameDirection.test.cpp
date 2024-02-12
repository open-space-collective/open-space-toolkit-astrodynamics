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

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

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

using ostk::astro::trajectory::LocalOrbitalFrameDirection;
using ostk::astro::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;

class OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection : public ::testing::Test
{
   protected:
    const LocalOrbitalFrameTransformProvider::Type type_ = LocalOrbitalFrameTransformProvider::Type::VNC;
    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();
    Shared<const LocalOrbitalFrameFactory> LOFFactorySPtr_ = LocalOrbitalFrameFactory::VNC(gcrfSPtr_);

    const Vector3d direction_ = {1.0, 0.0, 0.0};
    const Vector3d anotherDirection_ = {1.0, 1.0, 0.0};  // not normalized

    const LocalOrbitalFrameDirection localOrbitalFrameDirection_ =
        LocalOrbitalFrameDirection(direction_, LOFFactorySPtr_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection, Constructor)
{
    {
        EXPECT_NO_THROW(LocalOrbitalFrameDirection(direction_, LOFFactorySPtr_));
    }

    {
        EXPECT_NO_THROW(LocalOrbitalFrameDirection(anotherDirection_, LOFFactorySPtr_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection, EqualToOperator)
{
    {
        EXPECT_TRUE(localOrbitalFrameDirection_ == localOrbitalFrameDirection_);
    }

    {
        EXPECT_FALSE(localOrbitalFrameDirection_ == LocalOrbitalFrameDirection(anotherDirection_, LOFFactorySPtr_));
    }

    {
        EXPECT_FALSE(LocalOrbitalFrameDirection::Undefined() == localOrbitalFrameDirection_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection, NotEqualToOperator)
{
    {
        EXPECT_FALSE(localOrbitalFrameDirection_ != localOrbitalFrameDirection_);
    }

    {
        EXPECT_TRUE(localOrbitalFrameDirection_ != LocalOrbitalFrameDirection(anotherDirection_, LOFFactorySPtr_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << localOrbitalFrameDirection_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection, isDefined)
{
    {
        EXPECT_TRUE(localOrbitalFrameDirection_.isDefined());
    }

    {
        EXPECT_FALSE(LocalOrbitalFrameDirection(anotherDirection_, nullptr).isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection, AccessLocalOrbitalFrameFactory)
{
    {
        EXPECT_EQ(LOFFactorySPtr_, localOrbitalFrameDirection_.accessLocalOrbitalFrameFactory());
    }

    {
        EXPECT_ANY_THROW(LocalOrbitalFrameDirection(anotherDirection_, nullptr).accessLocalOrbitalFrameFactory());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection, Getters)
{
    {
        EXPECT_EQ(direction_, localOrbitalFrameDirection_.getValue());
        EXPECT_EQ(LOFFactorySPtr_, localOrbitalFrameDirection_.getLocalOrbitalFrameFactory());
    }

    {
        LocalOrbitalFrameDirection localOrbitalFrameDirection =
            LocalOrbitalFrameDirection(anotherDirection_, LOFFactorySPtr_);

        EXPECT_EQ(anotherDirection_.normalized(), localOrbitalFrameDirection.getValue());
        EXPECT_EQ(LOFFactorySPtr_, localOrbitalFrameDirection.getLocalOrbitalFrameFactory());
    }

    {
        EXPECT_ANY_THROW(LocalOrbitalFrameDirection(anotherDirection_, nullptr).getValue());
        EXPECT_ANY_THROW(LocalOrbitalFrameDirection(anotherDirection_, nullptr).getLocalOrbitalFrameFactory());
    }

    {
        EXPECT_ANY_THROW(LocalOrbitalFrameDirection::Undefined().getValue());
        EXPECT_ANY_THROW(LocalOrbitalFrameDirection::Undefined().getLocalOrbitalFrameFactory());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection, Undefined)
{
    {
        EXPECT_NO_THROW(LocalOrbitalFrameDirection::Undefined());
    }
}
