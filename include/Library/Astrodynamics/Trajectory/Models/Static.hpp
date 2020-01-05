////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Models/Static.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Models_Static__
#define __Library_Astrodynamics_Trajectory_Models_Static__

#include <Library/Astrodynamics/Trajectory/State.hpp>
#include <Library/Astrodynamics/Trajectory/Model.hpp>

#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Instant.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace models
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::physics::time::Instant ;
using library::physics::time::Interval ;
using library::physics::coord::Position ;

using library::astro::trajectory::Model ;
using library::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Static trajectory model

class Static : public virtual Model
{

    public:

                                Static                                      (   const   Position&                   aPosition                                   ) ;

        virtual Static*         clone                                       ( ) const override ;

        bool                    operator ==                                 (   const   Static&                     aStaticModel                                ) const ;

        bool                    operator !=                                 (   const   Static&                     aStaticModel                                ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Static&                     aStaticModel                                ) ;

        virtual bool            isDefined                                   ( ) const override ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

    protected:

        virtual bool            operator ==                                 (   const   Model&                      aModel                                      ) const override ;

        virtual bool            operator !=                                 (   const   Model&                      aModel                                      ) const override ;

    private:

        Position                position_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
