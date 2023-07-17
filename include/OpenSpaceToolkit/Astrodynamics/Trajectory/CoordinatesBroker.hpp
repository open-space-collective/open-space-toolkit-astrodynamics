/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesBroker__

#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
using ostk::core::types::Index;
using ostk::core::types::Size;
using ostk::core::types::String;

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

    /// @brief              Equals to operator
    ///
    /// @param              [in] aCoordinatesBroker The instance to compare to
    ///
    /// @return             True if both instances are considered equal

    bool operator==(const CoordinatesBroker& aCoordinatesBroker) const;

    /// @brief              Not equals to operator
    ///
    /// @param              [in] aCoordinatesBroker The instance to compare to
    ///
    /// @return             True if both instances are not considered equal

    bool operator!=(const CoordinatesBroker& aCoordinatesBroker) const;

    /// @brief              Adds a coordinates subset to be considered, returning the starting index it will occupy (or
    /// that it occupies if it was already added) in the state coordinates
    ///
    /// @param              [in] aCoordinatesSubset a coordinates subset to be considered
    ///
    /// @return             The starting index of the subset in the state coordinates

    Index addSubset(const CoordinatesSubset& aCoordinatesSubset);

    /// @brief              Checks if a coordinates subset ID has already been considered
    ///
    /// @param              [in] anId the coordinates subset ID to be checked
    ///
    /// @return             True if the coordinates subset ID is already considered

    bool hasSubset(const String& anId) const;

    /// @brief              Checks if a coordinates subset has already been considered
    ///
    /// @param              [in] aCoordinatesSubset the coordinates subset to be checked
    ///
    /// @return             True if the coordinates subset is already considered

    bool hasSubset(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief              Returns the starting index of a coordinates subset ID in the state coordinates
    ///
    /// @param              [in] anId the coordinates subset ID
    ///
    /// @return             The starting index of the subset ID in the state coordinates, throwing an exception if the
    /// coordinates subset is not present

    Index getSubsetIndex(const String& anId) const;

    /// @brief              Returns the starting index of a coordinates subset in the state coordinates
    ///
    /// @param              [in] aCoordinatesSubset the coordinates subset
    ///
    /// @return             The starting index of the subset in the state coordinates, throwing an exception if the
    /// coordinates subset is not present

    Index getSubsetIndex(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief              Returns the total number of coordinates
    ///
    /// @return             The total number of coordinates

    Size getNumberOfCoordinates() const;

    /// @brief              Returns the total number of coordinate subsets
    ///
    /// @return             The total number of coordinate subsets

    Size getNumberOfSubsets() const;

    /// @brief              Extracts the coordinates of a given subset from the full coordinates vector
    ///
    /// @param              [in] allCoordinates the full coordinates vecctor
    /// @param              [in] aCoordinatesSubset the coordinates subsets of interest
    ///
    /// @return             The coordinates of the subset

    VectorXd extract(const VectorXd& allCoordinates, const CoordinatesSubset& aCoordinatesSubset) const;

   private:
    Index nextCoordinatesSubsetIndex_;
    std::unordered_map<String, Index> coordinatesSubsetIndexMap_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
