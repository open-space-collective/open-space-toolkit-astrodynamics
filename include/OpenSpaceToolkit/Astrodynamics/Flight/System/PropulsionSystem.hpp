/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Unit.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Time.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

using ostk::core::types::Real;

using ostk::physics::data::Direction;
using ostk::physics::data::Scalar;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using ostk::physics::units::Time;
using ostk::physics::units::ElectricCurrent;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::Unit;

/// @brief     Define a propulsion system (constant thrust, constant Isp for now)
class PropulsionSystem
{
   public:
    static Unit thrustSIUnit;
    static Unit specificImpulseSIUnit;
    static Unit massFlowRateSIUnit;  // TBI: Define in ostk physics as proper units

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              PropulsionSystem propulsion = { ... };
    /// @endcode
    ///
    /// @param aThrust Thrust (scalar)
    /// @param aSpecificImpulse Specific impulse (scalar)
    PropulsionSystem(const Scalar& aThrust, const Scalar& aSpecificImpulse);

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              PropulsionSystem propulsion = { ... };
    /// @endcode
    ///
    /// @param aThrustInSIUnit Thrust value in SI Unit
    /// @param aSpecificImpulseInSIUnit Specific impulse in SI Unit
    PropulsionSystem(const Real& aThrustInSIUnit, const Real& aSpecificImpulseInSIUnit);

    /// @brief Equal to operator
    ///
    /// @param aPropulsionSystem A propulsion system
    /// @return True if propulsion systems are equal
    bool operator==(const PropulsionSystem& aPropulsionSystem) const;

    /// @brief Not equal to operator
    ///
    /// @param aPropulsionSystem A propulsion system
    /// @return True if propulsion systems are not equal
    bool operator!=(const PropulsionSystem& aPropulsionSystem) const;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aPropulsionSystem A propulsion system
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const PropulsionSystem& aPropulsionSystem);

    /// @brief Check if propulsion system is defined
    ///
    /// @return True if propulsion system is defined
    bool isDefined() const;

    /// @brief Print propulsion system
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Get propulsion system's thrust
    ///
    /// @code{.cpp}
    ///              Scalar thrust = propulsionSystem.getThrust();
    /// @endcode
    ///
    /// @return Scalar
    Scalar getThrust() const;

    /// @brief Get propulsion system's specific impulse
    ///              https://en.wikipedia.org/wiki/Specific_impulse
    ///
    /// @code{.cpp}
    ///              Scalar specificImpulse = propulsionSystem.getSpecificImpulse();
    /// @endcode
    ///
    /// @return Scalar
    Scalar getSpecificImpulse() const;

    /// @brief Get propulsion system's mass flow rate
    ///
    /// @code{.cpp}
    ///              Scalar massFlowRate = propulsionSystem.getMassFlowRate();
    /// @endcode
    ///
    /// @return Scalar
    Scalar getMassFlowRate() const;

    /// @brief Get propulsion system's acceleration
    ///
    /// @code{.cpp}
    ///              Scalar acceleration = propulsionSystem.getAcceleration();
    /// @endcode
    ///
    /// @return Scalar
    Scalar getAcceleration(const Mass& aMass) const;

    /// @brief Undefined propulsion system
    ///
    /// @code{.cpp}
    ///              PropulsionSystem propulsionSystem = PropulsionSystem::Undefined();
    /// @endcode
    ///
    /// @return An undefined propulsion system
    static PropulsionSystem Undefined();

    /// @brief Default propulsion system
    ///
    /// @code{.cpp}
    ///              PropulsionSystem propulsionSystem = PropulsionSystem::Default();
    /// @endcode
    ///
    /// @return A default propulsion system
    static PropulsionSystem Default();

   private:
    Scalar thrust_ = Scalar::Undefined();           /// Thrust [N]
    Scalar specificImpulse_ = Scalar::Undefined();  /// Specific impulse [s]
    Scalar massFlowRate_ = Scalar::Undefined();     /// Mass flow rate [kg/s]
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
