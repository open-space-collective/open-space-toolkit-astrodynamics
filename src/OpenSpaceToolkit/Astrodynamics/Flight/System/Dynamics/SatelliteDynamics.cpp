////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>

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

                                SatelliteDynamics::SatelliteDynamics        (   const   Environment&                anEnvironment,
                                                                                const   SatelliteSystem&            aSatelliteSystem,
                                                                                const   State&                      aState                                      )
                                :   Dynamics(),
                                    environment_(anEnvironment),
                                    gcrfSPtr_(Frame::GCRF()),
                                    satelliteSystem_(aSatelliteSystem),
                                    state_(aState)
{

    environment_.setInstant(aState.getInstant()) ;

}

                                SatelliteDynamics::SatelliteDynamics        (   const   SatelliteDynamics&          aSatelliteDynamics                          )
                                :   Dynamics(aSatelliteDynamics),
                                    environment_(aSatelliteDynamics.environment_),
                                    gcrfSPtr_(aSatelliteDynamics.gcrfSPtr_),
                                    satelliteSystem_(aSatelliteDynamics.satelliteSystem_),
                                    state_(aSatelliteDynamics.state_)
{

    environment_.setInstant(aSatelliteDynamics.state_.getInstant()) ;

}

                                SatelliteDynamics::~SatelliteDynamics       ( )
{

}

SatelliteDynamics*              SatelliteDynamics::clone                    ( ) const
{
    return new SatelliteDynamics(*this) ;
}

bool                            SatelliteDynamics::operator ==              (   const   SatelliteDynamics&          aSatelliteDynamics                          ) const
{

    if ((!this->isDefined()) || (!aSatelliteDynamics.isDefined()))
    {
        return false ;
    }

    return (environment_.getInstant() == aSatelliteDynamics.environment_.getInstant())
        && (environment_.getObjectNames() == aSatelliteDynamics.environment_.getObjectNames())
        && (satelliteSystem_ == aSatelliteDynamics.satelliteSystem_)
        && (state_ == aSatelliteDynamics.state_) ;

}

bool                            SatelliteDynamics::operator !=              (   const   SatelliteDynamics&          aSatelliteDynamics                          ) const
{
    return !((*this) == aSatelliteDynamics) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   SatelliteDynamics&          aSatelliteDynamics                          )
{

    aSatelliteDynamics.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            SatelliteDynamics::isDefined                ( ) const
{
    return environment_.isDefined() && satelliteSystem_.isDefined() && state_.isDefined() ;
}

void                            SatelliteDynamics::print                    (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Satellite Dynamics") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Environment:" << environment_ ;

    ostk::core::utils::Print::Separator(anOutputStream, "Satellite System") ;
    satelliteSystem_.print(anOutputStream, false) ;

    ostk::core::utils::Print::Separator(anOutputStream, "State") ;
    state_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

State                           SatelliteDynamics::getState                 ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Satellite Dynamics") ;
    }

    return state_ ;

}

void                            SatelliteDynamics::setState                 (   const   State&                      aState                                      )
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Satellite Dynamics") ;
    }

    state_ = aState ;
    environment_.setInstant(aState.getInstant()) ;

}

Dynamics::DynamicalEquationWrapper SatelliteDynamics::getDynamicalEquations ( )
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteDynamics") ;
    }

    return std::bind(&SatelliteDynamics::DynamicalEquations, this, std::placeholders::_1,  std::placeholders::_2,  std::placeholders::_3) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            SatelliteDynamics::DynamicalEquations       (   const   Dynamics::StateVector&      x,
                                                                                        Dynamics::StateVector&      dxdt,
                                                                                const   double                      t                                           )
{

    const Position currentPosition = Position::Meters({ x[0], x[1], x[2] }, gcrfSPtr_) ;

    // Update environment time
    const Instant currentInstant = state_.getInstant() + Duration::Seconds(t) ;
    environment_.setInstant(currentInstant) ;

    // Initialize gravitational acceleration vector
    Vector3d totalGravitationalAcceleration_SI = { 0.0, 0.0, 0.0 } ;

    // Access all objects in the environment and loop through them
    for (const auto& objectName : environment_.getObjectNames())
    {
        if (objectName == "Earth")
        {
            // Obtain gravitational acceleration from current object
            const Vector gravitationalAcceleration = environment_.accessCelestialObjectWithName(objectName)->getGravitationalFieldAt(currentPosition) ;

            // Add object's gravity to total gravitational acceleration
            totalGravitationalAcceleration_SI += gravitationalAcceleration.inFrame(gcrfSPtr_, currentInstant).getValue() ;
        }
        else
        {
            const double mu_ThirdBody_SI = environment_.accessCelestialObjectWithName(objectName)->getGravitationalParameter().in(GravitationalParameterSIUnit) ;

            // Obtain 3rd body position vector relative to Earth
            const Vector3d thirdBodyPositionCoordinates = environment_.accessCelestialObjectWithName(objectName)->getPositionIn(gcrfSPtr_).accessCoordinates() ;
            const double thirdBodyPositionMagnitudeCubed = std::pow(thirdBodyPositionCoordinates.norm(), 3) ;

            // Obtain satellite position vector relative to 3rd body
            const Vector3d relativePositionCoordinates = thirdBodyPositionCoordinates - currentPosition.accessCoordinates() ;
            const double relativePositionMagnitudeCubed = std::pow(relativePositionCoordinates.norm(), 3) ;

            totalGravitationalAcceleration_SI += mu_ThirdBody_SI * ( (relativePositionCoordinates / relativePositionMagnitudeCubed) - (thirdBodyPositionCoordinates / thirdBodyPositionMagnitudeCubed) ) ;
        }
    }

    // Integrate position and velocity states
    dxdt[0] = x[3] ;
    dxdt[1] = x[4] ;
    dxdt[2] = x[5] ;
    dxdt[3] = totalGravitationalAcceleration_SI[0] ;
    dxdt[4] = totalGravitationalAcceleration_SI[1] ;
    dxdt[5] = totalGravitationalAcceleration_SI[2] ;

}

// void                            SatelliteDynamics::Exponential_Dynamics
//                                                                             (   const   SatelliteDynamics::StateVector&     x,
//                                                                                         SatelliteDynamics::StateVector&     dxdt,
//                                                                                 const   double                                                                  ) const
// {
//     // Find position magnitude
//     const double positionMagnitude = std::sqrt(std::pow(x[0],2) + std::pow(x[1],2) + std::pow(x[2],2));

//     // Integrate velocity states
//     const double positionMagnitudeCubed = std::pow(positionMagnitude,3) ;

//     // Access constants
//     const double mu_SI = ((environment_.accessCelestialObjectWithName("Earth"))->getGravitationalParameter()).in(GravitationalParameterSIUnit) ;
//     const double Re = ((environment_.accessCelestialObjectWithName("Earth"))->getEquatorialRadius()).inMeters() ;
//     const double dragCoefficient = satelliteSystem_.getDragCoefficient() ;
//     const double surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea() ;
//     const double mass = satelliteSystem_.getMass().inKilograms() ;
//     const Vector3d earthAngularVelocity = { 0, 0, 7.2921159e-5 } ; // rad/s
//     const double rho0 = 6.967e-13 ; // kg/m^3
//     const double h0 = 500000 ;
//     const double H = 63822 ;

//     // Calculate relative velocity to wind
//     const Vector3d relativeVelocity = Vector3d({ x[3], x[4], x[5] }) - Vector3d({earthAngularVelocity.cross(Vector3d({ x[0], x[1], x[2] }))}) ;
//     const double relativeVelocityMagnitude = std::sqrt(std::pow(relativeVelocity[0],2) + std::pow(relativeVelocity[1],2) + std::pow(relativeVelocity[2],2)) ;

//     // Calculate density parameters
//     const double hEllipse = positionMagnitude - Re ;
//     const double rho = rho0 * std::exp( - (hEllipse - h0) / H ) ;

//     // Calculate drag acceleration
//     const Vector3d dragAcceleration = -0.5 * dragCoefficient * (surfaceArea / mass) * rho * relativeVelocityMagnitude * relativeVelocity ;

//     // Integrate position and velocity states
//     dxdt[0] = x[3] ;
//     dxdt[1] = x[4] ;
//     dxdt[2] = x[5] ;
//     dxdt[3] = -(mu_SI / positionMagnitudeCubed) * x[0] + dragAcceleration[0] ;
//     dxdt[4] = -(mu_SI / positionMagnitudeCubed) * x[1] + dragAcceleration[1] ;
//     dxdt[5] = -(mu_SI / positionMagnitudeCubed) * x[2] + dragAcceleration[2] ;

// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
