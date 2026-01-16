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
    enum class Criterion
    {
        AnyCrossing,
        PositiveCrossing,
        NegativeCrossing,
        WithinRange
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

    /// @brief Get the criterion of the Event Condition
    ///
    /// @return Enum representing the criterion of the Event Condition
    Criterion getCriterion() const;

    /// @brief Get target
    ///
    /// @return Target
    Angle getTargetAngle() const;

    /// @brief Get target range
    ///
    /// @return Target range
    Pair<Angle, Angle> getTargetRange() const;

    /// @brief Print the Event Condition
    ///
    /// @param [in, out] anOutputStream The output stream where the Event Condition will be printed
    /// @param displayDecorator A boolean indicating whether or not to display decorator during
    /// printing
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Evaluate the Angular Event Condition
    ///
    /// Returns the signed angular distance from the current angle to the target,
    /// normalized to the range [-π, π). Positive values indicate the current angle
    /// is "ahead" of the target (in the positive angular direction).
    ///
    /// @param aState The current state
    ///
    /// @return Real number representing the angular distance to target in radians
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

    /// @brief Create an angular condition that is satisfied when the angle is within a range
    ///
    /// @param aName A string representing the name of the Angular Event Condition
    /// @param anEvaluator A function evaluating a state to an angle in radians
    /// @param aTargetRange A pair of angles representing the range of angles that satisfy the
    /// @return Angular Event Condition
    static AngularCondition WithinRange(
        const String& aName,
        const std::function<Real(const State&)>& anEvaluator,
        const Pair<Angle, Angle>& aTargetRange
    );

    /// @brief Convert criterion to string
    ///
    /// @param aCriterion An enum representing the criterion
    ///
    /// @return String representing the given criterion
    static String StringFromCriterion(const Criterion& aCriterion);

   private:
    Criterion criterion_;
    std::function<bool(const Real&, const Real&, const Real&)> comparator_;
    Pair<Real, Real> targetRange_;

    /// @brief Returns whether this condition's evaluate() returns negative when satisfied.
    ///
    /// For NegativeCrossing criterion, the condition is satisfied when crossing
    /// from above to below the target.
    ///
    /// @return True if criterion is NegativeCrossing.
    bool evaluateNegativeWhenSatisfied() const;

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
