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

using ostk::math::object::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

class CoordinatesBroker;

/// @brief State coordinates subset. It contains information related to a particular group of coordinates. It does not
/// contain the coordinate values.
class CoordinatesSubset
{
   public:
    /// @brief Constructor
    ///
    ///              The default CoordinatesSubset instance is frame-invariant and implements element-wise
    ///              addition/subtraction.
    /// @code{.cpp}
    ///              CoordinateSubset coordinateSubset = {aName, aSize};
    /// @endcode
    ///
    /// @param aName A name
    /// @param aSize A size (i.e. dimension)
    CoordinatesSubset(const String& aName, const Size& aSize);

    /// @brief Equal to operator
    ///
    /// @param aCoordinatesSubset The coordinate subset to compare it to
    ///
    /// @return True if CoordinatesSubsets are equal
    bool operator==(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief Not equal to operator
    ///
    /// @param aCoordinatesSubset The coordinate subset to compare it to
    ///
    /// @return True if CoordinatesSubsets are not equal
    bool operator!=(const CoordinatesSubset& aCoordinatesSubset) const;

    /// @brief Return the unique identifier of the instance
    ///
    /// @return The unique identifier of the instance
    String getId() const;

    /// @brief Return the name of the instance
    ///
    /// @return The name of the instance
    String getName() const;

    /// @brief Return the size (i.e. dimension) of the instance
    ///
    /// @return The size (i.e. dimension) of the instance
    Size getSize() const;

    /// @brief Add two coordinates subsets
    ///
    /// @param anInstant the instant associated to the coordinates
    /// @param aFullCoordinatesVector first set of all coordinates
    /// @param anotherFullCoordinatesVector second set of all coordinates
    /// @param aFrameSPtr the reference frame in which the coordinates are resolved
    /// @param aCoordinatesBrokerSPtr a coordinates broker
    ///
    /// @return The resulting coordinates subset value (subset_1 + subset_2)
    virtual VectorXd add(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    ) const;

    /// @brief Subtract two coordinates subsets
    ///
    /// @param anInstant the instant associated to the coordinates
    /// @param aFullCoordinatesVector first set of all coordinates
    /// @param anotherFullCoordinatesVector second set of all coordinates
    /// @param aFrameSPtr the reference frame associated to the coordinates
    /// @param aCoordinatesBrokerSPtr a coordinates broker
    ///
    /// @return The resulting coordinates subset value (subset_1 - subset_2)
    virtual VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    ) const;

    /// @brief Transform the coordinate subset from one frame to another
    ///
    /// @param anInstant the reference frame associated to the coordinates
    /// @param aFullCoordinatesVector all coordinates
    /// @param fromFrameSPtr the reference frame associated to the coordinates
    /// @param toFrameSPtr the reference frame in which the coordinates are to be transformed
    /// @param aCoordinatesBrokerSPtr a coordinates broker
    ///
    /// @return The resulting coordinates subset value expressed in the desired reference frame
    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrameSPtr,
        const Shared<const Frame>& toFrameSPtr,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    ) const;

    /// @brief Return a default Mass instance
    ///
    /// @return The default Mass shared pointer instance
    static Shared<const CoordinatesSubset> Mass();

    /// @brief Return a default Surface Area instance
    ///
    /// @return The default Surface Area shared pointer instance
    static Shared<const CoordinatesSubset> SurfaceArea();

    /// @brief Return a default DragCoefficient instance
    ///
    /// @return The default DragCoefficient shared pointer instance
    static Shared<const CoordinatesSubset> DragCoefficient();

   private:
    String name_;
    Size size_;
    String id_;
};

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
