////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Access/Generator.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Access_Generator__
#define __Library_Astrodynamics_Access_Generator__

#include <Library/Astrodynamics/Access.hpp>
#include <Library/Astrodynamics/Trajectory.hpp>

#include <Library/Physics/Environment.hpp>
#include <Library/Physics/Coordinate/Spherical/AER.hpp>
#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Instant.hpp>
#include <Library/Physics/Units/Derived/Angle.hpp>
#include <Library/Physics/Units/Length.hpp>

#include <Library/Mathematics/Objects/Interval.hpp>

#include <Library/Core/Containers/Array.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace access
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::ctnr::Array ;

using library::math::obj::Interval ;

using library::physics::units::Length ;
using library::physics::units::Angle ;
using library::physics::time::Instant ;
using library::physics::coord::spherical::AER ;
using library::physics::Environment ;

using library::astro::Trajectory ;
using library::astro::Access ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Generator
{

    public:

                                Generator                                   (   const   Environment&                anEnvironment                               ) ;

                                Generator                                   (   const   Environment&                anEnvironment,
                                                                                const   std::function<bool (const AER&)>& anAerFilter,
                                                                                const   std::function<bool (const Access&)>& anAccessFilter                     =   {} ) ;

        bool                    isDefined                                   ( ) const ;

        Array<Access>           computeAccesses                             (   const   physics::time::Interval&    anInterval,
                                                                                const   Trajectory&                 aFromTrajectory,
                                                                                const   Trajectory&                 aToTrajectory                               ) const ;

        void                    setAerFilter                                (   const   std::function<bool (const AER&)>& anAerFilter                           ) ;
        
        void                    setAccessFilter                             (   const   std::function<bool (const Access&)>& anAccessFilter                     ) ;

        static Generator        Undefined                                   ( ) ;

        static Generator        AerRanges                                   (   const   Interval<Angle>&            anAzimuthRange,
                                                                                const   Interval<Angle>&            anElevationRange,
                                                                                const   Interval<Length>&           aRangeRange,
                                                                                const   Environment&                anEnvironment                               ) ;

    private:

        Environment             environment_ ;

        std::function<bool (const AER&)> aerFilter_ ;
        std::function<bool (const Access&)> accessFilter_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////