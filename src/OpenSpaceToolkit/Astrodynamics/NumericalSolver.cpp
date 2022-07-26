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

using ostk::physics::units::Length ;
using ostk::physics::units::Time ;
using ostk::physics::units::Derived ;

using boost::bind ;

static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                NumericalSolver::NumericalSolver            (   const   NumericalSolver::IntegrationLogType& anIntegrationLogType,
                                                                                const   NumericalSolver::IntegrationStepperType& anIntegrationStepperType,
                                                                                const   Real&                       aTimeStep,
                                                                                const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         )
                                :   integrationLogType_(anIntegrationLogType),
                                    integrationStepperType_(anIntegrationStepperType),
                                    timeStep_(aTimeStep),
                                    relativeTolerance_(aRelativeTolerance),
                                    absoluteTolerance_(anAbsoluteTolerance)

{

}

                                NumericalSolver::NumericalSolver            (   const   NumericalSolver&            aNumericalSolver                            )
                                :   integrationLogType_(aNumericalSolver.integrationLogType_),
                                    integrationStepperType_(aNumericalSolver.integrationStepperType_),
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

    return (integrationLogType_ == aNumericalSolver.integrationLogType_)
        && (integrationStepperType_ == aNumericalSolver.integrationStepperType_)
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

    bool integrationStepperTypeIsDefinedBool ;
    switch (integrationStepperType_)
    {
        case NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54:
            integrationStepperTypeIsDefinedBool = true ;
            break ;
        case NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78:
            integrationStepperTypeIsDefinedBool = true ;
            break ;
        default:
            integrationStepperTypeIsDefinedBool = false ;
            break ;
    }

     bool integrationLogTypeIsDefinedBool  ;
    switch (integrationLogType_)
    {
        case NumericalSolver::IntegrationLogType::NoLog:
            integrationLogTypeIsDefinedBool = true ;
            break ;
        case NumericalSolver::IntegrationLogType::LogConstant:
            integrationLogTypeIsDefinedBool = true ;
            break ;
        case NumericalSolver::IntegrationLogType::LogAdaptive:
            integrationLogTypeIsDefinedBool = true ;
            break ;
        default:
            integrationLogTypeIsDefinedBool = false ;
            break ;
    }

    return integrationLogTypeIsDefinedBool && integrationStepperTypeIsDefinedBool && timeStep_.isDefined() && relativeTolerance_.isDefined() && absoluteTolerance_.isDefined();
}

void                            NumericalSolver::print                      (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "NumericalSolver") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Integration logging type:"                << NumericalSolver::StringFromIntegrationLogType(integrationLogType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration stepper type:"                << NumericalSolver::StringFromIntegrationStepperType(integrationStepperType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration time step:"                << (timeStep_.isDefined() ? timeStep_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration relative tolerance:"                << (relativeTolerance_.isDefined() ? relativeTolerance_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Integration absolute tolerance:"                << (absoluteTolerance_.isDefined() ? absoluteTolerance_.toString() : "Undefined") ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

NumericalSolver::IntegrationLogType  NumericalSolver::getIntegrationLogType ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver") ;
    }

    return integrationLogType_ ;

}

NumericalSolver::IntegrationStepperType  NumericalSolver::getIntegrationStepperType ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver") ;
    }

    return integrationStepperType_ ;

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
                                                                                const   NumericalSolver::SystemOfEquationsCallback&  aSystemOfEquations ) const
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

    switch (integrationLogType_)
    {
        case NumericalSolver::IntegrationLogType::NoLog:
        {
            switch (integrationStepperType_)
                {
                    case NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep) ;
                        return aStateVector ;
                    case NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep) ;
                        return aStateVector ;
                }
        }
        case NumericalSolver::IntegrationLogType::LogConstant:
        {
            switch (integrationStepperType_)
                {
                    case NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54:
                        integrate_const (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                    case NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78:
                        integrate_const (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                }
        }
        case NumericalSolver::IntegrationLogType::LogAdaptive:
        {
            switch (integrationStepperType_)
                {
                    case NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                    case NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                }
        }
    }

    throw ostk::core::error::runtime::Undefined("No StateVector returned from Odeint") ;

    return NumericalSolver::StateVector(anInitialStateVector.size()) ;

}


NumericalSolver::StateVector    NumericalSolver::integrateStateFromInstantToInstant (   const   StateVector&        anInitialStateVector,
                                                                                        const   Instant&            aStartInstant,                              // Can take in an instant with a bit of overhead and make lighter internally
                                                                                        const   Instant&            anEndInstant,
                                                                                        const   NumericalSolver::SystemOfEquationsCallback&  aSystemOfEquations ) const
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

    switch (integrationLogType_)
    {
        case NumericalSolver::IntegrationLogType::NoLog: //[TBR] determine whether or not the adaptive is the best to use when no log is specified
        {
            switch (integrationStepperType_)
                {
                    case NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep) ;
                        return aStateVector ;
                    case NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep) ;
                        return aStateVector ;
                }
        }
        case NumericalSolver::IntegrationLogType::LogConstant:
        {
            switch (integrationStepperType_)
                {
                    case NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54:
                        integrate_const (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                    case NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78:
                        integrate_const (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                }
        }
        case NumericalSolver::IntegrationLogType::LogAdaptive:
        {
            switch (integrationStepperType_)
                {
                    case NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_54() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                    case NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78:
                        integrate_adaptive (make_controlled( absoluteTolerance_, relativeTolerance_, NumericalSolver::error_stepper_type_78() ), aSystemOfEquations, aStateVector, (0.0), propDurationInSecs, adjustedTimeStep, NumericalSolver::NumericalIntegrationLogger) ;
                        return aStateVector ;
                }
        }
    }

    throw ostk::core::error::runtime::Undefined("No StateVector returned from Odeint") ;
    return NumericalSolver::StateVector(anInitialStateVector.size()) ;

}



String                          NumericalSolver::StringFromIntegrationLogType (   const   NumericalSolver::IntegrationLogType&  anIntegrationLogType  )
{

    switch (anIntegrationLogType)
    {

        case NumericalSolver::IntegrationLogType::NoLog:
            return "NoLog" ;

        case NumericalSolver::IntegrationLogType::LogConstant:
            return "LogConstant" ;

        case NumericalSolver::IntegrationLogType::LogAdaptive:
            return "LogAdaptive" ;

        default:
            throw ostk::core::error::runtime::Wrong("Integration Stepper Type") ;

    }

    return String::Empty() ;

}

String                          NumericalSolver::StringFromIntegrationStepperType (   const   NumericalSolver::IntegrationStepperType&  anIntegrationStepperType  )
{

    switch (anIntegrationStepperType)
    {

        case NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54:
            return "RungeKuttaCashKarp54" ;
        case NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78:
            return "RungeKuttaFehlberg78" ;

        default:
            throw ostk::core::error::runtime::Wrong("Integration Stepper Type") ;

    }

    return String::Empty() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            NumericalSolver::NumericalIntegrationLogger (   const   NumericalSolver::StateVector&  x,
                                                                                const   double                      t                                           )
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
