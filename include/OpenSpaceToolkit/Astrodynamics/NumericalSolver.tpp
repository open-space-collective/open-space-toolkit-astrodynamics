/// Apache License 2.0

#include <boost/numeric/odeint.hpp>

#include <OpenSpaceToolkit/Core/Types/Index.hpp>

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

namespace ostk
{
namespace astro
{

using namespace boost::numeric::odeint;

using ostk::core::types::Index;

template <typename ConditionFunction>
NumericalSolver::Solution NumericalSolver::integrateDuration(
    const StateVector& anInitialStateVector,
    const Real& aDurationInSeconds,
    const SystemOfEquationsWrapper& aSystemOfEquations,
    const ConditionFunction& aCondition
)
{
    NumericalSolver::StateVector aStateVector = anInitialStateVector;

    // Ensure that the time step is the correct sign
    const double signedTimeStep = getSignedTimeStep(aDurationInSeconds);

    // TBI: Adapt this to any dense stepper type
    auto stepper =
        make_dense_output(absoluteTolerance_, relativeTolerance_, runge_kutta_dopri5<NumericalSolver::StateVector>());

    // initialize stepper
    double previousTime = 0.0;
    double currentTime = 0.0;
    stepper.initialize(aStateVector, currentTime, signedTimeStep);

    // do first step
    std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
    observeNumericalIntegration(stepper.current_state(), stepper.current_time());

    bool conditionSatisfied = false;
    Real previousValue = aCondition.evaluate(stepper.current_state(), stepper.current_time());
    NumericalSolver::StateVector currentState;

    while (currentTime < aDurationInSeconds)
    {
        std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
        currentState = stepper.current_state();

        observeNumericalIntegration(currentState, currentTime);

        const Real currentValue = aCondition.evaluate(currentState, currentTime);

        conditionSatisfied = aCondition(currentValue, previousValue);

        if (conditionSatisfied)
        {
            break;
        }

        previousValue = currentValue;
    }

    if (!conditionSatisfied)
    {
        return {currentState, currentTime};
    }

    // do bisection method
    NumericalSolver::StateVector midState(stepper.current_state());

    while (std::abs(currentTime - previousTime) > 1e-6)
    {
        const double midTime = 0.5 * (previousTime + currentTime);
        stepper.calc_state(midTime, midState);

        const Real currentValue = aCondition.evaluate(midState, midTime);

        if (aCondition(currentValue, previousValue))
        {
            currentTime = midTime;  // condition changer lies before midpoint
        }
        else
        {
            previousTime = midTime;  // condition changer lies after midpoint
        }

        previousValue = currentValue;
    }

    // we found the interval of size eps, take it's midpoint as final guess
    const double midTime = 0.5 * (previousTime + currentTime);
    stepper.calc_state(midTime, midState);
    observeNumericalIntegration(midState, midTime);

    return {midState, midTime};
}

template <typename ConditionFunction>
NumericalSolver::Solution NumericalSolver::integrateTime(
    const NumericalSolver::StateVector& anInitialStateVector,
    const Real& aStartTime,
    const Real& anEndTime,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const ConditionFunction& aCondition
)
{
    return integrateDuration(anInitialStateVector, anEndTime - aStartTime, aSystemOfEquations, aCondition);
}

}  // namespace astro
}  // namespace ostk
