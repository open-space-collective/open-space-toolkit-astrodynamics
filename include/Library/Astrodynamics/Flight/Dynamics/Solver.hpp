////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Dynamics/Solver.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Flight_Dynamics_Solver__
#define __Library_Astrodynamics_Flight_Dynamics_Solver__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Dynamics/System.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/Solvers/Base.hpp>

#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Unique.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace flight
{
namespace dynamics
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Unique ;
using library::core::types::Real ;

using library::astro::flight::dynamics::solvers::Base ;
using library::astro::flight::dynamics::System ;

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
