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
        typedef std::function<void(const StateVector&, StateVector&, const double)> SystemOfEquationsCallback ; // Function pointer type for returning dynamical equation's pointers

        typedef runge_kutta_cash_karp54<NumericalSolver::StateVector> error_stepper_type_54 ;
        typedef runge_kutta_fehlberg78<NumericalSolver::StateVector> error_stepper_type_78 ;

                                NumericalSolver                             (   const   NumericalSolver::IntegrationLogType& anIntegrationLogType               = NumericalSolver::IntegrationLogType::NoLog,
                                                                                const   NumericalSolver::IntegrationStepperType& anIntegrationStepperType       = NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54,
                                                                                const   Real&                       aTimeStep                                   = 5.0,
                                                                                const   Real&                       aRelativeTolerance                          = 1.0e-15,
                                                                                const   Real&                       anAbsoluteTolerance                         = 1.0e-15   ) ;

                                NumericalSolver                             (   const   NumericalSolver&            aNumericalSolver                            ) ;

        NumericalSolver*        clone                                       ( ) const ;

        bool                    operator ==                                 (   const   NumericalSolver&            aNumericalSolver                            ) const ;

        bool                    operator !=                                 (   const   NumericalSolver&            aNumericalSolver                            ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   NumericalSolver&            aNumericalSolver                                     ) ;

        bool                    isDefined                                   ( ) const ;

        void                    print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        NumericalSolver::IntegrationLogType getIntegrationLogType           ( ) const ;

        NumericalSolver::IntegrationStepperType getIntegrationStepperType   ( ) const ;

        Real                    getTimeStep                                 ( ) const ;

        Real                    getRelativeTolerance                        ( ) const ;

        Real                    getAbsoluteTolerance                        ( ) const ;

        StateVector             integrateStateFromInstantToInstant          (   const   StateVector&                anInitialStateVector,
                                                                                const   Instant&                    aStartInstant,
                                                                                const   Instant&                    anEndInstant,
                                                                                const   SystemOfEquationsCallback&  aSystemOfEquations ) const ;

        StateVector             integrateStateForDuration                   (   const   StateVector&                anInitialStateVector,
                                                                                const   Duration&                   anIntegrationDuration,
                                                                                const   SystemOfEquationsCallback&  aSystemOfEquations ) const ;

        static String           StringFromIntegrationStepperType            (   const   NumericalSolver::IntegrationStepperType&  aIntegrationStepperType            ) ;

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
