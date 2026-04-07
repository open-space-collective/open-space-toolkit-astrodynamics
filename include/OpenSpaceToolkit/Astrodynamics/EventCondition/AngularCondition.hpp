/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition__

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eventcondition
{

using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::physics::unit::Angle;

using ostk::astrodynamics::trajectory::State;

/// @brief An Event Condition that works with angles. Handles wrapping angle to [0,360).
class AngularCondition : public EventCondition
{
   public:
    /// @brief Criterion type for evaluating the angular condition.
    enum class Criterion
    {
        AnyCrossing,       ///< Triggered on any crossing of the target angle.
        PositiveCrossing,  ///< Triggered when angle crosses target in the positive direction.
        NegativeCrossing,  ///< Triggered when angle crosses target in the negative direction.
        WithinRange        ///< Triggered when angle is within a specified range.
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  AngularCondition angularCondition = {aName, aCriterion, anEvaluator, aTargetAngle};
    /// @endcode
    ///
    /// @param aName A string representing the name of the Angular Event Condition
    /// @param aCriterion An enum indicating the criterion used to determine if the Angular Event
    /// Condition is met
    /// @param anEvaluator A function evaluating a state to an angle in radians
    /// @param aTargetAngle A target angle
    AngularCondition(
        const String& aName,
        const Criterion& aCriterion,
        const std::function<Real(const State&)>& anEvaluator,
        const Angle& aTargetAngle
    );

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  AngularCondition angularCondition = {aName, aCriterion, anEvaluator, aTarget};
    /// @endcode
    ///
    /// @param aName A string representing the name of the Angular Event Condition
    /// @param aCriterion An enum indicating the criterion used to determine if the Angular Event
    /// Condition is met
    /// @param anEvaluator A function evaluating a state to an angle in radians
    /// @param aTarget A target
    AngularCondition(
        const String& aName,
        const Criterion& aCriterion,
        const std::function<Real(const State&)>& anEvaluator,
        const Target& aTarget
    );

    /// @brief Virtual destructor
    virtual ~AngularCondition();

    /// @brief Get the criterion of the Event Condition.
    ///
    /// @code{.cpp}
    ///     AngularCondition condition = { ... } ;
    ///     AngularCondition::Criterion criterion = condition.getCriterion() ;
    /// @endcode
    ///
    /// @return Enum representing the criterion of the Event Condition.
    Criterion getCriterion() const;

    /// @brief Get the target angle.
    ///
    /// @code{.cpp}
    ///     AngularCondition condition = { ... } ;
    ///     Angle targetAngle = condition.getTargetAngle() ;
    /// @endcode
    ///
    /// @return The target angle.
    Angle getTargetAngle() const;

    /// @brief Get the target range.
    ///
    /// @code{.cpp}
    ///     AngularCondition condition = { ... } ;
    ///     Pair<Angle, Angle> targetRange = condition.getTargetRange() ;
    /// @endcode
    ///
    /// @return The target angle range as a pair of (lower, upper) angles.
    Pair<Angle, Angle> getTargetRange() const;

    /// @brief Print the Event Condition
    ///
    /// @param [in, out] anOutputStream The output stream where the Event Condition will be printed
    /// @param displayDecorator A boolean indicating whether or not to display decorator during
    /// printing
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Evaluate the angular event function.
    ///
    /// Returns the signed angular distance from the current angle to the target,
    /// wrapped to [-pi, pi]. For WithinRange criterion, returns the distance
    /// from the nearest boundary (positive inside, negative outside).
    ///
    /// @param aState The state to evaluate
    /// @return Real number representing the angular event function value
    virtual Real evaluate(const State& aState) const override;

    /// @brief Check if the Event Condition is satisfied based on current state and previous
    ///                  state/time
    ///
    /// @param currentState The current state
    /// @param previousState The previous state
    ///
    /// @return Boolean value indicating if the Event Condition is met
    virtual bool isSatisfied(const State& currentState, const State& previousState) const override;

    /// @brief Create a copy of this AngularCondition
    ///
    /// @return Pointer to the cloned EventCondition
    virtual AngularCondition* clone() const override;

    /// @brief Create an angular condition that is satisfied when the angle is within a range.
    ///
    /// @code{.cpp}
    ///     AngularCondition condition = AngularCondition::WithinRange(
    ///         "Elevation", anEvaluator, { Angle::Degrees(10.0), Angle::Degrees(90.0) }
    ///     ) ;
    /// @endcode
    ///
    /// @param aName A string representing the name of the Angular Event Condition.
    /// @param anEvaluator A function evaluating a state to an angle in radians.
    /// @param aTargetRange A pair of angles representing the range.
    /// @return Angular Event Condition.
    static AngularCondition WithinRange(
        const String& aName,
        const std::function<Real(const State&)>& anEvaluator,
        const Pair<Angle, Angle>& aTargetRange
    );

    /// @brief Convert criterion to string.
    ///
    /// @code{.cpp}
    ///     String str = AngularCondition::StringFromCriterion(AngularCondition::Criterion::AnyCrossing) ;
    /// @endcode
    ///
    /// @param aCriterion An enum representing the criterion.
    /// @return String representing the given criterion.
    static String StringFromCriterion(const Criterion& aCriterion);

   private:
    Criterion criterion_;
    std::function<bool(const Real&, const Real&, const Real&)> comparator_;
    Pair<Real, Real> targetRange_;

    static bool IsPositiveCrossing(const Real& currentAngle, const Real& previousAngle, const Real& targetAngle);
    static bool IsNegativeCrossing(const Real& currentAngle, const Real& previousAngle, const Real& targetAngle);

    std::function<bool(const Real&, const Real&, const Real&)> GenerateComparator(const Criterion& aCriterion);

    AngularCondition(
        const String& aName,
        const Criterion& aCriterion,
        const std::function<Real(const State&)>& anEvaluator,
        const Pair<Angle, Angle>& aTargetRange
    );
};

}  // namespace eventcondition
}  // namespace astrodynamics
}  // namespace ostk

#endif
