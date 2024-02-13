/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianVelocity__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{
namespace coordinatessubset
{

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::time::Instant;
using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::CoordinatesBroker;
using ostk::astrodynamics::trajectory::state::coordinatessubset::CartesianPosition;

/// @brief Cartesian Velocity.
class CartesianVelocity : public CoordinateSubset
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              CartesianVelocity velocity = {aCartesianPositionSPtr, aName};
    /// @endcode
    ///
    /// @param aCartesianPositionSPtr the associated Cartesian position
    /// @param aName a name
    CartesianVelocity(const Shared<const CartesianPosition>& aCartesianPositionSPtr, const String& aName);

    /// @brief Destructor
    ~CartesianVelocity();

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
    ) const override;

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
    ) const override;

    /// @brief Transforms the coordinate subset from one frame to another
    ///
    /// @param anInstant the reference frame associated to the coordinates
    /// @param aFullCoordinatesVector all coordinates
    /// @param fromFrame the reference frame associated to the coordinates
    /// @param toFrame the reference frame in which the coordinates are to be transformed
    /// @param aCoordinatesBrokerSPtr a coordinates broker
    ///
    /// @return The resulting coordinates subset value expressed in the desired reference frame
    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    ) const;

    /// @brief Return the default instance
    ///
    /// @return The default instance
    static Shared<const CartesianVelocity> Default();

   private:
    Shared<const CartesianPosition> cartesianPositionSPtr_;
};

}  // namespace coordinatessubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
