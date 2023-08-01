/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_COECondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;
using ostk::math::obj::Vector3d;

using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::EventCondition;
using ostk::astro::trajectory::orbit::models::kepler::COE;

/// @brief                      A Classical Orbital Element based event condition

class COECondition : public EventCondition
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     COECondition condition = {aCriteria, aTarget, aGravitationalParameter};
    /// @endcode
    ///
    /// @param              [in] aName The name of the COECondition (optional, can be an empty string)
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] anElement The Element related to the COECondition
    /// @param              [in] aTarget A target value associated with the COECondition
    /// @param              [in] aGravitationalParameter The derived gravitational parameter

    COECondition(
        const String& aName,
        const Criteria& aCriteria,
        const COE::Element& anElement,
        const Real& aTarget,
        const Derived& aGravitationalParameter
    );

    /// @brief              Virtual destructor

    virtual ~COECondition();

    /// @brief              Get target
    ///
    /// @return             target

    Real getTarget() const;

    /// @brief              Get gravitational parameter
    ///
    /// @return             gravitational parameter

    Derived getGravitationalParameter() const;

    /// @brief               Get element
    ///
    /// @return              element

    COE::Element getElement() const;

    /// @brief              Evaluate the Event Condition
    ///
    /// @param              [in] aStateVector The current state vector
    /// @param              [in] aTime The current time
    ///
    /// @return             Real number representing the evaluation result of the Event Condition

    virtual Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override;

    /// @brief              Convert element to string
    ///
    /// @param              [in] anElement An element
    ///
    /// @return             String representing the element

    static String StringFromElement(const COE::Element& anElement);

    /// @brief              Semi Major Axis based constructor
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] aSemiMajorAxis A semi major axis
    /// @param              [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return             COECondition object

    static COECondition SemiMajorAxis(
        const Criteria& aCriteria, const Length& aSemiMajorAxis, const Derived& aGravitationalParameter
    );

    /// @brief              Eccentricity based constructor
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] anEccentricity An eccentricity
    /// @param              [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return             COECondition object

    static COECondition Eccentricity(
        const Criteria& aCriteria, const Real& anEccentricity, const Derived& aGravitationalParameter
    );

    /// @brief              Inclination based constructor
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] anInclination An inclination
    /// @param              [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return             COECondition object

    static COECondition Inclination(
        const Criteria& aCriteria, const Angle& aSemiMajorAxis, const Derived& aGravitationalParameter
    );

    /// @brief              Argument of Periapsis based constructor
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] anAOP An argument of periapsis
    /// @param              [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return             COECondition object

    static COECondition Aop(const Criteria& aCriteria, const Angle& anAOP, const Derived& aGravitationalParameter);

    /// @brief              Right Ascension of Ascending Node based constructor
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] aRAAN A right angle of ascending node
    /// @param              [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return             COECondition object

    static COECondition Raan(const Criteria& aCriteria, const Angle& aRAAN, const Derived& aGravitationalParameter);

    /// @brief              True Anomaly based constructor
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] aTrueAnomaly A true anomaly
    /// @param              [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return             COECondition object

    static COECondition TrueAnomaly(
        const Criteria& aCriteria, const Angle& aTrueAnomaly, const Derived& aGravitationalParameter
    );

    /// @brief              Mean Anomaly based constructor
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] aMeanAnomaly A mean anomaly
    /// @param              [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return             COECondition object

    static COECondition MeanAnomaly(
        const Criteria& aCriteria, const Angle& aMeanAnomaly, const Derived& aGravitationalParameter
    );

    /// @brief              Eccentric Anomaly based constructor
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] anEccentricAnomaly An eccentric anomaly
    /// @param              [in] aGravitationalParameter A gravitational parameter
    ///
    /// @return             COECondition object

    static COECondition EccentricAnomaly(
        const Criteria& aCriteria, const Angle& anEccentricAnomaly, const Derived& aGravitationalParameter
    );

   private:
    COE::Element element_;
    Real target_;
    Derived gravitationalParameter_;
    std::function<Real(const Vector3d&, const Vector3d&)> evaluator_;

    /// @brief              Get evaluation function from element
    ///
    /// @param              [in] anElement The element
    ///
    /// @return             Evaluation function

    std::function<Real(const Vector3d&, const Vector3d&)> getEvaluator(const COE::Element& anElement) const;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
