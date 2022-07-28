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
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>

#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <boost/numeric/odeint.hpp>
#include <boost/bind.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Real ;
using ostk::core::types::String ;
using ostk::core::types::Integer ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;

using namespace boost::numeric::odeint ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines a numerical ODE solver that use the Boost Odeint libraries. This class will be moved into OSTk-math in the future.

class NumericalSolver
{

    public:

        enum class IntegrationStepperType
        {
            RungeKuttaCashKarp54,
            RungeKuttaFehlberg78
        } ;

        enum class IntegrationLogType
        {
            NoLog,
            LogConstant,
            LogAdaptive
        } ;

        typedef std::vector<double> StateVector ; // Container used to hold the state vector
        typedef std::function<void(const StateVector&, StateVector&, const double)> SystemOfEquationsWrapper ; // Function pointer type for returning dynamical equation's pointers

        typedef runge_kutta_cash_karp54<NumericalSolver::StateVector> error_stepper_type_54 ;
        typedef runge_kutta_fehlberg78<NumericalSolver::StateVector> error_stepper_type_78 ;

        /// @brief              Constructor
        ///
        /// @code
        ///                     NumericalSolver numericalSolver = { anIntegrationLogType, anIntegrationStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance } ;
        /// @endcode
        ///
        /// @param              [in] anIntegrationLogType An enum indicating the amount of verbosity wanted to be logged during numerical integration
        /// @param              [in] anIntegrationStepperType An enum indicating the type of numerical stepper used to perform integration
        /// @param              [in] aTimeStep A number indicating the initial guess time step the numerical solver will take
        /// @param              [in] aRelativeTolerance A number indicating the relative intergration tolerance
        /// @param              [in] anAbsoluteTolerance A number indicating the absolute intergration tolerance

                                NumericalSolver                             (   const   NumericalSolver::IntegrationLogType& anIntegrationLogType               = NumericalSolver::IntegrationLogType::NoLog,
                                                                                const   NumericalSolver::IntegrationStepperType& anIntegrationStepperType       = NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54,
                                                                                const   Real&                       aTimeStep                                   = 5.0,
                                                                                const   Real&                       aRelativeTolerance                          = 1.0e-15,
                                                                                const   Real&                       anAbsoluteTolerance                         = 1.0e-15   ) ;

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
        /// @param              [in] aNumericalSolver An numerical solver
        /// @return             A reference to output stream

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   NumericalSolver&            aNumericalSolver                                     ) ;

        /// @brief              Check if numerical solver  is defined
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
        ///                     numericalsolver.getIntegrationLogType() ;
        /// @endcode
        ///
        /// @return             IntegrationLogType

        NumericalSolver::IntegrationLogType getIntegrationLogType           ( ) const ;

        /// @brief              Get integration stepper enum
        ///
        /// @code
        ///                     numericalsolver.getIntegrationStepperType() ;
        /// @endcode
        ///
        /// @return             IntegrationStepperType

        NumericalSolver::IntegrationStepperType getIntegrationStepperType   ( ) const ;

        /// @brief              Get initial time step guess
        ///
        /// @code
        ///                     numericalsolver.getTimeStep() ;
        /// @endcode
        ///
        /// @return             Real

        Real                    getTimeStep                                 ( ) const ;

        /// @brief              Get relative integration tolerance
        ///
        /// @code
        ///                     numericalsolver.getRelativeTolerance() ;
        /// @endcode
        ///
        /// @return             Real

        Real                    getRelativeTolerance                        ( ) const ;

        /// @brief              Get absolute integration tolerance
        ///
        /// @code
        ///                     numericalsolver.getAbsoluteTolerance() ;
        /// @endcode
        ///
        /// @return             Real

        Real                    getAbsoluteTolerance                        ( ) const ;

        /// @brief              Perform numerical integration from an instant to another instant
        ///
        /// @code
        ///                     StateVector stateVector = numericalsolver.integrateStateFromInstantToInstant(stateVector, instant, otherInstant, SystemofEquations) ;
        /// @endcode
        /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
        /// @param              [in] aStartInstant An instant to begin integrating from
        /// @param              [in] anEndInstant An instant to finish integrating at
        /// @param              [in] aSystemOfEquations An std::function wrapper with a particular signature that boost::odeint accepts to perform numerical integration
        /// @return             std::vector<double>

        StateVector             integrateStateFromInstantToInstant          (   const   StateVector&                anInitialStateVector,
                                                                                const   Instant&                    aStartInstant,
                                                                                const   Instant&                    anEndInstant,
                                                                                const   SystemOfEquationsWrapper&   aSystemOfEquations ) const ;

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
                                                                                const   SystemOfEquationsWrapper&   aSystemOfEquations ) const ;

        /// @brief              Get string from the integration stepper type
        ///
        /// @code
        ///                     NumericalSolver::StringFromIntegrationStepperType() ;
        /// @endcode
        ///
        /// @return             IntegrationStepperType

        static String           StringFromIntegrationStepperType            (   const   NumericalSolver::IntegrationStepperType&  aIntegrationStepperType            ) ;

        /// @brief              Get string from the integration log type
        ///
        /// @code
        ///                     NumericalSolver::StringFromIntegrationLogType() ;
        /// @endcode
        ///
        /// @return             IntegrationLogType

        static String           StringFromIntegrationLogType                (   const   NumericalSolver::IntegrationLogType&  aIntegrationLogType                    ) ;

    private:

        NumericalSolver::IntegrationLogType integrationLogType_ ;
        NumericalSolver::IntegrationStepperType integrationStepperType_ ;
        Real timeStep_ ;
        Real relativeTolerance_ ;
        Real absoluteTolerance_ ;

        static void             NumericalIntegrationLogger                  (   const   StateVector&                x,
                                                                                const   double                      t                                           ) ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
