/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <optional>
#include <utility>
#include <vector>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

ProbabilityOfCollisionAlgorithm::ProbabilityOfCollisionAlgorithm(const String& aName)
    : name_(aName)
{
}

String ProbabilityOfCollisionAlgorithm::getName() const
{
    return name_;
}

bool ProbabilityOfCollisionAlgorithm::isApplicable(const CloseApproach& aCloseApproach) const
{
    return this->identifyUnsatisfiedAssumptions(aCloseApproach).isEmpty();
}

ProbabilityOfCollisionAlgorithm::Analysis ProbabilityOfCollisionAlgorithm::computeProbabilityAnalysis(
    const CloseApproach& aCloseApproach,
    const Shared<const HardBody>& aHardBody1SPtr,
    const Shared<const HardBody>& aHardBody2SPtr,
    const bool& scaleObject1Covariance,
    const bool& scaleObject2Covariance,
    const Real& aSigmaScalingFactorLowerBound,
    const Real& aSigmaScalingFactorUpperBound,
    const Size& aMaximumPointPerPassCount,
    const Size& aMaximumPassCount,
    const Real& aConvergenceThreshold
) const
{
    if (!scaleObject1Covariance && !scaleObject2Covariance)
    {
        throw ostk::core::error::RuntimeError("At least one covariance must be scalable to find the probability region."
        );
    }

    if (aSigmaScalingFactorLowerBound <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Sigma scaling factor lower bound must be greater than 0.");
    }

    if (aSigmaScalingFactorLowerBound >= 1.0)
    {
        throw ostk::core::error::RuntimeError("Sigma scaling factor lower bound must be lower than 1.");
    }

    if (aSigmaScalingFactorUpperBound <= 1.0)
    {
        throw ostk::core::error::RuntimeError("Sigma scaling factor upper bound must be greater than 1.");
    }

    if (aMaximumPointPerPassCount < 3)
    {
        throw ostk::core::error::RuntimeError("Maximum point per pass count must be greater than or equal to 3.");
    }

    // Convergence is assessed between two successive passes
    if (aMaximumPassCount < 2)
    {
        throw ostk::core::error::RuntimeError("Maximum pass count must be greater than or equal to 2.");
    }

    if (aConvergenceThreshold <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Convergence threshold must be greater than 0.");
    }

    const Real probabilityOfCollision =
        this->computeProbabilityOfCollision(aCloseApproach, aHardBody1SPtr, aHardBody2SPtr);

    // Samples (covariance scaling factor, probability of collision), sorted by increasing scaling factor, starting with
    // the unscaled covariance(s)
    std::vector<std::pair<double, Real>> samples = {{1.0, probabilityOfCollision}};

    // Compute the probability of collision with scaled covariance(s)
    const auto computeScaledProbabilityOfCollision = [&](const double& aScalingFactor) -> Real
    {
        const CloseApproach scaledCloseApproach = aCloseApproach.scale(
            scaleObject1Covariance ? std::optional<Real>(aScalingFactor) : std::nullopt,
            scaleObject2Covariance ? std::optional<Real>(aScalingFactor) : std::nullopt
        );

        return this->computeProbabilityOfCollision(scaledCloseApproach, aHardBody1SPtr, aHardBody2SPtr);
    };

    const auto sampleInterval = [&](const double& aLowerBound, const double& anUpperBound) -> void
    {
        const double logLowerBound = std::log10(aLowerBound);
        const double logStep =
            (std::log10(anUpperBound) - logLowerBound) / static_cast<double>(aMaximumPointPerPassCount - 1);

        for (Size pointIndex = 0; pointIndex < aMaximumPointPerPassCount; ++pointIndex)
        {
            const double scalingFactor = std::pow(10.0, logLowerBound + static_cast<double>(pointIndex) * logStep);

            const bool isAlreadySampled = std::any_of(
                samples.begin(),
                samples.end(),
                [&scalingFactor](const std::pair<double, Real>& aSample) -> bool
                {
                    return aSample.first == scalingFactor;
                }
            );

            if (!isAlreadySampled)
            {
                samples.push_back({scalingFactor, computeScaledProbabilityOfCollision(scalingFactor)});
            }
        }

        std::sort(
            samples.begin(),
            samples.end(),
            [](const std::pair<double, Real>& aSample, const std::pair<double, Real>& anotherSample) -> bool
            {
                return aSample.first < anotherSample.first;
            }
        );
    };

    // Index of the maximum probability of collision sample (the lowest scaling factor in case of a tie)
    const auto findMaximumIndex = [&samples]() -> Size
    {
        Size maximumIndex = 0;

        for (Size sampleIndex = 1; sampleIndex < samples.size(); ++sampleIndex)
        {
            if (samples[sampleIndex].second > samples[maximumIndex].second)
            {
                maximumIndex = sampleIndex;
            }
        }

        return maximumIndex;
    };

    // First pass: sample the scaling interval (the covariances are scaled by the square of the sigma scaling factors)
    sampleInterval(
        aSigmaScalingFactorLowerBound * aSigmaScalingFactorLowerBound,
        aSigmaScalingFactorUpperBound * aSigmaScalingFactorUpperBound
    );

    // If the probability of collision does not change with the covariance scaling, the algorithm provides an upper
    // bound
    const bool isConstant = std::all_of(
        samples.begin(),
        samples.end(),
        [&probabilityOfCollision](const std::pair<double, Real>& aSample) -> bool
        {
            return aSample.second == probabilityOfCollision;
        }
    );

    if (isConstant)
    {
        return {
            probabilityOfCollision,
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Maximum,
            probabilityOfCollision,
        };
    }

    Size maximumIndex = findMaximumIndex();
    Real maximumProbabilityOfCollision = samples[maximumIndex].second;

    // Subsequent passes: sample the interval surrounding the current maximum, until convergence
    for (Size passIndex = 1; passIndex < aMaximumPassCount; ++passIndex)
    {
        const double lowerBound = samples[(maximumIndex > 0) ? (maximumIndex - 1) : 0].first;
        const double upperBound = samples[std::min<Size>(maximumIndex + 1, samples.size() - 1)].first;

        sampleInterval(lowerBound, upperBound);

        const Real previousMaximumProbabilityOfCollision = maximumProbabilityOfCollision;

        maximumIndex = findMaximumIndex();
        maximumProbabilityOfCollision = samples[maximumIndex].second;

        const bool hasConverged = std::abs(maximumProbabilityOfCollision - previousMaximumProbabilityOfCollision) <=
                                  aConvergenceThreshold * std::abs(previousMaximumProbabilityOfCollision);

        if (hasConverged)
        {
            const ProbabilityOfCollisionAlgorithm::ProbabilityRegion region =
                (samples[maximumIndex].first < 1.0) ? ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted
                                                    : ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust;

            return {probabilityOfCollision, region, maximumProbabilityOfCollision};
        }
    }

    throw ostk::core::error::RuntimeError(
        "The maximum probability of collision has not converged within [{}] passes.", aMaximumPassCount
    );
}

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
