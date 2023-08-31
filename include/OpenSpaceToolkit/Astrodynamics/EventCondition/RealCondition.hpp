/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_RealCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;

/// @brief                      An Event Condition that can evaluate each state to a real number.

class RealCondition: public EventCondition
{
   public:
    enum class Criteria
    {
        PositiveCrossing,
        NegativeCrossing,
        AnyCrossing,
        StrictlyPositive,
        StrictlyNegative,
        Undefined
    };

    /// @brief                  Constructor
    ///
    /// @code
    ///                         RealCondition RealCondition = {aName, aCriteria, anEvaluatro, aTarget};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Real Event Condition
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine if the Real Event
    /// Condition is met
    /// @param                  [in] anEvaluator A function evaluating a state and a time
    /// @param                  [in] aTarget A target value associated with the Real Event Condition

    RealCondition(
        const String& aName,
        const Criteria& aCriteria,
        const std::function<Real(const VectorXd&, const Real&)> anEvaluator,
        const Real& aTarget = 0.0
    );

    /// @brief                  Virtual destructor

    virtual ~RealCondition();

    /// @brief                  Get the name of the Event Condition
    ///
    /// @return                 String representing the name of the Event Condition

    String getName() const;

    /// @brief                  Get the criteria of the Event Condition
    ///
    /// @return                 Enum representing the criteria of the Event Condition

    Criteria getCriteria() const;

    /// @brief                  Get evaluator
    ///
    /// @return                 Evaluator

    std::function<Real(const VectorXd&, const Real&)> getEvaluator() const;

    /// @brief                  Get the target of the Event Condition
    ///
    /// @return                 Real number representing the target of the Event Condition

    Real getTarget() const;

    /// @brief                  Print the Event Condition
    ///
    /// @param                  [in, out] anOutputStream The output stream where the Event Condition will be printed
    /// @param                  [in] displayDecorator A boolean indicating whether or not to display decorator during
    /// printing

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Evaluate the Event Condition
    ///
    /// @param                  [in] aStateVector The current state vector
    /// @param                  [in] aTime The current time
    ///
    /// @return                 Real number representing the evaluation result of the Event Condition

    Real evaluate(const VectorXd& aStateVector, const Real& aTime) const;

    /// @brief                  Check if the Event Condition is satisfied based on current state/time and previous
    /// state/time
    ///                         This overload is useful when the Event Condition is a Conjunctive/Disjunctive of several
    ///                         conditions
    ///
    /// @param                  [in] currentStateVector The current state vector
    /// @param                  [in] currentTime The current time
    /// @param                  [in] previousStateVector The previous state vector
    /// @param                  [in] previousTime The previous time
    ///
    /// @return                 Boolean value indicating if the Event Condition is met

    bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const override;

    /// @brief                  Convert criteria to string
    ///
    /// @param                  [in] aCriteria An enum representing the criteria
    ///
    /// @return                 String representing the given criteria

    static String StringFromCriteria(const Criteria& aCriteria);

   private:
    Criteria criteria_;
    std::function<Real(const VectorXd&, const Real&)> evaluator_;
    Real target_;
    std::function<bool(const Real&, const Real&)> comparator_;

    static std::function<bool(const Real&, const Real&)> GenerateComparator(const Criteria& aCriteria);
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
