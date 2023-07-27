/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{
namespace thruster
{

using ostk::core::types::Real;

using ostk::math::obj::Vector3d;
using ostk::math::geom::d3::trf::rot::RotationMatrix;

using ostk::physics::data::Direction;
using ostk::physics::time::Instant;
using ostk::physics::units::Mass;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::State;

/// @brief                      Define the acceleration experienced by a point mass due to a constant thrust thruster dynamics

class ConstantThrustThruster : public Thruster
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const String aName = { ... };
    ///                     const SatelliteSystem aSatelliteSystem = { ... };
    ///                     const Direction aThrustDirection = { ... };
    ///                     ConstantThrustThruster constantThrustThrusterDynamics = { aName, aSatelliteSystem, aThrustDirection};
    /// @endcode
    ///
    /// @param              [in] aDirection A direction

    ConstantThrustThruster(const SatelliteSystem& aSatelliteSystem, const Direction& aThrustDirection, const String& aName = String::Empty());

    /// @brief              Destructor

    virtual ~ConstantThrustThruster() override;

    /// @brief              Clone constant thrust thruster dynamics
    ///
    /// @return             Pointer to cloned constant thrust thruster dynamics

    virtual ConstantThrustThruster* clone() const override;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aConstantThrustThruster A constant thrust thruster dynamics
    /// @return             True if constant thrust thruster dynamics are equal

    bool operator==(const ConstantThrustThruster& aConstantThrustThruster) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aThruster A constant thrust thruster dynamics
    /// @return             True if constant thrust thruster dynamics are not equal

    bool operator!=(const ConstantThrustThruster& aConstantThrustThruster) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aThruster A constant thrust thruster dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const ConstantThrustThruster& aConstantThrustThruster);

    /// @brief              Check if constant thrust thruster dynamics is defined
    ///
    /// @return             True if constant thrust thruster dynamics is defined

    virtual bool isDefined() const override;

    /// @brief              Get constant thrust thruster dynamics' thrust
    ///
    /// @code
    ///                     Scalar thrust = constantThrustThruster.getThrust() ;
    /// @endcode
    ///
    /// @return             Scalar

    Scalar getThrust() const;  // TBI: Let's see how to improve this a bit later

    /// @brief              Apply contribution to the state derivative
    ///
    /// @param              [in] x A state vector
    /// @param              [in] dxdt A state derivative vector
    /// @param              [in] anInstant An instant

    virtual void applyContribution(
        const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const Instant& anInstant
    ) const override;

    /// @brief              Print constant thrust constant thrust thruster dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    Direction direction_;
};

}  // namespace thruster
}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
