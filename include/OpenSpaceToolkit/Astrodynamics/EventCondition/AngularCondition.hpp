/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition__

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::type::Real;
using ostk::core::type::String;
using ostk::core::container::Pair;

using ostk::physics::units::Angle;

using ostk::astro::trajectory::State;

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

    /// @brief Check if the Event Condition is satisfied based on current state and previous
    ///                  state/time
    ///
    /// @param currentState The current state
    /// @param previousState The previous state
    ///
    /// @return Boolean value indicating if the Event Condition is met
    virtual bool isSatisfied(const State& currentState, const State& previousState) const override;

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
}  // namespace astro
}  // namespace ostk

#endif
