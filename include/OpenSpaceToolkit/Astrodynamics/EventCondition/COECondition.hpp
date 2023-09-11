/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;

using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::coord::Frame;

using ostk::astro::eventcondition::RealCondition;
using ostk::astro::eventcondition::AngularCondition;
using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::State;

/// @brief                      A Classical Orbital Element based event condition

class COECondition
{
   public:
    static RealCondition SemiMajorAxis(
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Length& aSemiMajorAxis,
        const Derived& aGravitationalParameter
    );

    /// @brief                  Eccentricity based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static RealCondition Eccentricity(
        const RealCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Real& anEccentricity,
        const Derived& aGravitationalParameter
    );

    /// @brief                  Inclination based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] anInclination An inclination
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static AngularCondition Inclination(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Angle& aSemiMajorAxis,
        const Derived& aGravitationalParameter
    );

    /// @brief                  Argument of Periapsis based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] anAOP An argument of periapsis
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static AngularCondition Aop(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Angle& anAOP,
        const Derived& aGravitationalParameter
    );

    /// @brief                  Right Ascension of Ascending Node based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] aRAAN A right angle of ascending node
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static AngularCondition Raan(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Angle& aRAAN,
        const Derived& aGravitationalParameter
    );

    /// @brief                  True Anomaly based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] aTrueAnomaly A true anomaly
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static AngularCondition TrueAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Angle& aTrueAnomaly,
        const Derived& aGravitationalParameter
    );

    /// @brief                  Mean Anomaly based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] aMeanAnomaly A mean anomaly
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static AngularCondition MeanAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Angle& aMeanAnomaly,
        const Derived& aGravitationalParameter
    );

    /// @brief                  Eccentric Anomaly based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] anEccentricAnomaly An eccentric anomaly
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static AngularCondition EccentricAnomaly(
        const AngularCondition::Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Angle& anEccentricAnomaly,
        const Derived& aGravitationalParameter
    );

   private:
    static std::function<Real(const State&)> GenerateEvaluator(
        const COE::Element& anElement, const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter
    );
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
