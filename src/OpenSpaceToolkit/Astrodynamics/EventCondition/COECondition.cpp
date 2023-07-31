/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

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

COECondition::COECondition(
    const String& aName,
    const Criteria& aCriteria,
    const COECondition::Element& anElement,
    const Real& aTarget,
    const Derived& aGravitationalParameter
)
    : EventCondition(aName, aCriteria),
      element_(anElement),
      target_(aTarget),
      gravitationalParameter_(aGravitationalParameter),
      evaluator_(getEvaluator(anElement))
{
}

COECondition::~COECondition() {}

Real COECondition::getTarget() const
{
    return target_;
}

Derived COECondition::getGravitationalParameter() const
{
    return gravitationalParameter_;
}

COECondition::Element COECondition::getElement() const
{
    return element_;
}

Real COECondition::evaluate(const VectorXd& aStateVector, const Real& aTime) const
{
    (void)aTime;

    const Vector3d positionVector = aStateVector.segment(0, 3);
    const Vector3d velocityVector = aStateVector.segment(3, 3);

    return evaluator_(positionVector, velocityVector) - target_;
}

String COECondition::StringFromElement(const COECondition::Element& anElement)
{
    switch (anElement)
    {
        case Element::SemiMajorAxis:
            return "Semi-major axis";
        case Element::Eccentricity:
            return "Eccentricity";
        case Element::Inclination:
            return "Inclination";
        case Element::ArgumentOfPeriapsis:
            return "Argument of periapsis";
        case Element::RightAngleOfAscendingNode:
            return "Right angle of ascending node";
        case Element::TrueAnomaly:
            return "True anomaly";
        case Element::MeanAnomaly:
            return "Mean anomaly";
        case Element::EccentricAnomaly:
            return "Eccentric anomaly";
    }

    throw ostk::core::error::runtime::Wrong("Element");
}

COECondition COECondition::SemiMajorAxis(
    const Criteria& aCriteria, const Length& aSemiMajorAxis, const Derived& aGravitationalParameter
)
{
    return {
        "Semi-major axis Condition",
        aCriteria,
        Element::SemiMajorAxis,
        aSemiMajorAxis.inMeters(),
        aGravitationalParameter,
    };
}

COECondition COECondition::Eccentricity(
    const Criteria& aCriteria, const Real& anEccentricity, const Derived& aGravitationalParameter
)
{
    return {
        "Eccentricity Condition",
        aCriteria,
        Element::Eccentricity,
        anEccentricity,
        aGravitationalParameter,
    };
}

COECondition COECondition::Inclination(
    const Criteria& aCriteria, const Angle& anInclination, const Derived& aGravitationalParameter
)
{
    return {
        "Inclination Condition",
        aCriteria,
        Element::Inclination,
        anInclination.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::ArgumentOfPeriapsis(
    const Criteria& aCriteria, const Angle& anArgumentOfPeriapsis, const Derived& aGravitationalParameter
)
{
    return {
        "Argument of periapsis Condition",
        aCriteria,
        Element::ArgumentOfPeriapsis,
        anArgumentOfPeriapsis.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::RightAngleOfAscendingNode(
    const Criteria& aCriteria, const Angle& aRightAngleOfAscendingNode, const Derived& aGravitationalParameter
)
{
    return {
        "Right angle of ascending node Condition",
        aCriteria,
        Element::RightAngleOfAscendingNode,
        aRightAngleOfAscendingNode.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::TrueAnomaly(
    const Criteria& aCriteria, const Angle& aTrueAnomaly, const Derived& aGravitationalParameter
)
{
    return {
        "True anomaly Condition",
        aCriteria,
        Element::TrueAnomaly,
        aTrueAnomaly.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::MeanAnomaly(
    const Criteria& aCriteria, const Angle& aMeanAnomaly, const Derived& aGravitationalParameter
)
{
    return {
        "Mean anomaly Condition",
        aCriteria,
        Element::MeanAnomaly,
        aMeanAnomaly.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::EccentricAnomaly(
    const Criteria& aCriteria, const Angle& anEccentricAnomaly, const Derived& aGravitationalParameter
)
{
    return {
        "Eccentric anomaly Condition",
        aCriteria,
        Element::EccentricAnomaly,
        anEccentricAnomaly.inRadians(),
        aGravitationalParameter,
    };
}

std::function<Real(const Vector3d&, const Vector3d&)> COECondition::getEvaluator(const Element& anElement) const
{
    return [anElement, this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
    {
        // TBI: Get frame from Broker
        const Position position = Position::Meters(aPositionVector, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond(aVelocityVector, Frame::GCRF());

        const COE coe = COE::Cartesian({position, velocity}, this->gravitationalParameter_);

        switch (anElement)
        {
            case Element::SemiMajorAxis:
                return coe.getSemiMajorAxis().inMeters();
            case Element::Eccentricity:
                return coe.getEccentricity();
            case Element::Inclination:
                return coe.getInclination().inRadians();
            case Element::ArgumentOfPeriapsis:
                return coe.getAop().inRadians();
            case Element::RightAngleOfAscendingNode:
                return coe.getRaan().inRadians();
            case Element::TrueAnomaly:
                return coe.getTrueAnomaly().inRadians();
            case Element::MeanAnomaly:
                return coe.getMeanAnomaly().inRadians();
            case Element::EccentricAnomaly:
                return coe.getEccentricAnomaly().inRadians();
        }

        throw ostk::core::error::RuntimeError("Invalid element.");
    };
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
