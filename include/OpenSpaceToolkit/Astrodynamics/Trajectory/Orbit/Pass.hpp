/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass__

#include <optional>

#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{

using ostk::core::type::String;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

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
        Complete,  ///< The pass is a complete revolution: all of its instants are known.
        Partial    ///< The pass is a partial revolution: at least one of its instants is unknown.
    };

    /// @enum Phase
    /// @brief The phase of the pass.
    enum class Phase
    {
        Ascending,  ///< The pass is in the ascending phase.
        Descending  ///< The pass is in the descending phase.
    };

    /// @brief Constructs a pass.
    ///
    /// @code{.cpp}
    ///              Pass pass = { 1, instantAtAscendingNode, instantAtNorthPoint, instantAtDescendingNode,
    ///              instantAtSouthPoint, instantAtPassBreak } ;
    /// @endcode
    ///
    /// @param aRevolutionNumber The revolution number of the pass (std::nullopt if unknown).
    /// @param anInstantAtAscendingNode The instant at the ascending node of the pass (std::nullopt if unknown).
    /// @param anInstantAtNorthPoint The instant at the north point of the pass (std::nullopt if unknown).
    /// @param anInstantAtDescendingNode The instant at the descending node of the pass (std::nullopt if unknown).
    /// @param anInstantAtSouthPoint The instant at the south point of the pass (std::nullopt if unknown).
    /// @param anInstantAtPassBreak The instant at the pass break, i.e. next ascending node (std::nullopt if unknown).
    Pass(
        const std::optional<int>& aRevolutionNumber,
        const std::optional<Instant>& anInstantAtAscendingNode,
        const std::optional<Instant>& anInstantAtNorthPoint,
        const std::optional<Instant>& anInstantAtDescendingNode,
        const std::optional<Instant>& anInstantAtSouthPoint,
        const std::optional<Instant>& anInstantAtPassBreak
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
    /// @return The revolution number of the pass, or std::nullopt if it is unknown.
    const std::optional<int>& getRevolutionNumber() const;

    /// @brief Gets the duration of the pass.
    ///
    /// @return The duration of the pass, or std::nullopt if the pass is not complete.
    std::optional<Duration> getDuration() const;

    /// @brief Get the start instant of the pass. For partial passes, this is the minimum known instant.
    ///
    /// @return The start instant of the pass, or std::nullopt if none of the instants of the pass is known.
    std::optional<Instant> getStartInstant() const;

    /// @brief Get the end instant of the pass. For partial passes, this is the maximum known instant.
    ///
    /// @return The end instant of the pass, or std::nullopt if none of the instants of the pass is known.
    std::optional<Instant> getEndInstant() const;

    /// @brief Gets the interval of the pass.
    ///
    /// @return The interval of the pass, or std::nullopt if the pass is not complete.
    std::optional<Interval> getInterval() const;

    /// @brief Get the instant at the ascending node of the pass.
    ///
    /// @return The instant at the ascending node of the pass, or std::nullopt if unknown.
    const std::optional<Instant>& getInstantAtAscendingNode() const;

    /// @brief Get the instant at the north point of the pass.
    ///
    /// @return The instant at the north point of the pass, or std::nullopt if unknown.
    const std::optional<Instant>& getInstantAtNorthPoint() const;

    /// @brief Get the instant at the descending node of the pass.
    ///
    /// @return The instant at the descending node of the pass, or std::nullopt if unknown.
    const std::optional<Instant>& getInstantAtDescendingNode() const;

    /// @brief Get the instant at the south point of the pass.
    ///
    /// @return The instant at the south point of the pass, or std::nullopt if unknown.
    const std::optional<Instant>& getInstantAtSouthPoint() const;

    /// @brief Get the instant at the break of the pass.
    ///
    /// @return The instant at the break of the pass, or std::nullopt if unknown.
    const std::optional<Instant>& getInstantAtPassBreak() const;

    /// @brief Print
    ///
    /// @param anOutputStream The output stream.
    /// @param displayDecorator Whether or not to display the decorator
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

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

    std::optional<int> revolutionNumber_;
    std::optional<Instant> instantAtAscendingNode_;
    std::optional<Instant> instantAtNorthPoint_;
    std::optional<Instant> instantAtDescendingNode_;
    std::optional<Instant> instantAtSouthPoint_;
    std::optional<Instant> instantAtPassBreak_;
};

}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
