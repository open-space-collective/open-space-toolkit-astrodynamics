/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_Disjunctive__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_Disjunctive__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalConnective.hpp>

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

using ostk::astro::eventcondition::LogicalConnective;

/// @brief                      Logical Disjunctive of provided Event Conditions
///
/// @ref                        https://en.wikipedia.org/wiki/Disjunction_(logical_connective)

class Disjunctive : public LogicalConnective
{
   public:
    Disjunctive(const Array<Shared<EventCondition>>& eventConditions);

    ~Disjunctive();

    virtual bool isSatisfied(const Real& currentValue, const Real& previousValue) const override;

    virtual bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const override;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
