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

RealCondition COECondition::SemiMajorAxis(
    const RealCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Length& aSemiMajorAxis,
    const Derived& aGravitationalParameter
)
{
    return {
        "Semi-major axis Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::SemiMajorAxis, aFrameSPtr, aGravitationalParameter),
        aSemiMajorAxis.inMeters(),
    };
}

RealCondition COECondition::Eccentricity(
    const RealCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Real& anEccentricity,
    const Derived& aGravitationalParameter
)
{
    return {
        "Eccentricity Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::Eccentricity, aFrameSPtr, aGravitationalParameter),
        anEccentricity,
    };
}

AngularCondition COECondition::Inclination(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& anInclination,
    const Derived& aGravitationalParameter
)
{
    return {
        "Inclination Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::Inclination, aFrameSPtr, aGravitationalParameter),
        anInclination,
    };
}

AngularCondition COECondition::Aop(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& anArgumentOfPeriapsis,
    const Derived& aGravitationalParameter
)
{
    return {
        "Argument of periapsis Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::Aop, aFrameSPtr, aGravitationalParameter),
        anArgumentOfPeriapsis,
    };
}

AngularCondition COECondition::Raan(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& aRightAscensionOfAscendingNode,
    const Derived& aGravitationalParameter
)
{
    return {
        "Right angle of ascending node Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::Raan, aFrameSPtr, aGravitationalParameter),
        aRightAscensionOfAscendingNode,
    };
}

AngularCondition COECondition::TrueAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& aTrueAnomaly,
    const Derived& aGravitationalParameter
)
{
    return {
        "True anomaly Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::TrueAnomaly, aFrameSPtr, aGravitationalParameter),
        aTrueAnomaly,
    };
}

AngularCondition COECondition::MeanAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& aMeanAnomaly,
    const Derived& aGravitationalParameter
)
{
    return {
        "Mean anomaly Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::MeanAnomaly, aFrameSPtr, aGravitationalParameter),
        aMeanAnomaly,
    };
}

AngularCondition COECondition::EccentricAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const Angle& anEccentricAnomaly,
    const Derived& aGravitationalParameter
)
{
    return {
        "Eccentric anomaly Condition",
        aCriterion,
        COECondition::GenerateEvaluator(COE::Element::EccentricAnomaly, aFrameSPtr, aGravitationalParameter),
        anEccentricAnomaly,
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
}  // namespace astro
}  // namespace ostk
