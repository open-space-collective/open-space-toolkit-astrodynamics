////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_NumericalSolver__
#define __OpenSpaceToolkit_Astrodynamics_NumericalSolver__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::types::String ;
using ostk::core::types::Size ;
using ostk::core::ctnr::Array ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines a numerical ODE solver that use the Boost Odeint libraries. This class will be moved into OSTk-math in the future.

class NumericalSolver
{

    public:

        enum class StepperType
        {
            RungeKuttaCashKarp54,
            RungeKuttaFehlberg78
        } ;

        enum class LogType
        {
            NoLog,
            LogConstant,
            LogAdaptive
        } ;

        typedef std::vector<double> StateVector ; // Container used to hold the state vector
        typedef std::function<void(const StateVector&, StateVector&, const double)> SystemOfEquationsWrapper ; // Function pointer type for returning dynamical equation's pointers

        /// @brief              Constructor
        ///
        /// @code
        ///                     NumericalSolver numericalSolver = { aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance } ;
        /// @endcode
        ///
        /// @param              [in] aLogType An enum indicating the amount of verbosity wanted to be logged during numerical integration
        /// @param              [in] aStepperType An enum indicating the type of numerical stepper used to perform integration
        /// @param              [in] aTimeStep A number indicating the initial guess time step the numerical solver will take
        /// @param              [in] aRelativeTolerance A number indicating the relative integration tolerance
        /// @param              [in] anAbsoluteTolerance A number indicating the absolute integration tolerance

                                NumericalSolver                             (   const   NumericalSolver::LogType&   aLogType,
                                                                                const   NumericalSolver::StepperType& aStepperType,
                                                                                const   Real&                       aTimeStep,
                                                                                const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

        /// @brief              Copy Constructor
        ///
        /// @param              [in] NumericalSolver A numerical solver

                                NumericalSolver                             (   const   NumericalSolver&            aNumericalSolver                            ) ;

        /// @brief              Clone numerical solver
        ///
        /// @return             Pointer to cloned numerical solver

        NumericalSolver*        clone                                       ( ) const ;

        /// @brief              Equal to operator
        ///
        /// @param              [in] aNumericalSolver A numerical solver
        /// @return             True if numerical solver are equal

        bool                    operator ==                                 (   const   NumericalSolver&            aNumericalSolver                            ) const ;

        /// @brief              Not equal to operator
        ///
        /// @param              [in] aNumericalSolver A numerical solver
        /// @return             True if numerical solver are not equal

        bool                    operator !=                                 (   const   NumericalSolver&            aNumericalSolver                            ) const ;

        /// @brief              Output stream operator
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] aNumericalSolver A numerical solver
        /// @return             A reference to output stream

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   NumericalSolver&            aNumericalSolver                            ) ;

        /// @brief              Check if numerical solver is defined
        ///
        /// @return             True if numerical solver is defined

        bool                    isDefined                                   ( ) const ;

        /// @brief              Print numerical solver
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] (optional) displayDecorators If true, display decorators

        void                    print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        /// @brief              Get integration logging enum
        ///
        /// @code
        ///                     numericalSolver.getLogType() ;
        /// @endcode
        ///
        /// @return             LogType

        NumericalSolver::LogType getLogType                                 ( ) const ;

        /// @brief              Get integration stepper enum
        ///
        /// @code
        ///                     numericalSolver.getStepperType() ;
        /// @endcode
        ///
        /// @return             StepperType

        NumericalSolver::StepperType getStepperType                         ( ) const ;

        /// @brief              Get initial time step guess
        ///
        /// @code
        ///                     numericalSolver.getTimeStep() ;
        /// @endcode
        ///
        /// @return             Real

        Real                    getTimeStep                                 ( ) const ;

        /// @brief              Get relative integration tolerance
        ///
        /// @code
        ///                     numericalSolver.getRelativeTolerance() ;
        /// @endcode
        ///
        /// @return             Real

        Real                    getRelativeTolerance                        ( ) const ;

        /// @brief              Get absolute integration tolerance
        ///
        /// @code
        ///                     numericalSolver.getAbsoluteTolerance() ;
        /// @endcode
        ///
        /// @return             Real

        Real                    getAbsoluteTolerance                        ( ) const ;

        /// @brief              Perform numerical integration from a starting instant to an array of states
        ///
        /// @code
        ///                     Array<StateVector> stateVectorArray = numericalSolver.integrateStatesAtSortedInstants(stateVector, instant, instantArray, systemOfEquations) ;
        /// @endcode
        ///
        /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
        /// @param              [in] aStartInstant An instant to begin integrating from
        /// @param              [in] anInstantArray An instant array to integrate to
        /// @param              [in] aSystemOfEquations An std::function wrapper with a particular signature that boost::odeint accepts to perform numerical integration
        /// @return             std::vector<std::vector<double>>

        Array<StateVector>      integrateStatesAtSortedInstants             (   const   StateVector&                anInitialStateVector,
                                                                                const   Instant&                    aStartInstant,
                                                                                const   Array<Instant>&             anInstantArray,
                                                                                const   SystemOfEquationsWrapper&   aSystemOfEquations                          ) ;

        /// @brief              Perform numerical integration from an instant to another instant
        ///
        /// @code
        ///                     StateVector stateVector = numericalSolver.integrateStateFromInstantToInstant(stateVector, instant, otherInstant, systemOfEquations) ;
        /// @endcode
        /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
        /// @param              [in] aStartInstant An instant to begin integrating from
        /// @param              [in] anEndInstant An instant to finish integrating at
        /// @param              [in] aSystemOfEquations An std::function wrapper with a particular signature that boost::odeint accepts to perform numerical integration
        /// @return             std::vector<double>

        StateVector             integrateStateFromInstantToInstant          (   const   StateVector&                anInitialStateVector,
                                                                                const   Instant&                    aStartInstant,
                                                                                const   Instant&                    anEndInstant,
                                                                                const   SystemOfEquationsWrapper&   aSystemOfEquations                          ) ;

        /// @brief              Perform numerical integration for a certain duration
        ///
        /// @code
        ///                     StateVector stateVector = numericalsolver.integrateStateFromInstantToInstant(stateVector, instant, otherInstant, SystemofEquations) ;
        /// @endcode
        /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
        /// @param              [in] anIntegrationDuration A duration over which to integration
        /// @param              [in] aSystemOfEquations An std::function wrapper with a particular signature that boost::odeint accepts to perform numerical integration
        /// @return             std::vector<double>

        StateVector             integrateStateForDuration                   (   const   StateVector&                anInitialStateVector,
                                                                                const   Duration&                   anIntegrationDuration,
                                                                                const   SystemOfEquationsWrapper&   aSystemOfEquations                          ) ;

        /// @brief              Get string from the integration stepper type
        ///
        /// @code
        ///                     NumericalSolver::StringFromStepperType(aStepperType) ;
        /// @endcode
        /// @param              [in] aStepperType An integration stepper type enum
        /// @return             StepperType

        static String           StringFromStepperType                       (   const   NumericalSolver::StepperType& aStepperType                              ) ;

        /// @brief              Get string from the integration log type
        ///
        /// @code
        ///                     NumericalSolver::StringFromLogType(aLogType) ;
        /// @endcode
        /// @param              [in] aLogType An integration log type enum
        /// @return             LogType

        static String           StringFromLogType                           (   const   NumericalSolver::LogType&   aLogType                                    ) ;

    private:

        NumericalSolver::LogType logType_ ;
        NumericalSolver::StepperType stepperType_ ;
        Real timeStep_ ;
        Real relativeTolerance_ ;
        Real absoluteTolerance_ ;
        std::vector<StateVector> states_ ;
        std::vector<double> instants_ ;

        void                    observeNumericalIntegration                 (   const   StateVector&                x,
                                                                                const   double                      t                                           ) ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
