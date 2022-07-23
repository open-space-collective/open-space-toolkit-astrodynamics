////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/SatelliteSystem.hpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_SatelliteSystem__
#define __OpenSpaceToolkit_Astrodynamics_Flight_SatelliteSystem__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>

#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Real ;
using ostk::core::types::String ;
using ostk::core::types::Integer ;

using ostk::math::obj::Matrix3d ;
using ostk::math::geom::d3::objects::Cuboid ;
using ostk::math::obj::Vector3d ;
using ostk::math::geom::d3::objects::Point ;

using ostk::physics::units::Mass ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines the dynamics system who's motion is being studied, in particular this a satellite system

class SatelliteSystem
{

    public:

                                SatelliteSystem                             (   const   Mass&                       aMass                                       = { 90.0, Mass::Unit::Kilogram },
                                                                                const   Matrix3d&                   anInertiaTensor                             = Matrix3d::Identity(),
                                                                                const   Cuboid&                     aSatelliteGeometry                          = { Point { 0.0, 0.0, 0.0 }, std::array<Vector3d, 3> { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, std::array<Real, 3> { 1.0, 2.0, 3.0 } },
                                                                                const   Real&                       aCrossSectionalSurfaceArea                  = 0.8,
                                                                                const   Real&                       aDragCoefficient                            = 2.2   ) ;

        SatelliteSystem*        clone                                       ( ) const ;

        bool                    operator ==                                 (   const   SatelliteSystem&            aSatelliteSystem                            ) const ;

        bool                    operator !=                                 (   const   SatelliteSystem&            aSatelliteSystem                            ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   SatelliteSystem&            aSatelliteSystem                                     ) ;

        bool                    isDefined                                   ( ) const ;

        void                    print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        Mass                    getMass                                     ( ) const ;

        Matrix3d                getInertiaTensor                            ( ) const ;

        Cuboid                  getGeometry                                 ( ) const ;

        Real                    getCrossSectionalSurfaceArea                ( ) const ;

        Real                    getDragCoefficient                          ( ) const ;

        static SatelliteSystem  DefaultYam                                  ( ) ;

    private:

        Mass                    mass_ ;
        Matrix3d                inertiaTensor_ ;
        Cuboid                  satelliteGeometry_ ;
        Real                    crossSectionalSurfaceArea_ ;
        Real                    dragCoefficient_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
