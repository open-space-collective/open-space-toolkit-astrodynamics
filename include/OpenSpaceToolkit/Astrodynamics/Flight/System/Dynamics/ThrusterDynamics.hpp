/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThrusterDynamics__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThrusterDynamics__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

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

using ostk::physics::data::Direction;
using ostk::physics::time::Instant;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionModel;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::State;

/// @brief                      Defined the acceleration experienced by a point mass due to thrust

class ThrusterDynamics : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const Direction aDirection = { ... };
    ///                     const SatelliteSystem aSatelliteSystem = { ... };
    ///                     ThrusterDynamics thrusterDynamics = { aDirection, aSatelliteSystem };
    /// @endcode
    ///
    /// @param              [in] aCelestial A Celestial Object

    ThrusterDynamics(const Direction& aDirection, const SatelliteSystem& aSatelliteSystem);

    /// @brief              Copy Constructor
    ///
    /// @param              [in] ThrusterDynamics A thruster dynamics

    ThrusterDynamics(const ThrusterDynamics& aThrusterDynamics);

    /// @brief              Destructor

    virtual ~ThrusterDynamics() override;

    /// @brief              Clone thruster dynamics
    ///
    /// @return             Pointer to cloned thruster dynamics

    virtual ThrusterDynamics* clone() const override;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aThrusterDynamics A thruster dynamics
    /// @return             True if thruster dynamics are equal

    bool operator==(const ThrusterDynamics& aThrusterDynamics) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aThrusterDynamics A thruster dynamics
    /// @return             True if thruster dynamics are not equal

    bool operator!=(const ThrusterDynamics& aThrusterDynamics) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aThrusterDynamics A thruster dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const ThrusterDynamics& aThrusterDynamics);

    /// @brief              Check if thruster dynamics is defined
    ///
    /// @return             True if thruster dynamics is defined

    virtual bool isDefined() const override;

    /// @brief              Print thruster dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    virtual void update(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant) override;

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
