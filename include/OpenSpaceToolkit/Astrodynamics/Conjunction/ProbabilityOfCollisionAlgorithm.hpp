/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

/// @brief Probability of collision algorithm (abstract).
///
/// @details Base class for algorithms that compute the probability of collision for a close approach.
///
/// The recommended way to use an algorithm is as follows:
/// 1. Check if the algorithm is applicable to the close approach.
/// 2. If the algorithm is applicable, compute the probability of collision analysis,
/// otherwise fall back to another algorithm.
/// 3. If the nominal probability of collision is in the robust region, take it,
/// otherwise take the maximum probability of collision.
///
/// @code{.cpp}
///     if (algorithm.isApplicable(closeApproach))
///     {
///         const ProbabilityOfCollisionAlgorithm::Analysis analysis =
///             algorithm.computeProbabilityAnalysis(closeApproach, hardBody1SPtr, hardBody2SPtr);
///
///         if (analysis.region == ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust)
///         {
///             return analysis.probabilityOfCollision;
///         }
///         else
///         {
///             return analysis.maximumProbabilityOfCollision;
///         }
///     }
/// @endcode
class ProbabilityOfCollisionAlgorithm
{
   public:
    /// @brief Probability region of a close approach.
    enum class ProbabilityRegion
    {
        Robust,   ///< The probability of collision is in the robust region.
        Diluted,  ///< The probability of collision is in the diluted region.
        Maximum   ///< The probability of collision is at the maximum (upper bound).
    };

    /// @brief Probability of collision analysis of a close approach.
    struct Analysis
    {
        Real probabilityOfCollision;         ///< The probability of collision.
        ProbabilityRegion region;            ///< The probability region.
        Real maximumProbabilityOfCollision;  ///< The maximum probability of collision.
    };

    /// @brief Destructor
    virtual ~ProbabilityOfCollisionAlgorithm() = default;

    /// @brief Get the name of the probability of collision algorithm
    ///
    /// @return The name of the algorithm
    String getName() const;

    /// @brief Identify unsatisfied assumptions for a close approach
    ///
    /// @details Use this before computing the probability of collision if you want to know
    /// why the algorithm might not be applicable to a close approach.
    ///
    /// The returned array might be non-exhaustive, as the algorithm implementation might
    /// exit early upon encountering an unsatisfied assumption.
    ///
    /// An empty array means that the algorithm is applicable to the close approach.
    ///
    /// Use isApplicable() instead if you only want to know if the algorithm is applicable,
    /// but do not necessarily care about the reasons why.
    ///
    /// @code{.cpp}
    ///     Array<String> unsatisfiedAssumptions = algorithm.identifyUnsatisfiedAssumptions(closeApproach);
    ///     if (unsatisfiedAssumptions.isEmpty())
    ///     {
    ///         algorithm.computeProbabilityAnalysis(closeApproach, hardBody1SPtr, hardBody2SPtr);
    ///     }
    /// @endcode
    ///
    /// @code{.cpp}
    ///     Array<String> unsatisfiedAssumptions = algorithm.identifyUnsatisfiedAssumptions(closeApproach);
    ///     if (unsatisfiedAssumptions.isEmpty())
    ///     {
    ///         algorithm.computeProbabilityOfCollision(closeApproach, hardBody1SPtr, hardBody2SPtr);
    ///     }
    /// @endcode
    ///
    /// @param aCloseApproach A close approach
    /// @return A non-exhaustive array of unsatisfied assumption descriptions
    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const = 0;

    /// @brief Compute the probability of collision
    ///
    /// @param aCloseApproach A close approach
    /// @param aHardBody1SPtr The hard body of Object 1
    /// @param aHardBody2SPtr The hard body of Object 2
    ///
    /// @return The probability of collision
    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const = 0;

    /// @brief Check if the algorithm is applicable to a close approach
    ///
    /// @details Use this before computing the probability of collision if you want to know
    /// whether the algorithm is applicable, but do not necessarily care about the reasons why.
    ///
    /// @code{.cpp}
    ///     if (algorithm.isApplicable(closeApproach))
    ///     {
    ///         algorithm.computeProbabilityAnalysis(closeApproach, hardBody1SPtr, hardBody2SPtr);
    ///     }
    /// @endcode
    ///
    /// @code{.cpp}
    ///     if (algorithm.isApplicable(closeApproach))
    ///     {
    ///         algorithm.computeProbabilityOfCollision(closeApproach, hardBody1SPtr, hardBody2SPtr);
    ///     }
    /// @endcode
    ///
    /// @param aCloseApproach A close approach
    /// @return True if the algorithm is applicable to the close approach
    bool isApplicable(const CloseApproach& aCloseApproach) const;

    /// @brief Compute the probability of collision analysis of a close approach
    ///
    /// @details This function computes the probability of collision and locates the maximum probability of
    /// collision by sampling the probability of collision as it scales the covariance(s) over the scaling interval.
    ///
    /// - If the maximum probability of collision is found to the "right" of the nominal probability of collision
    /// (i.e. a scaling factor greater than or equal to 1), then the close approach is in the robust region
    /// (ProbabilityRegion::Robust).
    /// - If the maximum probability of collision is found to the "left" of the nominal probability of collision
    /// (i.e. a scaling factor less than 1), then the close approach is in the diluted region
    /// (ProbabilityRegion::Diluted).
    /// - If the probability of collision does not change over the scaling interval, then the close approach is in the
    /// maximum region (ProbabilityRegion::Maximum). This may be the case for some algorithms that work without any
    /// covariance information and that are not affected by the covariance scaling, residing always in the "maximum"
    /// region (ProbabilityRegion::Maximum).
    ///
    /// Raises a RuntimeError if the maximum probability of collision has not converged (i.e. the maximum probability of
    /// collision of two successive passes differ by more than the convergence threshold) within the maximum number of
    /// passes.
    ///
    /// @param aCloseApproach A close approach
    /// @param aHardBody1SPtr The hard body of Object 1
    /// @param aHardBody2SPtr The hard body of Object 2
    /// @param scaleObject1Covariance Whether to scale Object 1 covariance. Defaults to true
    /// @param scaleObject2Covariance Whether to scale Object 2 covariance. Defaults to true
    /// @param aSigmaScalingFactorLowerBound A lower bound for the scaling factor of the standard deviations (the
    /// covariances are scaled by its square). Defaults to 0.01 (i.e. as low as 0.01x the original standard deviations)
    /// @param aSigmaScalingFactorUpperBound An upper bound for the scaling factor of the standard deviations (the
    /// covariances are scaled by its square). Defaults to 100.0 (i.e. as high as 100x the original standard
    /// deviations)
    /// @param aMaximumPointPerPassCount The maximum number of sampled scaling factors per pass (already sampled
    /// scaling factors are not sampled again). Defaults to 50
    /// @param aMaximumPassCount The maximum number of passes. Defaults to 10
    /// @param aConvergenceThreshold The relative tolerance on the maximum probability of collision between two
    /// successive passes. Defaults to 0.01 (i.e. 1%)
    ///
    /// @return The probability of collision analysis of the close approach
    Analysis computeProbabilityAnalysis(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr,
        const bool& scaleObject1Covariance = true,
        const bool& scaleObject2Covariance = true,
        const Real& aSigmaScalingFactorLowerBound = 0.01,
        const Real& aSigmaScalingFactorUpperBound = 100.0,
        const Size& aMaximumPointPerPassCount = 50,
        const Size& aMaximumPassCount = 10,
        const Real& aConvergenceThreshold = 0.01
    ) const;

   protected:
    /// @brief Constructor
    ///
    /// @param aName The name of the probability of collision algorithm
    ProbabilityOfCollisionAlgorithm(const String& aName);

    String name_;
};

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
