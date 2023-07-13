/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
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

/// @brief                      Define the acceleration experienced by a point mass due to thrust

class Thruster : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const Direction aDirection = { ... };
    ///                     const SatelliteSystem aSatelliteSystem = { ... };
    ///                     Thruster thrusterDynamics = { aDirection, aSatelliteSystem };
    /// @endcode
    ///
    /// @param              [in] aDirection A direction
    /// @param              [in] aSatelliteSystem A satellite system

    Thruster(const Direction& aDirection, const SatelliteSystem& aSatelliteSystem);

    /// @brief              Constructor
    ///
    /// @code
    ///                     const Direction aDirection = { ... };
    ///                     const SatelliteSystem aSatelliteSystem = { ... };
    ///                     const aName = { ... };
    ///                     Thruster thrusterDynamics = { aDirection, aSatelliteSystem, aName};
    /// @endcode
    ///
    /// @param              [in] aDirection A direction
    /// @param              [in] aSatelliteSystem A satellite system
    /// @param              [in] aName A name

    Thruster(const Direction& aDirection, const SatelliteSystem& aSatelliteSystem, const String& aName);

    /// @brief              Copy Constructor
    ///
    /// @param              [in] Thruster A thruster dynamics

    Thruster(const Thruster& aThruster);

    /// @brief              Destructor

    virtual ~Thruster() override;

    /// @brief              Clone thruster dynamics
    ///
    /// @return             Pointer to cloned thruster dynamics

    virtual Thruster* clone() const override;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aThruster A thruster dynamics
    /// @return             True if thruster dynamics are equal

    bool operator==(const Thruster& aThruster) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aThruster A thruster dynamics
    /// @return             True if thruster dynamics are not equal

    bool operator!=(const Thruster& aThruster) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aThruster A thruster dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Thruster& aThruster);

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
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
