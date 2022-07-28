////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/System.hpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::math::obj::Vector3d ;
using ostk::math::geom::d3::objects::Composite ;

using ostk::physics::units::Mass ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines the a dynamical system subject to equations of motion

class System
{

    public:

                                System                                      (   const   Mass&                       aMass,
                                                                                const   Composite&                  aCompositeGeometry                          ) ;

                                System                                      (   const   System&                     aSystem                                     ) ;

        virtual                 ~System                                     ( ) ;

        System*                 clone                                       ( ) const ;

        bool                    operator ==                                 (   const   System&                     aSystem                                     ) const ;

        bool                    operator !=                                 (   const   System&                     aSystem                                     ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   System&                     aSystem                                     ) ;

        virtual bool            isDefined                                   ( ) const ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        Mass                    getMass                                     ( ) const ;

        Composite               getGeometry                                 ( ) const ;

    private:

        Mass                    mass_ ;
        Composite               geometry_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
