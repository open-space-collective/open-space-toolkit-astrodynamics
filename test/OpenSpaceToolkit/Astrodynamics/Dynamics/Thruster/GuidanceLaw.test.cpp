/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster/GuidanceLaw.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::math::obj::Vector3d;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::dynamics::thruster::GuidanceLaw;

class MockGuidanceLaw : public GuidanceLaw
{
   public:
    MockGuidanceLaw(const String& aName)
        : GuidanceLaw(aName)
    {
    }

    MOCK_METHOD(
        Vector3d,
        computeAcceleration,
        (const Instant&, const Vector3d&, const Vector3d&, const Real&, const Shared<const Frame>&),
        (const, override)
    );
};

class OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw : public ::testing::Test
{
   protected:
    const String defaultName_ = "Test Guidance Law";
    const MockGuidanceLaw defaultGuidanceLaw_ {defaultName_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw, Constructor)
{
    EXPECT_NO_THROW(MockGuidanceLaw mockGuidanceLaw(defaultName_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << defaultGuidanceLaw_ << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultGuidanceLaw_.print(std::cout, true));
    EXPECT_NO_THROW(defaultGuidanceLaw_.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw, GetName)
{
    EXPECT_EQ(defaultGuidanceLaw_.getName(), defaultName_);
}
