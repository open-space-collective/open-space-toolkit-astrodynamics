////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/NumericalSolver.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                NumericalSolver::NumericalSolver            (   const   NumericalSolver::LogType&   aLogType,
                                                                                const   NumericalSolver::StepperType&  aStepperType,
                                                                                const   Real&                       aTimeStep,
                                                                                const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   logType_(aLogType),
                                    stepperType_(aStepperType),
                                    timeStep_(aTimeStep),
                                    relativeTolerance_(aRelativeTolerance),
                                    absoluteTolerance_(anAbsoluteTolerance)

{

}

                                NumericalSolver::NumericalSolver            (   const   NumericalSolver&            aNumericalSolver                            )
                                :   logType_(aNumericalSolver.logType_),
                                    stepperType_(aNumericalSolver.stepperType_),
                                    timeStep_(aNumericalSolver.timeStep_),
                                    relativeTolerance_(aNumericalSolver.relativeTolerance_),
                                    absoluteTolerance_(aNumericalSolver.absoluteTolerance_)

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

    bool stepperTypeIsDefinedBool ;
    switch (stepperType_)
    {
        case NumericalSolver::StepperType::RungeKuttaCashKarp54:
            stepperTypeIsDefinedBool = true ;
            break ;
        case NumericalSolver::StepperType::RungeKuttaFehlberg78:
            stepperTypeIsDefinedBool = true ;
            break ;
        default:
            stepperTypeIsDefinedBool = false ;
            break ;
    }

     bool logTypeIsDefinedBool  ;
    switch (logType_)
    {
        case NumericalSolver::LogType::NoLog:
            logTypeIsDefinedBool = true ;
            break ;
        case NumericalSolver::LogType::LogConstant:
            logTypeIsDefinedBool = true ;
            break ;
        case NumericalSolver::LogType::LogAdaptive:
            logTypeIsDefinedBool = true ;
            break ;
        default:
            logTypeIsDefinedBool = false ;
            break ;
    }

    return logTypeIsDefinedBool && stepperTypeIsDefinedBool && timeStep_.isDefined() && relativeTolerance_.isDefined() && absoluteTolerance_.isDefined();
}

void                            NumericalSolver::print                      (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "NumericalSolver") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Integration logging type:"                << NumericalSolver::StringFromLogType(logType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration stepper type:"                << NumericalSolver::StringFromStepperType(stepperType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration time step:"                << (timeStep_.isDefined() ? timeStep_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration relative tolerance:"                << (relativeTolerance_.isDefined() ? relativeTolerance_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration absolute tolerance:"                << (absoluteTolerance_.isDefined() ? absoluteTolerance_.toString() : "Undefined") ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

NumericalSolver::LogType  NumericalSolver::getLogType                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver") ;
    }

    return logType_ ;

}

NumericalSolver::StepperType  NumericalSolver::getStepperType               ( ) const
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

NumericalSolver::StateVector    NumericalSolver::integrateStateForDuration  (   const   StateVector&                anInitialStateVector,
                                                                                const   Duration&                   anIntegrationDuration,
                                                                                const   NumericalSolver::SystemOfEquationsWrapper&  aSystemOfEquations          ) const
{

    NumericalSolver::StateVector aStateVector = anInitialStateVector ;

    double propDurationInSecs ;
    if ( (anIntegrationDuration.inSeconds()).isZero() ) // If integration duration is zero seconds long, skip integration
    {
        return anInitialStateVector ;
    }
    else
    {
        propDurationInSecs = (double)(anIntegrationDuration.inSeconds()) ;
    }

    // Ensure integration starts in the correct direction with the initial time step guess
    const double adjustedTimeStep = timeStep_ * propDurationInSecs / std::abs(propDurationInSecs) ;

    switch (logType_)
    {
        case NumericalSolver::LogType::NoLog:
        {
            switch (stepperType_)
                {
                    case NumericalSolver::StepperType::RungeKuttaCashKarp54:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep) ;
                        return aStateVector ;
                    case NumericalSolver::StepperType::RungeKuttaFehlberg78:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep) ;
                        return aStateVector ;
                }
        }
        case NumericalSolver::LogType::LogConstant:
        {
            switch (stepperType_)
                {
                    case NumericalSolver::StepperType::RungeKuttaCashKarp54:
                        integrate_const (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                    case NumericalSolver::StepperType::RungeKuttaFehlberg78:
                        integrate_const (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                }
        }
        case NumericalSolver::LogType::LogAdaptive:
        {
            switch (stepperType_)
                {
                    case NumericalSolver::StepperType::RungeKuttaCashKarp54:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                    case NumericalSolver::StepperType::RungeKuttaFehlberg78:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                }
        }
    }

    throw ostk::core::error::runtime::Undefined("No StateVector returned from Odeint") ;

    return NumericalSolver::StateVector(anInitialStateVector.size()) ;

}


NumericalSolver::StateVector    NumericalSolver::integrateStateFromInstantToInstant (   const   StateVector&        anInitialStateVector,
                                                                                        const   Instant&            aStartInstant,
                                                                                        const   Instant&            anEndInstant,
                                                                                        const   NumericalSolver::SystemOfEquationsWrapper&  aSystemOfEquations  ) const
{

// [TBI] Incldue safety checks to make sure incoming parameters don't break stuff

    NumericalSolver::StateVector aStateVector = anInitialStateVector ;

    double propDurationInSecs ;
    if ( ((anEndInstant-aStartInstant).inSeconds()).isZero() ) // If integration duration is zero seconds long, skip integration
    {
        return anInitialStateVector ;
    }
    else
    {
        propDurationInSecs = (double)((anEndInstant-aStartInstant).inSeconds()) ;
    }

    // Ensure integration starts in the correct direction with the initial time step guess
    const double adjustedTimeStep = timeStep_ * propDurationInSecs / std::abs(propDurationInSecs) ;

    switch (logType_)
    {
        case NumericalSolver::LogType::NoLog: //[TBR] determine whether or not the adaptive is the best to use when no log is specified
        {
            switch (stepperType_)
                {
                    case NumericalSolver::StepperType::RungeKuttaCashKarp54:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep) ;
                        return aStateVector ;
                    case NumericalSolver::StepperType::RungeKuttaFehlberg78:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep) ;
                        return aStateVector ;
                }
        }
        case NumericalSolver::LogType::LogConstant:
        {
            switch (stepperType_)
                {
                    case NumericalSolver::StepperType::RungeKuttaCashKarp54:
                        integrate_const (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                    case NumericalSolver::StepperType::RungeKuttaFehlberg78:
                        integrate_const (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                }
        }
        case NumericalSolver::LogType::LogAdaptive:
        {
            switch (stepperType_)
                {
                    case NumericalSolver::StepperType::RungeKuttaCashKarp54:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                    case NumericalSolver::StepperType::RungeKuttaFehlberg78:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                }
        }
    }

    throw ostk::core::error::runtime::Undefined("No StateVector returned from Odeint") ;
    return NumericalSolver::StateVector(anInitialStateVector.size()) ;

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
            throw ostk::core::error::runtime::Wrong("Integration Stepper Type") ;

    }

    return String::Empty() ;

}

String                          NumericalSolver::StringFromStepperType      (   const   NumericalSolver::StepperType&  aStepperType                             )
{

    switch (aStepperType)
    {

        case NumericalSolver::StepperType::RungeKuttaCashKarp54:
            return "RungeKuttaCashKarp54" ;
        case NumericalSolver::StepperType::RungeKuttaFehlberg78:
            return "RungeKuttaFehlberg78" ;

        default:
            throw ostk::core::error::runtime::Wrong("Integration Stepper Type") ;

    }

    return String::Empty() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            NumericalSolver::NumericalIntegrationLogger (   const   NumericalSolver::StateVector&   x,
                                                                                const   double                          t                                       )
{
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
