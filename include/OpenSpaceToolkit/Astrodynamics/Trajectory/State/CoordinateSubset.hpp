/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset__

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

class CoordinateBroker;

/// @brief State coordinate subset. It contains information related to a particular group of coordinates. It does not
/// contain the coordinate values.
class CoordinateSubset
{
   public:
    /// @brief Constructor
    ///
    ///              The default CoordinateSubset instance is frame-invariant and implements element-wise
    ///              addition/subtraction.
    /// @code{.cpp}
    ///              CoordinateSubset coordinateSubset = {aName, aSize};
    /// @endcode
    ///
    /// @param aName A name
    /// @param aSize A size (i.e. dimension)
    CoordinateSubset(const String& aName, const Size& aSize);

    /// @brief Equal to operator
    ///
    /// @param aCoordinateSubset The coordinate subset to compare it to
    ///
    /// @return True if CoordinateSubsets are equal
    bool operator==(const CoordinateSubset& aCoordinateSubset) const;

    /// @brief Not equal to operator
    ///
    /// @param aCoordinateSubset The coordinate subset to compare it to
    ///
    /// @return True if CoordinateSubsets are not equal
    bool operator!=(const CoordinateSubset& aCoordinateSubset) const;

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

    /// @brief Add two coordinate subsets
    ///
    /// @param anInstant the instant associated to the coordinates
    /// @param aFullCoordinatesVector first set of all coordinates
    /// @param anotherFullCoordinatesVector second set of all coordinates
    /// @param aFrameSPtr the reference frame in which the coordinates are resolved
    /// @param aCoordinateBrokerSPtr a coordinate broker
    ///
    /// @return The resulting coordinate subset value (subset_1 + subset_2)
    virtual VectorXd add(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const;

    /// @brief Subtract two coordinate subsets
    ///
    /// @param anInstant the instant associated to the coordinates
    /// @param aFullCoordinatesVector first set of all coordinates
    /// @param anotherFullCoordinatesVector second set of all coordinates
    /// @param aFrameSPtr the reference frame associated to the coordinates
    /// @param aCoordinateBrokerSPtr a coordinate broker
    ///
    /// @return The resulting coordinate subset value (subset_1 - subset_2)
    virtual VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const;

    /// @brief Transform the coordinate subset from one frame to another
    ///
    /// @param anInstant the reference frame associated to the coordinates
    /// @param aFullCoordinatesVector all coordinates
    /// @param fromFrameSPtr the reference frame associated to the coordinates
    /// @param toFrameSPtr the reference frame in which the coordinates are to be transformed
    /// @param aCoordinateBrokerSPtr a coordinate broker
    ///
    /// @return The resulting coordinate subset value expressed in the desired reference frame
    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrameSPtr,
        const Shared<const Frame>& toFrameSPtr,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const;

    /// @brief Return a default Mass instance
    ///
    /// @return The default Mass shared pointer instance
    static Shared<const CoordinateSubset> Mass();

    /// @brief Return a default Surface Area instance
    ///
    /// @return The default Surface Area shared pointer instance
    static Shared<const CoordinateSubset> SurfaceArea();

    /// @brief Return a default DragCoefficient instance
    ///
    /// @return The default DragCoefficient shared pointer instance
    static Shared<const CoordinateSubset> DragCoefficient();

   private:
    String name_;
    Size size_;
    String id_;
};

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
