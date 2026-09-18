/// Apache License 2.0

#include <cmath>
#include <optional>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Frisbee2015.hpp>
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

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector2d;

using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::uncertainty::Covariance;

const Shared<const CoordinateSubset> Frisbee2015::CartesianPositionSubsetSPtr = CartesianPosition::Default();

Frisbee2015::Frisbee2015(
    const std::optional<Shared<const ProbabilityOfCollisionAlgorithm>>& aProbabilityOfCollisionAlgorithmSPtr,
    const bool& considerObject2Covariance,
    const bool& considerWorstCaseWhenBothCovarianceArePresent,
    const Derived& aRelativeSpeedThreshold,
    const Shared<const Frame>& aFrameSPtr
)
    : ProbabilityOfCollisionAlgorithm("Frisbee 2015 (Closed-form)", true),
      relativeSpeedThreshold_(aRelativeSpeedThreshold),
      frameSPtr_(aFrameSPtr),
      considerObject2Covariance_(considerObject2Covariance),
      considerWorstCaseWhenBothCovarianceArePresent_(considerWorstCaseWhenBothCovarianceArePresent),
      probabilityOfCollisionAlgorithmSPtr_(aProbabilityOfCollisionAlgorithmSPtr)
{
    if (aProbabilityOfCollisionAlgorithmSPtr.has_value())
    {
        if (aProbabilityOfCollisionAlgorithmSPtr.value() == nullptr)
        {
            throw ostk::core::error::runtime::Undefined("Probability of collision algorithm");
        }

        name_ = String::Format("Frisbee 2015 ({})", aProbabilityOfCollisionAlgorithmSPtr.value()->getName());
    }

    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!aFrameSPtr->isQuasiInertial())
    {
        throw ostk::core::error::runtime::Wrong("Frame", aFrameSPtr->getName());
    }
}

Array<String> Frisbee2015::identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const
{
    Array<String> unsatisfiedAssumptions = Array<String>::Empty();

    const std::optional<Covariance> object1Covariance = aCloseApproach.getObject1State().getCovariance();
    const bool object1HasPositionUncertainty =
        object1Covariance.has_value() &&
        object1Covariance->getCoordinateSubsets().contains(Frisbee2015::CartesianPositionSubsetSPtr);

    const std::optional<Covariance> object2Covariance = aCloseApproach.getObject2State().getCovariance();
    const bool object2HasPositionUncertainty =
        object2Covariance.has_value() &&
        object2Covariance->getCoordinateSubsets().contains(Frisbee2015::CartesianPositionSubsetSPtr);

    if (!considerObject2Covariance_ && !object1HasPositionUncertainty)
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 1");
    }
    else if (considerObject2Covariance_ && !object1HasPositionUncertainty && !object2HasPositionUncertainty)
    {
        unsatisfiedAssumptions.add("No object has position uncertainty");
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

Real Frisbee2015::computeProbabilityOfCollision(
    const CloseApproach& aCloseApproach,
    const Shared<const HardBody>& aHardBody1SPtr,
    const Shared<const HardBody>& aHardBody2SPtr
) const
{
    if ((aHardBody1SPtr == nullptr) || (aHardBody2SPtr == nullptr))
    {
        throw ostk::core::error::runtime::Undefined("Hard body");
    }

    // If the Object 2 covariance is not considered, use the Object 1 covariance only
    if (!considerObject2Covariance_)
    {
        return this->computeProbabilityOfCollision_(aCloseApproach, aHardBody1SPtr, aHardBody2SPtr);
    }

    const std::optional<Covariance> object1Covariance = aCloseApproach.getObject1State().getCovariance();
    const bool object1HasPositionUncertainty =
        object1Covariance.has_value() &&
        object1Covariance->getCoordinateSubsets().contains(Frisbee2015::CartesianPositionSubsetSPtr);

    // If Object 1 has no position uncertainty, use the Object 2 covariance
    if (!object1HasPositionUncertainty && considerObject2Covariance_)
    {
        return this->computeProbabilityOfCollision_(aCloseApproach.swap(), aHardBody2SPtr, aHardBody1SPtr);
    }

    const std::optional<Covariance> object2Covariance = aCloseApproach.getObject2State().getCovariance();
    const bool object2HasPositionUncertainty =
        object2Covariance.has_value() &&
        object2Covariance->getCoordinateSubsets().contains(Frisbee2015::CartesianPositionSubsetSPtr);

    // Only Object 1 has position uncertainty, or both covariances are present but the worst case is not considered:
    // use the Object 1 covariance
    if (!object2HasPositionUncertainty || !considerWorstCaseWhenBothCovarianceArePresent_)
    {
        return this->computeProbabilityOfCollision_(aCloseApproach, aHardBody1SPtr, aHardBody2SPtr);
    }

    // Both covariances are present: use the worst case
    const Real probabilityOfCollision1 =
        this->computeProbabilityOfCollision_(aCloseApproach, aHardBody1SPtr, aHardBody2SPtr);
    const Real probabilityOfCollision2 =
        this->computeProbabilityOfCollision_(aCloseApproach.swap(), aHardBody2SPtr, aHardBody1SPtr);
    return std::max(probabilityOfCollision1, probabilityOfCollision2);
}

Real Frisbee2015::computeProbabilityOfCollision_(
    const CloseApproach& aCloseApproach,
    const Shared<const HardBody>& aHardBody1SPtr,
    const Shared<const HardBody>& aHardBody2SPtr
) const
{
    const State object1State = aCloseApproach.getObject1State();
    const State object2State = aCloseApproach.getObject2State();

    const Shared<const Frame> encounterFrameSPtr = aCloseApproach.computeEncounterFrame(frameSPtr_);

    // Object 1 ("asset") 2x2 covariance in the encounter ("collision") frame (covAC)
    const MatrixXd covAC = object1State.getCovariance()
                               ->reduce({Frisbee2015::CartesianPositionSubsetSPtr})
                               .rotate(encounterFrameSPtr)
                               .getCoordinates()
                               .topLeftCorner(2, 2);

    // Relative position of Object 2 ("debris") in the encounter frame (rel)
    const Vector2d rel = object2State.inFrame(encounterFrameSPtr).getPosition().inMeters().getCoordinates().head(2);

    const Vector2d urel = rel.normalized();

    // Equation 7: sigma level of the collision plane miss with respect to the asset covariance
    const Real kaSquared = rel.dot(covAC.ldlt().solve(rel));

    // Equation 6: critical debris variance along the collision plane miss. If KA <= 1, VC would not be positive
    // semi-definite: the maximum is then obtained with the asset covariance only.
    const Real vC = (kaSquared > 1.0) ? (rel.squaredNorm() * (kaSquared - 1.0) / kaSquared) : Real(0.0);

    // Equation 3: degenerate debris covariance along the collision plane miss (with Vr = VC)
    const MatrixXd covDC = vC * urel * urel.transpose();

    // If no probability of collision algorithm is provided, use Frisbee's closed-form solution
    if (!probabilityOfCollisionAlgorithmSPtr_.has_value())
    {
        // Equation 4: combined critical collision plane covariance
        const MatrixXd covTC = covAC + covDC;

        // Combined hard-body radius (enclosing radius of the combined cross-section in the encounter plane)
        const HardBody::CrossSection combinedCrossSection = HardBody::CrossSection::combine(
            aHardBody1SPtr->calculateCrossSectionAt(object1State, encounterFrameSPtr),
            aHardBody2SPtr->calculateCrossSectionAt(object2State, encounterFrameSPtr)
        );

        const Real rHBSquared = std::pow(combinedCrossSection.getEnclosingRadius(), 2);

        // Equation 5: approximate maximum probability of collision
        const Real mahalanobisSquared = rel.dot(covTC.ldlt().solve(rel));
        return (rHBSquared * std::exp(-0.5 * mahalanobisSquared)) / (2.0 * std::sqrt(covTC.determinant()));
    }

    // Otherwise, build the degenerate close approach and compute the probability with the provided algorithm. The 2x2
    // collision plane covariances are expanded to 3x3 (no uncertainty along the relative velocity). covDC is only
    // symmetric up to rounding errors, so it is averaged with its transpose to make it exactly symmetric.
    MatrixXd object1PositionCovariance = MatrixXd::Zero(3, 3);
    object1PositionCovariance.topLeftCorner(2, 2) = covAC;

    MatrixXd object2PositionCovariance = MatrixXd::Zero(3, 3);
    object2PositionCovariance.topLeftCorner(2, 2) = 0.5 * (covDC + covDC.transpose());

    const CloseApproach degenerateCloseApproach = {
        object1State.withCovariance(Covariance(
            object1State.accessInstant(),
            object1PositionCovariance,
            encounterFrameSPtr,
            {Frisbee2015::CartesianPositionSubsetSPtr}
        )),
        object2State.withCovariance(Covariance(
            object2State.accessInstant(),
            object2PositionCovariance,
            encounterFrameSPtr,
            {Frisbee2015::CartesianPositionSubsetSPtr}
        )),
    };

    return probabilityOfCollisionAlgorithmSPtr_.value()->computeProbabilityOfCollision(
        degenerateCloseApproach, aHardBody1SPtr, aHardBody2SPtr
    );
}

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
