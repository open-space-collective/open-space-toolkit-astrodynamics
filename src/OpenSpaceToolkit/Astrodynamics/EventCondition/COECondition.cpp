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

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

using ostk::astro::trajectory::orbit::models::kepler::COE;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

COECondition::~COECondition() {}

Real COECondition::getTarget() const
{
    return target_;
}

Derived COECondition::getGravitationalParameter() const
{
    return {gravitationalParameterInSI_, GravitationalParameterSIUnit};
}

Real COECondition::evaluate(const VectorXd& aStateVector, const Real& aTime) const
{
    (void)aTime;

    const Vector3d positionVector = aStateVector.segment(0, 3);
    const Vector3d velocityVector = aStateVector.segment(3, 3);

    // const Real position = positionVector.norm();
    // const Real velocity = velocityVector.norm();

    return evaluator_(positionVector, velocityVector) - target_;
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
      gravitationalParameterInSI_(aGravitationalParameter.in(GravitationalParameterSIUnit)),
      evaluator_(getEvaluator(anElement))
{
}

std::function<Real(const Vector3d&, const Vector3d&)> COECondition::getEvaluator(const Element& anElement) const
{
    switch (anElement)
    {
        case COECondition::Element::SemiMajorAxis:
            return [this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
            {
                const Real position = aPositionVector.norm();
                const Real velocity = aVelocityVector.norm();

                const Real E = (0.5 * velocity * velocity) - (gravitationalParameterInSI_ / position);

                if (E == 0.0)
                {
                    throw ostk::core::error::runtime::Wrong("Specific orbital energy");
                }

                const Real a_m = -gravitationalParameterInSI_ / (2.0 * E);

                return a_m;
            };

        case COECondition::Element::Eccentricity:
            return [this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
            {
                const Real position = aPositionVector.norm();
                const Real velocity = aVelocityVector.norm();

                const Vector3d eccentricityVector =
                    (1.0 / gravitationalParameterInSI_) *
                    ((((velocity * velocity) - (gravitationalParameterInSI_ / position)) * aPositionVector) -
                     ((aPositionVector.dot(aVelocityVector)) * (aVelocityVector)));

                const Real e = eccentricityVector.norm();

                if ((std::abs(1.0 - e)) <= Real::Epsilon())
                {
                    throw ostk::core::error::runtime::ToBeImplemented("Support for parabolic orbits.");
                }

                return e;
            };

        case COECondition::Element::Inclination:
            return [this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
            {
                const Vector3d angularMomentumVector = aPositionVector.cross(aVelocityVector);

                const Real angularMomentum = angularMomentumVector.norm();

                if (angularMomentum == 0.0)
                {
                    throw ostk::core::error::runtime::Wrong("Angular momentum");
                }

                return std::acos(angularMomentumVector(2) / angularMomentum);
            };

        case COECondition::Element::ArgumentOfPeriapsis:
            return [this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
            {
                // ecc
                const Real position = aPositionVector.norm();
                const Real velocity = aVelocityVector.norm();

                const Vector3d eccentricityVector =
                    (1.0 / gravitationalParameterInSI_) *
                    ((((velocity * velocity) - (gravitationalParameterInSI_ / position)) * aPositionVector) -
                     ((aPositionVector.dot(aVelocityVector)) * (aVelocityVector)));

                const Real e = eccentricityVector.norm();

                if ((std::abs(1.0 - e)) <= Real::Epsilon())
                {
                    throw ostk::core::error::runtime::ToBeImplemented("Support for parabolic orbits.");
                }

                // inc

                const Vector3d angularMomentumVector = aPositionVector.cross(aVelocityVector);

                const Real angularMomentum = angularMomentumVector.norm();

                if (angularMomentum == 0.0)
                {
                    throw ostk::core::error::runtime::Wrong("Angular momentum");
                }

                const Real i_rad = std::acos(angularMomentumVector(2) / angularMomentum);

                // Node

                const Vector3d nodeVector = Vector3d::Z().cross(angularMomentumVector);

                const Real node = nodeVector.norm();

                const Real tolerance = 1e-11;

                if ((e >= tolerance) && ((i_rad >= tolerance) && (i_rad <= (Real::Pi() - tolerance))))
                {
                    if (node == 0.0)
                    {
                        throw ostk::core::error::runtime::Undefined("Node");
                    }

                    const Real aop_rad = std::acos((nodeVector.dot(eccentricityVector)) / (node * e));

                    if (eccentricityVector(2) < 0.0)
                    {
                        return Real::TwoPi() - aop_rad;
                    }

                    return aop_rad;
                }
                else if ((e >= tolerance) && ((i_rad < tolerance) || (i_rad > (Real::Pi() - tolerance))))
                {
                    const Real aop_rad = std::acos(eccentricityVector(0) / e);
                    if (eccentricityVector(1) < 0.0)
                    {
                        return Real::TwoPi() - aop_rad;
                    }

                    if (i_rad > (Real::Pi() - tolerance))
                    {
                        return aop_rad * -1.0;
                    }

                    if (aop_rad < 0.0)
                    {
                        return aop_rad + Real::TwoPi();
                    }

                    return aop_rad;
                }
                else if ((e < tolerance) && ((i_rad >= tolerance) && (i_rad <= (Real::Pi() - tolerance))))
                {
                    return 0.0;
                }
                else if ((e < tolerance) && ((i_rad < tolerance) || (i_rad > (Real::Pi() - tolerance))))
                {
                    return 0.0;
                }
            };

        case COECondition::Element::RightAngleOfAscendingNode:
            return [this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
            {
                throw ostk::core::error::runtime::ToBeImplemented(
                    "COECondition::getEvaluator(COECondition::Element::RightAngleOfAscendingNode)"
                );
            };

        case COECondition::Element::TrueAnomaly:
            return [this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
            {
                throw ostk::core::error::runtime::ToBeImplemented(
                    "COECondition::getEvaluator(COECondition::Element::TrueAnomaly)"
                );
            };

        case COECondition::Element::MeanAnomaly:
            return [this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
            {
                throw ostk::core::error::runtime::ToBeImplemented(
                    "COECondition::getEvaluator(COECondition::Element::MeanAnomaly)"
                );
            };

        case COECondition::Element::EccentricAnomaly:
            return [this](const Vector3d& aPositionVector, const Vector3d& aVelocityVector) -> Real
            {
                throw ostk::core::error::runtime::ToBeImplemented(
                    "COECondition::getEvaluator(COECondition::Element::EccentricAnomaly)"
                );
            };
    }
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
