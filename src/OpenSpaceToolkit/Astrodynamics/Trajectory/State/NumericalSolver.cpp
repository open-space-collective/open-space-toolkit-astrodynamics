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

NumericalSolver::ConditionSolution NumericalSolver::integrateTimeWithControlledStepper(
    const State& aState,
    const Instant& anInstant,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition
)
{
    const StateBuilder stateBuilder = {aState};

    const Real aDurationInSeconds = (anInstant - aState.accessInstant()).inSeconds();

    const auto createState = [&stateBuilder, &aState](const VectorXd& aStateVector, const double& aTime) -> State
    {
        return stateBuilder.build(aState.accessInstant() + Duration::Seconds(aTime), aStateVector);
    };

    // Ensure that the time step is the correct sign
    const double signedTimeStep = getSignedTimeStep(aDurationInSeconds);

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

    NumericalSolver::StateVector currentStateVector = aState.accessCoordinates();
    NumericalSolver::StateVector previousStateVector = aState.accessCoordinates();
    double previousTime = 0.0;
    double currentTime = 0.0;
    double dt = signedTimeStep;
    State currentState = State::Undefined();
    State previousState = aState;
    bool conditionSatisfied = false;

    // Manual stepping loop - integrate step-by-step to allow early termination
    switch (stepperType_)
    {
        case StepperType::RungeKutta4:
        {
            stepper_type_4 stepper;
            while (checkTimeLimit(currentTime) && !conditionSatisfied)
            {
                previousStateVector = currentStateVector;
                previousTime = currentTime;

                stepper.do_step(aSystemOfEquations, currentStateVector, currentTime, dt);
                currentTime += dt;

                currentState = createState(currentStateVector, currentTime);
                observeState(currentState);
                conditionSatisfied = anEventCondition.isSatisfied(currentState, previousState);
                previousState = currentState;
            }
            break;
        }
        case StepperType::RungeKuttaCashKarp54:
        {
            auto stepper = make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54());
            while (checkTimeLimit(currentTime) && !conditionSatisfied)
            {
                previousStateVector = currentStateVector;
                previousTime = currentTime;

                // Controlled steppers may reject steps, keep trying until step is accepted
                while (stepper.try_step(aSystemOfEquations, currentStateVector, currentTime, dt) ==
                       boost::numeric::odeint::controlled_step_result::fail)
                {
                }

                currentState = createState(currentStateVector, currentTime);
                observeState(currentState);
                conditionSatisfied = anEventCondition.isSatisfied(currentState, previousState);
                previousState = currentState;
            }
            break;
        }
        case StepperType::RungeKuttaFehlberg78:
        {
            auto stepper = make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78());
            while (checkTimeLimit(currentTime) && !conditionSatisfied)
            {
                previousStateVector = currentStateVector;
                previousTime = currentTime;

                // Controlled steppers may reject steps, keep trying until step is accepted
                while (stepper.try_step(aSystemOfEquations, currentStateVector, currentTime, dt) ==
                       boost::numeric::odeint::controlled_step_result::fail)
                {
                }

                currentState = createState(currentStateVector, currentTime);
                observeState(currentState);
                conditionSatisfied = anEventCondition.isSatisfied(currentState, previousState);
                previousState = currentState;
            }
            break;
        }
        case StepperType::RungeKuttaDopri5:
        {
            auto stepper = make_controlled(absoluteTolerance_, relativeTolerance_, dense_stepper_type_5());
            while (checkTimeLimit(currentTime) && !conditionSatisfied)
            {
                previousStateVector = currentStateVector;
                previousTime = currentTime;

                // Controlled steppers may reject steps, keep trying until step is accepted
                while (stepper.try_step(aSystemOfEquations, currentStateVector, currentTime, dt) ==
                       boost::numeric::odeint::controlled_step_result::fail)
                {
                }

                currentState = createState(currentStateVector, currentTime);
                observeState(currentState);
                conditionSatisfied = anEventCondition.isSatisfied(currentState, previousState);
                previousState = currentState;
            }
            break;
        }
        default:
            throw ostk::core::error::runtime::Wrong("Stepper type");
    }

    if (!conditionSatisfied)
    {
        // Need to integrate to exact end time
        const double finalTime = static_cast<double>(aDurationInSeconds);
        if (currentTime != finalTime)
        {
            // Integrate from current position to end
            switch (stepperType_)
            {
                case StepperType::RungeKutta4:
                {
                    integrate_const(
                        stepper_type_4(), aSystemOfEquations, currentStateVector, currentTime, finalTime, signedTimeStep
                    );
                    break;
                }
                case StepperType::RungeKuttaCashKarp54:
                {
                    integrate_adaptive(
                        make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()),
                        aSystemOfEquations,
                        currentStateVector,
                        currentTime,
                        finalTime,
                        signedTimeStep
                    );
                    break;
                }
                case StepperType::RungeKuttaFehlberg78:
                {
                    integrate_adaptive(
                        make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()),
                        aSystemOfEquations,
                        currentStateVector,
                        currentTime,
                        finalTime,
                        signedTimeStep
                    );
                    break;
                }
                case StepperType::RungeKuttaDopri5:
                {
                    integrate_adaptive(
                        make_controlled(absoluteTolerance_, relativeTolerance_, dense_stepper_type_5()),
                        aSystemOfEquations,
                        currentStateVector,
                        currentTime,
                        finalTime,
                        signedTimeStep
                    );
                    break;
                }
                default:
                    break;
            }
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
    observedStates_.pop_back();

    // Handle root finding based on strategy
    switch (rootFindingStrategy_)
    {
        case RootFindingStrategy::Boundary:
        {
            // Return the current state where condition was first satisfied
            const State solutionState = createState(currentStateVector, currentTime);
            observeState(solutionState);
            return {
                solutionState,
                true,
                0,
                true,
            };
        }

        case RootFindingStrategy::Linear:
        {
            // Linear interpolation for root finding
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

            const RootSolver::Solution solution =
                rootSolver_.bisection(checkConditionLinear, previousTime, currentTime);
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

        case RootFindingStrategy::Propagated:
        {
            // Re-integrate from previousState to find exact crossing
            const auto checkConditionPropagated = [&](const double& targetTime) -> double
            {
                NumericalSolver::StateVector tempStateVector = previousStateVector;

                // Integrate from previousTime to targetTime
                switch (stepperType_)
                {
                    case StepperType::RungeKutta4:
                    {
                        integrate_const(
                            stepper_type_4(),
                            aSystemOfEquations,
                            tempStateVector,
                            previousTime,
                            targetTime,
                            (targetTime - previousTime) / 10.0
                        );
                        break;
                    }
                    case StepperType::RungeKuttaCashKarp54:
                    {
                        integrate_adaptive(
                            make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()),
                            aSystemOfEquations,
                            tempStateVector,
                            previousTime,
                            targetTime,
                            (targetTime - previousTime) / 10.0
                        );
                        break;
                    }
                    case StepperType::RungeKuttaFehlberg78:
                    {
                        integrate_adaptive(
                            make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()),
                            aSystemOfEquations,
                            tempStateVector,
                            previousTime,
                            targetTime,
                            (targetTime - previousTime) / 10.0
                        );
                        break;
                    }
                    case StepperType::RungeKuttaDopri5:
                    {
                        integrate_adaptive(
                            make_controlled(absoluteTolerance_, relativeTolerance_, dense_stepper_type_5()),
                            aSystemOfEquations,
                            tempStateVector,
                            previousTime,
                            targetTime,
                            (targetTime - previousTime) / 10.0
                        );
                        break;
                    }
                    default:
                        throw ostk::core::error::runtime::Wrong("Stepper type");
                }

                const State propagatedState = createState(tempStateVector, targetTime);
                const bool isSatisfied =
                    anEventCondition.isSatisfied(propagatedState, createState(previousStateVector, previousTime));

                return isSatisfied ? 1.0 : -1.0;
            };

            const RootSolver::Solution solution =
                rootSolver_.bisection(checkConditionPropagated, previousTime, currentTime);

            // Final integration to the solution time
            const double solutionTime = static_cast<double>(solution.root);
            const double subStepSize = (solutionTime - previousTime) / 10.0;
            NumericalSolver::StateVector solutionStateVector = previousStateVector;
            switch (stepperType_)
            {
                case StepperType::RungeKutta4:
                {
                    integrate_const(
                        stepper_type_4(),
                        aSystemOfEquations,
                        solutionStateVector,
                        previousTime,
                        solutionTime,
                        subStepSize
                    );
                    break;
                }
                case StepperType::RungeKuttaCashKarp54:
                {
                    integrate_adaptive(
                        make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()),
                        aSystemOfEquations,
                        solutionStateVector,
                        previousTime,
                        solutionTime,
                        subStepSize
                    );
                    break;
                }
                case StepperType::RungeKuttaFehlberg78:
                {
                    integrate_adaptive(
                        make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()),
                        aSystemOfEquations,
                        solutionStateVector,
                        previousTime,
                        solutionTime,
                        subStepSize
                    );
                    break;
                }
                case StepperType::RungeKuttaDopri5:
                {
                    integrate_adaptive(
                        make_controlled(absoluteTolerance_, relativeTolerance_, dense_stepper_type_5()),
                        aSystemOfEquations,
                        solutionStateVector,
                        previousTime,
                        solutionTime,
                        subStepSize
                    );
                    break;
                }
                default:
                    throw ostk::core::error::runtime::Wrong("Stepper type");
            }

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

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
