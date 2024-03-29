/// Apache License 2.0

#include <iostream>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;

using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;

using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

RealCondition COECondition::SemiMajorAxis(
    const RealCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Semi-major axis Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::SemiMajorAxis, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

RealCondition COECondition::Eccentricity(
    const RealCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Eccentricity Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::Eccentricity, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition COECondition::Inclination(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Inclination Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::Inclination, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition COECondition::Aop(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Argument of periapsis Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::Aop, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition COECondition::Raan(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Right angle of ascending node Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::Raan, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition COECondition::TrueAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "True anomaly Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::TrueAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition COECondition::MeanAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Mean anomaly Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::MeanAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition COECondition::EccentricAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Eccentric anomaly Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::EccentricAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

std::function<Real(const State&)> COECondition::GenerateEvaluator(
    const COE::Element& anElement, const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter
)
{
    // The parameters must be captured by value as the function is being initialized during construction
    return [anElement, aFrameSPtr, aGravitationalParameter](const State& aState) -> Real
    {
        const State stateInFrame = aState.inFrame(aFrameSPtr);
        const Position positionInFrame = stateInFrame.getPosition();
        const Velocity velocityInFrame = stateInFrame.getVelocity();

        const COE coe = COE::Cartesian({positionInFrame, velocityInFrame}, aGravitationalParameter);

        switch (anElement)
        {
            case COE::Element::SemiMajorAxis:
                return coe.getSemiMajorAxis().inMeters();
            case COE::Element::Eccentricity:
                return coe.getEccentricity();
            case COE::Element::Inclination:
                return coe.getInclination().inRadians(0.0, Real::TwoPi());
            case COE::Element::Aop:
                return coe.getAop().inRadians(0.0, Real::TwoPi());
            case COE::Element::Raan:
                return coe.getRaan().inRadians(0.0, Real::TwoPi());
            case COE::Element::TrueAnomaly:
                return coe.getTrueAnomaly().inRadians(0.0, Real::TwoPi());
            case COE::Element::MeanAnomaly:
                return coe.getMeanAnomaly().inRadians(0.0, Real::TwoPi());
            case COE::Element::EccentricAnomaly:
                return coe.getEccentricAnomaly().inRadians(0.0, Real::TwoPi());
        }

        throw ostk::core::error::RuntimeError("Invalid element.");
    };
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
