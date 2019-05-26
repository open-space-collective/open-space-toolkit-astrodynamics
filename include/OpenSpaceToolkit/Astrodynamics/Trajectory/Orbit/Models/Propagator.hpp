////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator__

#include <OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/Solver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/System.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Unique.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
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

using ostk::core::types::Unique ;
using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::types::String ;

using ostk::physics::time::Instant ;
using ostk::physics::units::Length ;
using ostk::physics::units::Derived ;
using ostk::physics::env::obj::Celestial ;

using ostk::astro::trajectory::State ;
using ostk::astro::flight::dynamics::System ;
using ostk::astro::flight::dynamics::Solver ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Propagator : public ostk::astro::trajectory::orbit::Model
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
