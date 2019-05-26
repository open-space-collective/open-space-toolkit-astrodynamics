////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKutta4.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKutta4.hpp>

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

class RungeKutta4::Impl
{

    public:



                                Impl                                        ( ) ;

        virtual                 ~Impl                                       ( ) = 0 ;

        virtual Impl*           clone                                       ( ) const = 0 ;

        virtual void            step                                        (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           ) = 0 ;

        virtual void            reset                                       ( ) ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ClassicalRungeKutta4 : public RungeKutta4::Impl
{

    public:

                                ClassicalRungeKutta4                        ( ) ;

        virtual ClassicalRungeKutta4* clone                                 ( ) const override ;

        virtual void            step                                        (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           ) override ;

        virtual void            reset                                       ( ) override ;

    private:

        typedef boost::numeric::odeint::runge_kutta4<std::vector<double>> Stepper ;

        ClassicalRungeKutta4::Stepper stepper_ ;

} ;

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ControlledRungeKutta4 : public RungeKutta4::Impl
{

    public:

                                ControlledRungeKutta4                       (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

        virtual ControlledRungeKutta4* clone                                ( ) const override ;

        virtual void            step                                        (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           ) override ;

        virtual void            reset                                       ( ) override ;

    private:

        typedef boost::numeric::odeint::runge_kutta_cash_karp54<std::vector<double>> InternalStepper ;
        typedef boost::numeric::odeint::result_of::make_controlled<InternalStepper>::type Stepper ;

        Real                    relativeTolerance_ ;
        Real                    absoluteTolerance_ ;

        ControlledRungeKutta4::Stepper stepper_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                RungeKutta4::Impl::Impl                     ( )
{

}

                                RungeKutta4::Impl::~Impl                    ( )
{

}

void                            RungeKutta4::Impl::reset                    ( )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                ClassicalRungeKutta4::ClassicalRungeKutta4  ( )
                                :   RungeKutta4::Impl(),
                                    stepper_(Stepper())
{

}

ClassicalRungeKutta4*           ClassicalRungeKutta4::clone                 ( ) const
{
    return new ClassicalRungeKutta4(*this) ;
}

void                            ClassicalRungeKutta4::step                  (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           )
{

    using boost::numeric::odeint::integrate_const ;

    // std::cout << "stepping... " << anIntegrationStep.toString() << " [s]" << std::endl ;

    integrate_const(stepper_,
                    boost::ref(aSystem),
                    aSystem.accessState(),
                    0.0,
                    static_cast<double>(anIntegrationStep),
                    static_cast<double>(anIntegrationStep)) ;

    aSystem.setInstant(aSystem.getInstant() + Duration::Seconds(anIntegrationStep)) ;

}

void                            ClassicalRungeKutta4::reset                 ( )
{

    RungeKutta4::Impl::reset() ;

    stepper_ = ClassicalRungeKutta4::Stepper() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                ControlledRungeKutta4::ControlledRungeKutta4 (  const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   RungeKutta4::Impl(),
                                    relativeTolerance_(aRelativeTolerance),
                                    absoluteTolerance_(anAbsoluteTolerance),
                                    stepper_(make_controlled<ControlledRungeKutta4::InternalStepper>(absoluteTolerance_, relativeTolerance_))
{

}

ControlledRungeKutta4*          ControlledRungeKutta4::clone                ( ) const
{
    return new ControlledRungeKutta4(*this) ;
}

void                            ControlledRungeKutta4::step                 (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           )
{

    using boost::numeric::odeint::integrate_const ;

    // std::cout << "stepping... " << anIntegrationStep.toString() << " [s]" << std::endl ;

    integrate_const(stepper_,
                    boost::ref(aSystem),
                    aSystem.accessState(),
                    0.0,
                    static_cast<double>(anIntegrationStep),
                    static_cast<double>(anIntegrationStep)) ;

    aSystem.setInstant(aSystem.getInstant() + Duration::Seconds(anIntegrationStep)) ;

}

void                            ControlledRungeKutta4::reset                ( )
{

    RungeKutta4::Impl::reset() ;

    stepper_ = make_controlled<ControlledRungeKutta4::InternalStepper>(relativeTolerance_, absoluteTolerance_) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                RungeKutta4::RungeKutta4                    ( )
                                :   Base(),
                                    relativeTolerance_(Real::Undefined()),
                                    absoluteTolerance_(Real::Undefined()),
                                    implUPtr_(std::make_unique<ClassicalRungeKutta4>())
{

}

                                RungeKutta4::RungeKutta4                    (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   Base(),
                                    relativeTolerance_(aRelativeTolerance),
                                    absoluteTolerance_(anAbsoluteTolerance),
                                    implUPtr_(std::make_unique<ControlledRungeKutta4>(relativeTolerance_, absoluteTolerance_))
{

}

                                RungeKutta4::RungeKutta4                    (   const   RungeKutta4&                aRungeKutta4Solver                          )
                                :   Base(aRungeKutta4Solver),
                                    relativeTolerance_(aRungeKutta4Solver.relativeTolerance_),
                                    absoluteTolerance_(aRungeKutta4Solver.absoluteTolerance_),
                                    implUPtr_(aRungeKutta4Solver.implUPtr_->clone())
{

}

                                RungeKutta4::~RungeKutta4                   ( )
{

}

RungeKutta4*                    RungeKutta4::clone                          ( ) const
{
    return new RungeKutta4(*this) ;
}

Real                            RungeKutta4::getRelativeTolerance           ( ) const
{
    return relativeTolerance_ ;
}

Real                            RungeKutta4::getAbsoluteTolerance           ( ) const
{
    return absoluteTolerance_ ;
}

void                            RungeKutta4::step                           (           System&                     aSystem,
                                                                                const   Real&                       anIntegrationStep                           )
{
    implUPtr_->step(aSystem, anIntegrationStep) ;
}

void                            RungeKutta4::reset                            ( )
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
