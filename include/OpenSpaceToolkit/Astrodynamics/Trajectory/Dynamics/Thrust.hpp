/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Dynamics_Thrust__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Dynamics_Thrust__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace dynamics
{

using ostk::core::types::Real;

using ostk::math::obj::Vector3d;
using ostk::math::geom::d3::trf::rot::RotationMatrix;

using ostk::physics::data::Direction;
using ostk::physics::time::Instant;
using ostk::physics::units::Mass;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::trajectory::Dynamics;
using ostk::astro::trajectory::State;

/// @brief                      Define the acceleration experienced by a point mass due to thrust

class Thrust : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const Direction aDirection = { ... };
    ///                     const SatelliteSystem aSatelliteSystem = { ... };
    ///                     Thrust thrusterDynamics = { aDirection, aSatelliteSystem };
    /// @endcode
    ///
    /// @param              [in] aDirection A direction
    /// @param              [in] aSatelliteSystem A satellite system

    Thrust(const Direction& aDirection, const SatelliteSystem& aSatelliteSystem);

    /// @brief              Copy Constructor
    ///
    /// @param              [in] Thrust A thruster dynamics

    Thrust(const Thrust& aThrust);

    /// @brief              Destructor

    virtual ~Thrust() override;

    /// @brief              Clone thruster dynamics
    ///
    /// @return             Pointer to cloned thruster dynamics

    virtual Thrust* clone() const override;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aThrust A thruster dynamics
    /// @return             True if thruster dynamics are equal

    bool operator==(const Thrust& aThrust) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aThrust A thruster dynamics
    /// @return             True if thruster dynamics are not equal

    bool operator!=(const Thrust& aThrust) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aThrust A thruster dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Thrust& aThrust);

    /// @brief              Check if thruster dynamics is defined
    ///
    /// @return             True if thruster dynamics is defined

    virtual bool isDefined() const override;

    /// @brief              Apply contribution to the state derivative
    ///
    /// @param              [in] x A state vector
    /// @param              [out] dxdt A state derivative vector
    /// @param              [in] anInstant An instant

    virtual void applyContribution(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant) const override;

    /// @brief              Print thruster dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    Direction direction_;
    SatelliteSystem satelliteSystem_;
};

}  // namespace dynamics
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
