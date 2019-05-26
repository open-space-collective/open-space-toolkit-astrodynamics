////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/Propagator.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit_Models_Propagator__
#define __Library_Astrodynamics_Trajectory_Orbit_Models_Propagator__

#include <Library/Astrodynamics/Flight/Dynamics/Solver.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/System.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <Library/Astrodynamics/Trajectory/State.hpp>

#include <Library/Physics/Environment/Objects/Celestial.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Length.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Integer.hpp>
#include <Library/Core/Types/Unique.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Unique ;
using library::core::types::Integer ;
using library::core::types::Real ;
using library::core::types::String ;

using library::physics::time::Instant ;
using library::physics::units::Length ;
using library::physics::units::Derived ;
using library::physics::env::obj::Celestial ;

using library::astro::trajectory::State ;
using library::astro::flight::dynamics::System ;
using library::astro::flight::dynamics::Solver ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Propagator : public library::astro::trajectory::orbit::Model
{

    public:

                                Propagator                                  (   const   State&                      anInitialState,
                                                                                const   Integer&                    anInitialRevolutionNumber,
                                                                                const   System&                     aSystem,
                                                                                const   Solver&                     aSolver                                     ) ;

                                Propagator                                  (   const   Propagator&                 aPropagatorModel                            ) ;

                                ~Propagator                                 ( ) ;

        Propagator&             operator =                                  (   const   Propagator&                 aPropagatorModel                            ) ;

        virtual Propagator*     clone                                       ( ) const override ;

        bool                    operator ==                                 (   const   Propagator&                 aPropagatorModel                            ) const ;

        bool                    operator !=                                 (   const   Propagator&                 aPropagatorModel                            ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Propagator&                 aPropagatorModel                            ) ;

        virtual bool            isDefined                                   ( ) const override ;

        State                   getInitialState                             ( ) const ;

        Integer                 getInitialRevolutionNumber                  ( ) const ;

        System                  getSystem                                   ( ) const ;

        Solver                  getSolver                                   ( ) const ;

        virtual Instant         getEpoch                                    ( ) const override ;

        virtual Integer         getRevolutionNumberAtEpoch                  ( ) const override ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        virtual Integer         calculateRevolutionNumberAt                 (   const   Instant&                    anInstant                                   ) const override ; // [TBR] ?

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

    protected:

        virtual bool            operator ==                                 (   const   trajectory::Model&          aModel                                      ) const override ;

        virtual bool            operator !=                                 (   const   trajectory::Model&          aModel                                      ) const override ;

    private:

        State                   initialState_ ;
        Integer                 initialRevolutionNumber_ ;

        mutable System          system_ ;
        mutable Solver          solver_ ;

        void                    setup                                       ( ) ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
