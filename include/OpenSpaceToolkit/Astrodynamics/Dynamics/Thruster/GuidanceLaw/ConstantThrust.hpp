/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_ConstantThrust__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_ConstantThrust__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{
namespace thruster
{
namespace guidancelaw
{

using ostk::core::types::Real;

using ostk::math::obj::Vector3d;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::LocalOrbitalFrameDirection;

/// @brief                      Define the acceleration experienced by a point mass due to a constant thrust
/// guidance law

class ConstantThrust : public GuidanceLaw
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         const LocalOrbitalFrameDirection aThrustDirection = { ... };
    ///                         ConstantThrust constantThrust = {aThrustDirection};
    /// @endcode
    ///
    /// @param                  [in] aThrustDirection A local orbital frame thrust direction

    ConstantThrust(const LocalOrbitalFrameDirection& aThrustDirection);

    /// @brief                  Destructor

    virtual ~ConstantThrust() override;

    /// @brief                  Output stream operator
    ///
    /// @param                  in] anOutputStream An output stream
    /// @param                  [in] aThruster A constant thrust thruster dynamics
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const ConstantThrust& aConstantThrust);

    /// @brief                  Get local thrust direction
    ///
    /// @code
    ///                         LocalOrbitalFrameDirection localThrustDirection =
    ///                         constantThrustThruster.getLocalThrustDirection();
    /// @endcode
    ///
    /// @return                 Local orbital frame direction

    LocalOrbitalFrameDirection getLocalThrustDirection() const;

    /// @brief                  Compute acceleration
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aPositionCoordinates The position coordinates
    /// @param                  [in] aVelocityCoordinates The velocity coordinates
    /// @param                  [in] aThrustAcceleration The thrust acceleration
    /// @param                  [in] outputFrameSPtr The frame in which the acceleration is expressed
    ///
    /// @return                 Acceleration

    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override;

    /// @brief                  Print constant thrust thruster dynamics
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] displayDecorator A boolean indicating whether or not to display the decorator
    /// during printing

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief                  Intrack Constant thrust dynamics
    ///
    /// @param                  [in] velocityDirection A bool representing the direction of the thrust. Defaults to
    /// true.
    ///
    /// @return                 Constant Thrust dynamics

    static ConstantThrust Intrack(const bool& velocityDirection = true);

   private:
    LocalOrbitalFrameDirection localOrbitalFrameDirection_;
};

}  // namespace guidancelaw
}  // namespace thruster
}  // namespace dynamics
}  // namespace astro
}  // namespace ostk

#endif
