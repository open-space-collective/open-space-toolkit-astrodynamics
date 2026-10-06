/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <optional>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Alfano2005.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

#include <Eigen/Eigenvalues>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace probabilityofcollisionalgorithm
{

using ostk::core::container::Tuple;

using ostk::mathematics::object::Matrix2d;
using ostk::mathematics::object::Vector2d;

using ostk::physics::unit::Length;

using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;

const Shared<const CoordinateSubset> Alfano2005::CartesianPositionSubsetSPtr = CartesianPosition::Default();

const Size Alfano2005::LowerIntervalPairCount = 10;
const Size Alfano2005::UpperIntervalPairCount = 100000;

Alfano2005::Alfano2005(const Derived& aRelativeVelocityThreshold, const Shared<const Frame>& aFrameSPtr)
    : ProbabilityOfCollisionAlgorithm("Alfano 2005"),
      relativeVelocityThreshold_(aRelativeVelocityThreshold),
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

    const std::optional<CovarianceMatrix> object1CovarianceMatrix = aCloseApproach.getObject1CovarianceMatrix();
    if (!object1CovarianceMatrix.has_value() ||
        !object1CovarianceMatrix->getCoordinateSubsets().contains(Alfano2005::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 1");
    }

    const std::optional<CovarianceMatrix> object2CovarianceMatrix = aCloseApproach.getObject2CovarianceMatrix();
    if (!object2CovarianceMatrix.has_value() ||
        !object2CovarianceMatrix->getCoordinateSubsets().contains(Alfano2005::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 2");
    }

    const Real relativeVelocityMetersPerSecond =
        aCloseApproach.getRelativeVelocity().in(Derived::Unit::MeterPerSecond());
    const Real relativeVelocityThresholdMetersPerSecond =
        relativeVelocityThreshold_.in(Derived::Unit::MeterPerSecond());

    if (relativeVelocityMetersPerSecond < relativeVelocityThresholdMetersPerSecond)
    {
        unsatisfiedAssumptions.add(String::Format(
            "Relative velocity [{} m/s] is below the threshold [{} m/s]",
            relativeVelocityMetersPerSecond.toString(),
            relativeVelocityThresholdMetersPerSecond.toString()
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
    const Shared<const Frame> encounterFrameSPtr = aCloseApproach.getEncounterFrame(frameSPtr_);

    // Compute miss distance components in the encounter plane
    const Tuple<Length, Length, Length> missDistanceComponents =
        aCloseApproach.computeMissDistanceComponentsInFrame(encounterFrameSPtr);

    const Vector2d missDistanceInEncounterPlane = {
        std::get<0>(missDistanceComponents).inMeters(),
        std::get<1>(missDistanceComponents).inMeters(),
    };

    // Compute combined position covariance projected onto the encounter plane
    const CovarianceMatrix object1EncounterPositionUncertainty = object1State.accessCovarianceMatrix()
                                                                     .reduce({Alfano2005::CartesianPositionSubsetSPtr})
                                                                     .rotate(encounterFrameSPtr);
    const CovarianceMatrix object2EncounterPositionUncertainty = object2State.accessCovarianceMatrix()
                                                                     .reduce({Alfano2005::CartesianPositionSubsetSPtr})
                                                                     .rotate(encounterFrameSPtr);

    const Matrix2d combinedEncounterPlaneCovariance = (object1EncounterPositionUncertainty.accessCoordinates() +
                                                       object2EncounterPositionUncertainty.accessCoordinates())
                                                          .topLeftCorner<2, 2>();

    // Diagonalize the combined covariance: x-axis along the major principal axis, y-axis along the minor principal axis
    // (eigenvalues are sorted in increasing order)
    const Eigen::SelfAdjointEigenSolver<Matrix2d> eigenSolver(combinedEncounterPlaneCovariance);

    const Vector2d majorPrincipalAxis = eigenSolver.eigenvectors().col(1);
    const Vector2d minorPrincipalAxis = eigenSolver.eigenvectors().col(0);

    const Real sigma_x = std::sqrt(eigenSolver.eigenvalues()(1));
    const Real sigma_y = std::sqrt(eigenSolver.eigenvalues()(0));

    // Compute miss distance components along the principal axes (xm, ym)
    const Real xm = majorPrincipalAxis.dot(missDistanceInEncounterPlane);
    const Real ym = minorPrincipalAxis.dot(missDistanceInEncounterPlane);

    // Compute the combined hard-body radius (enclosing radius of the combined cross section in the encounter plane)
    const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
        aHardBody1SPtr->calculateCrossSectionAt(object1State, encounterFrameSPtr),
        aHardBody2SPtr->calculateCrossSectionAt(object2State, encounterFrameSPtr)
    );

    const Real combinedHardBodyRadius = combinedCrossSection.getEnclosingRadius();

    return Alfano2005::ComputeProbabilityOfCollision(xm, ym, sigma_x, sigma_y, combinedHardBodyRadius);
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
    const Real& sigma_x = aSigmaX;
    const Real& sigma_y = aSigmaY;
    const Real& ra = aCombinedHardBodyRadius;

    // Compute the number of interval pairs (m) and the integration step (dx), the integral over [-ra, ra] being folded
    // onto [-ra, 0]
    const Size m = Alfano2005::ComputeIntervalPairCount(xm, ym, sigma_x, sigma_y, ra);
    const Real dx = ra / (2.0 * m);

    // Compute the first term, slightly offset from -ra (zero-length chord)
    const Real x0 = 0.015 * dx - ra;
    const Real m0 = 2.0 * Alfano2005::ComputeIntegrand(x0, xm, ym, sigma_x, sigma_y, ra);

    // Compute the even terms, including the last term at x = 0
    Real mEvenSum = 0.0;
    for (Size i = 1; i < m; ++i)
    {
        const Real x2i = 2.0 * i * dx - ra;
        mEvenSum += Alfano2005::ComputeIntegrand(x2i, xm, ym, sigma_x, sigma_y, ra);
    }

    const Real mLast =
        std::exp(-xm * xm / (2.0 * sigma_x * sigma_x)) *
        (std::erf((-ym + ra) / (std::sqrt(2.0) * sigma_y)) - std::erf((-ym - ra) / (std::sqrt(2.0) * sigma_y)));

    const Real mEven = 2.0 * (mEvenSum + mLast);

    // Compute the odd terms
    Real mOddSum = 0.0;
    for (Size i = 1; i <= m; ++i)
    {
        const Real x2i1 = (2.0 * i - 1.0) * dx - ra;
        mOddSum += Alfano2005::ComputeIntegrand(x2i1, xm, ym, sigma_x, sigma_y, ra);
    }

    const Real mOdd = 4.0 * mOddSum;

    // Compute the probability of collision (Simpson's rule)
    return dx / (3.0 * sigma_x * std::sqrt(8.0 * M_PI)) * (m0 + mEven + mOdd);
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
    const Real& sigma_x = aSigmaX;
    const Real& sigma_y = aSigmaY;
    const Real& ra = aCombinedHardBodyRadius;

    // Half-chord of the hard-body circle at abscissa x
    const Real halfChord = std::sqrt(ra * ra - x * x);

    return (std::erf((-ym + halfChord) / (std::sqrt(2.0) * sigma_y)) -
            std::erf((-ym - halfChord) / (std::sqrt(2.0) * sigma_y))) *
           (std::exp(-(x - xm) * (x - xm) / (2.0 * sigma_x * sigma_x)) +
            std::exp(-(x + xm) * (x + xm) / (2.0 * sigma_x * sigma_x)));
}

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
