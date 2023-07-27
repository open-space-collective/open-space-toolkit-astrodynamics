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

/// @brief                      Define a propulsion system

class PropulsionSystem
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     PropulsionSystem propulsion = { ... };
    /// @endcode
    ///
    /// @param              [in] aThruster Thruster (scalar)
    /// @param              [in] aSpecificImpulse Specific impulse

    PropulsionSystem(const Scalar& aThruster, const Scalar& aSpecificImpulse);

    /// @brief              Constructor
    ///
    /// @code
    ///                     PropulsionSystem propulsion = { ... };
    /// @endcode
    ///
    /// @param              [in] aThruster Thruster (real)
    /// @param              [in] aSpecificImpulse Specific impulse

    PropulsionSystem(const Real& aThruster, const Real& aSpecificImpulse);

    /// @brief              Equal to operator
    ///
    /// @param              [in] aPropulsionSystem A propulsion system
    /// @return             True if propulsion systems are equal

    bool operator==(const PropulsionSystem& aPropulsionSystem) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aPropulsionSystem A propulsion system
    /// @return             True if propulsion systems are not equal

    bool operator!=(const PropulsionSystem& aPropulsionSystem) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aPropulsionSystem A propulsion system
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const PropulsionSystem& aPropulsionSystem);

    /// @brief              Check if propulsion system is defined
    ///
    /// @return             True if propulsion system is defined

    bool isDefined() const;

    /// @brief              Print propulsion system
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief              Get propulsion system's thrust
    ///
    /// @code
    ///                     Scalar thrust = propulsionSystem.getThrust() ;
    /// @endcode
    ///
    /// @return             Scalar

    Scalar getThrust() const;

    /// @brief              Get propulsion system's specific impulse
    ///                     https://en.wikipedia.org/wiki/Specific_impulse
    ///
    /// @code
    ///                     Scalar specificImpulse = propulsionSystem.getSpecificImpulse() ;
    /// @endcode
    ///
    /// @return             Scalar

    Scalar getSpecificImpulse() const;

    /// @brief              Get propulsion system's mass flow rate
    ///
    /// @code
    ///                     Scalar massFlowRate = propulsionSystem.getMassFlowRate() ;
    /// @endcode
    ///
    /// @return             Scalar

    Scalar getMassFlowRate() const;

    /// @brief              Get propulsion system's acceleration
    ///
    /// @code
    ///                     Real acceleration = propulsionSystem.getAcceleration() ;
    /// @endcode
    ///
    /// @return             Real

    Real getAcceleration(const Mass& aMass) const; // TBI: Should return acceleration unit

    static PropulsionSystem Undefined();

   private:
    Unit thrustSIUnit_ = Unit::Derived(Derived::Unit(
        Length::Unit::Meter,
        {1},
        Mass::Unit::Kilogram,
        {1},
        Time::Unit::Second,
        {-2},
        ElectricCurrent::Unit::Undefined,
        {0},
        Angle::Unit::Undefined,
        {0}
    ));
    Unit specificImpulseSIUnit_ = Unit::Derived(Derived::Unit(
        Length::Unit::Undefined,
        {0},
        Mass::Unit::Undefined,
        {0},
        Time::Unit::Second,
        {1},
        ElectricCurrent::Unit::Undefined,
        {0},
        Angle::Unit::Undefined,
        {0}
    ));
    Unit massFlowRateSIUnit_ = Unit::Derived(Derived::Unit(
        Length::Unit::Undefined,
        {0},
        Mass::Unit::Kilogram,
        {1},
        Time::Unit::Second,
        {-1},
        ElectricCurrent::Unit::Undefined,
        {0},
        Angle::Unit::Undefined,
        {0}
    ));

    Scalar thrust_ = Scalar::Undefined();           /// Thruster [N]
    Scalar specificImpulse_ = Scalar::Undefined();  /// Specific impulse [s]
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
