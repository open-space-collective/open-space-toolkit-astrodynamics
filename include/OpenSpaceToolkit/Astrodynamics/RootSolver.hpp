/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_RootSolver__
#define __OpenSpaceToolkit_Astrodynamics_RootSolver__

#include <boost/math/tools/roots.hpp>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::types::Size;
using ostk::core::types::Real;

/// @brief                      A Root Solver is a tool for finding the roots of equations.
class RootSolver
{
   public:
    struct Solution
    {
        Real root;
        Size numberOfIterations;
    };

    /// @brief              Constructor
    ///
    /// @code
    ///                     RootSolver rootSolver = {aMaximumNumberOfIterations};
    /// @endcode
    ///
    /// @param              [in] aMaximumNumberOfIterations The maximum number of iterations for the solver
    /// @param              [in] aNumberOfDigits The number of digits

    RootSolver(const Size& aMaximumNumberOfIterations, const Size& aNumberOfDigits);

    /// @brief              Virtual destructor

    virtual ~RootSolver();

    /// @brief              Get maximum number of iterations
    ///
    /// @return             Maximum number of iterations

    Size getMaximumNumberOfIterations() const;

    /// @brief              Get number of digits of precision
    ///
    /// @return             Number of digits of precision

    Size getNumberOfDigits() const;

    /// @brief              Solve for root given a function, an initial guess and function direction
    ///
    /// @param              [in] aFunction A function
    /// @param              [in] anInitialGuess An initial guess
    /// @param              [in] isRising A boolean indicating whether the function is rising
    ///
    /// @return             The solution

    Solution solve(
        const std::function<double(const double&)>& aFunction, const double& anInitialGuess, const bool& isRising
    ) const;

    /// @brief              Solve for root given a function, and bounds
    ///
    /// @param              [in] aFunction A function
    /// @param              [in] aLowerBound A lower bound
    /// @param              [in] anUpperBound An upper bound
    ///
    /// @return             The solution

    Solution solve(
        const std::function<double(const double&)>& aFunction, const double& aLowerBound, const double& anUpperBound
    ) const;

    /// @brief              Default root solver
    ///
    /// @return             Default root solver

    static RootSolver Default();

   private:
    Size maximumNumberOfIterations_;
    Size numberOfDigits_;

    boost::math::tools::eps_tolerance<double> tolerance_;

    static constexpr double factor_ = 2.0;
};

}  // namespace astro
}  // namespace ostk

#endif
