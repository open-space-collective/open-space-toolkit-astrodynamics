/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/OrbitalElementCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

namespace
{

template <typename OrbitalElementSet>
Real ExtractElement(const OrbitalElementSet& anOrbitalElementSet, const COE::Element& anElement)
{
    switch (anElement)
    {
        case COE::Element::SemiMajorAxis:
            return anOrbitalElementSet.getSemiMajorAxis().inMeters();
        case COE::Element::Eccentricity:
            return anOrbitalElementSet.getEccentricity();
        case COE::Element::Inclination:
            return anOrbitalElementSet.getInclination().inRadians(0.0, Real::TwoPi());
        case COE::Element::Aop:
            return anOrbitalElementSet.getAop().inRadians(0.0, Real::TwoPi());
        case COE::Element::Raan:
            return anOrbitalElementSet.getRaan().inRadians(0.0, Real::TwoPi());
        case COE::Element::TrueAnomaly:
            return anOrbitalElementSet.getTrueAnomaly().inRadians(0.0, Real::TwoPi());
        case COE::Element::MeanAnomaly:
            return anOrbitalElementSet.getMeanAnomaly().inRadians(0.0, Real::TwoPi());
        case COE::Element::EccentricAnomaly:
            return anOrbitalElementSet.getEccentricAnomaly().inRadians(0.0, Real::TwoPi());
        case COE::Element::ArgumentOfLatitude:
            return anOrbitalElementSet.getArgumentOfLatitude().inRadians(0.0, Real::TwoPi());
        default:
            throw ostk::core::error::runtime::Wrong("Element");
    }
}

}  // namespace

RealCondition OrbitalElementCondition::SemiMajorAxis(
    const Theory& aTheory,
    const RealCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return RealCondition(
        "Semi-Major Axis",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::SemiMajorAxis, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

RealCondition OrbitalElementCondition::Eccentricity(
    const Theory& aTheory,
    const RealCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return RealCondition(
        "Eccentricity",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::Eccentricity, aFrameSPtr, aGravitationalParameter),
        aTarget
    );
}

AngularCondition OrbitalElementCondition::Inclination(
    const Theory& aTheory,
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Inclination",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::Inclination, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition OrbitalElementCondition::Inclination(
    const Theory& aTheory,
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Inclination",
        GenerateEvaluator(aTheory, COE::Element::Inclination, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

AngularCondition OrbitalElementCondition::Aop(
    const Theory& aTheory,
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Argument of Periapsis",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::Aop, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition OrbitalElementCondition::Aop(
    const Theory& aTheory,
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Argument of Periapsis",
        GenerateEvaluator(aTheory, COE::Element::Aop, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

AngularCondition OrbitalElementCondition::Raan(
    const Theory& aTheory,
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Right Ascension of Ascending Node",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::Raan, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition OrbitalElementCondition::Raan(
    const Theory& aTheory,
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Right Ascension of Ascending Node",
        GenerateEvaluator(aTheory, COE::Element::Raan, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

AngularCondition OrbitalElementCondition::TrueAnomaly(
    const Theory& aTheory,
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "True Anomaly",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::TrueAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition OrbitalElementCondition::TrueAnomaly(
    const Theory& aTheory,
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "True Anomaly",
        GenerateEvaluator(aTheory, COE::Element::TrueAnomaly, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

AngularCondition OrbitalElementCondition::MeanAnomaly(
    const Theory& aTheory,
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Mean Anomaly",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::MeanAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition OrbitalElementCondition::MeanAnomaly(
    const Theory& aTheory,
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Mean Anomaly",
        GenerateEvaluator(aTheory, COE::Element::MeanAnomaly, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

AngularCondition OrbitalElementCondition::EccentricAnomaly(
    const Theory& aTheory,
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Eccentric Anomaly",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::EccentricAnomaly, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition OrbitalElementCondition::EccentricAnomaly(
    const Theory& aTheory,
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Eccentric Anomaly",
        GenerateEvaluator(aTheory, COE::Element::EccentricAnomaly, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

AngularCondition OrbitalElementCondition::ArgumentOfLatitude(
    const Theory& aTheory,
    const AngularCondition::Criterion& aCriterion,
    const Shared<const Frame>& aFrameSPtr,
    const EventCondition::Target& aTarget,
    const Derived& aGravitationalParameter
)
{
    return {
        "Argument of Latitude",
        aCriterion,
        GenerateEvaluator(aTheory, COE::Element::ArgumentOfLatitude, aFrameSPtr, aGravitationalParameter),
        aTarget
    };
}

AngularCondition OrbitalElementCondition::ArgumentOfLatitude(
    const Theory& aTheory,
    const Shared<const Frame>& aFrameSPtr,
    const Pair<Angle, Angle>& aTargetRange,
    const Derived& aGravitationalParameter
)
{
    return AngularCondition::WithinRange(
        "Argument of Latitude",
        GenerateEvaluator(aTheory, COE::Element::ArgumentOfLatitude, aFrameSPtr, aGravitationalParameter),
        aTargetRange
    );
}

std::function<Real(const State&)> OrbitalElementCondition::GenerateEvaluator(
    const Theory& aTheory,
    const COE::Element& anElement,
    const Shared<const Frame>& aFrameSPtr,
    const Derived& aGravitationalParameter
)
{
    return [aTheory, anElement, aFrameSPtr, aGravitationalParameter](const State& aState) -> Real
    {
        try
        {
            // Transform state to the target frame if necessary
            const State stateInTargetFrame = aState.inFrame(aFrameSPtr);

            const COE::CartesianState cartesianState = {
                stateInTargetFrame.getPosition(), stateInTargetFrame.getVelocity()
            };

            // Compute the orbital element set from the position and velocity, using the requested theory
            switch (aTheory)
            {
                case Theory::Osculating:
                    return ExtractElement(COE::Cartesian(cartesianState, aGravitationalParameter), anElement);
                case Theory::BrouwerLyddaneMeanShort:
                    return ExtractElement(
                        BrouwerLyddaneMeanShort::Cartesian(cartesianState, aGravitationalParameter), anElement
                    );
                case Theory::BrouwerLyddaneMeanLong:
                    return ExtractElement(
                        BrouwerLyddaneMeanLong::Cartesian(cartesianState, aGravitationalParameter), anElement
                    );
                default:
                    throw ostk::core::error::runtime::Wrong("Theory");
            }
        }
        catch (const std::exception& e)
        {
            throw ostk::core::error::RuntimeError("Cannot evaluate Orbital Element: [{}].", e.what());
        }
    };
}

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk
