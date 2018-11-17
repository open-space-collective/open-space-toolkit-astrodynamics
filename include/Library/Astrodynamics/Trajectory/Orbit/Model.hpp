////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Model.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit_Model__
#define __Library_Astrodynamics_Trajectory_Orbit_Model__

#include <Library/Astrodynamics/Trajectory/State.hpp>
#include <Library/Astrodynamics/Trajectory/Model.hpp>

#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Types/Integer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Integer ;

using library::physics::time::Instant ;

using library::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Model : public virtual library::astro::trajectory::Model
{

    public:

                                Model                                       ( ) ;

        virtual                 ~Model                                      ( ) = 0 ;

        virtual Model*          clone                                       ( ) const = 0 ;

        virtual bool            isDefined                                   ( ) const = 0 ;

        virtual Instant         getEpoch                                    ( ) const = 0 ;

        virtual Integer         getRevolutionNumberAtEpoch                  ( ) const = 0 ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const = 0 ;

        virtual Integer         calculateRevolutionNumberAt                 (   const   Instant&                    anInstant                                   ) const = 0 ; // [TBR]

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const = 0 ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////