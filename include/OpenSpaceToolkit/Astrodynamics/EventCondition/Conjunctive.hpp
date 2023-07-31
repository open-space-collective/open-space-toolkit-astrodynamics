/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_Conjunction__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_Conjunction__

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

/// @brief                      Logical Conjunctive of provided Event Conditions
///
/// @ref                        https://en.wikipedia.org/wiki/Logical_conjunction

class Conjunctive : public LogicalConnective
{
   public:
    Conjunctive(const Array<Shared<EventCondition>>& eventConditions);

    ~Conjunctive();

    // virtual bool evaluate(const VectorXd& aStateVector, const Real& aTime) const override;

    virtual bool isSatisfied(const Real& currentValue, const Real& previousValue) const override;

    virtual bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const override;

    // virtual bool isSatisfiedFromCache(const VectorXd& aStateVector, const Real& aTime) override;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
