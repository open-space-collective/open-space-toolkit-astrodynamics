////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/Solver.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Dynamics_Solver__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Dynamics_Solver__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/System.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/Solvers/Base.hpp>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Unique.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{
namespace dynamics
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Unique ;
using ostk::core::types::Real ;

using ostk::astro::flight::dynamics::solvers::Base ;
using ostk::astro::flight::dynamics::System ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Flight dynamics solver (Ordinary Differential Equations (ODE))

class Solver
{

    public:

        enum class Type
        {

            Undefined,
            RungeKutta4,
            RungeKuttaDormandPrince5,
            RungeKuttaFehlberg78

        } ;

                                Solver                                      (   const  	Solver::Type&               aType,
                                                                                const   Unique<Base>&               aSolverUPtr                                	) ;

                                Solver                                      (   const 	Solver&                     aSolver                                    	) ;

                                ~Solver                                     ( ) ;

        Solver&                 operator =                                	(   const  	Solver&                 	aSolver                                    	) ;

        bool                    isDefined                                   ( ) const ;

        Solver::Type            getType                                    	( ) const ;

        void                    step                                       	(           System&                     aSystem,
                                                                                const 	Real&                       anIntegrationStep                         	) ;

        void                    reset                                      	( ) ;

        static Solver           Undefined                                  	( ) ;

        static Solver           RungeKutta4                                 (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

        static Solver           RungeKuttaDormandPrince5                    (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

        static Solver           RungeKuttaFehlberg78                        (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

    private:

        Solver::Type            type_ ;
        Unique<Base>            solverUPtr_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
