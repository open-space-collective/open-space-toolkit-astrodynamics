////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKuttaFehlberg78.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKuttaFehlberg78.hpp>

#include <boost/numeric/odeint.hpp>

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

using boost::numeric::odeint::make_controlled ;

using library::physics::time::Duration ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RungeKuttaFehlberg78::Impl
{

    public:

        typedef boost::numeric::odeint::runge_kutta_fehlberg78<std::vector<double>> InternalStepper ;
        typedef boost::numeric::odeint::result_of::make_controlled<RungeKuttaFehlberg78::Impl::InternalStepper>::type Stepper ;

        Real                    relativeTolerance ;
        Real                    absoluteTolerance ;

        Stepper                 stepper ;



                                Impl                                        (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

                                ~Impl                                       ( ) ;

        void                    step                                        (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           ) ;

        void                    reset                                       ( ) ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                RungeKuttaFehlberg78::Impl::Impl            (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   relativeTolerance(aRelativeTolerance),
                                    absoluteTolerance(anAbsoluteTolerance),
                                    stepper(make_controlled<RungeKuttaFehlberg78::Impl::InternalStepper>(anAbsoluteTolerance, aRelativeTolerance))
{

}

                                RungeKuttaFehlberg78::Impl::~Impl           ( )
{

}

void                            RungeKuttaFehlberg78::Impl::step            (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           )
{

    using boost::numeric::odeint::integrate_const ;

    integrate_const(stepper,
                    boost::ref(aSystem),
                    aSystem.accessState(),
                    0.0,
                    static_cast<double>(anIntegrationStep),
                    static_cast<double>(anIntegrationStep)) ;

    aSystem.setInstant(aSystem.getInstant() + Duration::Seconds(anIntegrationStep)) ;

}

void                            RungeKuttaFehlberg78::Impl::reset           ( )
{
    stepper = make_controlled<RungeKuttaFehlberg78::Impl::InternalStepper>(absoluteTolerance, relativeTolerance) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                RungeKuttaFehlberg78::RungeKuttaFehlberg78  (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   Base(),
                                    implUPtr_(std::make_unique<RungeKuttaFehlberg78::Impl>(aRelativeTolerance, anAbsoluteTolerance))
{

}

                                RungeKuttaFehlberg78::RungeKuttaFehlberg78  (   const   RungeKuttaFehlberg78&       aRungeKuttaFehlberg78Solver                 )
                                :   Base(aRungeKuttaFehlberg78Solver),
                                    implUPtr_(std::make_unique<RungeKuttaFehlberg78::Impl>(aRungeKuttaFehlberg78Solver.getRelativeTolerance(), aRungeKuttaFehlberg78Solver.getAbsoluteTolerance()))
{

}

                                RungeKuttaFehlberg78::~RungeKuttaFehlberg78 ( )
{

}

RungeKuttaFehlberg78*           RungeKuttaFehlberg78::clone                 ( ) const
{
    return new RungeKuttaFehlberg78(*this) ;
}

Real                            RungeKuttaFehlberg78::getRelativeTolerance  ( ) const
{
    return implUPtr_->relativeTolerance ;
}

Real                            RungeKuttaFehlberg78::getAbsoluteTolerance  ( ) const
{
    return implUPtr_->absoluteTolerance ;
}

void                            RungeKuttaFehlberg78::step                  (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           )
{
    implUPtr_->step(aSystem, anIntegrationStep) ;
}

void                            RungeKuttaFehlberg78::reset                            ( )
{
    implUPtr_->reset() ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
