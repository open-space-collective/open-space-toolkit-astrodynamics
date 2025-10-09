/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ConstantThrust__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::type::Real;

using ostk::mathematics::object::Vector3d;

using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;

using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;

/// @brief Define the acceleration experienced by a point mass due to a constant thrust
/// guidance law
class ConstantThrust : public GuidanceLaw
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  const LocalOrbitalFrameDirection aThrustDirection = { ... };
    ///                  ConstantThrust constantThrust = {aThrustDirection};
    /// @endcode
    ///
    /// @param aThrustDirection A local orbital frame thrust direction
    ConstantThrust(const LocalOrbitalFrameDirection& aThrustDirection);

    /// @brief Destructor
    virtual ~ConstantThrust() override;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aConstantThrust A constant thrust guidance law
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const ConstantThrust& aConstantThrust);

    /// @brief Get local thrust direction
    ///
    /// @code{.cpp}
    ///                  LocalOrbitalFrameDirection localThrustDirection =
    ///                  constantThrustGuidanceLaw.getLocalThrustDirection();
    /// @endcode
    ///
    /// @return Local orbital frame direction
    LocalOrbitalFrameDirection getLocalThrustDirection() const;

    /// @brief Compute acceleration
    ///
    /// @param anInstant An instant
    /// @param aPositionCoordinates The position coordinates
    /// @param aVelocityCoordinates The velocity coordinates
    /// @param aThrustAcceleration The thrust acceleration
    /// @param outputFrameSPtr The frame in which the acceleration is expressed
    ///
    /// @return Acceleration
    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override;

    /// @brief Print constant thrust guidance law
    ///
    /// @param anOutputStream An output stream
    /// @param displayDecorator A boolean indicating whether or not to display the decorator
    /// during printing
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Intrack Constant thrust guidance law
    ///
    /// @param velocityDirection A bool representing the direction of the thrust, with true
    /// meaning along the velocity direction. Defaults to true.
    ///
    /// @return Constant Thrust guidance law
    static ConstantThrust Intrack(const bool& velocityDirection = true);

    /// @brief Create a constant thrust guidance law from a maneuver.
    ///
    /// The local orbital frame maneuver's mean thrust direction is calculated and used to create a
    /// constant thrust guidance law in said direction.
    ///
    /// If defined, a runtime error will be thrown if the maximum allowed angular offset between the original thrust
    /// acceleration direction and the mean thrust direction is violated.
    ///
    /// @code{.cpp}
    ///                  ConstantThrust constantThrustGuidanceLaw = ConstantThrust::FromManeuver(maneuver,
    ///                  LocalOrbitalFrameFactory::TNW(Frame::GCRF()));
    /// @endcode
    ///
    /// @param aManeuver A maneuver
    /// @param aLocalOrbitalFrameFactorySPtr A local orbital frame factory
    /// @param aMaximumAllowedAngularOffset A maximum allowed angular offset to consider (if any). Defaults to
    /// Undefined.
    ///
    /// @return Constant Thrust guidance law
    static ConstantThrust FromManeuver(
        const Maneuver& aManeuver,
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr,
        const Angle& aMaximumAllowedAngularOffset = Angle::Undefined()
    );

   private:
    LocalOrbitalFrameDirection localOrbitalFrameDirection_;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
