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

/// @brief State coordinate broker
/// @details Manages the mapping between coordinate subsets and their indices within a full state
/// coordinates vector. The broker tracks which coordinate subsets are present and their
/// positions, enabling extraction of individual subset values from the full vector.
class CoordinateBroker
{
   public:
    /// @brief Default constructor
    ///
    /// @code{.cpp}
    ///              CoordinateBroker coordinateBroker ;
    /// @endcode
    CoordinateBroker();

    /// @brief Constructor with coordinate subsets
    ///
    /// @code{.cpp}
    ///              CoordinateBroker coordinateBroker = {
    ///                  {CartesianPosition::Default(), CartesianVelocity::Default()}
    ///              } ;
    /// @endcode
    ///
    /// @param aCoordinateSubsetsArray The coordinate subsets to consider
    CoordinateBroker(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray);

    /// @brief Equality operator
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker1 = { ... } ;
    ///              CoordinateBroker broker2 = { ... } ;
    ///              bool isEqual = (broker1 == broker2) ;
    /// @endcode
    ///
    /// @param aCoordinateBroker A coordinate broker
    /// @return True if CoordinateBrokers are equal
    bool operator==(const CoordinateBroker& aCoordinateBroker) const;

    /// @brief Inequality operator
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker1 = { ... } ;
    ///              CoordinateBroker broker2 = { ... } ;
    ///              bool isNotEqual = (broker1 != broker2) ;
    /// @endcode
    ///
    /// @param aCoordinateBroker A coordinate broker
    /// @return True if CoordinateBrokers are not equal
    bool operator!=(const CoordinateBroker& aCoordinateBroker) const;

    /// @brief Access the considered coordinate subsets
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              const Array<Shared<const CoordinateSubset>>& subsets = broker.accessSubsets() ;
    /// @endcode
    ///
    /// @return A const reference to the considered coordinate subsets
    const Array<Shared<const CoordinateSubset>>& accessSubsets() const;

    /// @brief Get the total number of coordinates across all subsets
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              Size count = broker.getNumberOfCoordinates() ;
    /// @endcode
    ///
    /// @return The total number of coordinates
    Size getNumberOfCoordinates() const;

    /// @brief Get the total number of coordinate subsets
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              Size count = broker.getNumberOfSubsets() ;
    /// @endcode
    ///
    /// @return The total number of coordinate subsets
    Size getNumberOfSubsets() const;

    /// @brief Get the considered coordinate subsets
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              Array<Shared<const CoordinateSubset>> subsets = broker.getSubsets() ;
    /// @endcode
    ///
    /// @return The considered coordinate subsets
    Array<Shared<const CoordinateSubset>> getSubsets() const;

    /// @brief Add a coordinate subset to be considered
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker ;
    ///              Index index = broker.addSubset(CartesianPosition::Default()) ;
    /// @endcode
    ///
    /// @param aCoordinateSubsetSPtr A coordinate subset to be considered
    /// @return The starting index of the subset in the state coordinates (or the existing
    ///         index if the subset was already added)
    Index addSubset(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr);

    /// @brief Check if a coordinate subset has already been considered
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              bool exists = broker.hasSubset(CartesianPosition::Default()) ;
    /// @endcode
    ///
    /// @param aCoordinateSubsetSPtr The coordinate subset to be checked
    /// @return True if the coordinate subset is already considered
    bool hasSubset(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr) const;

    /// @brief Get the starting index of a coordinate subset
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              Index index = broker.getSubsetIndex(CartesianPosition::Default()) ;
    /// @endcode
    ///
    /// @param aCoordinateSubsetSPtr The coordinate subset to be checked
    /// @return The starting index of the coordinate subset
    Index getSubsetIndex(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr) const;

    /// @brief Extract the coordinates of a given subset from the full coordinates vector
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              VectorXd fullCoordinates = { ... } ;
    ///              CoordinateSubset subset = { ... } ;
    ///              VectorXd coordinates = broker.extractCoordinate(fullCoordinates, subset) ;
    /// @endcode
    ///
    /// @param aFullCoordinatesVector The full coordinates vector
    /// @param aCoordinateSubset The coordinate subset of interest
    /// @return The coordinates of the subset
    VectorXd extractCoordinate(const VectorXd& aFullCoordinatesVector, const CoordinateSubset& aCoordinateSubset) const;

    /// @brief Extract the coordinates of a given subset from the full coordinates vector
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              VectorXd fullCoordinates = { ... } ;
    ///              VectorXd coordinates = broker.extractCoordinate(
    ///                  fullCoordinates, CartesianPosition::Default()
    ///              ) ;
    /// @endcode
    ///
    /// @param aFullCoordinatesVector The full coordinates vector
    /// @param aCoordinateSubsetSPtr The coordinate subset of interest
    /// @return The coordinates of the subset
    VectorXd extractCoordinate(
        const VectorXd& aFullCoordinatesVector, const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr
    ) const;

    /// @brief Extract the coordinates of an array of subsets from the full coordinates vector
    ///
    /// @code{.cpp}
    ///              CoordinateBroker broker = { ... } ;
    ///              VectorXd fullCoordinates = { ... } ;
    ///              VectorXd coordinates = broker.extractCoordinates(
    ///                  fullCoordinates, {CartesianPosition::Default(), CartesianVelocity::Default()}
    ///              ) ;
    /// @endcode
    ///
    /// @param aFullCoordinatesVector The full coordinates vector
    /// @param aCoordinateSubsetsArray The array of coordinate subsets of interest
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
