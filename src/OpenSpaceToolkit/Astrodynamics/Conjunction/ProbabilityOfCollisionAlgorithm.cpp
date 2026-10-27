/// Apache License 2.0

#include <cmath>
#include <exception>
#include <optional>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>
#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

ProbabilityOfCollisionAlgorithm::ProbabilityOfCollisionAlgorithm(const String& aName, const bool& isMaximum)
    : name_(aName),
      isMaximum_(isMaximum)
{
}

String ProbabilityOfCollisionAlgorithm::getName() const
{
    return name_;
}

bool ProbabilityOfCollisionAlgorithm::isMaximum() const
{
    return isMaximum_;
}

bool ProbabilityOfCollisionAlgorithm::isApplicable(const CloseApproach& aCloseApproach) const
{
    return this->identifyUnsatisfiedAssumptions(aCloseApproach).isEmpty();
}

ProbabilityOfCollisionAlgorithm::Analysis ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
    const Shared<const ProbabilityOfCollisionAlgorithm>& aProbabilityOfCollisionAlgorithmSPtr,
    const CloseApproach& aCloseApproach,
    const Shared<const HardBody>& aHardBody1SPtr,
    const Shared<const HardBody>& aHardBody2SPtr,
    const bool& scaleObject1Covariance,
    const bool& scaleObject2Covariance
)
{
    if (aProbabilityOfCollisionAlgorithmSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Probability of collision algorithm");
    }

    if (aProbabilityOfCollisionAlgorithmSPtr->isMaximum())
    {
        throw ostk::core::error::RuntimeError(
            "The probability of collision is already an upper bound: the analysis is not applicable."
        );
    }

    if (!scaleObject1Covariance && !scaleObject2Covariance)
    {
        throw ostk::core::error::RuntimeError("At least one covariance must be scalable to find the probability region."
        );
    }

    const Real probabilityOfCollision = aProbabilityOfCollisionAlgorithmSPtr->computeProbabilityOfCollision(
        aCloseApproach, aHardBody1SPtr, aHardBody2SPtr
    );

    // Compute the probability of collision with scaled covariance(s)
    const auto computeScaledProbabilityOfCollision = [&](const double& aScalingFactor) -> Real
    {
        const CloseApproach scaledCloseApproach = aCloseApproach.scale(
            scaleObject1Covariance ? std::optional<Real>(aScalingFactor) : std::nullopt,
            scaleObject2Covariance ? std::optional<Real>(aScalingFactor) : std::nullopt
        );

        return aProbabilityOfCollisionAlgorithmSPtr->computeProbabilityOfCollision(
            scaledCloseApproach, aHardBody1SPtr, aHardBody2SPtr
        );
    };

    // Central difference ratio on the covariance scaling factor (+/- 0.1%): multiplying and dividing by the same ratio
    // keeps both points symmetric around the scaling factor on a logarithmic scale
    const double scalingFactorRatio = 1.001;

    // Slope of the probability of collision with respect to the logarithm of the covariance scaling factor: positive to
    // the left of the maximum, negative to its right (falling function)
    const auto computeSlope = [&](const double& aScalingFactor) -> double
    {
        return computeScaledProbabilityOfCollision(aScalingFactor * scalingFactorRatio) -
               computeScaledProbabilityOfCollision(aScalingFactor / scalingFactorRatio);
    };

    // Locate the maximum, starting from the unscaled covariance(s). The root solver may also fail by throwing (e.g. no
    // sign change found, or the scaled covariance(s) overflowing while bracketing), which is also a non-convergence:
    // the inputs have already been validated by the unscaled probability of collision computation.
    std::optional<RootSolver::Solution> solution = std::nullopt;

    try
    {
        solution = RootSolver::Default().bracketAndSolve(computeSlope, 1.0, false);
    }
    catch (const std::exception&)
    {
        solution = std::nullopt;
    }

    if (!solution.has_value() || !solution->hasConverged)
    {
        return {
            probabilityOfCollision,
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Unknown,
            std::nullopt,
            std::nullopt,
            std::nullopt,
        };
    }

    const double maximumScalingFactor = solution->root;

    const ProbabilityOfCollisionAlgorithm::ProbabilityRegion region =
        (maximumScalingFactor < 1.0) ? ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted
                                     : ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust;

    return {
        probabilityOfCollision,
        region,
        computeScaledProbabilityOfCollision(maximumScalingFactor),
        maximumScalingFactor,
        std::sqrt(maximumScalingFactor),
    };
}

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
