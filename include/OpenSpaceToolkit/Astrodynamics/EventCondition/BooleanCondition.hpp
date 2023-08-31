/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_BooleanCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::eventcondition::RealCondition;

/// @brief                      An Event Condition that can evaluate each state to a boolean.

class BooleanCondition : public RealCondition
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         BooleanCondition booleanCondition = {aName, aCriteria, anEvaluator,
    ///                         anInverseFlag};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Boolean Event Condition
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine if the Boolean Event
    /// Condition is met
    /// @param                  [in] anEvaluator A function evaluating a state and a time
    /// @param                  [in] anInverseFlag A flag indicating whether the condition is inverted

    BooleanCondition(
        const String& aName,
        const Criteria& aCriteria,
        const std::function<bool(const VectorXd&, const Real&)> anEvaluator,
        const bool& anInverseFlag = false
    );

    /// @brief                  Virtual destructor

    virtual ~BooleanCondition();

    /// @brief                  Check if the condition is inversed
    ///
    /// @return                 Boolean value indicating whether the condition is inversed

    bool isInversed() const;

    /// @brief                  Print the Boolean Event Condition
    ///
    /// @param                  [in, out] anOutputStream The output stream where the Boolean Event Condition will be
    /// printed
    /// @param                  [in] displayDecorator A boolean indicating whether or not to display decorator during
    /// printing

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

   private:
    bool inverse_;
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
