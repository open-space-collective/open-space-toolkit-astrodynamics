/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubset__

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Integer;
using ostk::core::types::String;

/// @brief  State coordinates subset.

class CoordinatesSubset
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     CoordinatesSubset coordinatesSubset("POSITION", 3) ;
    /// @endcode
    ///
    /// @param              [in] anId The coordinate subset unique identifier
    /// @param              [in] aSize The coordinate subset size
    CoordinatesSubset(const String& anId, const Integer& aSize);

    bool operator==(const CoordinatesSubset& aCoordinatesSubset) const;

    bool operator!=(const CoordinatesSubset& aCoordinatesSubset) const;

    bool isDefined() const;

    String getId() const;

    Integer getSize() const;

    static CoordinatesSubset Undefined();

    static CoordinatesSubset Position();

    static CoordinatesSubset Velocity();

    static CoordinatesSubset Mass();

   private:
    String id_;
    Integer size_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
