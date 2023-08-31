/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

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

using ostk::astro::eventcondition::RealCondition;
using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::State;

/// @brief                      A Classical Orbital Element based event condition

class COECondition : public RealCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         COECondition condition = {aCriterion, aTarget, aGravitationalParameter};
    /// @endcode
    ///
    /// @param                  [in] aName The name of the COECondition (optional, can be an empty string)
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] anElement The Element related to the COECondition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] aTarget A target value associated with the COECondition
    /// @param                  [in] aGravitationalParameter The derived gravitational parameter

    COECondition(
        const String& aName,
        const Criterion& aCriterion,
        const COE::Element& anElement,
        const Shared<const Frame>& aFrameSPtr const Real& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief                  Virtual destructor

    virtual ~COECondition();

    /// @brief                  Get gravitational parameter
    ///
    /// @return                 gravitational parameter

    Derived getGravitationalParameter() const;

    /// @brief                  Get element
    ///
    /// @return                 element

    COE::Element getElement() const;

    /// @brief                  Get frame
    ///
    /// @return                 frame

    Shared<const Frame> getFrame() const;

    /// @brief                  Semi Major Axis based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] aSemiMajorAxis A semi major axis
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition SemiMajorAxis(
        const Criterion& aCriterion,
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

    static COECondition Eccentricity(
        const Criterion& aCriterion,
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

    static COECondition Inclination(
        const Criterion& aCriterion,
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

    static COECondition Aop(
        const Criterion& aCriterion,
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

    static COECondition Raan(const Criterion& aCriterion, const Angle& aRAAN, const Derived& aGravitationalParameter);

    /// @brief                  True Anomaly based constructor
    ///
    /// @param                  [in] aCriterion An enum indicating the criterion used to determine the Event Condition
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] aTrueAnomaly A true anomaly
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition TrueAnomaly(
        const Criterion& aCriterion,
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

    static COECondition MeanAnomaly(
        const Criterion& aCriterion,
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

    static COECondition EccentricAnomaly(
        const Criterion& aCriterion,
        const Shared<const Frame>& aFrameSPtr,
        const Angle& anEccentricAnomaly,
        const Derived& aGravitationalParameter
    );

   private:
    COE::Element element_;
    Shared<const Frame> frameSPtr_;
    Derived gravitationalParameter_;

    /// @brief                  Get evaluation function from element
    ///
    /// @param                  [in] anElement The element
    /// @param                  [in] aFrameSPtr A frame in which the Element is to be computed
    /// @param                  [in] aGravitationalParameter The derived gravitational parameter
    ///
    /// @return                 Evaluation function

    static std::function<Real(const State&)> GenerateEvaluator(
        const COE::Element& anElement, const Shared<const Frame>& aFrameSPtr, const Derived& aGravitationalParameter
    );
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
