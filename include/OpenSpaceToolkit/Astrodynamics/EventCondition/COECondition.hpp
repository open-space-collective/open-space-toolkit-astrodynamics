/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::AngularCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

/// @brief A Classical Orbital Element based event condition
class COECondition
{
   public:
    /// @brief Semi-Major Axis based constructor
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aFrameSPtr A frame in which the Element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    static RealCondition SemiMajorAxis(
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Eccentricity based constructor
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aFrameSPtr A frame in which the Element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return COECondition object
    static RealCondition Eccentricity(
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Inclination based constructor
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aFrameSPtr A frame in which the Element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return COECondition object
    static AngularCondition Inclination(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Argument of Periapsis based constructor
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aFrameSPtr A frame in which the Element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return COECondition object
    static AngularCondition Aop(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Right Ascension of Ascending Node based constructor
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aFrameSPtr A frame in which the Element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return COECondition object
    static AngularCondition Raan(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief True Anomaly based constructor
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aFrameSPtr A frame in which the Element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return COECondition object
    static AngularCondition TrueAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Mean Anomaly based constructor
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aFrameSPtr A frame in which the Element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return COECondition object
    static AngularCondition MeanAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief Eccentric Anomaly based constructor
    ///
    /// @param aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param aFrameSPtr A frame in which the Element is to be computed
    /// @param aTarget A Target
    /// @param aGravitationalParameter A gravitational parameter
    ///
    /// @return COECondition object
    static AngularCondition EccentricAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const EventCondition::Target& aTarget,
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
