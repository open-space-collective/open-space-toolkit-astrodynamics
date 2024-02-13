/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateBroker__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateBroker__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::type::Index;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief State coordinate broker.
class CoordinateBroker
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  CoordinateBroker coordinatesBroker();
    /// @endcode
    CoordinateBroker();

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  CoordinateBroker coordinatesBroker({asubsetSPtr, anotherSubsetSPtr});
    /// @endcode
    ///
    /// @param aCoordinateSubsetsArray the coordinate subsets to consider
    CoordinateBroker(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray);

    /// @brief Equal to operator
    ///
    /// @param aCoordinateBroker A coordinate broker
    ///
    /// @return True if CoordinateBrokers equal
    bool operator==(const CoordinateBroker& aCoordinateBroker) const;

    /// @brief Not equal to operator
    ///
    /// @param aCoordinateBroker A coordinate broker
    ///
    /// @return True if CoordinateBrokers are not equal
    bool operator!=(const CoordinateBroker& aCoordinateBroker) const;

    /// @brief Return the considered coordinate subsets
    ///
    /// @return The considered coordinate subsets
    const Array<Shared<const CoordinateSubset>>& accessSubsets() const;

    /// @brief Return the total number of coordinates
    ///
    /// @return The total number of coordinates
    Size getNumberOfCoordinates() const;

    /// @brief Return the total number of coordinate subsets
    ///
    /// @return The total number of coordinate subsets
    Size getNumberOfSubsets() const;

    /// @brief Return the considered coordinate subsets
    ///
    /// @return The considered coordinate subsets
    Array<Shared<const CoordinateSubset>> getSubsets() const;

    /// @brief Add a coordinate subset to be considered, returning the starting index it will occupy
    /// (or that it occupies if it was already added) in the state coordinates
    ///
    /// @param aCoordinateSubsetSPtr a coordinate subset to be considered
    ///
    /// @return The starting index of the subset in the state coordinates
    Index addSubset(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr);

    /// @brief Check if a coordinate subset has already been considered
    ///
    /// @param aCoordinateSubsetSPtr the coordinate subset to be checked
    ///
    /// @return True if the coordinate subset is already considered
    bool hasSubset(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr) const;

    /// @brief Extract the coordinates of a given subset from the full coordinates vector
    ///
    /// @param aFullCoordinatesVector the full coordinates vecctor
    /// @param aCoordinateSubset the coordinate subsets of interest
    ///
    /// @return The coordinates of the subset
    VectorXd extractCoordinate(const VectorXd& aFullCoordinatesVector, const CoordinateSubset& aCoordinateSubset)
        const;

    /// @brief Extract the coordinates of a given subset from the full coordinates vector
    ///
    /// @param aFullCoordinatesVector the full coordinates vecctor
    /// @param aCoordinateSubsetSPtr the coordinate subsets of interest
    ///
    /// @return The coordinates of the subset
    VectorXd extractCoordinate(
        const VectorXd& aFullCoordinatesVector, const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr
    ) const;

    /// @brief Extract the coordinates of an array of subsets from the full coordinates vector
    ///
    /// @param aFullCoordinatesVector the full coordinates vecctor
    /// @param aCoordinateSubsetsArray the array of coordinate subsets of interest
    ///
    /// @return The coordinates of the array of subsets in the same order as the input subsets
    VectorXd extractCoordinates(
        const VectorXd& aFullCoordinatesVector, const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
    ) const;

   private:
    Index nextCoordinateSubsetIndex_;
    Array<Shared<const CoordinateSubset>> coordinateSubsets_;
    Map<String, Index> coordinateSubsetsIndexMap_;

    bool hasSubset(const String& anId) const;
    Index getSubsetIndex(const String& anId) const;
};

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
