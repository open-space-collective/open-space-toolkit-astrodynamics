/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Frisbee2015.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace probabilityofcollisionalgorithm
{

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector2d;

using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;

const Shared<const CoordinateSubset> Frisbee2015::CartesianPositionSubsetSPtr = CartesianPosition::Default();

Frisbee2015::Frisbee2015(
    const Derived& aRelativeVelocityThreshold,
    const Shared<const Frame>& aFrameSPtr,
    const bool& fallBackToObject2Covariance
)
    : relativeVelocityThreshold_(aRelativeVelocityThreshold),
      frameSPtr_(aFrameSPtr),
      fallBackToObject2Covariance_(fallBackToObject2Covariance)
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

String Frisbee2015::getName() const
{
    return "Frisbee 2015";
}

bool Frisbee2015::isMaximumProbabilityOfCollisionAlgorithm() const
{
    return true;
}

Array<String> Frisbee2015::identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const
{
    Array<String> unsatisfiedAssumptions = Array<String>::Empty();

    const CovarianceMatrix object1CovarianceMatrix = aCloseApproach.getObject1CovarianceMatrix();
    const bool object1HasPositionUncertainty =
        object1CovarianceMatrix.isDefined() &&
        object1CovarianceMatrix.getCoordinateSubsets().contains(Frisbee2015::CartesianPositionSubsetSPtr);

    const CovarianceMatrix object2CovarianceMatrix = aCloseApproach.getObject2CovarianceMatrix();
    const bool object2HasPositionUncertainty =
        object2CovarianceMatrix.isDefined() &&
        object2CovarianceMatrix.getCoordinateSubsets().contains(Frisbee2015::CartesianPositionSubsetSPtr);

    if (!fallBackToObject2Covariance_ && !object1HasPositionUncertainty)
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 1");
    }
    else if (fallBackToObject2Covariance_ && !object1HasPositionUncertainty && !object2HasPositionUncertainty)
    {
        unsatisfiedAssumptions.add("No object has position uncertainty");
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

Real Frisbee2015::computeProbabilityOfCollision(
    const CloseApproach& aCloseApproach, const Length& aCombinedHardBodyRadius
) const
{
    const CovarianceMatrix object1CovarianceMatrix = aCloseApproach.getObject1CovarianceMatrix();
    const bool object1HasPositionUncertainty =
        object1CovarianceMatrix.isDefined() &&
        object1CovarianceMatrix.getCoordinateSubsets().contains(Frisbee2015::CartesianPositionSubsetSPtr);

    if (!object1HasPositionUncertainty && fallBackToObject2Covariance_)
    {
        return this->computeProbabilityOfCollision_(aCloseApproach.flip(), aCombinedHardBodyRadius);
    }

    return this->computeProbabilityOfCollision_(aCloseApproach, aCombinedHardBodyRadius);
}

Real Frisbee2015::computeProbabilityOfCollision_(
    const CloseApproach& aCloseApproach, const Length& aCombinedHardBodyRadius
) const
{
    const Shared<const Frame> encounterFrameSPtr = aCloseApproach.getEncounterFrame(frameSPtr_);

    // Get the object 1 ("asset") 2x2 covariance in the encounter ("collision") frame (covAC)
    const MatrixXd covAC = aCloseApproach.getObject1CovarianceMatrix()
                               .reduce({Frisbee2015::CartesianPositionSubsetSPtr})
                               .inFrame(encounterFrameSPtr)
                               .accessCoordinates()
                               .topLeftCorner(2, 2);

    // Get the relative position of object 2 ("debris") in the encounter frame (rel)
    const Vector2d rel =
        aCloseApproach.getObject2State().inFrame(encounterFrameSPtr).getPosition().inMeters().getCoordinates().head(2);

    const Vector2d urel = rel.normalized();

    // Equation 7: sigma level of the collision-plane miss w.r.t. the asset covariance
    const Real kaSquared = rel.dot(covAC.ldlt().solve(rel));

    // Equation 6: critical debris variance along the collision-plane miss.
    // If KA <= 1, VC is not positive semi-definite; the maximum is obtained with the asset covariance only.
    const Real vC = (kaSquared > 1.0) ? (rel.squaredNorm() * (kaSquared - 1.0) / kaSquared) : Real(0.0);

    // Equation 3: degenerate debris covariance along the collision-plane miss (with Vr = VC)
    const MatrixXd covDC = vC * urel * urel.transpose();

    // Equation 4: combined critical collision-plane covariance
    const MatrixXd covTC = covAC + covDC;

    const Real rHBSquared = std::pow(aCombinedHardBodyRadius.inMeters(), 2);

    // Equation 5: approximate maximum probability of collision
    const Real mahalanobisSquared = rel.dot(covTC.ldlt().solve(rel));
    return (rHBSquared * std::exp(-0.5 * mahalanobisSquared)) / (2.0 * std::sqrt(covTC.determinant()));
}

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
