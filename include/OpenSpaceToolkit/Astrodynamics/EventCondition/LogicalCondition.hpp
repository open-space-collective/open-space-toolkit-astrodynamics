/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition__

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

/// @brief                      A logical connective event condition
///
/// @ref                        https://en.wikipedia.org/wiki/Logical_connective

class LogicalCondition : public EventCondition
{
   public:
    enum class Connective
    {
        Conjunction,
        Disjunction,
        Undefined
    };

    /// @brief                  Constructor
    ///
    /// @code
    ///                         LogicalCondition logicalCondition = {aName, eventConditions, aConnective};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Logical Connective Event Condition
    /// @param                  [in] aConnective A logical connective to apply to the EventCondition instances.
    /// @param                  [in] eventConditions An Array of shared pointers to EventCondition instances,
    ///                         representing the individual event conditions to be evaluated

    LogicalCondition(
        const String& aName,
        const LogicalCondition::Connective& aConnective,
        const Array<Shared<EventCondition>>& eventConditions
    );

    /// @brief                  Destructor

    ~LogicalCondition();

    /// @brief                  Get the logical connective
    ///
    /// @return                 The connective

    LogicalCondition::Connective getConnective() const;

    /// @brief                  Get the individual event conditions composing the Logical Connective Event Condition
    ///
    /// @return                 An Array of shared pointers to EventCondition instances,
    ///                         representing the individual event conditions.

    Array<Shared<EventCondition>> getEventConditions() const;

    virtual bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const override;

   protected:
    LogicalCondition::Connective connective_;
    Array<Shared<EventCondition>> eventConditions_;

};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
