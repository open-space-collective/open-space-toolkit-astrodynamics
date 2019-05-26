////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKuttaDormandPrince5.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Flight_Dynamics_Solvers_RungeKuttaDormandPrince5__
#define __Library_Astrodynamics_Flight_Dynamics_Solvers_RungeKuttaDormandPrince5__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
namespace solvers
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Unique ;
using library::core::types::Real ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Runge-Kutta Dormand-Prince 5 Ordinary Differential Equations (ODE) solver
///
/// @ref                        https://www.boost.org/doc/libs/1_70_0/libs/numeric/odeint/doc/html/boost/numeric/odeint/runge_kutta_dopri5.html

class RungeKuttaDormandPrince5 : public Base
{

    public:

        class Impl ;

                                RungeKuttaDormandPrince5                    ( ) ;

                                RungeKuttaDormandPrince5                    (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

                                RungeKuttaDormandPrince5                    (   const   RungeKuttaDormandPrince5&   aRungeKuttaDormandPrince5Solver             ) ;

                                ~RungeKuttaDormandPrince5                   ( ) ;

        virtual RungeKuttaDormandPrince5* clone                             ( ) const override ;

        Real                    getRelativeTolerance                        ( ) const ;

        Real                    getAbsoluteTolerance                        ( ) const ;

        virtual void            step                                        (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           ) override ;

        virtual void            reset                                       ( ) override ;

    private:

        Unique<RungeKuttaDormandPrince5::Impl> implUPtr_ ;

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
