/// Apache License 2.0

#include <iostream>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

COECondition::COECondition(
    const String& aName,
    const Criteria& aCriteria,
    const COE::Element& anElement,
    const Real& aTarget,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
    : RealEventCondition(
          aName,
          aCriteria,
          GenerateEvaluator(anElement, aFrameSPtr, aCoordinatesBrokerSPtr, aGravitationalParameter),
          aTarget
      ),
      element_(anElement),
      frameSPtr_(aFrameSPtr),
      gravitationalParameter_(aGravitationalParameter)
{
}

COECondition::~COECondition() {}

Derived COECondition::getGravitationalParameter() const
{
    return gravitationalParameter_;
}

COE::Element COECondition::getElement() const
{
    return element_;
}

COECondition COECondition::SemiMajorAxis(
    const Criteria& aCriteria,
    const Length& aSemiMajorAxis,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
{
    return {
        "Semi-major axis Condition",
        aCriteria,
        COE::Element::SemiMajorAxis,
        aSemiMajorAxis.inMeters(),
        aFrameSPtr,
        aGravitationalParameter,
        aCoordinatesBrokerSPtr,
    };
}

COECondition COECondition::Eccentricity(
    const Criteria& aCriteria,
    const Real& anEccentricity,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
{
    return {
        "Eccentricity Condition",
        aCriteria,
        COE::Element::Eccentricity,
        anEccentricity,
        aFrameSPtr,
        aGravitationalParameter,
        aCoordinatesBrokerSPtr,
    };
}

COECondition COECondition::Inclination(
    const Criteria& aCriteria,
    const Angle& anInclination,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
{
    return {
        "Inclination Condition",
        aCriteria,
        COE::Element::Inclination,
        anInclination.inRadians(),
        aFrameSPtr,
        aGravitationalParameter,
        aCoordinatesBrokerSPtr,
    };
}

COECondition COECondition::Aop(
    const Criteria& aCriteria,
    const Angle& anArgumentOfPeriapsis,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
{
    return {
        "Argument of periapsis Condition",
        aCriteria,
        COE::Element::Aop,
        anArgumentOfPeriapsis.inRadians(),
        aFrameSPtr,
        aGravitationalParameter,
        aCoordinatesBrokerSPtr,
    };
}

COECondition COECondition::Raan(
    const Criteria& aCriteria,
    const Angle& aRightAscensionOfAscendingNode,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
{
    return {
        "Right angle of ascending node Condition",
        aCriteria,
        COE::Element::Raan,
        aRightAscensionOfAscendingNode.inRadians(),
        aFrameSPtr,
        aGravitationalParameter,
        aCoordinatesBrokerSPtr,
    };
}

COECondition COECondition::TrueAnomaly(
    const Criteria& aCriteria,
    const Angle& aTrueAnomaly,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
{
    return {
        "True anomaly Condition",
        aCriteria,
        COE::Element::TrueAnomaly,
        aTrueAnomaly.inRadians(),
        aFrameSPtr,
        aGravitationalParameter,
        aCoordinatesBrokerSPtr,
    };
}

COECondition COECondition::MeanAnomaly(
    const Criteria& aCriteria,
    const Angle& aMeanAnomaly,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
{
    return {
        "Mean anomaly Condition",
        aCriteria,
        COE::Element::MeanAnomaly,
        aMeanAnomaly.inRadians(),
        aFrameSPtr,
        aGravitationalParameter,
        aCoordinatesBrokerSPtr,
    };
}

COECondition COECondition::EccentricAnomaly(
    const Criteria& aCriteria,
    const Angle& anEccentricAnomaly,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr
)
{
    return {
        "Eccentric anomaly Condition",
        aCriteria,
        COE::Element::EccentricAnomaly,
        anEccentricAnomaly.inRadians(),
        aFrameSPtr,
        aGravitationalParameter,
        aCoordinatesBrokerSPtr,
    };
}

std::function<Real(const VectorXd&, const Real&)> COECondition::GenerateEvaluator(
    const COE::Element& anElement,
    const Shared<const Frame>& aFrameSPtr,
    const Shared<CoordinatesBroker>& aCoordinatesBrokerSPtr,
    const Derived& aGravitationalParameter
)
{
    // The parameters must be captured by value as the function is being initialized during construction
    return [anElement, aFrameSPtr, aGravitationalParameter, aCoordinatesBrokerSPtr](
               const VectorXd& aStateVector, [[maybe_unused]] const Real& aTime
           ) -> Real
    {
        static const Shared<const CartesianPosition> positionSubset = CartesianPosition::Default();
        static const Shared<const CartesianVelocity> velocitySubset = CartesianVelocity::Default();
        const Position position =
            Position::Meters(aCoordinatesBrokerSPtr->extractCoordinates(aStateVector, positionSubset), aFrameSPtr);
        const Velocity velocity = Velocity::MetersPerSecond(
            aCoordinatesBrokerSPtr->extractCoordinates(aStateVector, velocitySubset), aFrameSPtr
        );

        const COE coe = COE::Cartesian({position, velocity}, aGravitationalParameter);

        switch (anElement)
        {
            case COE::Element::SemiMajorAxis:
                return coe.getSemiMajorAxis().inMeters();
            case COE::Element::Eccentricity:
                return coe.getEccentricity();
            case COE::Element::Inclination:
                return coe.getInclination().inRadians();
            case COE::Element::Aop:
                return coe.getAop().inRadians();
            case COE::Element::Raan:
                return coe.getRaan().inRadians();
            case COE::Element::TrueAnomaly:
                return coe.getTrueAnomaly().inRadians();
            case COE::Element::MeanAnomaly:
                return coe.getMeanAnomaly().inRadians();
            case COE::Element::EccentricAnomaly:
                return coe.getEccentricAnomaly().inRadians();
        }

        throw ostk::core::error::RuntimeError("Invalid element.");
    };
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
