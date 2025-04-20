/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_BrouwerLyddaneMeanLongCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_BrouwerLyddaneMeanLongCondition__

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
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

/// @brief A Brouwer-Lyddane Long Mean orbital elements based event condition
class BrouwerLyddaneMeanLongCondition
{
   public:
    /// @brief Semi-Major Axis based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static RealCondition SemiMajorAxis(
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Eccentricity based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static RealCondition Eccentricity(
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Inclination based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition Inclination(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Inclination based constructor
    ///
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition Inclination(
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Periapsis based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition Aop(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Periapsis based constructor
    ///
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition Aop(
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Right Ascension of Ascending Node based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition Raan(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Right Ascension of Ascending Node based constructor
    ///
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition Raan(
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief True Anomaly based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition TrueAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief True Anomaly based constructor
    ///
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition TrueAnomaly(
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Mean Anomaly based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition MeanAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Mean Anomaly based constructor
    ///
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition MeanAnomaly(
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Eccentric Anomaly based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition EccentricAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Eccentric Anomaly based constructor
    ///
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition EccentricAnomaly(
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Latitude based constructor
    ///
    /// @param aCriterion The criterion used to resolve the Event Condition
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition ArgumentOfLatitude(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Latitude based constructor
    ///
    /// @param aFrameSPtr A frame in which the element is to be computed
    /// @param aTargetRange A Target Range
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return BrouwerLyddaneMeanLongCondition object
    static AngularCondition ArgumentOfLatitude(
        const Shared<const Frame>& aFrameSPtr,
        const Pair<Angle, Angle>& aTargetRange,
        const Derived& aGravitationalParameter
    );

   private:
    static std::function<Real(const State&)> GenerateEvaluator(
        const COE::Element& anElement, const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter
    );
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
