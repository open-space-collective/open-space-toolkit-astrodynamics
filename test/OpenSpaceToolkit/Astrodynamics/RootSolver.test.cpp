/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::Size;

using ostk::astrodynamics::RootSolver;

class OpenSpaceToolkit_Astrodynamics_RootSolver : public ::testing::Test
{
   protected:
    const Size defaultMaxIterations_ = 100u;
    const Real defaultTolerance_ = 1e-12;
    const RootSolver defaultRootSolver_ = RootSolver(defaultMaxIterations_, defaultTolerance_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, Constructor)
{
    EXPECT_NO_THROW(RootSolver rootSolver(defaultMaxIterations_, defaultTolerance_));
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
    EXPECT_EQ(defaultMaxIterations_, defaultRootSolver_.getMaximumIterationCount());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, GetTolerance)
{
    EXPECT_EQ(defaultTolerance_, defaultRootSolver_.getTolerance());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, BracketAndSolve)
{
    // Define a simple quadratic function
    auto func = [](const double& x)
    {
        return (x * x) - 4;
    };

    EXPECT_NO_THROW(defaultRootSolver_.bracketAndSolve(func, 1.0, true));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, Solve)
{
    // Define a simple quadratic function
    auto func = [](const double& x)
    {
        return (x * x) - 4;
    };

    EXPECT_NO_THROW(defaultRootSolver_.solve(func, 1.0, 5.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_RootSolver, Bisection)
{
    // Define a simple quadratic function
    auto func = [](const double& x)
    {
        return (x * x) - 4;
    };

    EXPECT_NO_THROW(defaultRootSolver_.bisection(func, 1.0, 5.0));
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
