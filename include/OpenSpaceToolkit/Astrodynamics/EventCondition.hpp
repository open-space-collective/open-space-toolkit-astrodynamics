/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

/// @brief                      An Event Condition defines a criteria that can be evaluated
///                             based on a current/previous state vectors and times

class EventCondition
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
    ///                         EventCondition eventCondition = {aName, aCriteria};
    /// @endcode
    ///
    /// @param                  [in] aName A string representing the name of the Event Condition
    /// @param                  [in] aCriteria An enum indicating the criteria used to determine if the Event Condition
    /// is met

    EventCondition(const String& aName, const Criteria& aCriteria);

    /// @brief                  Virtual destructor

    virtual ~EventCondition();

    /// @brief                  Output stream operator
    ///
    /// @code
    ///                         std::cout << EventCondition(...) ;
    /// @endcode
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] anEventCondition An EventCondition
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const EventCondition& anEventCondition);

    /// @brief                  Get the name of the Event Condition
    ///
    /// @return                 String representing the name of the Event Condition

    String getName() const;

    /// @brief                  Get the criteria of the Event Condition
    ///
    /// @return                 Enum representing the criteria of the Event Condition

    Criteria getCriteria() const;

    /// @brief                  Get comparator
    ///
    /// @return                 Comparator

    std::function<bool(const Real&, const Real&)> getComparator() const;

    /// @brief                  Print the Event Condition
    ///
    /// @param                  [in, out] anOutputStream The output stream where the Event Condition will be printed
    /// @param                  [in] displayDecorator A boolean indicating whether or not to display decorator during
    /// printing

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

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

    virtual bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const = 0;

    /// @brief                  Convert criteria to string
    ///
    /// @param                  [in] aCriteria An enum representing the criteria
    ///
    /// @return                 String representing the given criteria

    static String StringFromCriteria(const Criteria& aCriteria);

   private:
    String name_;
    Criteria criteria_;
    std::function<bool(const Real&, const Real&)> comparator_;

    static std::function<bool(const Real&, const Real&)> getComparator(const Criteria& aCriteria);
};

}  // namespace astro
}  // namespace ostk

#endif
