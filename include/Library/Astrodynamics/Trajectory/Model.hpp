////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Model.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Model__
#define __Library_Astrodynamics_Trajectory_Model__

#include <Library/Astrodynamics/Trajectory/State.hpp>

#include <Library/Physics/Time/Instant.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::physics::time::Instant ;

using library::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Trajectory model (abstract)

class Model
{

    public:

                                Model                                       ( ) ;

        virtual                 ~Model                                      ( ) = 0 ;

        virtual Model*          clone                                       ( ) const = 0 ;

        virtual bool            operator ==                                 (   const   Model&                      aModel                                      ) const = 0 ;

        virtual bool            operator !=                                 (   const   Model&                      aModel                                      ) const = 0 ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Model&                      aModel                                      ) ;

        virtual bool            isDefined                                   ( ) const = 0 ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const = 0 ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const = 0 ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////