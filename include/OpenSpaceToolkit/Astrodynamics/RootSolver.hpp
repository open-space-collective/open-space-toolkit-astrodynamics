/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_RootSolver__
#define __OpenSpaceToolkit_Astrodynamics_RootSolver__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::types::Size;
using ostk::core::types::Real;

/// @brief     A Root Solver is a method for finding the roots (a.k.a. zeros) of an equation.
class RootSolver
{
   public:
    struct Solution
    {
        Real root;
        Size iterationCount;
        bool hasConverged;
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  RootSolver rootSolver = {aMaximumIterationCount, aTolerance};
    /// @endcode
    ///
    /// @param aMaximumIterationCount The maximum iteration count for the solver
    /// @param aTolerance The tolerance
    RootSolver(const Size& aMaximumIterationCount, const Real& aTolerance);

    /// @brief Virtual destructor
    virtual ~RootSolver();

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aRootSolver A Root Solver
    /// @return An output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const RootSolver& aRootSolver);

    /// @brief Get maximum iterations count
    ///
    /// @return Maximum iterations count
    Size getMaximumIterationCount() const;

    /// @brief Get number of digits of precision
    ///
    /// @return Number of digits of precision
    Real getTolerance() const;

    /// @brief Solve for root given a function, an initial guess and function direction
    ///
    /// @param aFunction A function
    /// @param anInitialGuess An initial guess
    /// @param isRising A boolean indicating whether the function is rising
    /// @param (optional) aFactor A factor
    ///
    /// @return The solution
    Solution bracketAndSolve(
        const std::function<double(const double&)>& aFunction,
        const double& anInitialGuess,
        const bool& isRising,
        const double& aFactor = 2.0
    ) const;

    /// @brief Solve for root given a function, and bounds
    ///
    /// @param aFunction A function
    /// @param aLowerBound A lower bound
    /// @param anUpperBound An upper bound
    ///
    /// @return The solution
    Solution solve(
        const std::function<double(const double&)>& aFunction, const double& aLowerBound, const double& anUpperBound
    ) const;

    /// @brief Bisection solve for root given a function, and bounds
    ///
    /// @param aFunction A function
    /// @param aLowerBound A lower bound
    /// @param anUpperBound An upper bound
    ///
    /// @return The solution
    Solution bisection(
        const std::function<double(const double&)>& aFunction, const double& aLowerBound, const double& anUpperBound
    ) const;

    /// @brief Print root solver
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Default root solver
    ///
    /// @return Default root solver
    static RootSolver Default();

   private:
    Size maximumIterationCount_;
    Real tolerance_;

    std::function<bool(const double&, const double&)> getToleranceFunction() const;
};

}  // namespace astro
}  // namespace ostk

#endif
