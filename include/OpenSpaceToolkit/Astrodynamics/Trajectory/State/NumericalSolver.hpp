/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_StateNumericalSolver__
#define __OpenSpaceToolkit_Astrodynamics_StateNumericalSolver__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Solver/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

using ostk::core::container::Array;

using ostk::physics::time::Instant;

using ostk::astrodynamics::RootSolver;
using ostk::astrodynamics::trajectory::State;
using MathNumericalSolver = ostk::mathematics::solver::NumericalSolver;

/// @brief Define an astrodynamics state contextual Numerical Solver. This class inherits from the OSTk Mathematics
/// Numerical Solver.
class NumericalSolver : public MathNumericalSolver
{
   public:
    /// @brief Structure to hold the condition solution.
    struct ConditionSolution
    {
        State state;                  ///< Final state after integration.
        bool conditionIsSatisfied;    ///< Whether the condition is met.
        Size iterationCount;          ///< Number of iterations performed.
        bool rootSolverHasConverged;  ///< Whether the root solver has converged.
    };

    /// @brief Strategy for finding the exact event crossing time during conditional integration.
    ///
    /// - DenseOutput: Use dense stepper's calc_state() for interpolation (RungeKuttaDopri5 only). Most accurate.
    /// - Linear: Linear interpolation between step endpoints. Fast but less accurate for nonlinear dynamics.
    /// - Propagated: Re-integrate with smaller sub-steps during bisection. Accurate but slower.
    /// - Boundary: Return the first step boundary where condition is satisfied. Simplest, no refinement.
    enum class RootFindingStrategy
    {
        DenseOutput,
        Linear,
        Propagated,
        Boundary
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  NumericalSolver numericalSolver = { aLogType, aStepperType, aTimeStep,
    ///                  aRelativeTolerance, anAbsoluteTolerance };
    /// @endcode
    ///
    /// @param aLogType An enum indicating the amount of verbosity wanted to be logged during
    ///                  numerical integration
    /// @param aStepperType An enum indicating the type of numerical stepper used to perform
    ///                  integration
    /// @param aTimeStep A number indicating the initial guess time step the numerical solver will
    ///                  take
    /// @param aRelativeTolerance A number indicating the relative integration tolerance
    /// @param anAbsoluteTolerance A number indicating the absolute integration tolerance
    /// @param aRootSolver A root solver to be used to solve the event condition
    /// @param aRootFindingStrategy Strategy for finding exact event crossing time. Defaults to
    ///                  DenseOutput (requires RungeKuttaDopri5)
    NumericalSolver(
        const NumericalSolver::LogType& aLogType,
        const NumericalSolver::StepperType& aStepperType,
        const Real& aTimeStep,
        const Real& aRelativeTolerance,
        const Real& anAbsoluteTolerance,
        const RootSolver& aRootSolver = RootSolver::Default(),
        const RootFindingStrategy& aRootFindingStrategy = RootFindingStrategy::DenseOutput
    );

    /// @brief Access observed states
    ///
    /// @code{.cpp}
    ///                  numericalSolver.accessObservedStates();
    /// @endcode
    ///
    /// @return Observed states
    const Array<State>& accessObservedStates() const;

    /// @brief Get root solver
    ///
    /// @code{.cpp}
    ///                  numericalSolver.getRootSolver();
    /// @endcode
    ///
    /// @return RootSolver
    RootSolver getRootSolver() const;

    /// @brief Get observed states
    ///
    /// @code{.cpp}
    ///                  numericalSolver.getObservedStates();
    /// @endcode
    ///
    /// @return Observed states
    Array<State> getObservedStates() const;

    /// @brief Get root finding strategy
    ///
    /// @code{.cpp}
    ///                  numericalSolver.getRootFindingStrategy();
    /// @endcode
    ///
    /// @return RootFindingStrategy
    RootFindingStrategy getRootFindingStrategy() const;

    /// @brief Perform numerical integration for a given array of time instants.
    ///
    /// @param aState Initial state for integration.
    /// @param aTimeArray Array of time instants.
    /// @param aSystemOfEquations System of equations to integrate.
    /// @return Array of states for each time instant.
    Array<State> integrateTime(
        const State& aState, const Array<Instant>& aTimeArray, const SystemOfEquationsWrapper& aSystemOfEquations
    );

    /// @brief Perform numerical integration from a start time to an end time.
    ///
    /// @param aState Initial state for integration.
    /// @param anInstant Time to integrate to.
    /// @param aSystemOfEquations System of equations to integrate.
    /// @return Final state after integration.
    State integrateTime(
        const State& aState, const Instant& anInstant, const SystemOfEquationsWrapper& aSystemOfEquations
    );

    /// @brief Perform numerical integration from a start time until either a condition or an end time
    /// is reached.
    ///
    /// @param aState Initial state for integration.
    /// @param anInstant Maximum time to integrate to.
    /// @param aSystemOfEquations System of equations to integrate.
    /// @param anEventCondition Condition to be checked.
    /// @return Structure containing the final state and condition-related information.
    ConditionSolution integrateTime(
        const State& aState,
        const Instant& anInstant,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const EventCondition& anEventCondition
    );

    /// @brief Undefined
    ///
    /// @return An undefined numerical solver
    static NumericalSolver Undefined();

    /// @brief Default
    ///
    /// @return A default numerical solver
    static NumericalSolver Default();

    /// @brief Create a fixed step size numerical solver.
    ///
    /// @param aTimeStep The time step to use for integration.
    /// @param aSystemOfEquations System of equations to integrate.
    ///
    /// @return A fixed step size numerical solver.
    static NumericalSolver FixedStepSize(const NumericalSolver::StepperType& aStepperType, const Real& aTimeStep);

    /// @brief Default conditional
    ///
    /// @param stateLogger A function that takes a `State` object and logs. Defaults to `nullptr`.
    /// @param aRootFindingStrategy Strategy for finding exact event crossing time. Defaults to DenseOutput.
    /// @return A default conditional numerical solver.
    static NumericalSolver DefaultConditional(
        const std::function<void(const State&)>& stateLogger = nullptr,
        const RootFindingStrategy& aRootFindingStrategy = RootFindingStrategy::DenseOutput
    );

    /// @brief Create a conditional numerical solver.
    ///
    /// @param aTimeStep The initial time step to use.
    /// @param aRelativeTolerance The relative tolerance to use.
    /// @param anAbsoluteTolerance The absolute tolerance to use.
    /// @param stateLogger A function that takes a `State` object and logs.
    /// @param aRootFindingStrategy Strategy for finding exact event crossing time. Defaults to DenseOutput.
    ///
    /// @return A conditional numerical solver.
    static NumericalSolver Conditional(
        const Real& aTimeStep,
        const Real& aRelativeTolerance,
        const Real& anAbsoluteTolerance,
        const std::function<void(const State&)>& stateLogger = nullptr,
        const RootFindingStrategy& aRootFindingStrategy = RootFindingStrategy::DenseOutput
    );

    /// @brief Convert RootFindingStrategy to string
    ///
    /// @code{.cpp}
    ///                  NumericalSolver::StringFromRootFindingStrategy(aStrategy);
    /// @endcode
    ///
    /// @param aStrategy A root finding strategy enum
    /// @return String representation
    static String StringFromRootFindingStrategy(const RootFindingStrategy& aStrategy);

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
    RootFindingStrategy rootFindingStrategy_;

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  NumericalSolver numericalSolver = { aLogType, aStepperType, aTimeStep,
    ///                  aRelativeTolerance, anAbsoluteTolerance };
    /// @endcode
    ///
    /// @param aLogType An enum indicating the amount of verbosity wanted to be logged during
    ///                  numerical integration
    /// @param aStepperType An enum indicating the type of numerical stepper used to perform
    ///                  integration
    /// @param aTimeStep A number indicating the initial guess time step the numerical solver will
    ///                  take
    /// @param aRelativeTolerance A number indicating the relative integration tolerance
    /// @param anAbsoluteTolerance A number indicating the absolute integration tolerance
    /// @param aRootSolver A root solver to be used to solve the event condition
    /// @param stateLogger A function that takes a `State` object and logs
    /// @param aRootFindingStrategy Strategy for finding exact event crossing time
    NumericalSolver(
        const NumericalSolver::LogType& aLogType,
        const NumericalSolver::StepperType& aStepperType,
        const Real& aTimeStep,
        const Real& aRelativeTolerance,
        const Real& anAbsoluteTolerance,
        const RootSolver& aRootSolver,
        const std::function<void(const State&)>& stateLogger,
        const RootFindingStrategy& aRootFindingStrategy
    );

    void observeState(const State& aState);

    /// @brief Integrate with dense output stepper (RKDP5 only)
    ConditionSolution integrateTimeWithDenseOutput(
        const State& aState,
        const Instant& anInstant,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const EventCondition& anEventCondition
    );

    /// @brief Integrate with controlled stepper using Linear/Propagated/Boundary strategy
    ConditionSolution integrateTimeWithControlledStepper(
        const State& aState,
        const Instant& anInstant,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const EventCondition& anEventCondition
    );
};

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
