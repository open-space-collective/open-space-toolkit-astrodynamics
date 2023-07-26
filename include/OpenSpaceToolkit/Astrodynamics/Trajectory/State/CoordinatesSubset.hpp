/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset__

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

    /// @brief              Equals to operator
    ///
    /// @param              [in] aCoordinatesSubset The coordinate subset to compare it to
    ///
    /// @return             True if both instances refer to the same subset and have the same size (i.e. dimension)

    bool operator==(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief              Not equals to operator
    ///
    /// @param              [in] aCoordinatesSubset The coordinate subset to compare it to
    ///
    /// @return             False if both instances refer to the same subset and have the same size (i.e. dimension)

    bool operator!=(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief              Returns the unique identifier of the instance
    ///
    /// @return             The unique identifier of the instance

    String getId() const;

    /// @brief              Returns the name of the instance
    ///
    /// @return             The name of the instance

    String getName() const;

    /// @brief              Returns the size (i.e. dimension) of the instance
    ///
    /// @return             The size (i.e. dimension) of the instance

    Size getSize() const;

    /// @brief              Adds two coordinates subsets
    ///
    /// @param              [in] anInstant the instant associated to the coordinates
    /// @param              [in] allCoordinates_1 first set of all coordinates
    /// @param              [in] allCoordinates_2 second set of all coordinates
    /// @param              [in] aFrame the reference frame in which the coordinates are resolved
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value (subset_1 + subset_2)

    VectorXd add(
        const Instant& anInstant,
        const VectorXd& allCoordinates_1,
        const VectorXd& allCoordinates_2,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

    /// @brief              Subtracts two coordinates subsets
    ///
    /// @param              [in] anInstant the instant associated to the coordinates
    /// @param              [in] allCoordinates_1 first set of all coordinates
    /// @param              [in] allCoordinates_2 second set of all coordinates
    /// @param              [in] aFrame the reference frame associated to the coordinates
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value (subset_1 - subset_2)

    VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& allCoordinates_1,
        const VectorXd& allCoordinates_2,
        const Shared<const Frame>& aFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

    /// @brief              Transforms the coordinate subset from one frame to another
    ///
    /// @param              [in] anInstant the reference frame associated to the coordinates
    /// @param              [in] allCoordinates all coordinates
    /// @param              [in] fromFrame the reference frame associated to the coordinates
    /// @param              [in] toFrame the reference frame in which the coordinates are to be transformed
    /// @param              [in] aCoordinatesBroker a coordinates broker
    ///
    /// @return             The resulting coordinates subset value expressed in the desired reference frame

    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& allCoordinates,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker
    ) const;

   private:
    String id_;
    String name_;
    Size size_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

namespace std
{

using ostk::core::types::String;

using ostk::astro::trajectory::CoordinatesSubset;

template <>
struct hash<CoordinatesSubset>
{
    size_t operator()(const CoordinatesSubset& aCoordinatesSubset) const
    {
        return hash<String>()(aCoordinatesSubset.getId());
    }
};

}  // namespace std

#endif
