////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/FullSatelliteDynamics.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/FullSatelliteDynamics.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::physics::units::Length ;
using ostk::physics::units::Time ;
using ostk::physics::units::Derived ;

static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                FullSatelliteDynamics::FullSatelliteDynamics        (   const   Environment&                anEnvironment,
                                                                                const   SatelliteSystem&            aSatelliteSystem                            )
                                :   Dynamics(),
                                    environment_(anEnvironment),
                                    gcrfSPtr_(Frame::GCRF()),
                                    satelliteSystem_(aSatelliteSystem),
                                    instant_(Instant::Undefined())
{

}

                                FullSatelliteDynamics::FullSatelliteDynamics        (   const   FullSatelliteDynamics&          aFullSatelliteDynamics                          )
                                :   Dynamics(aFullSatelliteDynamics),
                                    environment_(aFullSatelliteDynamics.environment_),
                                    gcrfSPtr_(aFullSatelliteDynamics.gcrfSPtr_),
                                    satelliteSystem_(aFullSatelliteDynamics.satelliteSystem_),
                                    instant_(Instant::Undefined())
{

}

                                FullSatelliteDynamics::~FullSatelliteDynamics       ( )
{

}

FullSatelliteDynamics*              FullSatelliteDynamics::clone                    ( ) const
{
    return new FullSatelliteDynamics(*this) ;
}

bool                            FullSatelliteDynamics::operator ==              (   const   FullSatelliteDynamics&          aFullSatelliteDynamics                          ) const
{

    if ((!this->isDefined()) || (!aFullSatelliteDynamics.isDefined()))
    {
        return false ;
    }

    return (environment_.getInstant() == aFullSatelliteDynamics.environment_.getInstant())
        && (environment_.getObjectNames() == aFullSatelliteDynamics.environment_.getObjectNames())
        && (satelliteSystem_ == aFullSatelliteDynamics.satelliteSystem_) ;

}

bool                            FullSatelliteDynamics::operator !=              (   const   FullSatelliteDynamics&          aFullSatelliteDynamics                          ) const
{
    return !((*this) == aFullSatelliteDynamics) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   FullSatelliteDynamics&          aFullSatelliteDynamics                          )
{

    aFullSatelliteDynamics.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            FullSatelliteDynamics::isDefined                ( ) const
{
    return environment_.isDefined() && satelliteSystem_.isDefined() ;
}

void                            FullSatelliteDynamics::print                    (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Satellite Dynamics") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Environment:" << environment_ ;

    ostk::core::utils::Print::Separator(anOutputStream, "Satellite System") ;
    satelliteSystem_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

Instant                         FullSatelliteDynamics::getInstant               ( ) const
{
    return instant_ ;
}

void                            FullSatelliteDynamics::setInstant               (   const   Instant&                    anInstant                                   )
{
    this->instant_ = anInstant ;
}

Dynamics::DynamicalEquationWrapper FullSatelliteDynamics::getDynamicalEquations ( )
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("FullSatelliteDynamics") ;
    }

    if (!this->instant_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    return std::bind(&FullSatelliteDynamics::DynamicalEquations, this, std::placeholders::_1,  std::placeholders::_2,  std::placeholders::_3) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            FullSatelliteDynamics::DynamicalEquations   (   const   Dynamics::StateVector&      x,
                                                                                Dynamics::StateVector&      dxdt,
                                                                                const   double                      t                                           )
{

    const Position currentPosition = Position::Meters({ x[0], x[1], x[2] }, gcrfSPtr_) ;

    // Check for radii below 70km altitude
    if (currentPosition.getCoordinates().norm() < Earth::Models::EGM2008::EquatorialRadius.inMeters() + 70000.0)
    {
        throw ostk::core::error::RuntimeError("Satellite altitude too low, has re-entered.") ;
    }

    // Update environment time
    const Instant currentInstant = instant_ + Duration::Seconds(t) ;
    environment_.setInstant(currentInstant) ;

    // Initialize gravitational acceleration vector
    Vector3d totalGravitationalAcceleration_SI = { 0.0, 0.0, 0.0 } ;

    // Determine forces

    // Access all objects in the environment and loop through them
    for (const auto& objectName : environment_.getObjectNames())
    {

        if (objectName != "Earth")
        {
            // Obtain 3rd body effect on center of Earth (origin in GCRF) aka 3rd body correction
            const Vector gravitationalAcceleration3rdBodyCorrection = environment_.accessCelestialObjectWithName(objectName)->getGravitationalFieldAt(Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr_)) ;

            // Subtract 3rd body correct from total gravitational acceleration
            totalGravitationalAcceleration_SI -= gravitationalAcceleration3rdBodyCorrection.inFrame(gcrfSPtr_, currentInstant).getValue() ;
        }

        // Obtain gravitational acceleration from current object
        const Vector gravitationalAcceleration = environment_.accessCelestialObjectWithName(objectName)->getGravitationalFieldAt(currentPosition) ;

        // Add object's gravity to total gravitational acceleration
        totalGravitationalAcceleration_SI += gravitationalAcceleration.inFrame(gcrfSPtr_, currentInstant).getValue() ;

    }

    // Dummy inertia tensor input
    double Ix = 1.0 ;
    double Iy = 1.0 ;
    double Iz = 3.0 ;

    // Dummy external torque input (same on all 3 axis for now)
    double externalTorque = 0 ;

    // Gravity gradient torque
    // Drag torque
    // 

    // Integrate full state

    // Position and Velocity

    // Position Derivative
    dxdt[0] = x[3] ;
    dxdt[1] = x[4] ;
    dxdt[2] = x[5] ;
    // Velocity Derivative
    dxdt[3] = totalGravitationalAcceleration_SI[0] ;
    dxdt[4] = totalGravitationalAcceleration_SI[1] ;
    dxdt[5] = totalGravitationalAcceleration_SI[2] ;

    // Quaternions and Angular Velocity

    // Quaternion Derivative (i.e. Quaternion Rates)
    dxdt[6] = -0.5 * (x[10]*x[7] + x[11]*x[8] + x[12]*x[9]) ;
    dxdt[7] = 0.5 * (x[10]*x[6] + x[12]*x[8] - x[11]*x[9]) ;
    dxdt[8] = 0.5 * (x[11]*x[6] - x[12]*x[7] + x[10]*x[9]) ;
    dxdt[9] = 0.5 * (x[12]*x[6] + x[11]*x[7] - x[10]*x[8]) ;
    // Angular Velocity Derivative
    dxdt[10] = (externalTorque / Ix) - ((Iz - Iy) / Ix) * x[11] * x[12] ;
    dxdt[11] = (externalTorque / Iy) - ((Ix - Iz) / Iy) * x[12] * x[10] ;
    dxdt[12] = (externalTorque / Iz) - ((Iy - Ix) / Iz) * x[10] * x[11] ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
