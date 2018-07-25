////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory__
#define __Library_Astrodynamics_Trajectory__

#include <Library/Astrodynamics/Trajectory/State.hpp>
#include <Library/Astrodynamics/Trajectory/Model.hpp>

#include <Library/Physics/Coordinate/Position.hpp>
#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Index.hpp>
#include <Library/Core/Types/Unique.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Unique ;
using library::core::types::Index ;
using library::core::types::String ;
using library::core::ctnr::Array ;

using library::physics::time::Instant ;
using library::physics::time::Interval ;
using library::physics::coord::Position ;

using library::astro::trajectory::Model ;
using library::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Path followed by an object through space as a function of time
///
/// @ref                        https://en.wikipedia.org/wiki/Trajectory

class Trajectory
{

    public:

                                Trajectory                                  ( ) ;

                                Trajectory                                  (   const   Model&                      aModel                                      ) ;

                                Trajectory                                  (   const   Array<State>&               aStateArray                                 ) ;

                                Trajectory                                  (   const   Trajectory&                 aTrajectory                                 ) ;

        Trajectory&             operator =                                  (   const   Trajectory&                 aTrajectory                                 ) = delete ; 

        bool                    operator ==                                 (   const   Trajectory&                 aTrajectory                                 ) const ;

        bool                    operator !=                                 (   const   Trajectory&                 aTrajectory                                 ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Trajectory&                 aTrajectory                                 ) ;

        bool                    isDefined                                   ( ) const ;

        State                   getStateAt                                  (   const   Instant&                    anInstant                                   ) const ;

        Array<State>            getStatesAt                                 (   const   Array<Instant>&             anInstantArray                              ) const ;

        Position                getPositionAt                               (   const   Instant&                    anInstant                                   ) const ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        static Trajectory       Undefined                                   ( ) ;

    protected:

        const Model&            accessModel                                 ( ) const ;

    private:

        Unique<Model>           modelUPtr_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////