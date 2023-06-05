/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThrusterDynamics__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThrusterDynamics__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
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

using ostk::core::ctnr::Array;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;

using ostk::physics::Environment;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::data::Direction;
using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::State;

/// @brief                      Defined the acceleration experienced by a point mass due to gravity

struct Propulsion
{
    /// @brief              Constructor
    ///
    /// @code
    ///                     Propulsion propulsion = { ... };
    /// @endcode
    ///
    /// @param              [in] aThrust Thrust
    /// @param              [in] aSpecificImpulse Specific impulse

    Propulsion(const Real& aThrust, const Real& aSpecificImpulse)
        : thrust_(aThrust),
          specificImpulse_(aSpecificImpulse)
    {
        massFlowRate_ = thrust_ / (specificImpulse_ * 9.80665);  // TBI: gravity parameter should be replaced by a thruster constant
    }

    bool isDefined() const
    {
        return thrust_.isDefined() && specificImpulse_.isDefined() && massFlowRate_.isDefined();
    }

    void print(std::ostream& anOutputStream, bool displayDecorator) const
    {
        displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propulsion") : void();

        anOutputStream << "[Thrust = " << thrust_ << ", Specific Impulse = " << specificImpulse_
                       << ", Mass Flow Rate = " << massFlowRate_ << "]";

        displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
    }

    Real thrust_ = Real::Undefined();           /// Thrust [N]
    Real specificImpulse_ = Real::Undefined();  /// Specific impulse [s]
    Real massFlowRate_ = Real::Undefined();     /// Mass flow rate [kg/s]
};

class ThrusterDynamics : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const Propulsion = { ... };
    ///                     ThrusterDynamics thrusterDynamics = { aPropulsion };
    /// @endcode
    ///
    /// @param              [in] aCelestial A Celestial Object

    ThrusterDynamics(const Propulsion& aPropulsion, const Direction& aDirection);

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

    void update(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant);

   private:
    Propulsion propulsion_;
    Direction direction_;
};

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
