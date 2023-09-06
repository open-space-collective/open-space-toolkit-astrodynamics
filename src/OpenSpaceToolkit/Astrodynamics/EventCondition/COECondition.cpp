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
using ostk::astro::trajectory::State;

COECondition::COECondition(
    const String& aName,
    const Criterion& aCriterion,
    const COE::Element& anElement,
    const Shared<const Frame>& aFrameSPtr,
    const Real& aTarget,
    const Derived& aGravitationalParameter
)
    : RealCondition(
          aName, aCriterion, GenerateEvaluator(anElement, aFrameSPtr, aGravitationalParameter, aTarget), aTarget
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

Shared<const Frame> COECondition::getFrame() const
{
    return frameSPtr_;
}

COECondition COECondition::SemiMajorAxis(
    const Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Length& aSemiMajorAxis,
    const Derived& aGravitationalParameter
)
{
    return {
        "Semi-major axis Condition",
        aCriterion,
        COE::Element::SemiMajorAxis,
        aFrameSPtr,
        aSemiMajorAxis.inMeters(),
        aGravitationalParameter,
    };
}

COECondition COECondition::Eccentricity(
    const Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Real& anEccentricity,
    const Derived& aGravitationalParameter
)
{
    return {
        "Eccentricity Condition",
        aCriterion,
        COE::Element::Eccentricity,
        aFrameSPtr,
        anEccentricity,
        aGravitationalParameter,
    };
}

COECondition COECondition::Inclination(
    const Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& anInclination,
    const Derived& aGravitationalParameter
)
{
    return {
        "Inclination Condition",
        aCriterion,
        COE::Element::Inclination,
        aFrameSPtr,
        anInclination.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::Aop(
    const Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& anArgumentOfPeriapsis,
    const Derived& aGravitationalParameter
)
{
    return {
        "Argument of periapsis Condition",
        aCriterion,
        COE::Element::Aop,
        aFrameSPtr,
        anArgumentOfPeriapsis.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::Raan(
    const Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& aRightAscensionOfAscendingNode,
    const Derived& aGravitationalParameter
)
{
    return {
        "Right angle of ascending node Condition",
        aCriterion,
        COE::Element::Raan,
        aFrameSPtr,
        aRightAscensionOfAscendingNode.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::TrueAnomaly(
    const Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& aTrueAnomaly,
    const Derived& aGravitationalParameter
)
{
    return {
        "True anomaly Condition",
        aCriterion,
        COE::Element::TrueAnomaly,
        aFrameSPtr,
        aTrueAnomaly.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::MeanAnomaly(
    const Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& aMeanAnomaly,
    const Derived& aGravitationalParameter
)
{
    return {
        "Mean anomaly Condition",
        aCriterion,
        COE::Element::MeanAnomaly,
        aFrameSPtr,
        aMeanAnomaly.inRadians(),
        aGravitationalParameter,
    };
}

COECondition COECondition::EccentricAnomaly(
    const Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& anEccentricAnomaly,
    const Derived& aGravitationalParameter
)
{
    return {
        "Eccentric anomaly Condition",
        aCriterion,
        COE::Element::EccentricAnomaly,
        aFrameSPtr,
        anEccentricAnomaly.inRadians(),
        aGravitationalParameter,
    };
}

std::function<Real(const State&)> COECondition::GenerateEvaluator(
    const COE::Element& anElement,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter,
    const Real& aTarget
)
{
    // The parameters must be captured by value as the function is being initialized during construction
    return [anElement, aFrameSPtr, aGravitationalParameter, aTarget](const State& aState) -> Real
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
                return RealCondition::GetCrossZDelta(coe.getInclination().inRadians(), aTarget);
            case COE::Element::Aop:
                return RealCondition::GetCrossZDelta(coe.getAop().inRadians(), aTarget);
            case COE::Element::Raan:
                return RealCondition::GetCrossZDelta(coe.getRaan().inRadians(), aTarget);
            case COE::Element::TrueAnomaly:
                return RealCondition::GetCrossZDelta(coe.getTrueAnomaly().inRadians(), aTarget);
            case COE::Element::MeanAnomaly:
                return RealCondition::GetCrossZDelta(coe.getMeanAnomaly().inRadians(), aTarget);
            case COE::Element::EccentricAnomaly:
                return RealCondition::GetCrossZDelta(coe.getEccentricAnomaly().inRadians(), aTarget);
        }

        throw ostk::core::error::RuntimeError("Invalid element.");
    };
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
