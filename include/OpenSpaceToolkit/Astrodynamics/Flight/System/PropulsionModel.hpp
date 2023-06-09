/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionModel__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionModel__

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

/// @brief                      Defines the dynamics system who's motion is being studied, in particular this is a
/// satellite system

class PropulsionModel
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     PropulsionModel propulsion = { ... };
    /// @endcode
    ///
    /// @param              [in] aThrust Thrust
    /// @param              [in] aSpecificImpulse Specific impulse

    PropulsionModel(const Scalar& aThrust, const Scalar& aSpecificImpulse);

    PropulsionModel(const Real& aThrust, const Real& aSpecificImpulse);

    bool operator==(const PropulsionModel& aPropulsionModel) const;

    bool operator!=(const PropulsionModel& aPropulsionModel) const;

    bool isDefined() const;

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    Scalar getThrust() const;

    Scalar getSpecificImpulse() const;

    Scalar getMassFlowRate() const;

    Real getAcceleration(const Mass& aMass) const;

    static PropulsionModel Undefined();

   private:
    const Unit thrustSIUnit_ = Unit::Derived(Derived::Unit(
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
    const Unit specificImpulseSIUnit_ = Unit::Derived(Derived::Unit(
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
    const Unit massFlowRateSIUnit_ = Unit::Derived(Derived::Unit(
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

    Scalar thrust_ = Scalar::Undefined();           /// Thrust [N]
    Scalar specificImpulse_ = Scalar::Undefined();  /// Specific impulse [s]
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
