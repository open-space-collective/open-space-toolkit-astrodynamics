////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKutta4.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Flight_Dynamics_Solvers_RungeKutta4__
#define __Library_Astrodynamics_Flight_Dynamics_Solvers_RungeKutta4__

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

/// @brief                      Runge-Kutta 4 Ordinary Differential Equations (ODE) solver
///
///                             Classical Runge-Kutta of fourth order.
///
/// @ref                        https://www.boost.org/doc/libs/1_70_0/libs/numeric/odeint/doc/html/boost/numeric/odeint/runge_kutta4.html

class RungeKutta4 : public Base
{

    public:

        class Impl ;

                                RungeKutta4                                 ( ) ;

                                RungeKutta4                                 (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

                                RungeKutta4                                 (   const   RungeKutta4&                aRungeKutta4Solver                          ) ;

                                ~RungeKutta4                                ( ) ;

        virtual RungeKutta4*    clone                                       ( ) const override ;

        Real                    getRelativeTolerance                        ( ) const ;

        Real                    getAbsoluteTolerance                        ( ) const ;

        virtual void            step                                        (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           ) override ;

        virtual void            reset                                       ( ) override ;

    private:

        Real                    relativeTolerance_ ;
        Real                    absoluteTolerance_ ;

        Unique<RungeKutta4::Impl> implUPtr_ ;

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
