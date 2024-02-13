/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_LogicalCondition__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::core::container::Array;
using ostk::core::type::Shared;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::trajectory::State;

/// @brief A logical connective event condition.
///
/// @ref                        https://en.wikipedia.org/wiki/Logical_connective
class LogicalCondition : public EventCondition
{
   public:
    enum class Type
    {
        And,
        Or
    };

    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///                  LogicalCondition logicalCondition = {aName, eventConditions, aConnective};
    /// @endcode
    ///
    /// @param aName A string representing the name of the Logical Connective Event Condition.
    /// @param aConnective A logical connective to apply to the Event Condition instances.
    /// @param eventConditions An Array of shared pointers to Event Condition instances,
    ///                  representing the individual event conditions to be evaluated.
    LogicalCondition(
        const String& aName,
        const LogicalCondition::Type& aConnective,
        const Array<Shared<EventCondition>>& eventConditions
    );

    /// @brief Destructor.
    ~LogicalCondition();

    /// @brief Get the type of the logical connective.
    ///
    /// @return The type.
    LogicalCondition::Type getType() const;

    /// @brief Update the target of the individual event conditions composing the Logical Connective
    /// Event Condition.
    ///
    /// @param aState
    virtual void updateTarget(const State& aState) override;

    /// @brief Get the individual event conditions composing the Logical Connective Event Condition.
    ///
    /// @return An Array of shared pointers to EventCondition instances,
    ///                  representing the individual event conditions.
    Array<Shared<EventCondition>> getEventConditions() const;

    /// @brief Check if the Logical Connective Event Condition is satisfied.
    ///
    /// @param currentState A state.
    /// @param previousState A state.
    ///
    /// @return True if the Logical Connective Event Condition is satisfied.
    virtual bool isSatisfied(const State& currentState, const State& previousState) const override;

    /// @brief Print the Logical Connective Event Condition.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator A boolean indicating whether or not to display decorator during
    ///                  the printing process.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    typedef std::function<
        bool(const Array<Shared<EventCondition>>&, const State& currentState, const State& previousState)>
        evaluationSignature;

    Type type_;
    Array<Shared<EventCondition>> eventConditions_;
    evaluationSignature evaluator_;

    static evaluationSignature GenerateEvaluator(const Type& aType);
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
