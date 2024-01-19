/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass__

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{

using ostk::core::types::Integer;
using ostk::core::types::String;

using ostk::physics::time::Interval;
using ostk::physics::time::Instant;

/// @brief A revolution of an orbiting object
///
/// @ref                        http://help.agi.com/stk/11.3.0/index.htm#vo/sat_pass.htm
class Pass
{
   public:
    enum class Type
    {

        Undefined,
        Complete,
        Partial

    };

    enum class Phase
    {

        Undefined,
        Ascending,
        Descending

    };

    enum class Quarter
    {

        Undefined,
        First,
        Second,
        Third,
        Fourth

    };

    Pass(
        const Pass::Type& aType,
        const Integer& aRevolutionNumber,
        const Interval& anInterval,
        const Instant& anInstantAtDescendingNode,
        const Instant& anInstantAtNorthPoint,
        const Instant& anInstantAtSouthPoint
    );

    bool operator==(const Pass& aPass) const;

    bool operator!=(const Pass& aPass) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Pass& aPass);

    bool isDefined() const;

    bool isComplete() const;

    Pass::Type getType() const;

    Integer getRevolutionNumber() const;

    Interval getInterval() const;

    const Instant& accessInstantAtAscendingNode() const;

    const Instant& accessInstantAtDescendingNode() const;

    const Instant& accessInstantAtNorthPoint() const;

    const Instant& accessInstantAtSouthPoint() const;

    static Pass Undefined();

    static String StringFromType(const Pass::Type& aType);

    static String StringFromPhase(const Pass::Phase& aPhase);

    static String StringFromQuarter(const Pass::Quarter& aQuarter);

   private:
    Pass::Type type_;

    Integer revolutionNumber_;
    Interval interval_;
    Instant instantAtDescendingNode_;
    Instant instantAtNorthPoint_;
    Instant instantAtSouthPoint_;
};

}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
