/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventConditions_SemiMajorAxisCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventConditions_SemiMajorAxisCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::EventCondition;

/// @brief                      A semi major axis based event condition

class SemiMajorAxisCondition : public EventCondition
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     SemiMajorAxisCondition condition = {aCriteria, aLength, aGravitationalParameter};
    /// @endcode
    ///
    /// @param              [in] aCriteria An enum indicating the criteria used to determine the Event Condition
    /// @param              [in] aLength A Length

    SemiMajorAxisCondition(const Criteria& aCriteria, const Length& aLength, const Derived& aGravitationalParameter);

    /// @brief              Virtual destructor

    virtual ~SemiMajorAxisCondition();

    /// @brief              Get SMA
    ///
    /// @return             Semi Major Axis

    Length getSemiMajorAxis() const;

    /// @brief              Get SMA
    ///
    /// @return             Semi Major Axis

    Derived getGravitationalParameter() const;

    /// @brief              Evaluate the Event Condition
    ///
    /// @param              [in] aStateVector The current state vector
    /// @param              [in] aTime The current time
    ///
    /// @return             Real number representing the evaluation result of the Event Condition

    virtual Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override;

   private:
    Real smaInMeters_;
    Real gravitationalParameterInSI_;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
