/// Apache License 2.0

#include <boost/math/tools/roots.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>

namespace ostk
{
namespace astro
{

RootSolver::RootSolver(const Size& aMaximumIterationCount, const Size& aDigitCount)
    : maximumIterationCount_(aMaximumIterationCount),
      digitCount_(aDigitCount)
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

Size RootSolver::getDigitCount() const
{
    return digitCount_;
}

RootSolver::Solution RootSolver::solve(
    const std::function<double(const double&)>& aFunction, const double& anInitialGuess, const bool& isRising
) const
{
    const boost::math::tools::eps_tolerance<double> tolerance(digitCount_);
    std::uintmax_t iteratorCount = maximumIterationCount_;

    std::pair<Real, Real> r = boost::math::tools::bracket_and_solve_root(
        aFunction, anInitialGuess, factor_, isRising, tolerance, iteratorCount
    );

    return {
        r.first + (r.second - r.first) / 2.0,
        (Size)iteratorCount,
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
    const boost::math::tools::eps_tolerance<double> tolerance(digitCount_);

    std::pair<Real, Real> r =
        boost::math::tools::toms748_solve(aFunction, lowerBound, upperBound, tolerance, iteratorCount);

    return {
        r.first + (r.second - r.first) / 2.0,
        (Size)iteratorCount,
    };
}

void RootSolver::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Root Solver") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Maximum Iterations Count:" << maximumIterationCount_;
    ostk::core::utils::Print::Line(anOutputStream) << "Number of Digits:" << digitCount_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

RootSolver RootSolver::Default()
{
    // recommended tolerance for double precision calculations from boost documentation
    return {100u, (std::numeric_limits<double>::digits / 2) + 1};
}

}  // namespace astro
}  // namespace ostk
