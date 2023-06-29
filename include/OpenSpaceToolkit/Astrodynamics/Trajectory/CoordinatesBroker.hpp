/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker__

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Integer;

using ostk::astro::trajectory::CoordinatesSubset;

/// @brief  State coordinates broker.

class CoordinatesBroker
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     CoordinatesBroker coordinatesSubset() ;
    /// @endcode

    CoordinatesBroker();

    /// @brief              Adds a coordinates subset to be considered, returning the starting index it will occupy (or
    /// that it occupies if it was already added) in the state coordinates
    ///
    /// @param              [in] aCoordinatesSubset a coordinate subset to be considered
    ///
    /// @return             The starting index of the subset in the state coordinates

    Integer addSubset(const CoordinatesSubset& aCoordinatesSubset);

    /// @brief              Checks if a coordinates subset has already been considered
    ///
    /// @param              [in] aCoordinatesSubset the coordinates subset to be checked
    ///
    /// @return             True if the coordinates subset is already considered

    bool hasSubset(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief              Returns the starting index of a coordinates subset in the state coordinates
    ///
    /// @param              [in] aCoordinatesSubset the coordinates subset
    ///
    /// @return             The starting index of the subset int the state coordinates, throwing an exception if the
    /// coordinates subset is not present

    Integer getSubsetIndex(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief              Returns the total number of coordinates
    ///
    /// @return             The total number of coordinates

    Integer getNumberOfCoordinates() const;

    /// @brief              Returns the total number of coordinate subsets
    ///
    /// @return             The total number of coordinate subsets

    Integer getNumberOfSubsets() const;

   private:
    Integer nextIndex_;
    std::unordered_map<CoordinatesSubset, Integer> map_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
