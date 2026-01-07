/// Apache License 2.0

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

using namespace boost::numeric::odeint;

using ostk::physics::time::Duration;

using ostk::astrodynamics::RootSolver;
using ostk::astrodynamics::trajectory::StateBuilder;

typedef runge_kutta4<NumericalSolver::StateVector> stepper_type_4;
typedef runge_kutta_cash_karp54<NumericalSolver::StateVector> error_stepper_type_54;
typedef runge_kutta_fehlberg78<NumericalSolver::StateVector> error_stepper_type_78;
typedef runge_kutta_dopri5<NumericalSolver::StateVector> dense_stepper_type_5;

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const RootSolver& aRootSolver
)
    : NumericalSolver(
          aLogType,
          aStepperType,
          aTimeStep,
          aRelativeTolerance,
          anAbsoluteTolerance,
          aRootSolver,
          RootFindingStrategy::Propagated
      )
{
}

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const RootSolver& aRootSolver,
    const RootFindingStrategy& aRootFindingStrategy
)
    : MathNumericalSolver(aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance),
      rootSolver_(aRootSolver),
      observedStates_(),
      stateLogger_(nullptr),
      rootFindingStrategy_(aRootFindingStrategy)
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

    return rootSolver_;
}

Array<State> NumericalSolver::getObservedStates() const
{
    return accessObservedStates();
}

NumericalSolver::RootFindingStrategy NumericalSolver::getRootFindingStrategy() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return rootFindingStrategy_;
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
            aState.accessCoordinateBroker(),
        };
        states.add(state);
    }

    return states;
}

State NumericalSolver::integrateTime(
    const State& aState, const Instant& anEndTime, const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    observedStates_ = {};

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
    observedStates_ = {aState};

    const Real aDurationInSeconds = (anInstant - aState.accessInstant()).inSeconds();

    // Check trivial cases
    if (aDurationInSeconds.isZero())
    {
        return {
            aState,
            false,
            0,
            false,
        };
    }

    bool conditionSatisfied = anEventCondition.isSatisfied(aState, aState);
    if (conditionSatisfied)
    {
        return {
            aState,
            true,
            0,
            true,
        };
    }

    return integrateTimeWithControlledStepper(aState, anInstant, aSystemOfEquations, anEventCondition);
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
        RootFindingStrategy::Propagated,
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
        RootFindingStrategy::Propagated,
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
        RootFindingStrategy::Boundary,  // RK4 doesn't support dense output, use Boundary as default
    };
}

NumericalSolver NumericalSolver::DefaultConditional(const std::function<void(const State&)>& stateLogger)
{
    return {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
        stateLogger,
        RootFindingStrategy::Propagated,
    };
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
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        aTimeStep,
        aRelativeTolerance,
        anAbsoluteTolerance,
        RootSolver::Default(),
        stateLogger,
        RootFindingStrategy::Propagated,
    };
}

String NumericalSolver::StringFromRootFindingStrategy(const RootFindingStrategy& aStrategy)
{
    switch (aStrategy)
    {
        case RootFindingStrategy::Propagated:
            return "Propagated";
        case RootFindingStrategy::Linear:
            return "Linear";
        case RootFindingStrategy::Boundary:
            return "Boundary";
        default:
            throw ostk::core::error::runtime::Wrong("Root Finding Strategy");
    }
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
    : NumericalSolver(
          aLogType,
          aStepperType,
          aTimeStep,
          aRelativeTolerance,
          anAbsoluteTolerance,
          aRootSolver,
          stateLogger,
          RootFindingStrategy::Propagated
      )
{
}

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const RootSolver& aRootSolver,
    const std::function<void(const State& aState)>& stateLogger,
    const RootFindingStrategy& aRootFindingStrategy
)
    : MathNumericalSolver(aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance),
      rootSolver_(aRootSolver),
      observedStates_(),
      stateLogger_(stateLogger),
      rootFindingStrategy_(aRootFindingStrategy)
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

namespace
{

/// @brief Type trait to detect fixed-step steppers (e.g., RK4)
template <typename Stepper>
struct IsFixedStepStepper : std::false_type
{
};

template <>
struct IsFixedStepStepper<stepper_type_4> : std::true_type
{
};

/// @brief Perform a single integration step with a fixed-step stepper
template <typename Stepper, typename System>
inline typename std::enable_if<IsFixedStepStepper<Stepper>::value>::type doStep(
    Stepper& stepper, const System& system, NumericalSolver::StateVector& stateVector, double& currentTime, double& dt
)
{
    stepper.do_step(system, stateVector, currentTime, dt);
    currentTime += dt;
}

/// @brief Perform a single integration step with a controlled stepper (retries until accepted)
template <typename Stepper, typename System>
inline typename std::enable_if<!IsFixedStepStepper<Stepper>::value>::type doStep(
    Stepper& stepper, const System& system, NumericalSolver::StateVector& stateVector, double& currentTime, double& dt
)
{
    while (stepper.try_step(system, stateVector, currentTime, dt) ==
           boost::numeric::odeint::controlled_step_result::fail)
    {
    }
}

/// @brief Integrate to a target time with a fixed-step stepper
template <typename Stepper, typename System>
inline void integrateToTime(
    Stepper& stepper,
    NumericalSolver::StateVector& stateVector,
    double startTime,
    double endTime,
    double stepSize,
    const System& system
)
{
    integrate_adaptive(stepper, system, stateVector, startTime, endTime, stepSize);
}

}  // namespace

/// @brief Templated implementation of conditional integration
template <typename Stepper>
NumericalSolver::ConditionSolution integrateTimeWithStepperImpl(
    Stepper& stepper,
    const State& aState,
    const Instant& anInstant,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition,
    double signedTimeStep,
    const RootSolver& rootSolver,
    NumericalSolver::RootFindingStrategy rootFindingStrategy,
    Array<State>& observedStates,
    const std::function<void(const State&)>& observeState
)
{
    const StateBuilder stateBuilder = {aState};
    const Real aDurationInSeconds = (anInstant - aState.accessInstant()).inSeconds();

    const auto createState = [&stateBuilder, &aState](const VectorXd& aStateVector, const double& aTime) -> State
    {
        return stateBuilder.build(aState.accessInstant() + Duration::Seconds(aTime), aStateVector);
    };

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

    NumericalSolver::StateVector currentStateVector = aState.accessCoordinates();
    NumericalSolver::StateVector previousStateVector = aState.accessCoordinates();
    double previousTime = 0.0;
    double currentTime = 0.0;
    double dt = signedTimeStep;
    State currentState = State::Undefined();
    State previousState = aState;
    bool conditionSatisfied = false;

    // Main stepping loop
    while (checkTimeLimit(currentTime) && !conditionSatisfied)
    {
        previousStateVector = currentStateVector;
        previousTime = currentTime;

        doStep(stepper, aSystemOfEquations, currentStateVector, currentTime, dt);

        currentState = createState(currentStateVector, currentTime);
        observeState(currentState);
        conditionSatisfied = anEventCondition.isSatisfied(currentState, previousState);
        previousState = currentState;
    }

    if (!conditionSatisfied)
    {
        const double finalTime = static_cast<double>(aDurationInSeconds);
        if (currentTime != finalTime)
        {
            integrateToTime<Stepper>(
                stepper, currentStateVector, currentTime, finalTime, signedTimeStep, aSystemOfEquations
            );
        }

        const State finalState = createState(currentStateVector, finalTime);
        observeState(finalState);
        return {
            finalState,
            false,
            0,
            false,
        };
    }

    // Remove the state that triggered the condition (we'll find the exact crossing)
    observedStates.pop_back();

    // Handle root finding based on strategy
    switch (rootFindingStrategy)
    {
        case NumericalSolver::RootFindingStrategy::Boundary:
        {
            const State solutionState = createState(currentStateVector, currentTime);
            observeState(solutionState);
            return {
                solutionState,
                true,
                0,
                true,
            };
        }

        case NumericalSolver::RootFindingStrategy::Linear:
        {
            const auto linearInterpolate = [&previousStateVector, &currentStateVector, previousTime, currentTime](
                                               const double& t
                                           ) -> NumericalSolver::StateVector
            {
                const double alpha = (t - previousTime) / (currentTime - previousTime);
                return previousStateVector * (1.0 - alpha) + currentStateVector * alpha;
            };

            const auto checkConditionLinear = [&](const double& aTime) -> double
            {
                const NumericalSolver::StateVector interpolatedCoords = linearInterpolate(aTime);
                const State interpolatedState = createState(interpolatedCoords, aTime);
                const bool isSatisfied =
                    anEventCondition.isSatisfied(interpolatedState, createState(previousStateVector, previousTime));
                return isSatisfied ? 1.0 : -1.0;
            };

            const RootSolver::Solution solution = rootSolver.bisection(checkConditionLinear, previousTime, currentTime);

            const NumericalSolver::StateVector solutionStateVector = linearInterpolate(solution.root);
            const State solutionState = createState(solutionStateVector, solution.root);
            observeState(solutionState);

            return {
                solutionState,
                true,
                solution.iterationCount,
                solution.hasConverged,
            };
        }

        case NumericalSolver::RootFindingStrategy::Propagated:
        {
            const auto checkConditionPropagated = [&](const double& targetTime) -> double
            {
                NumericalSolver::StateVector tempStateVector = previousStateVector;
                const double subStepSize = (targetTime - previousTime) / 10.0;

                integrateToTime<Stepper>(
                    stepper, tempStateVector, previousTime, targetTime, subStepSize, aSystemOfEquations
                );

                const State propagatedState = createState(tempStateVector, targetTime);
                const bool isSatisfied =
                    anEventCondition.isSatisfied(propagatedState, createState(previousStateVector, previousTime));
                return isSatisfied ? 1.0 : -1.0;
            };

            const RootSolver::Solution solution =
                rootSolver.bisection(checkConditionPropagated, previousTime, currentTime);

            const double solutionTime = static_cast<double>(solution.root);
            const double subStepSize = (solutionTime - previousTime) / 10.0;
            NumericalSolver::StateVector solutionStateVector = previousStateVector;

            integrateToTime<Stepper>(
                stepper, solutionStateVector, previousTime, solutionTime, subStepSize, aSystemOfEquations
            );

            const State solutionState = createState(solutionStateVector, solutionTime);
            observeState(solutionState);

            return {
                solutionState,
                true,
                solution.iterationCount,
                solution.hasConverged,
            };
        }

        default:
            throw ostk::core::error::runtime::Wrong("Root Finding Strategy");
    }
}

NumericalSolver::ConditionSolution NumericalSolver::integrateTimeWithControlledStepper(
    const State& aState,
    const Instant& anInstant,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition
)
{
    const auto observer = [this](const State& state)
    {
        this->observeState(state);
    };

    const Real aDurationInSeconds = (anInstant - aState.accessInstant()).inSeconds();
    const double signedTimeStep = getSignedTimeStep(aDurationInSeconds);

    switch (stepperType_)
    {
        case StepperType::RungeKutta4:
        {
            stepper_type_4 stepper;
            return integrateTimeWithStepperImpl<stepper_type_4>(
                stepper,
                aState,
                anInstant,
                aSystemOfEquations,
                anEventCondition,
                signedTimeStep,
                rootSolver_,
                rootFindingStrategy_,
                observedStates_,
                observer
            );
        }
        case StepperType::RungeKuttaCashKarp54:
        {
            auto stepper = make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54());
            return integrateTimeWithStepperImpl<decltype(stepper)>(
                stepper,
                aState,
                anInstant,
                aSystemOfEquations,
                anEventCondition,
                signedTimeStep,
                rootSolver_,
                rootFindingStrategy_,
                observedStates_,
                observer
            );
        }
        case StepperType::RungeKuttaFehlberg78:
        {
            auto stepper = make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78());
            return integrateTimeWithStepperImpl<decltype(stepper)>(
                stepper,
                aState,
                anInstant,
                aSystemOfEquations,
                anEventCondition,
                signedTimeStep,
                rootSolver_,
                rootFindingStrategy_,
                observedStates_,
                observer
            );
        }
        case StepperType::RungeKuttaDopri5:
        {
            auto stepper = make_controlled(absoluteTolerance_, relativeTolerance_, dense_stepper_type_5());
            return integrateTimeWithStepperImpl<decltype(stepper)>(
                stepper,
                aState,
                anInstant,
                aSystemOfEquations,
                anEventCondition,
                signedTimeStep,
                rootSolver_,
                rootFindingStrategy_,
                observedStates_,
                observer
            );
        }
        default:
            throw ostk::core::error::runtime::Wrong("Stepper type");
    }
}

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
