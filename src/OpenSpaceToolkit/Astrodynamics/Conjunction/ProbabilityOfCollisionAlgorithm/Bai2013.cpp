/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Bai2013.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace probabilityofcollisionalgorithm
{

using ostk::core::container::Tuple;

using ostk::physics::coordinate::Velocity;
using ostk::physics::unit::Angle;

using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;

const Shared<const CoordinateSubset> Bai2013::CartesianPositionSubsetSPtr = CartesianPosition::Default();

Bai2013::Bai2013(
    const Derived& aRelativeVelocityThreshold,
    const Real& anEccentricityThreshold,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter
)
    : relativeVelocityThreshold_(aRelativeVelocityThreshold),
      eccentricityThreshold_(anEccentricityThreshold),
      frameSPtr_(aFrameSPtr),
      gravitationalParameter_(aGravitationalParameter)
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

String Bai2013::getName() const
{
    return "Bai 2013";
}

bool Bai2013::isMaximumProbabilityOfCollisionAlgorithm() const
{
    return false;
}

Array<String> Bai2013::identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const
{
    Array<String> unsatisfiedAssumptions = Array<String>::Empty();

    const CovarianceMatrix object1CovarianceMatrix = aCloseApproach.getObject1CovarianceMatrix();
    if (!object1CovarianceMatrix.isDefined() ||
        !object1CovarianceMatrix.getCoordinateSubsets().contains(Bai2013::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 1");
    }

    const CovarianceMatrix object2CovarianceMatrix = aCloseApproach.getObject2CovarianceMatrix();
    if (!object2CovarianceMatrix.isDefined() ||
        !object2CovarianceMatrix.getCoordinateSubsets().contains(Bai2013::CartesianPositionSubsetSPtr))
    {
        unsatisfiedAssumptions.add("No position uncertainty for Object 2");
    }

    const Real object1Eccentricity = this->computeEccentricity(aCloseApproach.getObject1State());
    if (object1Eccentricity > eccentricityThreshold_)
    {
        unsatisfiedAssumptions.add(String::Format(
            "Object 1 eccentricity [{}] is above the threshold [{}]",
            object1Eccentricity.toString(),
            eccentricityThreshold_.toString()
        ));
    }

    const Real object2Eccentricity = this->computeEccentricity(aCloseApproach.getObject2State());
    if (object2Eccentricity > eccentricityThreshold_)
    {
        unsatisfiedAssumptions.add(String::Format(
            "Object 2 eccentricity [{}] is above the threshold [{}]",
            object2Eccentricity.toString(),
            eccentricityThreshold_.toString()
        ));
    }

    // TODO: check alignment of principal axes

    return unsatisfiedAssumptions;
}

Real Bai2013::computeProbabilityOfCollision(const CloseApproach& aCloseApproach, const Length& aCombinedHardBodyRadius)
    const
{
    const State object1State = aCloseApproach.getObject1State();
    const State object2State = aCloseApproach.getObject2State();

    // RSW frames (equivalent to QSW frames)
    const Shared<const LocalOrbitalFrameFactory> qswFrameFactorySPtr = LocalOrbitalFrameFactory::QSW(frameSPtr_);

    const Shared<const Frame> object1QSWFrameSPtr = qswFrameFactorySPtr->generateFrame(object1State);
    const Shared<const Frame> object2QSWFrameSPtr = qswFrameFactorySPtr->generateFrame(object2State);

    // Compute miss distance components (r, s, w)
    const Tuple<Length, Length, Length> missDistanceComponents =
        aCloseApproach.computeMissDistanceComponentsInLocalOrbitalFrame(qswFrameFactorySPtr);

    const Real r = std::get<0>(missDistanceComponents).inMeters();
    const Real s = std::get<1>(missDistanceComponents).inMeters();
    const Real w = std::get<2>(missDistanceComponents).inMeters();

    // Compute angle between inertial velocity vectors (phi)
    const Real phi =
        Angle::Between(
            object1State.inFrame(frameSPtr_).getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates(),
            object2State.inFrame(frameSPtr_).getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates()
        )
            .inRadians();

    // Compute RSW position uncertainty matrices
    const CovarianceMatrix object1RSWPositionUncertainty = aCloseApproach.getObject1CovarianceMatrix()
                                                               .reduce({Bai2013::CartesianPositionSubsetSPtr})
                                                               .inFrame(object1QSWFrameSPtr);
    const CovarianceMatrix object2RSWPositionUncertainty = aCloseApproach.getObject2CovarianceMatrix()
                                                               .reduce({Bai2013::CartesianPositionSubsetSPtr})
                                                               .inFrame(object2QSWFrameSPtr);

    const Real sigma_r_squared = object1RSWPositionUncertainty.accessCoordinates()(0, 0) +
                                 object2RSWPositionUncertainty.accessCoordinates()(0, 0);
    const Real sigma_s_squared = object1RSWPositionUncertainty.accessCoordinates()(1, 1) +
                                 object2RSWPositionUncertainty.accessCoordinates()(1, 1);
    const Real sigma_w_squared = object1RSWPositionUncertainty.accessCoordinates()(2, 2) +
                                 object2RSWPositionUncertainty.accessCoordinates()(2, 2);

    const Real sigma_sw_squared =
        sigma_s_squared * std::pow(std::cos(0.5 * phi), 2) + sigma_w_squared * std::pow(std::sin(0.5 * phi), 2);

    // Compute the probability of collision
    const Real ra = aCombinedHardBodyRadius.inMeters();
    const Real sigma_r = std::sqrt(sigma_r_squared);
    const Real sigma_sw = std::sqrt(sigma_sw_squared);

    return std::exp(-0.5 * (r * r / sigma_r_squared + (s * s + w * w) / sigma_sw_squared)) *
           (1.0 - std::exp(-ra * ra / (2.0 * sigma_r * sigma_sw)));
}

Real Bai2013::computeEccentricity(const State& aState) const
{
    const State stateInFrame = aState.inFrame(frameSPtr_);

    const COE coe = COE::Cartesian({stateInFrame.getPosition(), stateInFrame.getVelocity()}, gravitationalParameter_);

    return coe.getEccentricity();
}

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
