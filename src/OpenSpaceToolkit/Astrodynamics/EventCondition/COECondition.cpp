/// Apache License 2.0

#include <iostream>

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
    const Criterion& aCriterion,
    const COE::Element& anElement,
    const Real& aTarget,
    const Derived& aGravitationalParameter
)
    : RealCondition(aName, aCriterion, GenerateEvaluator(anElement, aGravitationalParameter), aTarget),
      element_(anElement),
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
    const Criterion& aCriterion, const Length& aSemiMajorAxis, const Derived& aGravitationalParameter
)
{
    return {
        "Semi-major axis Condition",
        aCriterion,
        COE::Element::SemiMajorAxis,
        aSemiMajorAxis.inMeters(),
        aGravitationalParameter,
    };
}

COECondition COECondition::Eccentricity(
    const Criterion& aCriterion, const Real& anEccentricity, const Derived& aGravitationalParameter
)
{
    return {
        "Eccentricity Condition",
        aCriterion,
        COE::Element::Eccentricity,
        anEccentricity,
        aGravitationalParameter,
    };
}

COECondition COECondition::Inclination(
    const Criterion& aCriterion, const Angle& anInclination, const Derived& aGravitationalParameter
)
{
    return {
        "Inclination Condition",
        aCriterion,
        COE::Element::Inclination,
        anInclination.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::Aop(
    const Criterion& aCriterion, const Angle& anArgumentOfPeriapsis, const Derived& aGravitationalParameter
)
{
    return {
        "Argument of periapsis Condition",
        aCriterion,
        COE::Element::Aop,
        anArgumentOfPeriapsis.inRadians(),
        aGravitationalParameter
    };
}

COECondition COECondition::Raan(
    const Criterion& aCriterion, const Angle& aRightAscensionOfAscendingNode, const Derived& aGravitationalParameter
)
{
    return {
        "Right angle of ascending node Condition",
        aCriterion,
        COE::Element::Raan,
        aRightAscensionOfAscendingNode.inRadians(),
        aGravitationalParameter
    };
}

COECondition COECondition::TrueAnomaly(
    const Criterion& aCriterion, const Angle& aTrueAnomaly, const Derived& aGravitationalParameter
)
{
    return {
        "True anomaly Condition",
        aCriterion,
        COE::Element::TrueAnomaly,
        aTrueAnomaly.inRadians(),
        aGravitationalParameter
    };
}

COECondition COECondition::MeanAnomaly(
    const Criterion& aCriterion, const Angle& aMeanAnomaly, const Derived& aGravitationalParameter
)
{
    return {
        "Mean anomaly Condition",
        aCriterion,
        COE::Element::MeanAnomaly,
        aMeanAnomaly.inRadians(),
        aGravitationalParameter
    };
}

COECondition COECondition::EccentricAnomaly(
    const Criterion& aCriterion, const Angle& anEccentricAnomaly, const Derived& aGravitationalParameter
)
{
    return {
        "Eccentric anomaly Condition",
        aCriterion,
        COE::Element::EccentricAnomaly,
        anEccentricAnomaly.inRadians(),
        aGravitationalParameter
    };
}

std::function<Real(const VectorXd&, const Real&)> COECondition::GenerateEvaluator(
    const COE::Element& anElement, const Derived& aGravitationalParameter
)
{
    // The parameters must be captured by value as the function is being initialized during construction
    return
        [anElement, aGravitationalParameter](const VectorXd& aStateVector, [[maybe_unused]] const Real& aTime) -> Real
    {
        // TBI: get frame and coordinate positions from state
        // TBI: specify a frame in which the COE is to be computed
        const Position position = Position::Meters(aStateVector.segment(0, 3), Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond(aStateVector.segment(3, 3), Frame::GCRF());

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
