/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::String;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::time::Instant;
using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::GuidanceLaw;

class MockGuidanceLaw : public GuidanceLaw
{
   public:
    MockGuidanceLaw(const String& aName)
        : GuidanceLaw(aName)
    {
    }

    MOCK_METHOD(
        Vector3d,
        calculateThrustAccelerationAt,
        (const Instant&, const Vector3d&, const Vector3d&, const Real&, const Shared<const Frame>&),
        (const, override)
    );
};

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw : public ::testing::Test
{
   protected:
    const String defaultName_ = "Test Guidance Law";
    const MockGuidanceLaw defaultGuidanceLaw_ {defaultName_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw, Constructor)
{
    EXPECT_NO_THROW(MockGuidanceLaw mockGuidanceLaw(defaultName_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << defaultGuidanceLaw_ << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultGuidanceLaw_.print(std::cout, true));
    EXPECT_NO_THROW(defaultGuidanceLaw_.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw, GetName)
{
    EXPECT_EQ(defaultGuidanceLaw_.getName(), defaultName_);
}
