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

/// @brief                      A Root Solver is a method for finding the roots (a.k.a. zeros) of an equation.
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
    ///                     RootSolver rootSolver = {aMaximumIterationsCount, aNumberOfDigits};
    /// @endcode
    ///
    /// @param              [in] aMaximumIterationsCount The maximum iterations count for the solver
    /// @param              [in] aNumberOfDigits The number of digits

    RootSolver(const Size& aMaximumIterationsCount, const Size& aNumberOfDigits);

    /// @brief              Virtual destructor

    virtual ~RootSolver();

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aRootSolver A Root Solver
    /// @return             An output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const RootSolver& aRootSolver);

    /// @brief              Get maximum iterations count
    ///
    /// @return             Maximum iterations count

    Size getMaximumIterationsCount() const;

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

    /// @brief              Print root solver
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief              Default root solver
    ///
    /// @return             Default root solver

    static RootSolver Default();

   private:
    Size maximumIterationsCount_;
    Size numberOfDigits_;

    boost::math::tools::eps_tolerance<double> tolerance_;

    static constexpr double factor_ = 2.0;
};

}  // namespace astro
}  // namespace ostk

#endif
