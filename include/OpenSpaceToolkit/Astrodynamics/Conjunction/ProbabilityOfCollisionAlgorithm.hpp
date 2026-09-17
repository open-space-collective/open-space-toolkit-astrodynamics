/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::physics::unit::Length;

using ostk::astrodynamics::RootSolver;

/// @brief Probability of collision algorithm (abstract).
///
/// @details Base class for algorithms that compute the probability of collision for a close approach.
class ProbabilityOfCollisionAlgorithm
{
   public:
    /// @brief Probability region of a close approach.
    enum class ProbabilityRegion
    {
        Robust,   ///< The close approach is in the robust region.
        Diluted,  ///< The close approach is in the diluted region.
        Unknown   ///< The probability region is unknown.
    };

    /// @brief Destructor
    virtual ~ProbabilityOfCollisionAlgorithm() = default;

    /// @brief Get the name of the probability of collision algorithm
    ///
    /// @return The name of the algorithm
    virtual String getName() const = 0;

    /// @brief Check if the algorithm computes a maximum probability of collision
    ///
    /// @return True if the algorithm computes a maximum probability of collision
    virtual bool isMaximumProbabilityOfCollisionAlgorithm() const = 0;

    /// @brief Check if the algorithm is applicable to a close approach
    ///
    /// @details This method should be called before computing the probability of collision
    /// to check whether the close approach falls under all of the assumptions that the
    /// algorithm makes (e.g. high-velocity encounter, orbital geometry, etc.).
    ///
    /// @param aCloseApproach A close approach
    /// @return True if the algorithm is applicable to the close approach
    virtual bool isApplicable(const CloseApproach& aCloseApproach) const = 0;

    /// @brief Compute the probability region of a close approach
    ///
    /// @details This function may return ProbabilityRegion::Unknown under certain conditions:
    /// - The algorithm is a maximum probability of collision algorithm
    /// - The algorithm is not applicable to the close approach
    /// - The root solver did not converge
    ///
    /// @param aCloseApproach A close approach
    /// @param aCombinedHardBodyRadius A combined hard-body radius
    /// @param aScalingFactorLowerBound A lower bound for the covariance scaling factor. Defaults to 0.1
    /// @param aScalingFactorUpperBound An upper bound for the covariance scaling factor. Defaults to 10.0
    /// @param aRootSolver A root solver. Defaults to a Root Solver with 100 iterations and a tolerance of 0.1
    /// @param scaleObject1Covariance Whether to scale Object 1 covariance. Defaults to true
    /// @param scaleObject2Covariance Whether to scale Object 2 covariance. Defaults to true
    ///
    /// @return The probability region of the close approach
    virtual ProbabilityRegion computeProbabilityRegion(
        const CloseApproach& aCloseApproach,
        const Length& aCombinedHardBodyRadius,
        const Real& aScalingFactorLowerBound = 0.1,
        const Real& aScalingFactorUpperBound = 10.0,
        const RootSolver& aRootSolver = RootSolver(100, 0.1),
        const bool& scaleObject1Covariance = true,
        const bool& scaleObject2Covariance = true
    ) const;

    /// @brief Compute the probability of collision
    ///
    /// @param aCloseApproach A close approach
    /// @param aCombinedHardBodyRadius A combined hard-body radius
    ///
    /// @return The probability of collision
    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach, const Length& aCombinedHardBodyRadius
    ) const = 0;

   private:
    /// @brief Find the covariance scaling factor at which the probability of collision slope is zero
    ///
    /// @details Uses a Root Solver to find the zero of the central-difference slope of the probability of
    /// collision. For a scaling factor `k` in `[lower, upper]`, the objective is
    /// `computeProbabilityOfCollisionSlope(k, aSlopeStepSize)`.
    ///
    /// @param aCloseApproach A close approach
    /// @param aCombinedHardBodyRadius A combined hard-body radius
    /// @param aScalingFactorLowerBound A lower bound for the covariance scaling factor
    /// @param aScalingFactorUpperBound An upper bound for the covariance scaling factor
    /// @param aRootSolver A root solver
    /// @param scaleObject1Covariance Whether to scale Object 1 covariance
    /// @param scaleObject2Covariance Whether to scale Object 2 covariance
    /// @param aSlopeStepSize A step around the scaling factor
    ///
    /// @return The root solver solution
    RootSolver::Solution findProbabilityOfCollisionExtremum(
        const CloseApproach& aCloseApproach,
        const Length& aCombinedHardBodyRadius,
        const Real& aScalingFactorLowerBound,
        const Real& aScalingFactorUpperBound,
        const RootSolver& aRootSolver,
        const bool& scaleObject1Covariance,
        const bool& scaleObject2Covariance,
        const Real& aSlopeStepSize
    ) const;

    /// @brief Compute the slope of the probability of collision with respect to the covariance scaling factor
    ///
    /// @details Uses the central difference formula:
    /// `(Pc(scalingFactor + step) - Pc(scalingFactor - step)) / (2 * step)`.
    ///
    /// @param aCloseApproach A close approach
    /// @param aCombinedHardBodyRadius A combined hard-body radius
    /// @param aScalingFactor A covariance scaling factor
    /// @param aSlopeStepSize A step around the scaling factor
    ///
    /// @return The probability of collision slope
    Real computeProbabilityOfCollisionSlope(
        const CloseApproach& aCloseApproach,
        const Length& aCombinedHardBodyRadius,
        const Real& aScalingFactor,
        const Real& aSlopeStepSize
    ) const;
};

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
