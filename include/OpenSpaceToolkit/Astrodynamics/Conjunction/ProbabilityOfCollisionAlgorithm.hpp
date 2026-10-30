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
/// The recommended way to use the algorithms is as follows:
/// 1. List the algorithms to use, in order of preference.
/// 2. Loop over the list and pick the first algorithm applicable to the close approach (see isApplicable()), then
/// check whether it computes a maximum probability of collision (see isMaximum()).
/// 3. If the algorithm does not compute a maximum probability of collision, compute the probability of collision
/// analysis (see ComputeProbabilityAnalysis) and check its region:
///     - ProbabilityRegion::Robust: use the probability of collision.
///     - ProbabilityRegion::Diluted: use the maximum probability of collision as an upper bound.
///     - ProbabilityRegion::Unknown: try the next applicable algorithm in the list.
/// 4. If the algorithm computes a maximum probability of collision, compute the probability of collision (see
/// computeProbabilityOfCollision()) and use it as an upper bound.
///
/// @code{.cpp}
///     const Array<Shared<const ProbabilityOfCollisionAlgorithm>> algorithms = {
///         std::make_shared<Alfano2005>(),
///         std::make_shared<Frisbee2015>(),
///     };
///
///     for (const auto& algorithmSPtr : algorithms)
///     {
///         if (!algorithmSPtr->isApplicable(closeApproach))
///         {
///             continue;
///         }
///
///         if (algorithmSPtr->isMaximum())
///         {
///             // Upper bound
///             return algorithmSPtr->computeProbabilityOfCollision(closeApproach, hardBody1SPtr, hardBody2SPtr);
///         }
///
///         const ProbabilityOfCollisionAlgorithm::Analysis analysis =
///             ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
///                 algorithmSPtr, closeApproach, hardBody1SPtr, hardBody2SPtr
///             );
///
///         if (analysis.region == ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust)
///         {
///             return analysis.probabilityOfCollision;
///         }
///
///         if (analysis.region == ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted)
///         {
///             // Upper bound
///             return analysis.maximumProbabilityOfCollision.value();
///         }
///
///         // ProbabilityRegion::Unknown: try the next algorithm
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
        Unknown   ///< The maximum probability of collision could not be computed.
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

    /// @brief Check if the probability of collision algorithm computes a maximum probability of collision (i.e. an
    /// upper bound)
    ///
    /// @return True if the algorithm computes a maximum probability of collision
    bool isMaximum() const;

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
    ///         algorithm.computeProbabilityOfCollision(closeApproach, hardBody1SPtr, hardBody2SPtr);
    ///     }
    /// @endcode
    ///
    /// @param aCloseApproach A close approach
    /// @return True if the algorithm is applicable to the close approach
    bool isApplicable(const CloseApproach& aCloseApproach) const;

    /// @brief Compute the probability of collision analysis of a close approach using a probability of collision
    /// algorithm
    ///
    /// @details The analysis is not applicable to algorithms computing a maximum probability of collision (see
    /// isMaximum()), as their probability of collision is already an upper bound.
    ///
    /// This function computes the probability of collision and locates the maximum probability of
    /// collision as it scales the covariance(s): starting from the unscaled covariance(s), a root solver (bracket and
    /// solve) locates where the slope of the probability of collision with respect to the (logarithmic) scaling factor
    /// changes sign. The probability of collision is assumed to have a single maximum.
    ///
    /// - If the maximum probability of collision is found to the "right" of the probability of collision (i.e. a
    /// scaling factor greater than or equal to 1), then the close approach is in the robust region
    /// (ProbabilityRegion::Robust).
    /// - If the maximum probability of collision is found to the "left" of the probability of collision (i.e. a
    /// scaling factor less than 1), then the close approach is in the diluted region (ProbabilityRegion::Diluted).
    /// - If the root solver does not converge (e.g. the probability of collision does not change with the covariance
    /// scaling, or has no maximum), then the region is unknown (ProbabilityRegion::Unknown).
    ///
    /// The scaling factors yielding the maximum probability of collision are also returned, both as a covariance
    /// scaling factor and as a sigma scaling factor. In the unknown region (ProbabilityRegion::Unknown), the maximum
    /// probability of collision and its scaling factors are empty.
    ///
    /// Raises an Undefined error if the probability of collision algorithm is undefined, and a RuntimeError if it
    /// computes a maximum probability of collision or if neither covariance is to be scaled.
    ///
    /// @ref Alfano, Salvatore. (2020). Relating Position Uncertainty to Maximum Conjunction Probability.
    /// The Journal of the Astronautical Sciences. 53. 193-205. 10.1007/BF03546350.
    ///
    /// @param aProbabilityOfCollisionAlgorithmSPtr A probability of collision algorithm (not computing a maximum
    /// probability of collision)
    /// @param aCloseApproach A close approach
    /// @param aHardBody1SPtr The hard body of Object 1
    /// @param aHardBody2SPtr The hard body of Object 2
    /// @param scaleObject1Covariance Whether to scale Object 1 covariance. Defaults to true
    /// @param scaleObject2Covariance Whether to scale Object 2 covariance. Defaults to true
    ///
    /// @return The probability of collision analysis of the close approach
    static Analysis ComputeProbabilityAnalysis(
        const Shared<const ProbabilityOfCollisionAlgorithm>& aProbabilityOfCollisionAlgorithmSPtr,
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr,
        const bool& scaleObject1Covariance = true,
        const bool& scaleObject2Covariance = true
    );

   protected:
    /// @brief Constructor
    ///
    /// @param aName The name of the probability of collision algorithm
    /// @param isMaximum Whether the algorithm computes a maximum probability of collision (i.e. an upper bound)
    ProbabilityOfCollisionAlgorithm(const String& aName, const bool& isMaximum);

    String name_;
    bool isMaximum_;
};

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
