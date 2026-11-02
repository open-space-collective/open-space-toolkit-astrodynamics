/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <optional>
#include <string>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Foster1992.hpp>
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

const Shared<const CoordinateSubset> Foster1992::CartesianPositionSubsetSPtr = CartesianPosition::Default();

Foster1992::Foster1992(
    const Derived& aRelativeSpeedThreshold,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& anAngularStepSize,
    const Size& aNumberOfAnnularSectors
)
    : ProbabilityOfCollisionAlgorithm("Foster 1992", false),
      relativeSpeedThreshold_(aRelativeSpeedThreshold),
      frameSPtr_(aFrameSPtr),
      numberOfAnnularSectors_(aNumberOfAnnularSectors),
      angularStepSize_(Real::Undefined()),
      angularSliceSines_(Array<double>::Empty()),
      angularSliceCosines_(Array<double>::Empty())
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!aFrameSPtr->isQuasiInertial())
    {
        throw ostk::core::error::runtime::Wrong("Frame", aFrameSPtr->getName());
    }

    if (!anAngularStepSize.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Angular step size");
    }

    const Real angularStepSizeDegrees = anAngularStepSize.inDegrees();

    if ((angularStepSizeDegrees <= 0.0) || (angularStepSizeDegrees > 360.0))
    {
        throw ostk::core::error::runtime::Wrong("Angular step size", anAngularStepSize.toString());
    }

    if (aNumberOfAnnularSectors == 0)
    {
        throw ostk::core::error::runtime::Wrong("Number of annular sectors", std::to_string(aNumberOfAnnularSectors));
    }

    // Number of angular slices covering a full turn, rounded up if the angular step size does not divide it evenly (the
    // relative margin prevents a step size dividing it evenly, e.g. 0.5 deg, from being rounded up because of rounding
    // errors)
    const Size angularSliceCount =
        static_cast<Size>(std::ceil((2.0 * M_PI / anAngularStepSize.inRadians()) * (1.0 - 1e-12)));

    angularStepSize_ = 2.0 * M_PI / angularSliceCount;

    // Sine and cosine of the angle at the center of each angular slice
    angularSliceSines_.reserve(angularSliceCount);
    angularSliceCosines_.reserve(angularSliceCount);

    for (Size j = 0; j < angularSliceCount; ++j)
    {
        const double theta = (j + 0.5) * double(angularStepSize_);

        angularSliceSines_.add(std::sin(theta));
        angularSliceCosines_.add(std::cos(theta));
    }
}

Array<String> Foster1992::identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const
{
    Array<String> unsatisfiedAssumptions = Array<String>::Empty();

    const std::optional<Covariance> object1Covariance = aCloseApproach.getObject1State().getCovariance();
    if (!object1Covariance.has_value() ||
        !object1Covariance->getCoordinateSubsets().contains(Foster1992::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 1");
    }

    const std::optional<Covariance> object2Covariance = aCloseApproach.getObject2State().getCovariance();
    if (!object2Covariance.has_value() ||
        !object2Covariance->getCoordinateSubsets().contains(Foster1992::CartesianPositionSubsetSPtr))
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

Real Foster1992::computeProbabilityOfCollision(
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

    // Miss distance and combined position uncertainty along the principal axes of the combined covariance, in which
    // the combined covariance is diagonal (the probability of collision being invariant under this rotation)
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

    // Center of the hard-body circle in polar coordinates (R0, phi), the U and W axes being the x and y principal axes
    // (phi being measured from the W axis: (xm, ym) = (R0 * sin(phi), R0 * cos(phi)))
    const Real xm = rotatedEncounterFrameParameters.xM.inMeters();
    const Real ym = rotatedEncounterFrameParameters.yM.inMeters();

    return this->computeProbabilityOfCollision_(
        std::hypot(double(xm), double(ym)),
        Angle::Radians(std::atan2(double(xm), double(ym))),
        rotatedEncounterFrameParameters.sigmaX.inMeters(),
        rotatedEncounterFrameParameters.sigmaY.inMeters(),
        combinedHardBodyRadius
    );
}

Real Foster1992::computeProbabilityOfCollision_(
    const Real& aR0, const Angle& aPhi, const Real& aSigmaU, const Real& aSigmaW, const Real& anObj
) const
{
    const double sigmaU = aSigmaU;
    const double sigmaW = aSigmaW;
    const double obj = anObj;

    // Center of the hard-body circle along the U and W axes
    const double u0 = aR0 * std::sin(aPhi.inRadians());
    const double w0 = aR0 * std::cos(aPhi.inRadians());

    const double dTheta = angularStepSize_;
    const double dR = obj / numberOfAnnularSectors_;

    // Alfano (2007), Equation 2: polar coordinates (r, theta) centered on the hard-body circle. The probability density
    // function is evaluated at the center of each cell (annular sector and angular slice), with the midpoint rule, the
    // area of a cell being r * dR * dTheta.
    double sum = 0.0;

    for (Size i = 0; i < numberOfAnnularSectors_; ++i)
    {
        const double r = (i + 0.5) * dR;

        for (Size j = 0; j < angularSliceSines_.getSize(); ++j)
        {
            const double u = (u0 - r * angularSliceSines_[j]) / sigmaU;
            const double w = (w0 - r * angularSliceCosines_[j]) / sigmaW;

            sum += std::exp(-0.5 * (u * u + w * w)) * r;
        }
    }

    const double probabilityOfCollision = sum * dR * dTheta / (2.0 * M_PI * sigmaU * sigmaW);

    // The midpoint rule may overestimate the probability of collision, up to values greater than 1, when the
    // probability density function is poorly resolved by the slices: if a standard deviation is small compared to the
    // radial or angular step size (e.g. when the combined hard-body radius is much larger than the smaller standard
    // deviation), the few slices close to the peak of the density are weighted by their full area although the density
    // decays sharply across them. A probability being at most 1, the result is capped.
    return std::min(probabilityOfCollision, 1.0);
}

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
