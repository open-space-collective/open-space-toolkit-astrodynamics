////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/NumericalSolver.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <boost/numeric/odeint.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost::numeric::odeint ;

typedef runge_kutta_cash_karp54<NumericalSolver::StateVector> error_stepper_type_54 ;
typedef runge_kutta_fehlberg78<NumericalSolver::StateVector> error_stepper_type_78 ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                NumericalSolver::NumericalSolver            (   const   NumericalSolver::LogType&   aLogType,
                                                                                const   NumericalSolver::StepperType& aStepperType,
                                                                                const   Real&                       aTimeStep,
                                                                                const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   logType_(aLogType),
                                    stepperType_(aStepperType),
                                    timeStep_(aTimeStep),
                                    relativeTolerance_(aRelativeTolerance),
                                    absoluteTolerance_(anAbsoluteTolerance),
                                    states_(),
                                    times_ ()
{

}

                                NumericalSolver::NumericalSolver            (   const   NumericalSolver&            aNumericalSolver                            )
                                :   logType_(aNumericalSolver.logType_),
                                    stepperType_(aNumericalSolver.stepperType_),
                                    timeStep_(aNumericalSolver.timeStep_),
                                    relativeTolerance_(aNumericalSolver.relativeTolerance_),
                                    absoluteTolerance_(aNumericalSolver.absoluteTolerance_),
                                    states_(),
                                    times_ ()
{

}

NumericalSolver*                NumericalSolver::clone                      ( ) const
{
    return new NumericalSolver(*this) ;
}

bool                            NumericalSolver::operator ==                (   const   NumericalSolver&            aNumericalSolver                            ) const
{

    if ((!this->isDefined()) || (!aNumericalSolver.isDefined()))
    {
        return false ;
    }

    return (logType_ == aNumericalSolver.logType_)
        && (stepperType_ == aNumericalSolver.stepperType_)
        && (timeStep_ == aNumericalSolver.timeStep_)
        && (relativeTolerance_ == aNumericalSolver.relativeTolerance_)
        && (absoluteTolerance_ == aNumericalSolver.absoluteTolerance_) ;

}

bool                            NumericalSolver::operator !=                (   const   NumericalSolver&            aNumericalSolver                            ) const
{
    return !((*this) == aNumericalSolver) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   NumericalSolver&            aNumericalSolver                            )
{

    aNumericalSolver.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            NumericalSolver::isDefined                  ( ) const
{
    return timeStep_.isDefined() && relativeTolerance_.isDefined() && absoluteTolerance_.isDefined() ;
}

void                            NumericalSolver::print                      (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Numerical Solver") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Integration logging type:"           << NumericalSolver::StringFromLogType(logType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration stepper type:"           << NumericalSolver::StringFromStepperType(stepperType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration time step:"              << (timeStep_.isDefined() ? timeStep_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration relative tolerance:"     << (relativeTolerance_.isDefined() ? relativeTolerance_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration absolute tolerance:"     << (absoluteTolerance_.isDefined() ? absoluteTolerance_.toString() : "Undefined") ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

NumericalSolver::LogType        NumericalSolver::getLogType                 ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver") ;
    }

    return logType_ ;

}

NumericalSolver::StepperType    NumericalSolver::getStepperType             ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver") ;
    }

    return stepperType_ ;

}

Real                            NumericalSolver::getTimeStep                ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver") ;
    }

    return timeStep_ ;

}

Real                            NumericalSolver::getRelativeTolerance       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver") ;
    }

    return relativeTolerance_ ;

}

Real                            NumericalSolver::getAbsoluteTolerance       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver") ;
    }

    return absoluteTolerance_ ;

}

Array<NumericalSolver::StateVector> NumericalSolver::integrateStatesAtSortedInstants (  const   StateVector&        anInitialStateVector,
                                                                                        const   Instant&            aStartInstant,
                                                                                        const   Array<Instant>&     anInstantArray,
                                                                                        const   NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations   )
{

    NumericalSolver::StateVector aStateVector = anInitialStateVector ;
    states_.clear() ;
    times_.clear() ;

    // Check if instant array has zero length
    if (anInstantArray.size() == 0)
    {
        throw ostk::core::error::RuntimeError("Instant Array is empty") ;
    }

    // Check if the incoming instant array is the same as the start state if it has length 1
    if ((anInstantArray.size() == 1) && (anInstantArray[0] == aStartInstant))
    {
        states_.push_back(anInitialStateVector) ;
        return states_ ;
    }

    // Add start instant to the start of array and convert to integration seconds
    Array<double> anIntegrationDurationInSecsArray = { 0 } ;

    for (const auto& instant : anInstantArray)
    {
        anIntegrationDurationInSecsArray.add((instant - aStartInstant).inSeconds()) ;
    }

    // Ensure integration starts in the correct direction with the initial time step guess
    const double adjustedTimeStep = timeStep_ * anIntegrationDurationInSecsArray[1] / std::abs(anIntegrationDurationInSecsArray[1]) ;

    switch (stepperType_)
    {
        case NumericalSolver::StepperType::RungeKuttaCashKarp54:
        {
            integrate_times(make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()), aSystemOfEquations, aStateVector, anIntegrationDurationInSecsArray.begin(), anIntegrationDurationInSecsArray.end(), adjustedTimeStep, [&] (const NumericalSolver::StateVector &x , double t) -> void {this->NumericalIntegrationObserver(x, t) ;} ) ;
            break ;
        }

        case NumericalSolver::StepperType::RungeKuttaFehlberg78:
        {
            integrate_times(make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()), aSystemOfEquations, aStateVector, anIntegrationDurationInSecsArray.begin(), anIntegrationDurationInSecsArray.end(), adjustedTimeStep, [&] (const NumericalSolver::StateVector &x , double t) -> void {this->NumericalIntegrationObserver(x, t) ;} ) ;
            break ;
        }

        default:
            throw ostk::core::error::runtime::Wrong("Stepper type") ;
    }

    return states_ ;

}

NumericalSolver::StateVector    NumericalSolver::integrateStateForDuration  (   const   StateVector&                anInitialStateVector,
                                                                                const   Duration&                   anIntegrationDuration,
                                                                                const   NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations           )
{

    NumericalSolver::StateVector aStateVector = anInitialStateVector ;
    states_.clear() ;
    times_.clear() ;

    if ((anIntegrationDuration.inSeconds()).isZero()) // If integration duration is zero seconds long, skip integration
    {
        return anInitialStateVector ;
    }

    const double integrationDurationInSecs = anIntegrationDuration.inSeconds() ;

    // Ensure integration starts in the correct direction with the initial time step guess
    const double adjustedTimeStep = timeStep_ * integrationDurationInSecs / std::abs(integrationDurationInSecs) ;

    switch (stepperType_)
    {
        case NumericalSolver::StepperType::RungeKuttaCashKarp54:
        {
            switch (logType_)
            {
                case NumericalSolver::LogType::NoLog:
                case NumericalSolver::LogType::LogAdaptive:
                {
                    integrate_adaptive(make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()), aSystemOfEquations, aStateVector, (0.0), integrationDurationInSecs, adjustedTimeStep, [&] (const NumericalSolver::StateVector &x , double t) -> void {this->NumericalIntegrationObserver(x, t) ;} ) ;
                    return aStateVector ;
                }

                case NumericalSolver::LogType::LogConstant:
                {
                    integrate_const(make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()), aSystemOfEquations, aStateVector, (0.0), integrationDurationInSecs, adjustedTimeStep, [&] (const NumericalSolver::StateVector &x , double t) -> void {this->NumericalIntegrationObserver(x, t) ;} ) ;
                    return aStateVector ;
                }
                default:
                    throw ostk::core::error::runtime::Wrong("Log type") ;
            }
        }

        case NumericalSolver::StepperType::RungeKuttaFehlberg78:
        {
            switch (logType_)
            {
                case NumericalSolver::LogType::NoLog:
                case NumericalSolver::LogType::LogAdaptive:
                {
                    integrate_adaptive(make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()), aSystemOfEquations, aStateVector, (0.0), integrationDurationInSecs, adjustedTimeStep, [&] (const NumericalSolver::StateVector &x , double t) -> void {this->NumericalIntegrationObserver(x, t) ;} ) ;
                    return aStateVector ;
                }

                case NumericalSolver::LogType::LogConstant:
                {
                    integrate_const(make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()), aSystemOfEquations, aStateVector, (0.0), integrationDurationInSecs, adjustedTimeStep, [&] (const NumericalSolver::StateVector &x , double t) -> void {this->NumericalIntegrationObserver(x, t) ;} ) ;
                    return aStateVector ;
                }
                default:
                    throw ostk::core::error::runtime::Wrong("Log type") ;
            }
        }

        default:
            throw ostk::core::error::runtime::Wrong("Stepper type") ;
    }

    throw ostk::core::error::RuntimeError("No State Vector returned from Odeint.") ;

    return NumericalSolver::StateVector(anInitialStateVector.size()) ;

}


NumericalSolver::StateVector    NumericalSolver::integrateStateFromInstantToInstant (   const   StateVector&        anInitialStateVector,
                                                                                        const   Instant&            aStartInstant,
                                                                                        const   Instant&            anEndInstant,
                                                                                        const   NumericalSolver::SystemOfEquationsWrapper&  aSystemOfEquations  )
{
    return this->integrateStateForDuration(anInitialStateVector, (anEndInstant - aStartInstant), aSystemOfEquations) ;
}

String                          NumericalSolver::StringFromLogType          (   const   NumericalSolver::LogType&   aLogType                                    )
{

    switch (aLogType)
    {

        case NumericalSolver::LogType::NoLog:
            return "NoLog" ;

        case NumericalSolver::LogType::LogConstant:
            return "LogConstant" ;

        case NumericalSolver::LogType::LogAdaptive:
            return "LogAdaptive" ;

        default:
            throw ostk::core::error::runtime::Wrong("Log Type") ;

    }

}

String                          NumericalSolver::StringFromStepperType      (   const   NumericalSolver::StepperType& aStepperType                              )
{

    switch (aStepperType)
    {

        case NumericalSolver::StepperType::RungeKuttaCashKarp54:
            return "RungeKuttaCashKarp54" ;

        case NumericalSolver::StepperType::RungeKuttaFehlberg78:
            return "RungeKuttaFehlberg78" ;

        default:
            throw ostk::core::error::runtime::Wrong("Stepper Type") ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            NumericalSolver::NumericalIntegrationObserver ( const   NumericalSolver::StateVector& x,
                                                                                const   double                        t                                         )
{

    states_.push_back(x) ;
    times_.push_back(t) ;

    switch (logType_)
    {
        case NumericalSolver::LogType::LogAdaptive:
        case NumericalSolver::LogType::LogConstant:
            std::cout.precision(3) ;
            std::cout.setf(std::ios::fixed,std::ios::floatfield) ;

            std::cout << std::left << std::setw(15) << t ;

            std::cout.precision(10) ;
            std::cout.setf(std::ios::scientific,std::ios::floatfield) ;

            for (size_t i = 0; i < x.size(); i++)
            {
                std::cout << std::internal << std::setw(16) << x[i] << "     " ;
            }
            std::cout << std::endl ;

            std::cout.setf(std::ios::fixed,std::ios::floatfield) ;

        default:
        {}
    }


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
