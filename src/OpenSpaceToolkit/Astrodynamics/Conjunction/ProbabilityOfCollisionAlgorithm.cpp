/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

bool ProbabilityOfCollisionAlgorithm::isApplicable(const CloseApproach& aCloseApproach) const
{
    return this->identifyUnsatisfiedAssumptions(aCloseApproach).isEmpty();
}

ProbabilityOfCollisionAlgorithm::ProbabilityRegion ProbabilityOfCollisionAlgorithm::computeProbabilityRegion(
    const CloseApproach& aCloseApproach,
    const Length& aCombinedHardBodyRadius,
    const Real& aScalingFactorLowerBound,
    const Real& aScalingFactorUpperBound,
    const RootSolver& aRootSolver,
    const bool& scaleObject1Covariance,
    const bool& scaleObject2Covariance
) const
{
    if (this->isMaximumProbabilityOfCollisionAlgorithm())
    {
        return ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Unknown;
    }

    if (!this->isApplicable(aCloseApproach))
    {
        return ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Unknown;
    }

    if (!scaleObject1Covariance && !scaleObject2Covariance)
    {
        throw ostk::core::error::RuntimeError("At least one covariance must be scalable to find the probability region."
        );
    }

    if (!aScalingFactorLowerBound.isDefined() || !aScalingFactorUpperBound.isDefined())
    {
        throw ostk::core::error::RuntimeError("Scaling factor lower bound and upper bound must be defined.");
    }

    if (aScalingFactorLowerBound < 0.0)
    {
        throw ostk::core::error::RuntimeError("Scaling factor lower bound must be greater than or equal to 0.");
    }

    if (aScalingFactorLowerBound >= 1.0)
    {
        throw ostk::core::error::RuntimeError("Scaling factor lower bound must be lower than 1.");
    }

    if (aScalingFactorUpperBound <= 1.0)
    {
        throw ostk::core::error::RuntimeError("Scaling factor upper bound must be greater than 1.");
    }

    if (aScalingFactorUpperBound <= aScalingFactorLowerBound)
    {
        throw ostk::core::error::RuntimeError("Scaling factor upper bound must be greater than the lower bound.");
    }

    const Real slopeStepSize = aRootSolver.getTolerance() / 2.0;

    const RootSolver::Solution upperIntervalExtremum = this->findProbabilityOfCollisionExtremum(
        aCloseApproach,
        aCombinedHardBodyRadius,
        1.0,
        aScalingFactorUpperBound,
        aRootSolver,
        scaleObject1Covariance,
        scaleObject2Covariance,
        slopeStepSize
    );

    if (upperIntervalExtremum.hasConverged)
    {
        // We found an extremum "to the right" of our current covariance size. If this extremum is a
        // maximum, it means that we are in the robust region.
        const Real lowerBoundSlope = this->computeProbabilityOfCollisionSlope(
            aCloseApproach, aCombinedHardBodyRadius, upperIntervalExtremum.lowerBound, slopeStepSize
        );
        const Real upperBoundSlope = this->computeProbabilityOfCollisionSlope(
            aCloseApproach, aCombinedHardBodyRadius, upperIntervalExtremum.upperBound, slopeStepSize
        );

        if ((lowerBoundSlope > 0.0) && (upperBoundSlope < 0.0))
        {
            return ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust;
        }
    }

    const RootSolver::Solution lowerIntervalExtremum = this->findProbabilityOfCollisionExtremum(
        aCloseApproach,
        aCombinedHardBodyRadius,
        aScalingFactorLowerBound,
        1.0,
        aRootSolver,
        scaleObject1Covariance,
        scaleObject2Covariance,
        slopeStepSize
    );

    if (lowerIntervalExtremum.hasConverged)
    {
        // We found an extremum "to the left" of our current covariance size. If this extremum is a
        // maximum, it means that we are in the dilution region.
        const Real lowerBoundSlope = this->computeProbabilityOfCollisionSlope(
            aCloseApproach, aCombinedHardBodyRadius, lowerIntervalExtremum.lowerBound, slopeStepSize
        );
        const Real upperBoundSlope = this->computeProbabilityOfCollisionSlope(
            aCloseApproach, aCombinedHardBodyRadius, lowerIntervalExtremum.upperBound, slopeStepSize
        );

        if ((lowerBoundSlope > 0.0) && (upperBoundSlope < 0.0))
        {
            return ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted;
        }
    }

    // We could not find the location of the maximum probability of collision
    return ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Unknown;
}

RootSolver::Solution ProbabilityOfCollisionAlgorithm::findProbabilityOfCollisionExtremum(
    const CloseApproach& aCloseApproach,
    const Length& aCombinedHardBodyRadius,
    const Real& aScalingFactorLowerBound,
    const Real& aScalingFactorUpperBound,
    const RootSolver& aRootSolver,
    const bool& scaleObject1Covariance,
    const bool& scaleObject2Covariance,
    const Real& aSlopeStepSize
) const
{
    (void)scaleObject1Covariance;
    (void)scaleObject2Covariance;

    const auto slopeFunction = [&](const double& aScalingFactor) -> double
    {
        return this->computeProbabilityOfCollisionSlope(
            aCloseApproach, aCombinedHardBodyRadius, aScalingFactor, aSlopeStepSize
        );
    };

    return aRootSolver.solve(slopeFunction, aScalingFactorLowerBound, aScalingFactorUpperBound);
}

Real ProbabilityOfCollisionAlgorithm::computeProbabilityOfCollisionSlope(
    const CloseApproach& aCloseApproach,
    const Length& aCombinedHardBodyRadius,
    const Real& aScalingFactor,
    const Real& aSlopeStepSize
) const
{
    const Real lowerScalingFactor = aScalingFactor - aSlopeStepSize;
    const Real upperScalingFactor = aScalingFactor + aSlopeStepSize;

    const CloseApproach lowerScaledCloseApproach = aCloseApproach.scale(lowerScalingFactor, lowerScalingFactor);
    const CloseApproach upperScaledCloseApproach = aCloseApproach.scale(upperScalingFactor, upperScalingFactor);

    const Real lowerProbabilityOfCollision =
        this->computeProbabilityOfCollision(lowerScaledCloseApproach, aCombinedHardBodyRadius);
    const Real upperProbabilityOfCollision =
        this->computeProbabilityOfCollision(upperScaledCloseApproach, aCombinedHardBodyRadius);

    return (upperProbabilityOfCollision - lowerProbabilityOfCollision) / (2.0 * aSlopeStepSize);
}

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
