/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::State;

/// @brief An Event Condition defines a criterion that can be evaluated
///                      based on a current/previous state vectors and times
class EventCondition
{
   public:
    struct Target
    {
        enum class Type
        {
            Absolute,
            Relative
        };

        Target(const Real& aValue, const Type& aType = Type::Absolute);
        Target(const Angle& anAngle, const Type& aType = Type::Absolute);
        Target(const Length& aLength, const Type& aType = Type::Absolute);

        bool operator==(const Target& aTarget) const;
        bool operator!=(const Target& aTarget) const;

        bool isDefined() const;

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
    ///                  std::cout << EventCondition(...) ;
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
