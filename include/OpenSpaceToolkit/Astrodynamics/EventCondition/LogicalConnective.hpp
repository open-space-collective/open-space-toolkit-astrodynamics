/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalConnective__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::ctnr::Array;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;

/// @brief                      Define the abstract parent class for Logical Connective Event Conditions

class LogicalConnective : public EventCondition
{
   public:
    LogicalConnective(const String& aName, const Array<Shared<EventCondition>>& eventConditions);

    ~LogicalConnective();

    Array<Shared<EventCondition>> getEventConditions() const;

    virtual Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override;

    // virtual void setValue(const VectorXd& aStateVector, const Real& aTime) override;

   protected:
    Array<Shared<EventCondition>> eventConditions_;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
