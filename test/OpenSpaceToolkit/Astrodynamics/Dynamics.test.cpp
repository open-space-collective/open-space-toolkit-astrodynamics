/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Index;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::Environment;
using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::Dynamics;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

class DynamicsMock : public Dynamics
{
   public:
    DynamicsMock(const String& aName)
        : Dynamics(aName) {};

    MOCK_METHOD(bool, isDefined, (), (const, override));

    MOCK_METHOD(Array<Shared<const CoordinatesSubset>>, getReadCoordinatesSubsets, (), (const, override));

    MOCK_METHOD(Array<Shared<const CoordinatesSubset>>, getWriteCoordinatesSubsets, (), (const, override));

    MOCK_METHOD(
        VectorXd,
        computeContribution,
        (const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr),
        (const, override)
    );
};

class OpenSpaceToolkit_Astrodynamics_Dynamics : public ::testing::Test
{
   protected:
    const String defaultName_ = "Test";
    const DynamicsMock defaultDynamics_ = {defaultName_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultDynamics_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultDynamics_.print(std::cout, true));
        EXPECT_NO_THROW(defaultDynamics_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, Getters)
{
    {
        EXPECT_EQ(defaultName_, defaultDynamics_.getName());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics, FromEnvironment)
{
    {
        EXPECT_NO_THROW(Dynamics::FromEnvironment(Environment::Default()));
    }
}
