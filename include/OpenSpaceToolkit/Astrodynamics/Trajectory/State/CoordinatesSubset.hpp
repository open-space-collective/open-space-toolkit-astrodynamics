/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesSubset__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{

using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

class CoordinatesBroker;

/// @brief  State coordinates subset. It contains information related to a particular group of coordinates. It does not
/// hold coordinates value information.

class CoordinatesSubset
{
   public:
    /// @brief              Constructor
    ///
    /// @param              [in] aName A name
    /// @param              [in] aSize A size (i.e. dimension)

    CoordinatesSubset(const String& aName, const Size& aSize);

    /// @brief              Equal to operator
    ///
    /// @param              [in] aCoordinatesSubset The coordinate subset to compare it to
    ///
    /// @return             True if CoordinatesSubsets are equal

    bool operator==(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aCoordinatesSubset The coordinate subset to compare it to
    ///
    /// @return             True if CoordinatesSubsets are not equal

    bool operator!=(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief              Return the unique identifier of the instance
    ///
    /// @return             The unique identifier of the instance

    String getId() const;

    /// @brief              Return the name of the instance
    ///
    /// @return             The name of the instance

    String getName() const;

    /// @brief              Return the size (i.e. dimension) of the instance
    ///
    /// @return             The size (i.e. dimension) of the instance

    Size getSize() const;

    /// @brief              Add two coordinates subsets
    ///
    /// @param              [in] anInstant the instant associated to the coordinates
    /// @param              [in] aFullCoordinates first set of all coordinates
    /// @param              [in] anotherFullCoordinates second set of all coordinates
    /// @param              [in] aFrame the reference frame in which the coordinates are resolved
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value (subset_1 + subset_2)

    virtual VectorXd add(
        const Instant& anInstant,
        const VectorXd& aFullCoordinates,
        const VectorXd& anotherFullCoordinates,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const = 0;

    /// @brief              Subtract two coordinates subsets
    ///
    /// @param              [in] anInstant the instant associated to the coordinates
    /// @param              [in] aFullCoordinates first set of all coordinates
    /// @param              [in] anotherFullCoordinates second set of all coordinates
    /// @param              [in] aFrame the reference frame associated to the coordinates
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value (subset_1 - subset_2)

    virtual VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& aFullCoordinates,
        const VectorXd& anotherFullCoordinates,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const = 0;

    /// @brief              Transform the coordinate subset from one frame to another
    ///
    /// @param              [in] anInstant the reference frame associated to the coordinates
    /// @param              [in] allCoordinates all coordinates
    /// @param              [in] fromFrame the reference frame associated to the coordinates
    /// @param              [in] toFrame the reference frame in which the coordinates are to be transformed
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value expressed in the desired reference frame

    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& allCoordinates,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const = 0;

   private:
    String id_;
    String name_;
    Size size_;
};

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
