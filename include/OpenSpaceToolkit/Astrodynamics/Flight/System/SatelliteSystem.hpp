////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
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
namespace system
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Real ;

using ostk::math::obj::Matrix3d ;
using ostk::math::geom::d3::objects::Cuboid ;
using ostk::math::geom::d3::objects::Composite ;
using ostk::math::obj::Vector3d ;
using ostk::math::geom::d3::objects::Point ;

using ostk::physics::units::Mass ;

using ostk::astro::flight::System ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines the dynamics system who's motion is being studied, in particular this is a satellite system

class SatelliteSystem : public System
{

    public:

                                SatelliteSystem                             (   const   Mass&                       aMass,
                                                                                const   Composite&                  aSatelliteGeometry,
                                                                                const   Matrix3d&                   anInertiaTensor,
                                                                                const   Real&                       aCrossSectionalSurfaceArea,
                                                                                const   Real&                       aDragCoefficient                            ) ;

                                SatelliteSystem                             (   const   SatelliteSystem&            aSatelliteSystem                            ) ;

        virtual                 ~SatelliteSystem                            ( ) override ;

        SatelliteSystem*        clone                                       ( ) const ;

        bool                    operator ==                                 (   const   SatelliteSystem&            aSatelliteSystem                            ) const ;

        bool                    operator !=                                 (   const   SatelliteSystem&            aSatelliteSystem                            ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   SatelliteSystem&            aSatelliteSystem                            ) ;

        virtual bool            isDefined                                   ( ) const override ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        Matrix3d                getInertiaTensor                            ( ) const ;

        Real                    getCrossSectionalSurfaceArea                ( ) const ;

        Real                    getDragCoefficient                          ( ) const ;

    private:

        Matrix3d                inertiaTensor_ ;
        Real                    crossSectionalSurfaceArea_ ;
        Real                    dragCoefficient_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
