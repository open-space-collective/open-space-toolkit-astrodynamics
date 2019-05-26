////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/Solvers/RungeKuttaFehlberg78.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Dynamics_Solvers_RungeKuttaFehlberg78__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Dynamics_Solvers_RungeKuttaFehlberg78__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
namespace solvers
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Unique ;
using ostk::core::types::Real ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Runge-Kutta Fehlberg 78 Ordinary Differential Equations (ODE) solver
///
///                             Standard method for high-precision applications.
///
/// @ref                        https://www.boost.org/doc/libs/1_70_0/libs/numeric/odeint/doc/html/boost/numeric/odeint/runge_kutta_fehlberg78.html

class RungeKuttaFehlberg78 : public Base
{

    public:

        class Impl ;

                                RungeKuttaFehlberg78                        ( ) ;

                                RungeKuttaFehlberg78                        (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

                                RungeKuttaFehlberg78                        (   const   RungeKuttaFehlberg78&       aRungeKuttaFehlberg78Solver                 ) ;

                                ~RungeKuttaFehlberg78                       ( ) ;

        virtual RungeKuttaFehlberg78* clone                                 ( ) const override ;

        Real                    getRelativeTolerance                        ( ) const ;

        Real                    getAbsoluteTolerance                        ( ) const ;

        virtual void            step                                        (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           ) override ;

        virtual void            reset                                       ( ) override ;

    private:

        Unique<RungeKuttaFehlberg78::Impl> implUPtr_ ;

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
