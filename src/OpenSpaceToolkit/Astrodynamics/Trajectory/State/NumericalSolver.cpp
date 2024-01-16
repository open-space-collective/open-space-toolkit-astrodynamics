/// Apache License 2.0

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{

using namespace boost::numeric::odeint;

using ostk::physics::time::Duration;

using ostk::astro::RootSolver;
using ostk::astro::trajectory::StateBuilder;

typedef runge_kutta_dopri5<NumericalSolver::StateVector> dense_stepper_type_5;

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const RootSolver& aRootSolver
)
    : MathNumericalSolver(aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance),
      rootSolver_(aRootSolver),
      observedStates_(),
      stateLogger_(nullptr)
{
}

const Array<State>& NumericalSolver::accessObservedStates() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return observedStates_;
}

RootSolver NumericalSolver::getRootSolver() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return this->rootSolver_;
}

Array<State> NumericalSolver::getObservedStates() const
{
    return accessObservedStates();
}

Array<State> NumericalSolver::integrateTime(
    const State& aState,
    const Array<Instant>& anInstantArray,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    const Array<Real> durationArray = anInstantArray.map<Real>(
        [&aState](const Instant& anInstant) -> Real
        {
            return (anInstant - aState.accessInstant()).inSeconds();
        }
    );

    const Array<NumericalSolver::Solution> solutions =
        MathNumericalSolver::integrateDuration(aState.accessCoordinates(), durationArray, aSystemOfEquations);

    Array<State> states;
    states.reserve(solutions.getSize());
    for (Index i = 0; i < solutions.getSize(); ++i)
    {
        const NumericalSolver::Solution& solution = solutions.at(i);
        const State state = {
            anInstantArray[i],
            solution.first,
            aState.accessFrame(),
            aState.accessCoordinatesBroker(),
        };
        states.add(state);
    }

    return states;
}

State NumericalSolver::integrateTime(
    const State& aState, const Instant& anEndTime, const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    observedStates_ = {aState};

    const StateBuilder stateBuilder = {aState};

    const NumericalSolver::Solution solution = MathNumericalSolver::integrateDuration(
        aState.accessCoordinates(), (anEndTime - aState.accessInstant()).inSeconds(), aSystemOfEquations
    );

    for (const auto& state : MathNumericalSolver::getObservedStateVectors())
    {
        observedStates_.add(stateBuilder.build(aState.accessInstant() + Duration::Seconds(state.second), state.first));
    }

    return stateBuilder.build(anEndTime, solution.first);
}

NumericalSolver::ConditionSolution NumericalSolver::integrateTime(
    const State& aState,
    const Instant& anInstant,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition
)
{
    if (stepperType_ != NumericalSolver::StepperType::RungeKuttaDopri5)
    {
        throw ostk::core::error::runtime::ToBeImplemented(
            "Integrating with conditions is only supported with RungeKuttaDopri5 stepper type."
        );
    }

    observedStates_ = {aState};

    const StateBuilder stateBuilder = {aState};

    const Real aDurationInSeconds = (anInstant - aState.accessInstant()).inSeconds();

    const auto createState = [&stateBuilder, &aState](const VectorXd& aStateVector, const double& aTime) -> State
    {
        return stateBuilder.build(aState.accessInstant() + Duration::Seconds(aTime), aStateVector);
    };

    if (aDurationInSeconds.isZero())
    {
        return {
            aState,
            false,
            0,
            false,
        };
    }

    // Ensure that the time step is the correct sign
    const double signedTimeStep = getSignedTimeStep(aDurationInSeconds);

    // TBI: Adapt this to any dense stepper type
    auto stepper = make_dense_output(absoluteTolerance_, relativeTolerance_, dense_stepper_type_5());

    // initialize stepper
    double currentTime = 0.0;
    stepper.initialize(aState.accessCoordinates(), currentTime, signedTimeStep);

    // do first step
    double previousTime;
    std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);

    State previousState = createState(stepper.current_state(), stepper.current_time());
    observeState(previousState);

    bool conditionSatisfied = false;

    // account for integration direction
    std::function<bool(const double&)> checkTimeLimit;
    if (aDurationInSeconds > 0.0)
    {
        checkTimeLimit = [&aDurationInSeconds](const double& aTime) -> bool
        {
            return aTime < aDurationInSeconds;
        };
    }
    else
    {
        checkTimeLimit = [&aDurationInSeconds](const double& aTime) -> bool
        {
            return aTime > aDurationInSeconds;
        };
    }

    State currentState = State::Undefined();

    while (checkTimeLimit(currentTime))
    {
        std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
        currentState = createState(stepper.current_state(), currentTime);

        conditionSatisfied = anEventCondition.isSatisfied(currentState, previousState);

        if (conditionSatisfied)
        {
            break;
        }

        observeState(currentState);
        previousState = currentState;
    }

    if (!conditionSatisfied)
    {
        NumericalSolver::StateVector currentStateVector(stepper.current_state());
        stepper.calc_state(aDurationInSeconds, currentStateVector);

        return {
            createState(currentStateVector, aDurationInSeconds),
            false,
            0,
            false,
        };
    }

    const auto checkCondition = [&anEventCondition, &stepper, &createState](const double& aTime) -> double
    {
        NumericalSolver::StateVector stateVector(stepper.current_state());
        stepper.calc_state(aTime, stateVector);

        const bool isSatisfied = anEventCondition.isSatisfied(
            createState(stateVector, aTime), createState(stepper.previous_state(), stepper.previous_time())
        );

        return isSatisfied ? 1.0 : -1.0;
    };

    // Condition at previousTime => True
    // Condition at currentTime => True
    // Initial state satisfies the condition, return the initial state
    if (checkCondition(previousTime) == checkCondition(currentTime))
    {
        return {
            aState,
            true,
            0,
            true,
        };
    }

    // Condition at previousTime => False
    // Condition at currentTime => True
    // Search for the exact time of the condition change
    const RootSolver::Solution solution = rootSolver_.bisection(checkCondition, previousTime, currentTime);
    NumericalSolver::StateVector solutionStateVector(aState.accessCoordinates().size());
    const double solutionTime = solution.root;

    stepper.calc_state(solutionTime, solutionStateVector);
    const State solutionState = createState(solutionStateVector, solutionTime);
    observeState(solutionState);

    return {
        solutionState,
        true,
        solution.iterationCount,
        solution.hasConverged,
    };
}

NumericalSolver NumericalSolver::Undefined()
{
    return {
        LogType::NoLog,
        StepperType::RungeKuttaCashKarp54,
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        RootSolver::Default(),
        nullptr,
    };
}

NumericalSolver NumericalSolver::Default()
{
    return {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
        nullptr,
    };
}

NumericalSolver NumericalSolver::FixedStepSize(const NumericalSolver::StepperType& aStepperType, const Real& aTimeStep)
{
    if (aStepperType != NumericalSolver::StepperType::RungeKutta4)
    {
        throw ostk::core::error::RuntimeError("Fixed step size is only supported with RungeKutta4 stepper type.");
    }

    return {
        NumericalSolver::LogType::NoLog,
        aStepperType,
        aTimeStep,
        1.0,
        1.0,
        RootSolver::Default(),
        nullptr,
    };
}

NumericalSolver NumericalSolver::DefaultConditional(const std::function<void(const State&)>& stateLogger)
{
    return NumericalSolver::Conditional(5.0, 1.0e-12, 1.0e-12, stateLogger);
}

NumericalSolver NumericalSolver::Conditional(
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const std::function<void(const State&)>& stateLogger
)
{
    const NumericalSolver::LogType logType =
        stateLogger != nullptr ? NumericalSolver::LogType::LogAdaptive : NumericalSolver::LogType::NoLog;

    return {
        logType,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        aTimeStep,
        aRelativeTolerance,
        anAbsoluteTolerance,
        RootSolver::Default(),
        stateLogger,
    };
}

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const RootSolver& aRootSolver,
    const std::function<void(const State& aState)>& stateLogger
)
    : MathNumericalSolver(aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance),
      rootSolver_(aRootSolver),
      observedStates_(),
      stateLogger_(stateLogger)
{
}

void NumericalSolver::observeState(const State& aState)
{
    observedStates_.add(aState);

    if (stateLogger_ != nullptr && getLogType() != NumericalSolver::LogType::NoLog)
    {
        stateLogger_(aState);
    }
}

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
