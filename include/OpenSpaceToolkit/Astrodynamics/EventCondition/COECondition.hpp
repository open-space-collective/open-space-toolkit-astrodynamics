/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>
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

using ostk::math::obj::VectorXd;
using ostk::math::obj::Vector3d;

using ostk::physics::coord::Frame;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::eventcondition::RealEventCondition;
using ostk::astro::trajectory::orbit::models::kepler::COE;

/// @brief                      A Classical Orbital Element based event condition

class COECondition : public RealEventCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         COECondition condition = {aCriteria, aTarget, aGravitationalParameter};
    /// @endcode
    ///
    /// @param                  [in] aName The name of the COECondition (optional, can be an empty string)
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] anElement The Element related to the COECondition
    /// @param                  [in] aTarget A target value associated with the COECondition
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter The derived gravitational parameter

    COECondition(
        const String& aName,
        const Criteria& aCriteria,
        const COE::Element& anElement,
        const Real& aTarget,
        const Shared<const Frame>& aFrameSPTr,
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

    /// @brief                  Semi Major Axis based constructor
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] aSemiMajorAxis A semi major axis
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition SemiMajorAxis(
        const Criteria& aCriteria, const Length& aSemiMajorAxis,
        const Shared<const Frame>& aFrameSPTr, const Derived& aGravitationalParameter
    );

    /// @brief                  Eccentricity based constructor
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition Eccentricity(
        const Criteria& aCriteria, const Real& anEccentricity,
        const Shared<const Frame>& aFrameSPTr, const Derived& aGravitationalParameter
    );

    /// @brief                  Inclination based constructor
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] anInclination An inclination
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition Inclination(
        const Criteria& aCriteria, const Angle& aSemiMajorAxis,
        const Shared<const Frame>& aFrameSPTr, const Derived& aGravitationalParameter
    );

    /// @brief                  Argument of Periapsis based constructor
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] anAOP An argument of periapsis
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition Aop(const Criteria& aCriteria, const Angle& anAOP,
    const Shared<const Frame>& aFrameSPTr, const Derived& aGravitationalParameter);

    /// @brief                  Right Ascension of Ascending Node based constructor
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] aRAAN A right angle of ascending node
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition Raan(const Criteria& aCriteria, const Angle& aRAAN,
    const Shared<const Frame>& aFrameSPTr, const Derived& aGravitationalParameter);

    /// @brief                  True Anomaly based constructor
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] aTrueAnomaly A true anomaly
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition TrueAnomaly(
        const Criteria& aCriteria, const Angle& aTrueAnomaly,
        const Shared<const Frame>& aFrameSPTr, const Derived& aGravitationalParameter
    );

    /// @brief                  Mean Anomaly based constructor
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] aMeanAnomaly A mean anomaly
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition MeanAnomaly(
        const Criteria& aCriteria, const Angle& aMeanAnomaly,
        const Shared<const Frame>& aFrameSPTr, const Derived& aGravitationalParameter
    );

    /// @brief                  Eccentric Anomaly based constructor
    ///
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param                  [in] anEccentricAnomaly An eccentric anomaly
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return                 COECondition object

    static COECondition EccentricAnomaly(
        const Criteria& aCriteria, const Angle& anEccentricAnomaly,
        const Shared<const Frame>& aFrameSPTr, const Derived& aGravitationalParameter
    );

   private:
    COE::Element element_;
    Shared<const Frame>& frameSPtr_;
    Derived gravitationalParameter_;

    /// @brief                  Get evaluation function from element
    ///
    /// @param                  [in] anElement The element
    /// @param                  [in] aFrameSPtr The reference frame used to compute the element
    /// @param                  [in] aGravitationalParameter The derived gravitational parameter
    ///
    /// @return                 Evaluation function

    static std::function<Real(const VectorXd&, const Real&)> GenerateEvaluator(
        const COE::Element& anElement,
        const Shared<const Frame>& aFrameSPtr,
        const Derived& aGravitationalParameter
    );
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
