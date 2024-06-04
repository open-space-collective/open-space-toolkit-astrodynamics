/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Index;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::Environment;
using ostk::physics::time::Instant;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class DynamicsMock : public Dynamics
{
   public:
    DynamicsMock(const String& aName)
        : Dynamics(aName) {};

    MOCK_METHOD(bool, isDefined, (), (const, override));

    MOCK_METHOD(Array<Shared<const CoordinateSubset>>, getReadCoordinateSubsets, (), (const, override));

    MOCK_METHOD(Array<Shared<const CoordinateSubset>>, getWriteCoordinateSubsets, (), (const, override));

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
