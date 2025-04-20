/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

RealCondition COECondition::SemiMajorAxis(
    const RealCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return RealCondition(
        "Semi-Major Axis",
        aCriterion,
        GenerateEvaluator(COE::Element::SemiMajorAxis, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

RealCondition COECondition::Eccentricity(
    const RealCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return RealCondition(
        "Eccentricity",
        aCriterion,
        GenerateEvaluator(COE::Element::Eccentricity, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

AngularCondition COECondition::Inclination(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition(
        "Inclination",
        aCriterion,
        GenerateEvaluator(COE::Element::Inclination, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

AngularCondition COECondition::Inclination(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Inclination", GenerateEvaluator(COE::Element::Inclination, aFrameSPtr, aGravitationalParameter), aTargetRange
    );
}

AngularCondition COECondition::Aop(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition(
        "Argument of Periapsis",
        aCriterion,
        GenerateEvaluator(COE::Element::Aop, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

AngularCondition COECondition::Aop(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Argument of Periapsis", GenerateEvaluator(COE::Element::Aop, aFrameSPtr, aGravitationalParameter), aTargetRange
    );
}

AngularCondition COECondition::Raan(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition(
        "Right Ascension of Ascending Node",
        aCriterion,
        GenerateEvaluator(COE::Element::Raan, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

AngularCondition COECondition::Raan(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Right Ascension of Ascending Node",
        GenerateEvaluator(COE::Element::Raan, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

AngularCondition COECondition::TrueAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition(
        "True Anomaly",
        aCriterion,
        GenerateEvaluator(COE::Element::TrueAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

AngularCondition COECondition::TrueAnomaly(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "True Anomaly", GenerateEvaluator(COE::Element::TrueAnomaly, aFrameSPtr, aGravitationalParameter), aTargetRange
    );
}

AngularCondition COECondition::MeanAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition(
        "Mean Anomaly",
        aCriterion,
        GenerateEvaluator(COE::Element::MeanAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

AngularCondition COECondition::MeanAnomaly(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Mean Anomaly", GenerateEvaluator(COE::Element::MeanAnomaly, aFrameSPtr, aGravitationalParameter), aTargetRange
    );
}

AngularCondition COECondition::EccentricAnomaly(
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition(
        "Eccentric Anomaly",
        aCriterion,
        GenerateEvaluator(COE::Element::EccentricAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

AngularCondition COECondition::EccentricAnomaly(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Eccentric Anomaly",
        GenerateEvaluator(COE::Element::EccentricAnomaly, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

std::function<Real(const State&)> COECondition::GenerateEvaluator(
    const COE::Element& anElement, const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter
)
{
    return [anElement, aFrameSPtr, aGravitationalParameter](const State& aState) -> Real
    {
        try
        {
            // Transform state to the target frame if necessary
            const State stateInTargetFrame = aState.inFrame(aFrameSPtr);

            // Compute the COE set from the position and velocity
            const COE coe = COE::Cartesian(
                {stateInTargetFrame.getPosition(), stateInTargetFrame.getVelocity()}, aGravitationalParameter
            );

            // Return the requested element value
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
                case COE::Element::ArgumentOfLatitude:
                    return coe.getArgumentOfLatitude().inRadians();
                default:
                    throw ostk::core::error::runtime::Wrong("Element");
            }
        }
        catch (const std::exception& e)
        {
            throw ostk::core::error::RuntimeError("Cannot evaluate COE: [{}].", e.what());
        }
    };
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
