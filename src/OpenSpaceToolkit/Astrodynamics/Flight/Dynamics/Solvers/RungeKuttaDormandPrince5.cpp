////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/Solvers/RungeKuttaDormandPrince5.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/Solvers/RungeKuttaDormandPrince5.hpp>

#include <boost/numeric/odeint.hpp>

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

using boost::numeric::odeint::make_controlled ;

using ostk::physics::time::Duration ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RungeKuttaDormandPrince5::Impl
{

    public:

        typedef boost::numeric::odeint::runge_kutta_dopri5<std::vector<double>> InternalStepper ;
        typedef boost::numeric::odeint::result_of::make_controlled<RungeKuttaDormandPrince5::Impl::InternalStepper>::type Stepper ;

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

                                RungeKuttaDormandPrince5::Impl::Impl        (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   relativeTolerance(aRelativeTolerance),
                                    absoluteTolerance(anAbsoluteTolerance),
                                    stepper(make_controlled<RungeKuttaDormandPrince5::Impl::InternalStepper>(anAbsoluteTolerance, aRelativeTolerance))
{

}

                                RungeKuttaDormandPrince5::Impl::~Impl       ( )
{

}

void                            RungeKuttaDormandPrince5::Impl::step        (           System&                     aSystem,
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

void                            RungeKuttaDormandPrince5::Impl::reset       ( )
{
    stepper = make_controlled<RungeKuttaDormandPrince5::Impl::InternalStepper>(absoluteTolerance, relativeTolerance) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                RungeKuttaDormandPrince5::RungeKuttaDormandPrince5 ( const Real&                    aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   Base(),
                                    implUPtr_(std::make_unique<RungeKuttaDormandPrince5::Impl>(aRelativeTolerance, anAbsoluteTolerance))
{

}

                                RungeKuttaDormandPrince5::RungeKuttaDormandPrince5 ( const RungeKuttaDormandPrince5& aRungeKuttaDormandPrince5Solver            )
                                :   Base(aRungeKuttaDormandPrince5Solver),
                                    implUPtr_(std::make_unique<RungeKuttaDormandPrince5::Impl>(aRungeKuttaDormandPrince5Solver.getRelativeTolerance(), aRungeKuttaDormandPrince5Solver.getAbsoluteTolerance()))
{

}

                                RungeKuttaDormandPrince5::~RungeKuttaDormandPrince5 ( )
{

}

RungeKuttaDormandPrince5*       RungeKuttaDormandPrince5::clone             ( ) const
{
    return new RungeKuttaDormandPrince5(*this) ;
}

Real                            RungeKuttaDormandPrince5::getRelativeTolerance ( ) const
{
    return implUPtr_->relativeTolerance ;
}

Real                            RungeKuttaDormandPrince5::getAbsoluteTolerance ( ) const
{
    return implUPtr_->absoluteTolerance ;
}

void                            RungeKuttaDormandPrince5::step              (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           )
{
    implUPtr_->step(aSystem, anIntegrationStep) ;
}

void                            RungeKuttaDormandPrince5::reset             ( )
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
