/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Type/Unique.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::core::type::Real;
using ostk::core::type::String;
using ostk::core::type::Unique;

using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::State;

/// @brief An Event Condition defines a criterion that can be evaluated
///                      based on a current/previous state vectors and times
class EventCondition
{
   public:
    /// @brief Target value for an event condition.
    struct Target
    {
        /// @brief Target type.
        enum class Type
        {
            Absolute,  ///< Absolute target value.
            Relative   ///< Relative target value (offset from initial state).
        };

        /// @brief Constructor
        ///
        /// @param aValue A real target value
        /// @param aType A target type (Absolute or Relative)
        Target(const Real& aValue, const Type& aType = Type::Absolute);

        /// @brief Constructor
        ///
        /// @param anAngle An angle target value
        /// @param aType A target type (Absolute or Relative)
        Target(const Angle& anAngle, const Type& aType = Type::Absolute);

        /// @brief Constructor
        ///
        /// @param aLength A length target value
        /// @param aType A target type (Absolute or Relative)
        Target(const Length& aLength, const Type& aType = Type::Absolute);

        /// @brief Equality operator
        ///
        /// @param aTarget A target to compare against
        /// @return True if targets are equal
        bool operator==(const Target& aTarget) const;

        /// @brief Inequality operator
        ///
        /// @param aTarget A target to compare against
        /// @return True if targets are not equal
        bool operator!=(const Target& aTarget) const;

        /// @brief Check if target is defined
        ///
        /// @return True if target is defined
        bool isDefined() const;

        /// @brief Get string representation of a target type
        ///
        /// @param aType A target type
        /// @return String representation of the target type
        static String StringFromType(const Type& aType);

        const Real value;
        const Type type;
        mutable Real valueOffset = 0.0;
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  EventCondition eventCondition = {aName, anEvaluator, aTarget};
    /// @endcode
    ///
    /// @param aName A string representing the name of the Real Event Condition
    /// @param anEvaluator A function evaluating a state
    /// @param aTarget A target associated with the Real Event Condition
    EventCondition(
        const String& aName, const std::function<Real(const State&)>& anEvaluator, const EventCondition::Target& aTarget
    );

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  EventCondition eventCondition = {aName, anEvaluator, aTargetValue};
    /// @endcode
    ///
    /// @param aName A string representing the name of the Real Event Condition
    /// @param anEvaluator A function evaluating a state
    /// @param aTargetValue A target value associated with the Real Event Condition
    EventCondition(const String& aName, const std::function<Real(const State&)>& anEvaluator, const Real& aTargetValue);

    /// @brief Virtual destructor
    virtual ~EventCondition();

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << EventCondition(...);
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param anEventCondition An EventCondition
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const EventCondition& anEventCondition);

    /// @brief Get the name of the Event Condition
    ///
    /// @return String representing the name of the Event Condition
    String getName() const;

    /// @brief Get evaluator
    ///
    /// @return Evaluator
    std::function<Real(const State&)> getEvaluator() const;

    /// @brief Get the target
    ///
    /// @return Target
    Target getTarget() const;

    /// @brief Update the target value if the Event Condition is relative
    ///
    /// @param aState A state to calculate the relative target from
    virtual void updateTarget(const State& aState);

    /// @brief Print the Event Condition
    ///
    /// @param [in, out] anOutputStream The output stream where the Event Condition will be printed
    /// @param displayDecorator A boolean indicating whether or not to display decorator during
    ///                  printing
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Create a copy of this EventCondition
    ///
    /// @return Pointer to the cloned EventCondition
    virtual EventCondition* clone() const = 0;

    /// @brief Check if the Event Condition is satisfied based on current state and previous
    ///                  state/time
    ///
    /// @param currentState The current state
    /// @param previousState The previous state
    ///
    /// @return Boolean value indicating if the Event Condition is met
    virtual bool isSatisfied(const State& currentState, const State& previousState) const = 0;

   protected:
    String name_;
    std::function<Real(const State&)> evaluator_;
    Target target_;
};

}  // namespace astrodynamics
}  // namespace ostk

#endif
