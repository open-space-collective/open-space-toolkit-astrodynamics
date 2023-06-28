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
    CoordinatesBroker();

    Integer addSubset(CoordinatesSubset& aCoordinatesSubset);

    bool hasSubset(CoordinatesSubset& aCoordinatesSubset) const;

    Integer getSubsetIndex(CoordinatesSubset& aCoordinatesSubset) const;

    Integer getNumberOfCoordinates() const;

    Integer getNumberOfSubsets() const;

   private:
    Integer nextIndex_;
    std::unordered_map<CoordinatesSubset, Integer> map_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
