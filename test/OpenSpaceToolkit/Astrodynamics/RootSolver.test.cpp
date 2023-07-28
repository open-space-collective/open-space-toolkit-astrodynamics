/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>

#include <Global.test.hpp>

using ostk::core::types::Size;

using ostk::astro::RootSolver;

class OpenSpaceToolkit_Astrodynamics_RootSolver : public ::testing::Test
{
   protected:
    const Size defaultMaxIterations_ = 100u;
    const Size defaultNumDigits_ = (std::numeric_limits<double>::digits / 2) + 1;
    const RootSolver defaultRootSolver_ = RootSolver(defaultMaxIterations_, defaultNumDigits_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, Constructor)
{
    EXPECT_NO_THROW(RootSolver rootSolver(defaultMaxIterations_, defaultNumDigits_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultRootSolver_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, GetMaximumNumberOfIterations)
{
    EXPECT_EQ(defaultMaxIterations_, defaultRootSolver_.getMaximumIterationsCount());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, GetNumberOfDigits)
{
    EXPECT_EQ(defaultNumDigits_, defaultRootSolver_.getNumberOfDigits());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, Solve_WithFunctionAndInitialGuess)
{
    // Define a simple quadratic function
    auto func = [](const double& x)
    {
        return (x * x) - 4;
    };

    EXPECT_NO_THROW(defaultRootSolver_.solve(func, 1.0, true));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, Solve_WithFunctionAndBounds)
{
    // Define a simple quadratic function
    auto func = [](const double& x)
    {
        return (x * x) - 4;
    };

    EXPECT_NO_THROW(defaultRootSolver_.solve(func, 1.0, 5.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultRootSolver_.print(std::cout, true));
        EXPECT_NO_THROW(defaultRootSolver_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, Default)
{
    EXPECT_NO_THROW(RootSolver::Default());
}
