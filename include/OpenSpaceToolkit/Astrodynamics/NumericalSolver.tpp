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
    if (aDurationInSeconds.isZero())
    {
        return {anInitialStateVector, 0.0};
    }

    NumericalSolver::StateVector aStateVector = anInitialStateVector;

    // Ensure that the time step is the correct sign
    const double signedTimeStep = getSignedTimeStep(aDurationInSeconds);

    // TBI: Adapt this to any dense stepper type
    auto stepper =
        make_dense_output(absoluteTolerance_, relativeTolerance_, runge_kutta_dopri5<NumericalSolver::StateVector>());

    // initialize stepper
    double currentTime = 0.0;
    stepper.initialize(aStateVector, currentTime, signedTimeStep);

    // do first step
    double previousTime;
    std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
    observeNumericalIntegration(stepper.current_state(), stepper.current_time());

    bool conditionSatisfied = false;
    Real currentValue = Real::Undefined();
    Real previousValue = aCondition.evaluate(stepper.current_state(), stepper.current_time());
    NumericalSolver::StateVector currentState;

    // account for integration direction
    const auto checkTimeLimit = [&currentTime, &aDurationInSeconds]()
    {
        if (aDurationInSeconds > 0.0)
        {
            return currentTime < aDurationInSeconds;
        }
        return currentTime > aDurationInSeconds;
    };

    while (checkTimeLimit())
    {
        std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
        currentState = stepper.current_state();

        currentValue = aCondition.evaluate(currentState, currentTime);

        conditionSatisfied = aCondition(currentValue, previousValue);

        if (conditionSatisfied)
        {
            break;
        }

        observeNumericalIntegration(currentState, currentTime);

        previousValue = currentValue;
    }

    // TBI: Share information upstream on if the condition was satisfied
    if (!conditionSatisfied)
    {
        stepper.calc_state(aDurationInSeconds, currentState);
        return {currentState, aDurationInSeconds};
    }

    // Search for the exact time of the condition change
    NumericalSolver::StateVector midState(currentState);
    double midTime;

    // TBI: Make this a parameter
    const Index maxIterationCount = 100;

    for (Index iterationCount = 0; iterationCount < maxIterationCount; ++iterationCount)
    {
        midTime = 0.5 * (previousTime + currentTime);
        stepper.calc_state(midTime, midState);

        const Real midValue = aCondition.evaluate(midState, midTime);

        if (aCondition(midValue, previousValue))
        {
            // root lies between previousTime and midTime
            // update current -> mid
            currentTime = midTime;
            currentValue = midValue;
        }
        else
        {
            // root lies between midTime and currentTime
            // update previous -> mid
            previousTime = midTime;  // condition changer lies after midpoint
            previousValue = midValue;
        }

        // TBI: Make tolerance a parameter
        if (std::abs(midValue) < 1e-6)
        {
            break;
        }
    }

    // TBI: Share information upstream on the number of iterations + success
    // if (iterationCount == maxIterationCount)
    // {
    //   do thing
    // }

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
    NumericalSolver::Solution solution =
        integrateDuration(anInitialStateVector, anEndTime - aStartTime, aSystemOfEquations, aCondition);
    solution.second += aStartTime;

    return solution;
}

}  // namespace astro
}  // namespace ostk
