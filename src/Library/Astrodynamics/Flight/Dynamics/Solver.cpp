////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Dynamics/Solver.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKuttaFehlberg78.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKuttaDormandPrince5.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/Solvers/RungeKutta4.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/Solver.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

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

                                Solver::Solver                              (   const    Solver::Type&              aType,
                                                                                const     Unique<Base>&             aSolverUPtr                                 )
                                :   type_(aType),
                                    solverUPtr_((aSolverUPtr != nullptr) ? aSolverUPtr->clone() : nullptr)
{

}

                                Solver::Solver                             (    const    Solver&                    aSolver                                     )
                                :   type_(aSolver.type_),
                                    solverUPtr_((aSolver.solverUPtr_ != nullptr) ? aSolver.solverUPtr_->clone() : nullptr)
{

}

                                Solver::~Solver                             ( )
{

}

Solver&                         Solver::operator =                          (   const   Solver&                     aSolver                                     )
{

    if (this != &aSolver)
    {

        type_ = aSolver.type_ ;

        solverUPtr_.reset((aSolver.solverUPtr_ != nullptr) ? aSolver.solverUPtr_->clone() : nullptr) ;

    }

    return *this ;

}

bool                            Solver::isDefined                           ( ) const
{
    return (type_ != Solver::Type::Undefined) && (solverUPtr_ != nullptr) ;
}

Solver::Type                    Solver::getType                             ( ) const
{
    return type_ ;
}

void                            Solver::step                                (           System&                     aSystem,
                                                                                const 	Real&                       anIntegrationStep                         	)
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Solver") ;
    }

    solverUPtr_->step(aSystem, anIntegrationStep) ;

}

void                            Solver::reset                               ( )
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Solver") ;
    }

    solverUPtr_->reset() ;

}

Solver                          Solver::Undefined                           ( )
{
    return { Solver::Type::Undefined, nullptr } ;
}

Solver                          Solver::RungeKutta4                         (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
{

    using library::astro::flight::dynamics::solvers::RungeKutta4 ;

    return { Solver::Type::RungeKutta4, std::make_unique<RungeKutta4>(aRelativeTolerance, anAbsoluteTolerance) } ;

}

Solver                          Solver::RungeKuttaDormandPrince5            (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
{

    using library::astro::flight::dynamics::solvers::RungeKuttaDormandPrince5 ;

    return { Solver::Type::RungeKuttaDormandPrince5, std::make_unique<RungeKuttaDormandPrince5>(aRelativeTolerance, anAbsoluteTolerance) } ;

}

Solver                          Solver::RungeKuttaFehlberg78                (   const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
{

    using library::astro::flight::dynamics::solvers::RungeKuttaFehlberg78 ;

    return { Solver::Type::RungeKuttaFehlberg78, std::make_unique<RungeKuttaFehlberg78>(aRelativeTolerance, anAbsoluteTolerance) } ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
