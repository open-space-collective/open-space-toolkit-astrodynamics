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

/// @brief  State coordinates subset.

class CoordinatesSubset
{
   public:
    /// @brief              Constructor
    ///
    /// @param              [in] aName The coordinate subset name
    /// @param              [in] aSize The coordinate subset size

    CoordinatesSubset(const String& aName, const Size& aSize);

    bool operator==(const CoordinatesSubset& aCoordinatesSubset) const;

    bool operator!=(const CoordinatesSubset& aCoordinatesSubset) const;

    String getId() const;

    String getName() const;

    Size getSize() const;

    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& allCoordinates,
        const Shared<const Frame>& fromFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker,
        const Shared<const Frame>& toFrame
    ) const = 0;

    // virtual VectorXd operator+(const CoordinatesSubset& aCoordinatesSbuset) const = 0;
    // static CoordinatesSubset Undefined();

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
