/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_StateNumericalSolver__
#define __OpenSpaceToolkit_Astrodynamics_StateNumericalSolver__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Solvers/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{

using ostk::core::ctnr::Array;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::State;
using ostk::astro::RootSolver;
using MathNumericalSolver = ostk::math::solvers::NumericalSolver;

/// @brief                      Defines an astrodynamics state contextual Numerical Solver. This class inherits from the
///                             OSTk Mathematics Numerical Solver.

class NumericalSolver : public MathNumericalSolver
{
   public:
    /// @brief                  Structure to hold the condition solution.

    struct ConditionSolution
    {
        State state;                  ///< Final state after integration.
        bool conditionIsSatisfied;    ///< Whether the condition is met.
        Size iterationCount;          ///< Number of iterations performed.
        bool rootSolverHasConverged;  ///< Whether the root solver has converged.
    };

    /// @brief                  Constructor
    ///
    /// @code
    ///                         NumericalSolver numericalSolver = { aLogType, aStepperType, aTimeStep,
    ///                         aRelativeTolerance, anAbsoluteTolerance };
    /// @endcode
    ///
    /// @param                  [in] aLogType An enum indicating the amount of verbosity wanted to be logged during
    ///                         numerical integration
    /// @param                  [in] aStepperType An enum indicating the type of numerical stepper used to perform
    ///                         integration
    /// @param                  [in] aTimeStep A number indicating the initial guess time step the numerical solver will
    ///                         take
    /// @param                  [in] aRelativeTolerance A number indicating the relative integration tolerance
    /// @param                  [in] anAbsoluteTolerance A number indicating the absolute integration tolerance
    /// @param                  [in] aRootSolver A root solver to be used to solve the event condition

    NumericalSolver(
        const NumericalSolver::LogType& aLogType,
        const NumericalSolver::StepperType& aStepperType,
        const Real& aTimeStep,
        const Real& aRelativeTolerance,
        const Real& anAbsoluteTolerance,
        const RootSolver& aRootSolver = RootSolver::Default()
    );

    /// @brief                  Access observed states
    ///
    /// @code
    ///                         numericalSolver.accessObservedStates();
    /// @endcode
    ///
    /// @return                 Observed states

    const Array<State>& accessObservedStates() const;

    /// @brief                  Get root solver
    ///
    /// @code
    ///                         numericalSolver.getRootSolver();
    /// @endcode
    ///
    /// @return                 RootSolver

    RootSolver getRootSolver() const;

    /// @brief                  Get observed states
    ///
    /// @code
    ///                         numericalSolver.getObservedStates();
    /// @endcode
    ///
    /// @return                 Observed states

    Array<State> getObservedStates() const;

    /// @brief                  Perform numerical integration for a given array of time instants.
    ///
    /// @param                  [in] aState Initial state for integration.
    /// @param                  [in] aTimeArray Array of time instants.
    /// @param                  [in] aSystemOfEquations System of equations to integrate.
    /// @return                 Array of states for each time instant.

    Array<State> integrateTime(
        const State& aState, const Array<Instant>& aTimeArray, const SystemOfEquationsWrapper& aSystemOfEquations
    );

    /// @brief                  Perform numerical integration from a start time to an end time.
    ///
    /// @param                  [in] aState Initial state for integration.
    /// @param                  [in] anInstant Time to integrate to.
    /// @param                  [in] aSystemOfEquations System of equations to integrate.
    /// @return                 Final state after integration.

    State integrateTime(
        const State& aState, const Instant& anInstant, const SystemOfEquationsWrapper& aSystemOfEquations
    );

    /// @brief                  Perform numerical integration from a start time until either a condition or an end time
    /// is reached.
    ///
    /// @param                  [in] aState Initial state for integration.
    /// @param                  [in] anInstant Maximum time to integrate to.
    /// @param                  [in] aSystemOfEquations System of equations to integrate.
    /// @param                  [in] anEventCondition Condition to be checked.
    /// @return                 Structure containing the final state and condition-related information.

    ConditionSolution integrateTime(
        const State& aState,
        const Instant& anInstant,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const EventCondition& anEventCondition
    );

    /// @brief                  Undefined
    ///
    /// @return                 An undefined numerical solver

    static NumericalSolver Undefined();

    /// @brief                  Default
    ///
    /// @return                 A default numerical solver

    static NumericalSolver Default();

    /// @brief                  Create a fixed step size numerical solver.
    ///
    /// @param                  [in] aTimeStep The time step to use for integration.
    /// @param                  [in] aSystemOfEquations System of equations to integrate.
    ///
    /// @return                 A fixed step size numerical solver.

    static NumericalSolver FixedStepSize(const NumericalSolver::StepperType& aStepperType, const Real& aTimeStep);

    /// @brief                  Default conditional
    ///
    /// @param                  [in] stateLogger A function that takes a `State` object and logs. Defaults to `nullptr`.
    /// @return                 A default conditional numerical solver.

    static NumericalSolver DefaultConditional(const std::function<void(const State&)>& stateLogger = nullptr);

    /// @brief Create a conditional numerical solver.
    ///
    /// @param                  [in] aTimeStep The initial time step to use.
    /// @param                  [in] aRelativeTolerance The relative tolerance to use.
    /// @param                  [in] anAbsoluteTolerance The absolute tolerance to use.
    /// @param                  [in] stateLogger A function that takes a `State` object and logs.
    ///
    /// @return                 A conditional numerical solver.

    static NumericalSolver Conditional(
        const Real& aTimeStep,
        const Real& aRelativeTolerance,
        const Real& anAbsoluteTolerance,
        const std::function<void(const State&)>& stateLogger
    );

    /// Delete undesired methods from parent

    Array<MathNumericalSolver::Solution> integrateTime(
        const MathNumericalSolver::StateVector& anInitialStateVector,
        const Real& aStartTime,
        const Array<Real>& aTimeArray,
        const SystemOfEquationsWrapper& aSystemOfEquations
    ) = delete;

    MathNumericalSolver::Solution integrateTime(
        const MathNumericalSolver::StateVector& anInitialStateVector,
        const Real& aStartTime,
        const Real& anEndTime,
        const SystemOfEquationsWrapper& aSystemOfEquations
    ) = delete;

    MathNumericalSolver::Solution integrateDuration(
        const MathNumericalSolver::StateVector& anInitialStateVector,
        const Real& aDurationInSeconds,
        const SystemOfEquationsWrapper& aSystemOfEquations
    ) = delete;

    Array<MathNumericalSolver::Solution> integrateDuration(
        const MathNumericalSolver::StateVector& anInitialStateVector,
        const Array<Real>& aDurationArray,
        const SystemOfEquationsWrapper& aSystemOfEquations
    ) = delete;

    Array<MathNumericalSolver::Solution> getObservedStateVectors() const = delete;

    const Array<MathNumericalSolver::Solution>& accessObservedStateVectors() const = delete;

   private:
    RootSolver rootSolver_;
    Array<State> observedStates_;
    std::function<void(const State&)> stateLogger_;

    /// @brief                  Constructor
    ///
    /// @code
    ///                         NumericalSolver numericalSolver = { aLogType, aStepperType, aTimeStep,
    ///                         aRelativeTolerance, anAbsoluteTolerance };
    /// @endcode
    ///
    /// @param                  [in] aLogType An enum indicating the amount of verbosity wanted to be logged during
    ///                         numerical integration
    /// @param                  [in] aStepperType An enum indicating the type of numerical stepper used to perform
    ///                         integration
    /// @param                  [in] aTimeStep A number indicating the initial guess time step the numerical solver will
    ///                         take
    /// @param                  [in] aRelativeTolerance A number indicating the relative integration tolerance
    /// @param                  [in] anAbsoluteTolerance A number indicating the absolute integration tolerance
    /// @param                  [in] aRootSolver A root solver to be used to solve the event condition
    /// @param                  [in] stateLogger A function that takes a `State` object and logs

    NumericalSolver(
        const NumericalSolver::LogType& aLogType,
        const NumericalSolver::StepperType& aStepperType,
        const Real& aTimeStep,
        const Real& aRelativeTolerance,
        const Real& anAbsoluteTolerance,
        const RootSolver& aRootSolver,
        const std::function<void(const State&)>& stateLogger
    );

    void observeState(const State& aState);
};

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
