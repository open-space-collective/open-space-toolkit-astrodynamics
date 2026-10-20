/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm__

#include <optional>

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
/// 3. If the nominal probability of collision is in the robust region, take it, if it is in the diluted region, take
/// the maximum probability of collision, otherwise (unknown region) the maximum probability of collision could not be
/// located.
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
///
///         if (analysis.region == ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted)
///         {
///             return analysis.maximumProbabilityOfCollision.value();
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
        Unknown   ///< The maximum probability of collision could not be located.
    };

    /// @brief Probability of collision analysis of a close approach.
    struct Analysis
    {
        Real probabilityOfCollision;                        ///< The probability of collision.
        ProbabilityRegion region;                           ///< The probability region.
        std::optional<Real> maximumProbabilityOfCollision;  ///< The maximum probability of collision (empty in the
                                                            ///< unknown region).
        std::optional<Real> maximumProbabilityOfCollisionScalingFactor;  ///< The covariance scaling factor yielding the
                                                                         ///< maximum probability of collision (empty
                                                                         ///< in the unknown region).
        std::optional<Real> maximumProbabilityOfCollisionSigmaScalingFactor;  ///< The sigma scaling factor (i.e. the
                                                                              ///< square root of the covariance
                                                                              ///< scaling factor) yielding the maximum
                                                                              ///< probability of collision (empty in
                                                                              ///< the unknown region).
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
    /// collision as it scales the covariance(s): starting from the unscaled covariance(s), a root solver (bracket and
    /// solve) locates where the slope of the probability of collision with respect to the (logarithmic) scaling factor
    /// changes sign. The probability of collision is assumed to have a single maximum.
    ///
    /// - If the maximum probability of collision is found to the "right" of the nominal probability of collision
    /// (i.e. a scaling factor greater than or equal to 1), then the close approach is in the robust region
    /// (ProbabilityRegion::Robust).
    /// - If the maximum probability of collision is found to the "left" of the nominal probability of collision
    /// (i.e. a scaling factor less than 1), then the close approach is in the diluted region
    /// (ProbabilityRegion::Diluted).
    /// - If the root solver does not converge (e.g. the probability of collision does not change with the covariance
    /// scaling, or has no maximum), then the region is unknown (ProbabilityRegion::Unknown).
    ///
    /// The scaling factors yielding the maximum probability of collision are also returned, both as a covariance
    /// scaling factor and as a sigma scaling factor. In the unknown region (ProbabilityRegion::Unknown), the maximum
    /// probability of collision and its scaling factors are empty.
    ///
    /// Raises a RuntimeError if neither covariance is to be scaled.
    ///
    /// @ref Alfano, Salvatore. (2020). Relating Position Uncertainty to Maximum Conjunction Probability.
    /// The Journal of the Astronautical Sciences. 53. 193-205. 10.1007/BF03546350.
    ///
    /// @param aCloseApproach A close approach
    /// @param aHardBody1SPtr The hard body of Object 1
    /// @param aHardBody2SPtr The hard body of Object 2
    /// @param scaleObject1Covariance Whether to scale Object 1 covariance. Defaults to true
    /// @param scaleObject2Covariance Whether to scale Object 2 covariance. Defaults to true
    ///
    /// @return The probability of collision analysis of the close approach
    Analysis computeProbabilityAnalysis(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr,
        const bool& scaleObject1Covariance = true,
        const bool& scaleObject2Covariance = true
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
