////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/SatelliteSystem.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/SatelliteSystem.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                SatelliteSystem::SatelliteSystem            (   const   Mass&                       aMass,
                                                                                const   Matrix3d&                   anInertiaTensor,
                                                                                const   Cuboid&                     aSatelliteGeometry,
                                                                                const   Real&                       aCrossSectionalSurfaceArea,
                                                                                const   Real&                       aDragCoefficient                             )
                                :   mass_(aMass),
                                    inertiaTensor_(anInertiaTensor),
                                    satelliteGeometry_(aSatelliteGeometry),
                                    crossSectionalSurfaceArea_(aCrossSectionalSurfaceArea),
                                    dragCoefficient_(aDragCoefficient)

{

}

SatelliteSystem*                SatelliteSystem::clone                      ( ) const
{
    return new SatelliteSystem(*this) ;
}

bool                            SatelliteSystem::operator ==                (   const   SatelliteSystem&            aSatelliteSystem                            ) const
{

    if ((!this->isDefined()) || (!aSatelliteSystem.isDefined()))
    {
        return false ;
    }

    return (mass_ == aSatelliteSystem.mass_)
        && (inertiaTensor_ == aSatelliteSystem.inertiaTensor_)
        && (satelliteGeometry_ == aSatelliteSystem.satelliteGeometry_)
        && (crossSectionalSurfaceArea_ == aSatelliteSystem.crossSectionalSurfaceArea_)
        && (dragCoefficient_ == aSatelliteSystem.dragCoefficient_) ;

}

bool                            SatelliteSystem::operator !=                (   const   SatelliteSystem&            aSatelliteSystem                            ) const
{
    return !((*this) == aSatelliteSystem) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   SatelliteSystem&            aSatelliteSystem                            )
{

    aSatelliteSystem.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            SatelliteSystem::isDefined                  ( ) const
{
    return mass_.isDefined() && inertiaTensor_.isDefined() && satelliteGeometry_.isDefined() && dragCoefficient_.isDefined() ;
}

void                            SatelliteSystem::print                      (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "SatelliteSystem") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Mass:"                    << (mass_.isDefined() ? mass_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Inertia Tensor:" << (inertiaTensor_.isDefined() ? inertiaTensor_.toString() : "Undefined") ;

    ostk::core::utils::Print::Separator(anOutputStream, "Satellite Cuboid Geometry") ;
    satelliteGeometry_.print(anOutputStream, false) ;

    ostk::core::utils::Print::Line(anOutputStream) << "Cross Sectional Surface Area:" << (crossSectionalSurfaceArea_.isDefined() ? crossSectionalSurfaceArea_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Drag Coefficient:" << (dragCoefficient_.isDefined() ? dragCoefficient_.toString() : "Undefined") ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

Mass                            SatelliteSystem::getMass                    ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteSystem") ;
    }

    return mass_ ;

}


Matrix3d                        SatelliteSystem::getInertiaTensor           ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteSystem") ;
    }

    return inertiaTensor_ ;

}

Cuboid                          SatelliteSystem::getGeometry                ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteSystem") ;
    }

    return satelliteGeometry_ ;

}

Real                            SatelliteSystem::getCrossSectionalSurfaceArea( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteSystem") ;
    }

    return crossSectionalSurfaceArea_ ;

}

Real                            SatelliteSystem::getDragCoefficient         ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteSystem") ;
    }

    return dragCoefficient_ ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
