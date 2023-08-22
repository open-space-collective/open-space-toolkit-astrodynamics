/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinatesBroker__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Map.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Map;
using ostk::core::types::Index;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::astro::trajectory::state::CoordinatesSubset;

/// @brief  State coordinates broker.

class CoordinatesBroker
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     CoordinatesBroker coordinatesBroker();
    /// @endcode

    CoordinatesBroker();

    /// @brief              Constructor
    ///
    /// @code
    ///                     CoordinatesBroker coordinatesBroker({asubsetSPtr, anotherSubsetSPtr});
    /// @endcode
    ///
    /// @param              [in] aCoordinatesSubsetsArray the coordinates subsets to consider

    CoordinatesBroker(const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray);

    /// @brief              Equal to operator
    ///
    /// @param              [in] aCoordinatesBroker A coordinates broker
    ///
    /// @return             True if CoordinateBrokers equal

    bool operator==(const CoordinatesBroker& aCoordinatesBroker) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aCoordinatesBroker A coordinates broker
    ///
    /// @return             True if CoordinateBrokers are not equal

    bool operator!=(const CoordinatesBroker& aCoordinatesBroker) const;

    /// @brief              Return the considered coordinate subsets
    ///
    /// @return             The considered coordinate subsets

    const Array<Shared<const CoordinatesSubset>>& accessSubsets() const;

    /// @brief              Return the total number of coordinates
    ///
    /// @return             The total number of coordinates

    Size getNumberOfCoordinates() const;

    /// @brief              Return the total number of coordinate subsets
    ///
    /// @return             The total number of coordinate subsets

    Size getNumberOfSubsets() const;

    /// @brief              Return the considered coordinate subsets
    ///
    /// @return             The considered coordinate subsets

    Array<Shared<const CoordinatesSubset>> getSubsets() const;

    /// @brief              Add a coordinates subset to be considered, returning the starting index it will occupy (or
    /// that it occupies if it was already added) in the state coordinates
    ///
    /// @param              [in] aCoordinatesSubsetSPtr a coordinates subset to be considered
    ///
    /// @return             The starting index of the subset in the state coordinates

    Index addSubset(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr);

    /// @brief              Check if a coordinates subset has already been considered
    ///
    /// @param              [in] aCoordinatesSubsetSPtr the coordinates subset to be checked
    ///
    /// @return             True if the coordinates subset is already considered

    bool hasSubset(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const;

    /// @brief              Extract the coordinates of a given subset from the full coordinates vector
    ///
    /// @param              [in] aFullCoordinatesVector the full coordinates vecctor
    /// @param              [in] aCoordinatesSubset the coordinates subsets of interest
    ///
    /// @return             The coordinates of the subset

    VectorXd extractCoordinates(const VectorXd& aFullCoordinatesVector, const CoordinatesSubset& aCoordinatesSubset)
        const;

    /// @brief              Extract the coordinates of a given subset from the full coordinates vector
    ///
    /// @param              [in] aFullCoordinatesVector the full coordinates vecctor
    /// @param              [in] aCoordinatesSubsetSPtr the coordinates subsets of interest
    ///
    /// @return             The coordinates of the subset

    VectorXd extractCoordinates(
        const VectorXd& aFullCoordinatesVector, const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr
    ) const;

   private:
    Index nextCoordinatesSubsetIndex_;
    Array<Shared<const CoordinatesSubset>> coordinatesSubsets_;
    Map<String, Index> coordinatesSubsetsIndexMap_;

    bool hasSubset(const String& anId) const;
    Index getSubsetIndex(const String& anId) const;
};

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
