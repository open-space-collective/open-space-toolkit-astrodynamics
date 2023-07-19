/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_NumericalSolver__
#define __OpenSpaceToolkit_Astrodynamics_NumericalSolver__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Size;
using ostk::core::types::String;

/// @brief                      Defines a numerical ODE solver that use the Boost Odeint libraries. This class will be
/// moved into OSTk-math in the future.

class NumericalSolver
{
   public:
    enum class StepperType
    {
        RungeKutta4,
        RungeKuttaCashKarp54,
        RungeKuttaFehlberg78
    };

    enum class LogType
    {
        NoLog,
        LogConstant,
        LogAdaptive
    };

    typedef std::vector<double> StateVector;  // Container used to hold the state vector
    typedef std::function<void(const StateVector&, StateVector&, const double)>
        SystemOfEquationsWrapper;                // Function pointer type for returning dynamical equation's pointers
    typedef Pair<StateVector, double> Solution;  // Container used to hold the state vector and time

    /// @brief              Constructor
    ///
    /// @code
    ///                     NumericalSolver numericalSolver = { aLogType, aStepperType, aTimeStep, aRelativeTolerance,
    ///                     anAbsoluteTolerance } ;
    /// @endcode
    ///
    /// @param              [in] aLogType An enum indicating the amount of verbosity wanted to be logged during
    /// numerical integration
    /// @param              [in] aStepperType An enum indicating the type of numerical stepper used to perform
    /// integration
    /// @param              [in] aTimeStep A number indicating the initial guess time step the numerical solver will
    /// take
    /// @param              [in] aRelativeTolerance A number indicating the relative integration tolerance
    /// @param              [in] anAbsoluteTolerance A number indicating the absolute integration tolerance

    NumericalSolver(
        const NumericalSolver::LogType& aLogType,
        const NumericalSolver::StepperType& aStepperType,
        const Real& aTimeStep,
        const Real& aRelativeTolerance,
        const Real& anAbsoluteTolerance
    );

    /// @brief              Clone numerical solver
    ///
    /// @return             Pointer to cloned numerical solver

    NumericalSolver* clone() const;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aNumericalSolver A numerical solver
    /// @return             True if numerical solver are equal

    bool operator==(const NumericalSolver& aNumericalSolver) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aNumericalSolver A numerical solver
    /// @return             True if numerical solver are not equal

    bool operator!=(const NumericalSolver& aNumericalSolver) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aNumericalSolver A numerical solver
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const NumericalSolver& aNumericalSolver);

    /// @brief              Check if numerical solver is defined
    ///
    /// @return             True if numerical solver is defined

    bool isDefined() const;

    /// @brief              Print numerical solver
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief              Get integration logging enum
    ///
    /// @code
    ///                     numericalSolver.getLogType() ;
    /// @endcode
    ///
    /// @return             LogType

    NumericalSolver::LogType getLogType() const;

    /// @brief              Get integration stepper enum
    ///
    /// @code
    ///                     numericalSolver.getStepperType() ;
    /// @endcode
    ///
    /// @return             StepperType

    NumericalSolver::StepperType getStepperType() const;

    /// @brief              Get initial time step guess
    ///
    /// @code
    ///                     numericalSolver.getTimeStep() ;
    /// @endcode
    ///
    /// @return             Real

    Real getTimeStep() const;

    /// @brief              Get relative integration tolerance
    ///
    /// @code
    ///                     numericalSolver.getRelativeTolerance() ;
    /// @endcode
    ///
    /// @return             Real

    Real getRelativeTolerance() const;

    /// @brief              Get absolute integration tolerance
    ///
    /// @code
    ///                     numericalSolver.getAbsoluteTolerance() ;
    /// @endcode
    ///
    /// @return             Real

    Real getAbsoluteTolerance() const;

    /// @brief              Perform numerical integration from a starting time to an array of states
    ///
    /// @code
    ///                     Array<Solution> solutions =
    ///                     numericalSolver.integrateTime(stateVector, startTime, timeArray, systemOfEquations);
    /// @endcode
    ///
    /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
    /// @param              [in] aStartTime A time to begin integrating from
    /// @param              [in] aTimeArray A array of times to integrate to
    /// @param              [in] aSystemOfEquations A std::function wrapper with a particular signature that
    /// boost::odeint accepts to perform numerical integration
    /// @return             std::vector<std::vector<double>>

    Array<Solution> integrateTime(
        const StateVector& anInitialStateVector,
        const Real& aStartTime,
        const Array<Real>& aTimeArray,
        const SystemOfEquationsWrapper& aSystemOfEquations
    );

    /// @brief              Perform numerical integration from a start time to an end time
    ///
    /// @code
    ///                     Solution solution = numericalSolver.integrateTime(stateVector, startTime, endTime,
    ///                     systemOfEquations);
    /// @endcode
    /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
    /// @param              [in] aStartTime A time to begin integrating from
    /// @param              [in] anEndTime An time to integrate to
    /// @param              [in] aSystemOfEquations A std::function wrapper with a particular signature that
    /// boost::odeint accepts to perform numerical integration
    /// @return             std::vector<double>

    Solution integrateTime(
        const StateVector& anInitialStateVector,
        const Real& aStartTime,
        const Real& anEndTime,
        const SystemOfEquationsWrapper& aSystemOfEquations
    );

    /// @brief              Perform numerical integration for a certain duration
    ///
    /// @code
    ///                     Solution solution = numericalsolver.integrateTime(stateVector, durationSeconds,
    ///                     SystemofEquations) ;
    /// @endcode
    /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
    /// @param              [in] aDurationInSeconds A duration over which to integrate
    /// @param              [in] aSystemOfEquations A std::function wrapper with a particular signature that
    ///                              boost::odeint accepts to perform numerical integration
    /// @return             std::vector<double>

    Solution integrateDuration(
        const StateVector& anInitialStateVector,
        const Real& aDurationInSeconds,
        const SystemOfEquationsWrapper& aSystemOfEquations
    );

    /// @brief              Perform numerical integration for an array of durations
    ///
    /// @code
    ///                     Array<Solution> solutions = numericalsolver.integrateTime(stateVector, durationArray,
    ///                     SystemofEquations);
    /// @endcode
    /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
    /// @param              [in] aDurationArray An array of durations over which to integrate
    /// @param              [in] aSystemOfEquations A std::function wrapper with a particular signature that
    ///                              boost::odeint accepts to perform numerical integration
    /// @return             std::vector<double>

    Array<Solution> integrateDuration(
        const StateVector& anInitialStateVector,
        const Array<Real>& aDurationArray,
        const SystemOfEquationsWrapper& aSystemOfEquations
    );

    /// @brief              Perform numerical integration for a certain duration
    ///
    /// @code
    ///                     StateVector stateVector = numericalsolver.integrateStateFromInstantToInstant(stateVector,
    ///                     instant, otherInstant, SystemofEquations) ;
    /// @endcode
    /// @param              [in] anInitialStateVector An initial n-dimensional state vector to begin integrating at
    /// @param              [in] anIntegrationDuration A duration over which to integration
    /// @param              [in] aSystemOfEquations An std::function wrapper with a particular signature that
    /// boost::odeint accepts to perform numerical integration
    /// @return             std::vector<double>

    template <typename ConditionFunction>
    StateVector integrateTimes(
        const StateVector& anInitialStateVector,
        const Real& aStartTime,
        const Real& anEndTime,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const ConditionFunction& anEventCondition
    );

    template <typename ConditionFunction>
    StateVector integrateDurations(
        const StateVector& anInitialStateVector,
        const Real& aDurationInSeconds,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const ConditionFunction& anEventCondition
    );

    /// @brief              Get string from the integration stepper type
    ///
    /// @code
    ///                     NumericalSolver::StringFromStepperType(aStepperType) ;
    /// @endcode
    /// @param              [in] aStepperType An integration stepper type enum
    /// @return             StepperType

    static String StringFromStepperType(const NumericalSolver::StepperType& aStepperType);

    /// @brief              Get string from the integration log type
    ///
    /// @code
    ///                     NumericalSolver::StringFromLogType(aLogType) ;
    /// @endcode
    /// @param              [in] aLogType An integration log type enum
    /// @return             LogType

    static String StringFromLogType(const NumericalSolver::LogType& aLogType);

    /// @brief              Undefined
    ///
    /// @return             An undefined numerical solver

    static NumericalSolver Undefined();

    /// @brief              Default
    ///
    /// @return             A default numerical solver

    static NumericalSolver Default();

   private:
    NumericalSolver::LogType logType_;
    NumericalSolver::StepperType stepperType_;
    Real timeStep_;
    Real relativeTolerance_;
    Real absoluteTolerance_;
    std::vector<Solution> observedStates_;

    void observeNumericalIntegration(const StateVector& x, const double t);

    double getSignedTimeStep(const Real& aReal) const;
};

}  // namespace astro
}  // namespace ostk

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.tpp>

#endif
