/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration__

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{
namespace coordinatesubset
{

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

/// @brief Cartesian Acceleration coordinate subset.
///
/// @details Represents the acceleration vector in Cartesian coordinates (x, y, z).
/// Depends on associated CartesianPosition and CartesianVelocity subsets for frame transformations.
class CartesianAcceleration : public CoordinateSubset
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Shared<const CartesianPosition> positionSPtr = CartesianPosition::Default() ;
    ///     Shared<const CartesianVelocity> velocitySPtr = CartesianVelocity::Default() ;
    ///     CartesianAcceleration acceleration = { positionSPtr, velocitySPtr, "Acceleration" } ;
    /// @endcode
    ///
    /// @param aCartesianPositionSPtr Shared pointer to a CartesianPosition coordinate subset.
    /// @param aCartesianVelocitySPtr Shared pointer to a CartesianVelocity coordinate subset.
    /// @param aName Name of the coordinate subset.
    CartesianAcceleration(
        const Shared<const CartesianPosition>& aCartesianPositionSPtr,
        const Shared<const CartesianVelocity>& aCartesianVelocitySPtr,
        const String& aName
    );

    /// @brief Destructor.
    ~CartesianAcceleration();

    /// @brief Transforms the coordinates to a different frame.
    ///
    /// @param anInstant The instant at which the transformation is performed
    /// @param aFullCoordinatesVector The full coordinates vector to be transformed
    /// @param fromFrame Shared pointer to the initial frame of reference
    /// @param toFrame Shared pointer to the target frame of reference
    /// @param aCoordinateBrokerSPtr Shared pointer to the coordinate broker
    ///
    /// @return The transformed coordinates vector
    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const override;

    /// @brief Return the default instance.
    ///
    /// @code{.cpp}
    ///     Shared<const CartesianAcceleration> accelerationSPtr = CartesianAcceleration::Default() ;
    /// @endcode
    ///
    /// @return Shared pointer to the default CartesianAcceleration instance.
    static Shared<const CartesianAcceleration> Default();

    /// @brief Return the thrust acceleration instance.
    ///
    /// @code{.cpp}
    ///     Shared<const CartesianAcceleration> thrustAccelSPtr = CartesianAcceleration::ThrustAcceleration() ;
    /// @endcode
    ///
    /// @return Shared pointer to the thrust acceleration instance.
    static Shared<const CartesianAcceleration> ThrustAcceleration();

   private:
    Shared<const CartesianPosition> cartesianPositionSPtr_;
    Shared<const CartesianVelocity> cartesianVelocitySPtr_;
};

}  // namespace coordinatesubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
