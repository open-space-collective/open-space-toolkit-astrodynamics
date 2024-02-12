/// Apache License 2.0

#include <boost/math/tools/roots.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>

namespace ostk
{
namespace astro
{

RootSolver::RootSolver(const Size& aMaximumIterationCount, const Real& aTolerance)
    : maximumIterationCount_(aMaximumIterationCount),
      tolerance_(aTolerance)
{
}

RootSolver::~RootSolver() {}

std::ostream& operator<<(std::ostream& anOutputStream, const RootSolver& aRootSolver)
{
    aRootSolver.print(anOutputStream, false);

    return anOutputStream;
}

Size RootSolver::getMaximumIterationCount() const
{
    return maximumIterationCount_;
}

Real RootSolver::getTolerance() const
{
    return tolerance_;
}

RootSolver::Solution RootSolver::bracketAndSolve(
    const std::function<double(const double&)>& aFunction,
    const double& anInitialGuess,
    const bool& isRising,
    const double& aFactor
) const
{
    std::uintmax_t iteratorCount = maximumIterationCount_;

    std::pair<Real, Real> r = boost::math::tools::bracket_and_solve_root(
        aFunction, anInitialGuess, aFactor, isRising, getToleranceFunction(), iteratorCount
    );

    return {
        r.first + (r.second - r.first) / 2.0,
        (Size)iteratorCount,
        (Size)iteratorCount < maximumIterationCount_,
    };
}

RootSolver::Solution RootSolver::solve(
    const std::function<double(const double&)>& aFunction, const double& aLowerBound, const double& anUpperBound
) const
{
    // account for the fact that the function may be decreasing
    const double lowerBound = std::min(aLowerBound, anUpperBound);
    const double upperBound = std::max(aLowerBound, anUpperBound);

    std::uintmax_t iteratorCount = maximumIterationCount_;

    std::pair<Real, Real> r =
        boost::math::tools::toms748_solve(aFunction, lowerBound, upperBound, getToleranceFunction(), iteratorCount);

    return {
        r.first + (r.second - r.first) / 2.0,
        (Size)iteratorCount,
        (Size)iteratorCount < maximumIterationCount_,
    };
}

RootSolver::Solution RootSolver::bisection(
    const std::function<double(const double&)>& aFunction, const double& aLowerBound, const double& anUpperBound
) const
{
    // account for the fact that the function may be decreasing
    const double lowerBound = std::min(aLowerBound, anUpperBound);
    const double upperBound = std::max(aLowerBound, anUpperBound);

    std::uintmax_t iteratorCount = maximumIterationCount_;

    std::pair<Real, Real> r =
        boost::math::tools::bisect(aFunction, lowerBound, upperBound, getToleranceFunction(), iteratorCount);

    return {
        r.first + (r.second - r.first) / 2.0,
        (Size)iteratorCount,
        (Size)iteratorCount < maximumIterationCount_,
    };
}

void RootSolver::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Root Solver") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Maximum Iterations Count: " << maximumIterationCount_;
    ostk::core::utils::Print::Line(anOutputStream) << "Tolerance: " << tolerance_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

RootSolver RootSolver::Default()
{
    return {100u, 1e-8};
}

std::function<bool(const double&, const double&)> RootSolver::getToleranceFunction() const
{
    return [this](const double& aLowerBound, const double& anUpperBound) -> bool
    {
        return std::fabs(aLowerBound - anUpperBound) <= tolerance_;
    };
}

}  // namespace astro
}  // namespace ostk
