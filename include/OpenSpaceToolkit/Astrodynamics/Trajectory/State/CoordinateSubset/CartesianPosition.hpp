/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianPosition__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>

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

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Cartesian Position.
class CartesianPosition : public CoordinateSubset
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     CartesianPosition cartesianPosition = {aName};
    /// @endcode
    ///
    /// @param aName a name
    CartesianPosition(const String& aName);

    /// @brief Destructor
    ~CartesianPosition();

    /// @brief Transforms the coordinate subset from one frame to another
    ///
    /// @param anInstant the reference frame associated to the coordinates
    /// @param aFullCoordinatesVector all coordinates
    /// @param fromFrame the reference frame associated to the coordinates
    /// @param toFrame the reference frame in which the coordinates are to be transformed
    /// @param aCoordinateBrokerSPtr a coordinate broker
    ///
    /// @return The resulting coordinate subset value expressed in the desired reference frame
    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrame,
        const Shared<const Frame>& toFrame,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const override;

    /// @brief Return the default instance
    ///
    /// @return The default instance
    static Shared<const CartesianPosition> Default();
};

}  // namespace coordinatesubset
}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
