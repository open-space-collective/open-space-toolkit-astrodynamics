/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BrouwerLyddaneMeanLongCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

RealCondition BrouwerLyddaneMeanLongCondition::SemiMajorAxis(
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

RealCondition BrouwerLyddaneMeanLongCondition::Eccentricity(
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

AngularCondition BrouwerLyddaneMeanLongCondition::Inclination(
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

AngularCondition BrouwerLyddaneMeanLongCondition::Inclination(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Inclination", GenerateEvaluator(COE::Element::Inclination, aFrameSPtr, aGravitationalParameter), aTargetRange
    );
}

AngularCondition BrouwerLyddaneMeanLongCondition::Aop(
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

AngularCondition BrouwerLyddaneMeanLongCondition::Aop(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Argument of Periapsis", GenerateEvaluator(COE::Element::Aop, aFrameSPtr, aGravitationalParameter), aTargetRange
    );
}

AngularCondition BrouwerLyddaneMeanLongCondition::Raan(
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

AngularCondition BrouwerLyddaneMeanLongCondition::Raan(
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

AngularCondition BrouwerLyddaneMeanLongCondition::TrueAnomaly(
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

AngularCondition BrouwerLyddaneMeanLongCondition::TrueAnomaly(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "True Anomaly", GenerateEvaluator(COE::Element::TrueAnomaly, aFrameSPtr, aGravitationalParameter), aTargetRange
    );
}

AngularCondition BrouwerLyddaneMeanLongCondition::MeanAnomaly(
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

AngularCondition BrouwerLyddaneMeanLongCondition::MeanAnomaly(
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Mean Anomaly", GenerateEvaluator(COE::Element::MeanAnomaly, aFrameSPtr, aGravitationalParameter), aTargetRange
    );
}

AngularCondition BrouwerLyddaneMeanLongCondition::EccentricAnomaly(
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

AngularCondition BrouwerLyddaneMeanLongCondition::EccentricAnomaly(
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

std::function<Real(const State&)> BrouwerLyddaneMeanLongCondition::GenerateEvaluator(
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
            const BrouwerLyddaneMeanLong orbitalElements = BrouwerLyddaneMeanLong::Cartesian(
                {stateInTargetFrame.getPosition(), stateInTargetFrame.getVelocity()}, aGravitationalParameter
            );

            // Return the requested element value
            switch (anElement)
            {
                case COE::Element::SemiMajorAxis:
                    return orbitalElements.getSemiMajorAxis().inMeters();
                case COE::Element::Eccentricity:
                    return orbitalElements.getEccentricity();
                case COE::Element::Inclination:
                    return orbitalElements.getInclination().inRadians(0.0, Real::TwoPi());
                case COE::Element::Aop:
                    return orbitalElements.getAop().inRadians(0.0, Real::TwoPi());
                case COE::Element::Raan:
                    return orbitalElements.getRaan().inRadians(0.0, Real::TwoPi());
                case COE::Element::TrueAnomaly:
                    return orbitalElements.getTrueAnomaly().inRadians(0.0, Real::TwoPi());
                case COE::Element::MeanAnomaly:
                    return orbitalElements.getMeanAnomaly().inRadians(0.0, Real::TwoPi());
                case COE::Element::EccentricAnomaly:
                    return orbitalElements.getEccentricAnomaly().inRadians(0.0, Real::TwoPi());
                case COE::Element::ArgumentOfLatitude:
                    return orbitalElements.getArgumentOfLatitude().inRadians(0.0, Real::TwoPi());
                default:
                    throw ostk::core::error::runtime::Wrong("Element");
            }
        }
        catch (const std::exception& e)
        {
            throw ostk::core::error::RuntimeError("Cannot evaluate Brouwer-Lyddane Mean Long: [{}].", e.what());
        }
    };
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
