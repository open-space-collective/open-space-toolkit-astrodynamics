/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_StateNumericalSolver__
#define __OpenSpaceToolkit_Astrodynamics_StateNumericalSolver__

#include <optional>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>

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
    /// @param aTimeStep A number indicating the initial guess time step (in seconds) the numerical solver will
    ///                  take
    /// @param aRelativeTolerance A number indicating the relative integration tolerance
    /// @param anAbsoluteTolerance A number indicating the absolute integration tolerance
    /// @param aRootSolver A root solver to be used to solve the event condition
    /// @param aStateLogger A function called with each observed state, when logging is enabled. Defaults to none.
    NumericalSolver(
        const NumericalSolver::LogType& aLogType,
        const NumericalSolver::StepperType& aStepperType,
        double aTimeStep,
        double aRelativeTolerance,
        double anAbsoluteTolerance,
        const RootSolver& aRootSolver = RootSolver::Default(),
        const std::function<void(const State&)>& aStateLogger = nullptr
    );

    /// @brief Get root solver
    ///
    /// @code{.cpp}
    ///                  numericalSolver.getRootSolver();
    /// @endcode
    ///
    /// @return RootSolver
    const RootSolver& getRootSolver() const;

    /// @brief Get observed states
    ///
    /// @code{.cpp}
    ///                  numericalSolver.getObservedStates();
    /// @endcode
    ///
    /// @return Observed states
    const Array<State>& getObservedStates() const;

    /// @brief Get the maximum step size
    ///
    /// @return Maximum step size in seconds, or std::nullopt if not set
    std::optional<double> getMaxStepSize() const;

    /// @brief Set the maximum step size the integrator is allowed to take during conditional
    ///        integration. Useful for detecting discrete/step-function events (e.g., thrust
    ///        toggles) that can be missed when the adaptive stepper grows its step too large.
    ///
    /// @param aMaxStepSize Maximum step size in seconds. Use std::nullopt to remove the
    ///                     limit.
    void setMaxStepSize(const std::optional<double>& aMaxStepSize);

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
    /// @details The observed states are cleared before the integration, unless they are to be appended: the integration
    /// is then a continuation of the previous one. The observed states beyond the initial state (in the direction of
    /// the integration) are discarded, the initial state is observed unless it already is the last observed state, and
    /// the states observed by this integration are appended.
    ///
    /// @param aState Initial state for integration.
    /// @param anInstant Time to integrate to.
    /// @param aSystemOfEquations System of equations to integrate.
    /// @param appendObservedStates (optional) If true, append to the observed states, otherwise clear them. Defaults
    /// to false.
    /// @return Final state after integration.
    State integrateTime(
        const State& aState,
        const Instant& anInstant,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const bool& appendObservedStates = false
    );

    /// @brief Perform numerical integration from a start time until either a condition or an end time
    /// is reached.
    ///
    /// @details The observed states are cleared before the integration, unless they are to be appended: the integration
    /// is then a continuation of the previous one. The observed states beyond the initial state (in the direction of
    /// the integration) are discarded, the initial state is observed unless it already is the last observed state, and
    /// the states observed by this integration are appended.
    ///
    /// @param aState Initial state for integration.
    /// @param anInstant Maximum time to integrate to.
    /// @param aSystemOfEquations System of equations to integrate.
    /// @param anEventCondition Condition to be checked.
    /// @param appendObservedStates (optional) If true, append to the observed states, otherwise clear them. Defaults
    /// to false.
    /// @return Structure containing the final state and condition-related information.
    ConditionSolution integrateTime(
        const State& aState,
        const Instant& anInstant,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const EventCondition& anEventCondition,
        const bool& appendObservedStates = false
    );

    /// @brief Construct a default numerical solver.
    ///
    /// @code{.cpp}
    ///     NumericalSolver solver = NumericalSolver::Default();
    /// @endcode
    ///
    /// @return A default numerical solver.
    static NumericalSolver Default();

    /// @brief Create a fixed step size numerical solver.
    ///
    /// @code{.cpp}
    ///     NumericalSolver solver = NumericalSolver::FixedStepSize(
    ///         NumericalSolver::StepperType::RungeKutta4, 30.0
    ///     );
    /// @endcode
    ///
    /// @param aTimeStep The time step (in seconds) to use for integration.
    /// @param aSystemOfEquations System of equations to integrate.
    ///
    /// @return A fixed step size numerical solver.
    static NumericalSolver FixedStepSize(const NumericalSolver::StepperType& aStepperType, double aTimeStep);

    /// Delete undesired methods from parent
    Array<MathNumericalSolver::Solution> integrateTime(
        const MathNumericalSolver::StateVector& anInitialStateVector,
        double aStartTime,
        const Array<double>& aTimeArray,
        const SystemOfEquationsWrapper& aSystemOfEquations
    ) = delete;

    MathNumericalSolver::Solution integrateTime(
        const MathNumericalSolver::StateVector& anInitialStateVector,
        double aStartTime,
        double anEndTime,
        const SystemOfEquationsWrapper& aSystemOfEquations
    ) = delete;

    MathNumericalSolver::Solution integrateDuration(
        const MathNumericalSolver::StateVector& anInitialStateVector,
        double aDurationInSeconds,
        const SystemOfEquationsWrapper& aSystemOfEquations
    ) = delete;

    Array<MathNumericalSolver::Solution> integrateDuration(
        const MathNumericalSolver::StateVector& anInitialStateVector,
        const Array<double>& aDurationArray,
        const SystemOfEquationsWrapper& aSystemOfEquations
    ) = delete;

    const Array<MathNumericalSolver::Solution>& getObservedStateVectors() const = delete;

   private:
    RootSolver rootSolver_;
    Array<State> observedStates_;
    std::function<void(const State&)> stateLogger_;
    std::optional<double> maxStepSize_ = std::nullopt;

    /// @brief Observe a state, storing it's coordinates in the observedStates_ array
    ///
    /// @param aState The state to observe
    void observeState(const State& aState);

    /// @brief Resume the observed states from a state, to continue a previous integration.
    ///
    /// @details Discards the observed states beyond the state (in the direction of the integration), and observes the
    /// state unless it already is the last observed state.
    ///
    /// @param aState The initial state of the integration
    /// @param anInstant The instant the integration goes to
    void resumeObservedStates(const State& aState, const Instant& anInstant);

    /// @brief Integrate with controlled stepper using the unified dense-output refinement.
    ///
    /// @param aState The initial state for integration
    /// @param anInstant The instant to integrate to
    /// @param aSystemOfEquations The system of equations to integrate
    /// @param anEventCondition The event condition to check
    /// @param appendObservedStates True to append to the observed states, false to clear them
    /// @return The condition solution
    ConditionSolution integrateTimeWithControlledStepper(
        const State& aState,
        const Instant& anInstant,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const EventCondition& anEventCondition,
        const bool& appendObservedStates
    );

    /// @brief Templated implementation of conditional integration. Defined out-of-line in
    ///        the .cpp so the boost odeint stepper types do not leak into this header.
    ///
    /// @param aStepper The stepper instance used for the main integration loop.
    /// @param aState Initial state.
    /// @param anInstant Maximum time to integrate to.
    /// @param aSystemOfEquations System of equations to integrate.
    /// @param anEventCondition Condition to detect.
    /// @param aSignedTimeStep Signed initial step size (negative for backward integration).
    /// @param appendObservedStates True to append to the observed states, false to clear them.
    template <typename Stepper>
    ConditionSolution integrateTimeWithStepperImpl_(
        Stepper& aStepper,
        const State& aState,
        const Instant& anInstant,
        const SystemOfEquationsWrapper& aSystemOfEquations,
        const EventCondition& anEventCondition,
        double aSignedTimeStep,
        const bool& appendObservedStates
    );
};

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
