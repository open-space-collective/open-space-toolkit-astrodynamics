/// Apache License 2.0

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
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

using ostk::core::container::Pair;

using ostk::mathematics::curvefitting::Interpolator;

using ostk::physics::time::Duration;

using ostk::astrodynamics::RootSolver;
using ostk::astrodynamics::trajectory::model::Tabulated;
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
        RootFindingStrategy::Propagated,
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
        case RootFindingStrategy::LinearInterpolation:
            return "LinearInterpolation";
        case RootFindingStrategy::Skip:
            return "Skip";
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

/// @brief Integrate to a target time with a stepper
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

/// @brief Integrate to a target times with a stepper
template <typename Stepper, typename System>
inline Array<NumericalSolver::Solution> integrateToTimes(
    Stepper& stepper, NumericalSolver::StateVector stateVector, double startTime, double endTime, const System& system
)
{
    const int numberOfSteps = 20;
    const double stepSize = endTime / numberOfSteps;
    const VectorXd durations = VectorXd::LinSpaced(numberOfSteps, startTime, endTime);

    Array<NumericalSolver::Solution> stateVectors = Array<NumericalSolver::Solution>::Empty();
    stateVectors.reserve(numberOfSteps);
    const auto observer = [&stateVectors](const VectorXd& aStateVector, const double& aTime) -> void
    {
        stateVectors.add(NumericalSolver::Solution(aStateVector, aTime));
    };
    integrate_times(stepper, system, stateVector, durations.begin(), durations.end(), stepSize, observer);

    return stateVectors;
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
    observedStates = {aState};

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

    // Remove the state that triggered the condition (we'll find the exact crossing)
    observedStates.pop_back();

    if (!conditionSatisfied)
    {
        const double finalTime = static_cast<double>(aDurationInSeconds);

        if (currentTime != finalTime)
        {
            // Compute step size with correct sign for the direction from currentTime to finalTime.
            // This handles the case where we overshot finalTime and need to integrate backwards.
            const double adjustmentStepSize = (finalTime - currentTime) / 10.0;
            integrateToTime<Stepper>(
                stepper, currentStateVector, currentTime, finalTime, adjustmentStepSize, aSystemOfEquations
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

    std::function<NumericalSolver::StateVector(const double&)> stateGenerator;

    // Handle root finding based on strategy
    switch (rootFindingStrategy)
    {
        case NumericalSolver::RootFindingStrategy::Skip:
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

        case NumericalSolver::RootFindingStrategy::LinearInterpolation:
        {
            stateGenerator = [&previousStateVector, &currentStateVector, previousTime, currentTime](
                                 const double& targetTime
                             ) -> NumericalSolver::StateVector
            {
                const double alpha = (targetTime - previousTime) / (currentTime - previousTime);
                return previousStateVector * (1.0 - alpha) + currentStateVector * alpha;
            };

            break;
        }

        case NumericalSolver::RootFindingStrategy::Propagated:
        {
            stateGenerator = [&stepper,
                              &aSystemOfEquations,
                              &anEventCondition,
                              &createState,
                              &previousStateVector,
                              &previousTime,
                              &previousState](const double& targetTime) -> NumericalSolver::StateVector
            {
                NumericalSolver::StateVector stateVectorAtTargetTime = previousStateVector;
                const double subStepSize = (targetTime - previousTime) / 10.0;

                integrateToTime<Stepper>(
                    stepper, stateVectorAtTargetTime, previousTime, targetTime, subStepSize, aSystemOfEquations
                );

                return stateVectorAtTargetTime;
            };

            break;
        }

        case NumericalSolver::RootFindingStrategy::CubicInterpolation:
        {
            const Array<NumericalSolver::Solution> stateVectors =
                integrateToTimes<Stepper>(stepper, previousStateVector, previousTime, currentTime, aSystemOfEquations);
            const Array<State> states = stateVectors.map<State>(
                [&createState](const NumericalSolver::Solution& aSolution) -> State
                {
                    return createState(aSolution.first, aSolution.second);
                }
            );

            const Tabulated tabulated = Tabulated(states, Interpolator::Type::CubicSpline);

            stateGenerator = [tabulated, &aState](const double& targetTime) -> NumericalSolver::StateVector
            {
                const Instant instant = aState.accessInstant() + Duration::Seconds(targetTime);
                return tabulated.calculateStateAt(instant).accessCoordinates();
            };

            break;
        }

        default:
            throw ostk::core::error::runtime::Wrong("Root Finding Strategy");
    }

    // Since previousState gets updated in the stepping loop, we need to reset it here
    previousState = createState(previousStateVector, previousTime);

    const auto checkCondition = [&anEventCondition, &createState, &previousState, &stateGenerator](const double& aTime
                                ) -> double
    {
        const NumericalSolver::StateVector stateCoordinates = stateGenerator(aTime);
        const State interpolatedState = createState(stateCoordinates, aTime);
        const bool isSatisfied = anEventCondition.isSatisfied(interpolatedState, previousState);
        return isSatisfied ? 1.0 : -1.0;
    };

    // Condition at previousTime => False
    // Condition at currentTime => True
    // Search for the exact time of the condition change
    const RootSolver::Solution solution = rootSolver.bisection(checkCondition, previousTime, currentTime);

    // Ensure that the solution time has crossed the condition
    const double solutionTime = (signedTimeStep > 0.0) ? solution.upperBound : solution.lowerBound;
    const double subStepSize = (solutionTime - previousTime) / 10.0;
    NumericalSolver::StateVector solutionStateVector = previousStateVector;

    integrateToTime<Stepper>(stepper, solutionStateVector, previousTime, solutionTime, subStepSize, aSystemOfEquations);

    const State solutionState = createState(solutionStateVector, solutionTime);

    // If the solution state is not the same as the initial state, add it to the observed states
    if (solutionState.accessInstant() != aState.accessInstant())
    {
        observeState(solutionState);
    }

    return {
        solutionState,
        true,
        solution.iterationCount,
        solution.hasConverged,
    };
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
