/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{

using ostk::core::type::Integer;
using ostk::core::type::String;

using ostk::physics::time::Instant;
using ostk::physics::time::Duration;

/// @class Pass
/// @brief A revolution of an orbiting object.
///
/// @details This class represents a pass, which is a revolution of an orbiting object.
/// It provides methods to get the type of the pass, the revolution number, and the instants at various points of the
/// pass.
/// @see http://help.agi.com/stk/11.3.0/index.htm#vo/sat_pass.htm
class Pass
{
   public:
    /// @enum Type
    /// @brief The type of the pass.
    enum class Type
    {
        Undefined,  ///< The type is undefined.
        Complete,   ///< The pass is a complete revolution.
        Partial     ///< The pass is a partial revolution.
    };

    /// @enum Phase
    /// @brief The phase of the pass.
    enum class Phase
    {
        Undefined,  ///< The phase is undefined.
        Ascending,  ///< The pass is in the ascending phase.
        Descending  ///< The pass is in the descending phase.
    };

    /// @brief Constructs a pass.
    ///
    /// @param aRevolutionNumber The revolution number of the pass.
    /// @param anInstantAtAscendingNode The instant at the ascending node of the pass.
    /// @param anInstantAtNorthPoint The instant at the north point of the pass.
    /// @param anInstantAtDescendingNode The instant at the descending node of the pass.
    /// @param anInstantAtSouthPoint The instant at the south point of the pass.
    /// @param anInstantAtPassBreak The instant at the pass break, i.e. next ascending node.
    Pass(
        const Integer& aRevolutionNumber,
        const Instant& anInstantAtAscendingNode,
        const Instant& anInstantAtNorthPoint,
        const Instant& anInstantAtDescendingNode,
        const Instant& anInstantAtSouthPoint,
        const Instant& anInstantAtPassBreak
    );

    /// @brief Equality operator.
    ///
    /// @param aPass The pass to compare to.
    /// @return True if the passes are equal.
    bool operator==(const Pass& aPass) const;

    /// @brief Inequality operator.
    ///
    /// @param aPass The pass to compare to.
    /// @return True if the passes are not equal.
    bool operator!=(const Pass& aPass) const;

    /// @brief Output stream operator.
    ///
    /// @param [in,out] anOutputStream The output stream.
    /// @param aPass The pass to output.
    /// @return The output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Pass& aPass);

    /// @brief Checks if the pass is defined.
    ///
    /// @return True if the pass is defined.
    bool isDefined() const;

    /// @brief Checks if the pass is complete.
    ///
    /// @return True if the pass is complete.
    bool isComplete() const;

    /// @brief Gets the type of the pass.
    ///
    /// @return The type of the pass.
    Pass::Type getType() const;

    /// @brief Gets the revolution number of the pass.
    ///
    /// @return The revolution number of the pass.
    Integer getRevolutionNumber() const;

    /// @brief Gets the duration of the pass.
    ///
    /// @return The duration of the pass.
    Duration getDuration() const;

    /// @brief Accesses the instant at the ascending node of the pass.
    ///
    /// @return The instant at the ascending node of the pass.
    const Instant& accessInstantAtAscendingNode() const;

    /// @brief Accesses the instant at the north point of the pass.
    ///
    /// @return The instant at the north point of the pass.
    const Instant& accessInstantAtNorthPoint() const;

    /// @brief Accesses the instant at the descending node of the pass.
    ///
    /// @return The instant at the descending node of the pass.
    const Instant& accessInstantAtDescendingNode() const;

    /// @brief Accesses the instant at the south point of the pass.
    ///
    /// @return The instant at the south point of the pass.
    const Instant& accessInstantAtSouthPoint() const;

    /// @brief Accesses the instant at the break of the pass.
    ///
    /// @return The instant at the break of the pass.
    const Instant& accessInstantAtPassBreak() const;

    /// @brief Print
    ///
    /// @param anOutputStream The output stream.
    /// @param displayDecorator Whether or not to display the decorator
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Creates an undefined pass.
    ///
    /// @return An undefined pass.
    static Pass Undefined();

    /// @brief Converts a pass type to a string.
    ///
    /// @param aType The pass type.
    /// @return The string representation of the pass type.
    static String StringFromType(const Pass::Type& aType);

    /// @brief Converts a pass phase to a string.
    ///
    /// @param aPhase The pass phase.
    /// @return The string representation of the pass phase.
    static String StringFromPhase(const Pass::Phase& aPhase);

   private:
    Pass::Type type_;

    Integer revolutionNumber_;
    Instant instantAtAscendingNode_;
    Instant instantAtNorthPoint_;
    Instant instantAtDescendingNode_;
    Instant instantAtSouthPoint_;
    Instant instantAtPassBreak_;
};

}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
