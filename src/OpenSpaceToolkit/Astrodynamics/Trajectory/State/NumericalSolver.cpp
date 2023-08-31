/// Apache License 2.0

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

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
      observedStates_()
{
}

NumericalSolver* NumericalSolver::clone() const
{
    return new NumericalSolver(*this);
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
    for (const auto& stateVector : solutions)
    {
        const State state = {
            aState.accessInstant() + Duration::Seconds(stateVector.second),
            stateVector.first,
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
    observedStates_.clear();

    const NumericalSolver::Solution solution = MathNumericalSolver::integrateDuration(
        aState.accessCoordinates(), (anEndTime - aState.accessInstant()).inSeconds(), aSystemOfEquations
    );

    for (const auto& state : MathNumericalSolver::getObservedStateVectors())
    {
        observedStates_.add({
            aState.accessInstant() + Duration::Seconds(state.second),
            state.first,
            aState.accessFrame(),
            aState.accessCoordinatesBroker(),
        });
    }

    return {
        aState.accessInstant() + Duration::Seconds(solution.second),
        solution.first,
        aState.accessFrame(),
        aState.accessCoordinatesBroker(),
    };
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

    observedStates_.clear();

    const Real aDurationInSeconds = (anInstant - aState.accessInstant()).inSeconds();

    if (aDurationInSeconds.isZero())
    {
        return {
            aState,
            false,
            0,
            false,
        };
    }

    NumericalSolver::StateVector aStateVector = aState.accessCoordinates();

    // Ensure that the time step is the correct sign
    const double signedTimeStep = getSignedTimeStep(aDurationInSeconds);

    // TBI: Adapt this to any dense stepper type
    auto stepper = make_dense_output(absoluteTolerance_, relativeTolerance_, dense_stepper_type_5());

    // initialize stepper
    double currentTime = 0.0;
    stepper.initialize(aStateVector, currentTime, signedTimeStep);

    // do first step
    double previousTime;
    std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
    observedStates_.add({
        aState.accessInstant() + Duration::Seconds(stepper.current_time()),
        stepper.current_state(),
        aState.accessFrame(),
        aState.accessCoordinatesBroker(),
    });

    bool conditionSatisfied = false;
    NumericalSolver::StateVector currentState;

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

    while (checkTimeLimit(currentTime))
    {
        std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
        currentState = stepper.current_state();

        conditionSatisfied =
            anEventCondition.isSatisfied(currentState, currentTime, stepper.previous_state(), previousTime);

        if (conditionSatisfied)
        {
            break;
        }

        observedStates_.add({
            aState.accessInstant() + Duration::Seconds(currentTime),
            currentState,
            aState.accessFrame(),
            aState.accessCoordinatesBroker(),
        });
    }

    if (!conditionSatisfied)
    {
        stepper.calc_state(aDurationInSeconds, currentState);

        return {
            {
                aState.accessInstant() + Duration::Seconds(aDurationInSeconds),
                currentState,
                aState.accessFrame(),
                aState.accessCoordinatesBroker(),
            },
            false,
            0,
            false,
        };
    }

    const auto checkCondition = [&anEventCondition, &stepper](const double& aTime) -> double
    {
        NumericalSolver::StateVector state(stepper.current_state());
        stepper.calc_state(aTime, state);

        const bool isSatisfied =
            anEventCondition.isSatisfied(state, aTime, stepper.previous_state(), stepper.previous_time());
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
    NumericalSolver::StateVector solutionState(currentState.size());
    const double solutionTime = solution.root;

    stepper.calc_state(solutionTime, solutionState);
    observedStates_.add({
        aState.accessInstant() + Duration::Seconds(solutionTime),
        solutionState,
        aState.accessFrame(),
        aState.accessCoordinatesBroker(),
    });

    return {
        {
            aState.accessInstant() + Duration::Seconds(solutionTime),
            solutionState,
            aState.accessFrame(),
            aState.accessCoordinatesBroker(),
        },
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
    };
}

NumericalSolver NumericalSolver::DefaultConditional()
{
    return {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        5.0,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
    };
}

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
