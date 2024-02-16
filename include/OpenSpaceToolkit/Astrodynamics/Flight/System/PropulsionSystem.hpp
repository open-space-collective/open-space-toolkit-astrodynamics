/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace system
{

using ostk::core::type::Real;

using ostk::physics::unit::Mass;

/// @brief Define a propulsion system (constant thrust, constant Isp for now)
class PropulsionSystem
{
   public:
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
    ///              Real thrust = propulsionSystem.getThrust();
    /// @endcode
    ///
    /// @return Real
    Real getThrust() const;

    /// @brief Get propulsion system's specific impulse
    ///              https://en.wikipedia.org/wiki/Specific_impulse
    ///
    /// @code{.cpp}
    ///              Real specificImpulse = propulsionSystem.getSpecificImpulse();
    /// @endcode
    ///
    /// @return Real
    Real getSpecificImpulse() const;

    /// @brief Get propulsion system's mass flow rate
    ///
    /// @code{.cpp}
    ///              Real massFlowRate = propulsionSystem.getMassFlowRate();
    /// @endcode
    ///
    /// @return Real
    Real getMassFlowRate() const;

    /// @brief Get propulsion system's acceleration
    ///
    /// @code{.cpp}
    ///              Real acceleration = propulsionSystem.getAcceleration();
    /// @endcode
    ///
    /// @return Real
    Real getAcceleration(const Mass& aMass) const;

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
    Real thrust_;           /// Thrust [N]
    Real specificImpulse_;  /// Specific impulse [s]
    Real massFlowRate_;     /// Mass flow rate [kg/s]
};

}  // namespace system
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
