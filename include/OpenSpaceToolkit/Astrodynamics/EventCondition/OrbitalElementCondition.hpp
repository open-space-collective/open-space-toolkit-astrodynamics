/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_OrbitalElementCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_OrbitalElementCondition__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::AngularCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanShort;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

/// @brief An orbital element based event condition. Supports evaluating the element using different orbital element
/// theories (osculating or Brouwer-Lyddane mean).
class OrbitalElementCondition
{
   public:
    /// @brief The orbital element theory used to compute the orbital elements from a Cartesian state
    enum class Theory
    {
        Osculating,               ///< Osculating (Classical) orbital elements
        BrouwerLyddaneMeanShort,  ///< Brouwer-Lyddane Mean Short orbital elements
        BrouwerLyddaneMeanLong,   ///< Brouwer-Lyddane Mean Long orbital elements
    };

    /// @brief Semi-Major Axis based constructor
    ///
    /// @code{.cpp}
    ///     RealCondition condition = OrbitalElementCondition::SemiMajorAxis(
    ///         OrbitalElementCondition::Theory::BrouwerLyddaneMeanShort,
    ///         RealCondition::Criterion::AnyCrossing,
    ///         Frame::GCRF(),
    ///         EventCondition::Target(7000000.0),
    ///         gravitationalParameter
    ///     ) ;
    /// @endcode
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static RealCondition SemiMajorAxis(
        const Theory& aTheory,
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Eccentricity based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static RealCondition Eccentricity(
        const Theory& aTheory,
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Inclination based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition Inclination(
        const Theory& aTheory,
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Inclination based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition Inclination(
        const Theory& aTheory,
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Periapsis based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition Aop(
        const Theory& aTheory,
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Periapsis based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition Aop(
        const Theory& aTheory,
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Right Ascension of Ascending Node based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition Raan(
        const Theory& aTheory,
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Right Ascension of Ascending Node based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition Raan(
        const Theory& aTheory,
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief True Anomaly based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition TrueAnomaly(
        const Theory& aTheory,
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief True Anomaly based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition TrueAnomaly(
        const Theory& aTheory,
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Mean Anomaly based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition MeanAnomaly(
        const Theory& aTheory,
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Mean Anomaly based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition MeanAnomaly(
        const Theory& aTheory,
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Eccentric Anomaly based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition EccentricAnomaly(
        const Theory& aTheory,
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Eccentric Anomaly based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition EccentricAnomaly(
        const Theory& aTheory,
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Latitude based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition ArgumentOfLatitude(
        const Theory& aTheory,
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Latitude based constructor
    ///
    /// @param aTheory The orbital element theory to use
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition ArgumentOfLatitude(
        const Theory& aTheory,
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Generic constructor for a Real-valued element (SemiMajorAxis or Eccentricity)
    ///
    /// @code{.cpp}
    ///     RealCondition condition = OrbitalElementCondition::Construct(
    ///         OrbitalElementCondition::Theory::BrouwerLyddaneMeanShort,
    ///         COE::Element::SemiMajorAxis,
    ///         RealCondition::Criterion::AnyCrossing,
    ///         Frame::GCRF(),
    ///         EventCondition::Target(7000000.0),
    ///         gravitationalParameter
    ///     ) ;
    /// @endcode
    ///
    /// @param aTheory The orbital element theory to use
    /// @param anElement The orbital element. Must be SemiMajorAxis or Eccentricity
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static RealCondition Construct(
        const Theory& aTheory,
        const COE::Element& anElement,
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Generic constructor for an Angular element (any element other than SemiMajorAxis or Eccentricity)
    ///
    /// @code{.cpp}
    ///     AngularCondition condition = OrbitalElementCondition::Construct(
    ///         OrbitalElementCondition::Theory::BrouwerLyddaneMeanShort,
    ///         COE::Element::Inclination,
    ///         AngularCondition::Criterion::AnyCrossing,
    ///         Frame::GCRF(),
    ///         EventCondition::Target(Angle::Degrees(45.0)),
    ///         gravitationalParameter
    ///     ) ;
    /// @endcode
    ///
    /// @param aTheory The orbital element theory to use
    /// @param anElement The orbital element. Must not be SemiMajorAxis or Eccentricity
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition Construct(
        const Theory& aTheory,
        const COE::Element& anElement,
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Generic constructor for an Angular element (any element other than SemiMajorAxis or Eccentricity),
    /// satisfied when the element is within a target range
    ///
    /// @param aTheory The orbital element theory to use
    /// @param anElement The orbital element. Must not be SemiMajorAxis or Eccentricity
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return Event Condition
    static AngularCondition Construct(
        const Theory& aTheory,
        const COE::Element& anElement,
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

   private:
    static std::function<Real(const State&)> GenerateEvaluator(
        const Theory& aTheory,
        const COE::Element& anElement,
        const Shared<const Frame>& aFrameSPtr,
        const Derived& aGravitationalParameter
    );
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
