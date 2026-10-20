/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <optional>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Alfano2005.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace probabilityofcollisionalgorithm
{

using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::uncertainty::Covariance;

const Shared<const CoordinateSubset> Alfano2005::CartesianPositionSubsetSPtr = CartesianPosition::Default();

// As per the original paper, the lower limit is set to 10 and the upper limit is set to 50;
// "sufficient for combined object radii up to 100 meters, miss distances up to 100 kilometers, and covariance aspect
// ratios up to 500".
const Size Alfano2005::LowerIntervalPairCount = 10;
const Size Alfano2005::UpperIntervalPairCount = 50;

Alfano2005::Alfano2005(const Derived& aRelativeSpeedThreshold, const Shared<const Frame>& aFrameSPtr)
    : ProbabilityOfCollisionAlgorithm("Alfano 2005"),
      relativeSpeedThreshold_(aRelativeSpeedThreshold),
      frameSPtr_(aFrameSPtr)
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!aFrameSPtr->isQuasiInertial())
    {
        throw ostk::core::error::runtime::Wrong("Frame", aFrameSPtr->getName());
    }
}

Array<String> Alfano2005::identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const
{
    Array<String> unsatisfiedAssumptions = Array<String>::Empty();

    const std::optional<Covariance> object1Covariance = aCloseApproach.getObject1State().getCovariance();
    if (!object1Covariance.has_value() ||
        !object1Covariance->getCoordinateSubsets().contains(Alfano2005::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 1");
    }

    const std::optional<Covariance> object2Covariance = aCloseApproach.getObject2State().getCovariance();
    if (!object2Covariance.has_value() ||
        !object2Covariance->getCoordinateSubsets().contains(Alfano2005::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 2");
    }

    const Real relativeSpeedMetersPerSecond = aCloseApproach.computeRelativeSpeed().in(Derived::Unit::MeterPerSecond());
    const Real relativeSpeedThresholdMetersPerSecond = relativeSpeedThreshold_.in(Derived::Unit::MeterPerSecond());

    if (relativeSpeedMetersPerSecond < relativeSpeedThresholdMetersPerSecond)
    {
        unsatisfiedAssumptions.add(String::Format(
            "Relative speed [{} m/s] is below the threshold [{} m/s]",
            relativeSpeedMetersPerSecond.toString(),
            relativeSpeedThresholdMetersPerSecond.toString()
        ));
    }

    return unsatisfiedAssumptions;
}

Real Alfano2005::computeProbabilityOfCollision(
    const CloseApproach& aCloseApproach,
    const Shared<const HardBody>& aHardBody1SPtr,
    const Shared<const HardBody>& aHardBody2SPtr
) const
{
    if ((aHardBody1SPtr == nullptr) || (aHardBody2SPtr == nullptr))
    {
        throw ostk::core::error::runtime::Undefined("Hard body");
    }

    const State object1State = aCloseApproach.getObject1State();
    const State object2State = aCloseApproach.getObject2State();

    // Encounter frame (centered on Object 1, z-axis along the relative velocity, x-y plane is the encounter plane)
    const Shared<const Frame> encounterFrameSPtr = aCloseApproach.computeEncounterFrame(frameSPtr_);

    // Miss distance and combined position uncertainty along the principal axes of the combined covariance
    const CloseApproach::RotatedEncounterFrameParameters rotatedEncounterFrameParameters =
        aCloseApproach.computeRotatedEncounterFrameParameters(encounterFrameSPtr);

    // Compute the combined hard-body radius (enclosing radius of the combined cross section in the rotated encounter
    // plane)
    const Shared<const Frame>& rotatedEncounterFrameSPtr = rotatedEncounterFrameParameters.rotatedEncounterFrameSPtr;

    const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
        aHardBody1SPtr->calculateCrossSectionAt(object1State, rotatedEncounterFrameSPtr),
        aHardBody2SPtr->calculateCrossSectionAt(object2State, rotatedEncounterFrameSPtr)
    );

    const Real combinedHardBodyRadius = combinedCrossSection.getEnclosingRadius();

    return Alfano2005::ComputeProbabilityOfCollision(
        rotatedEncounterFrameParameters.xM.inMeters(),
        rotatedEncounterFrameParameters.yM.inMeters(),
        rotatedEncounterFrameParameters.sigmaX.inMeters(),
        rotatedEncounterFrameParameters.sigmaY.inMeters(),
        combinedHardBodyRadius
    );
}

Real Alfano2005::ComputeProbabilityOfCollision(
    const Real& aMissDistanceX,
    const Real& aMissDistanceY,
    const Real& aSigmaX,
    const Real& aSigmaY,
    const Real& aCombinedHardBodyRadius
)
{
    const Real& xm = aMissDistanceX;
    const Real& ym = aMissDistanceY;
    const Real& sigmaX = aSigmaX;
    const Real& sigmaY = aSigmaY;
    const Real& ra = aCombinedHardBodyRadius;

    // Compute the number of interval pairs (m) and the integration step (dx), the integral over [-ra, ra] being folded
    // onto [-ra, 0]
    const Size m = Alfano2005::ComputeIntervalPairCount(xm, ym, sigmaX, sigmaY, ra);
    const Real dx = ra / (2.0 * m);

    // Compute the first term, slightly offset from -ra (zero-length chord)
    const Real x0 = 0.015 * dx - ra;
    const Real m0 = 2.0 * Alfano2005::ComputeIntegrand(x0, xm, ym, sigmaX, sigmaY, ra);

    // Compute the even terms, including the last term at x = 0
    Real mEvenSum = 0.0;
    for (Size i = 1; i < m; ++i)
    {
        const Real x2i = 2.0 * i * dx - ra;
        mEvenSum += Alfano2005::ComputeIntegrand(x2i, xm, ym, sigmaX, sigmaY, ra);
    }

    const Real mLast =
        std::exp(-xm * xm / (2.0 * sigmaX * sigmaX)) *
        (std::erf((-ym + ra) / (std::sqrt(2.0) * sigmaY)) - std::erf((-ym - ra) / (std::sqrt(2.0) * sigmaY)));

    const Real mEven = 2.0 * (mEvenSum + mLast);

    // Compute the odd terms
    Real mOddSum = 0.0;
    for (Size i = 1; i <= m; ++i)
    {
        const Real x2i1 = (2.0 * i - 1.0) * dx - ra;
        mOddSum += Alfano2005::ComputeIntegrand(x2i1, xm, ym, sigmaX, sigmaY, ra);
    }

    const Real mOdd = 4.0 * mOddSum;

    // Compute the probability of collision (Simpson's rule)
    return dx / (3.0 * sigmaX * std::sqrt(8.0 * M_PI)) * (m0 + mEven + mOdd);
}

Size Alfano2005::ComputeIntervalPairCount(
    const Real& aMissDistanceX,
    const Real& aMissDistanceY,
    const Real& aSigmaX,
    const Real& aSigmaY,
    const Real& aCombinedHardBodyRadius
)
{
    const double missDistance = std::sqrt(aMissDistanceX * aMissDistanceX + aMissDistanceY * aMissDistanceY);
    const double smallestLength = std::min({double(aSigmaX), double(aSigmaY), missDistance});

    // A zero smallest length (e.g. zero miss distance) leads to the upper bound
    if (smallestLength <= 0.0)
    {
        return Alfano2005::UpperIntervalPairCount;
    }

    const double intervalPairCount = std::clamp(
        5.0 * double(aCombinedHardBodyRadius) / smallestLength,
        double(Alfano2005::LowerIntervalPairCount),
        double(Alfano2005::UpperIntervalPairCount)
    );

    return static_cast<Size>(intervalPairCount);
}

Real Alfano2005::ComputeIntegrand(
    const Real& anAbscissa,
    const Real& aMissDistanceX,
    const Real& aMissDistanceY,
    const Real& aSigmaX,
    const Real& aSigmaY,
    const Real& aCombinedHardBodyRadius
)
{
    const Real& x = anAbscissa;
    const Real& xm = aMissDistanceX;
    const Real& ym = aMissDistanceY;
    const Real& sigmaX = aSigmaX;
    const Real& sigmaY = aSigmaY;
    const Real& ra = aCombinedHardBodyRadius;

    // Half-chord of the hard-body circle at abscissa x
    const Real halfChord = std::sqrt(ra * ra - x * x);

    return (std::erf((-ym + halfChord) / (std::sqrt(2.0) * sigmaY)) -
            std::erf((-ym - halfChord) / (std::sqrt(2.0) * sigmaY))) *
           (std::exp(-(x - xm) * (x - xm) / (2.0 * sigmaX * sigmaX)) +
            std::exp(-(x + xm) * (x + xm) / (2.0 * sigmaX * sigmaX)));
}

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
