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
NumericalSolver::StateVector NumericalSolver::integrateDurations(
    const StateVector& anInitialStateVector,
    const Real& aDurationInSeconds,
    const SystemOfEquationsWrapper& aSystemOfEquations,
    const ConditionFunction& aCondition
)
{
    NumericalSolver::StateVector aStateVector = anInitialStateVector;

    // Ensure that the time step is the correct sign
    Integer durationSign = (aDurationInSeconds > 0.0) - (aDurationInSeconds < 0.0);
    const double adjustedTimeStep = timeStep_ * static_cast<double>(durationSign);

    // TBI: Adapt this to any dense stepper type
    auto stepper =
        make_dense_output(absoluteTolerance_, relativeTolerance_, runge_kutta_dopri5<NumericalSolver::StateVector>());

    // initialize stepper
    double t = 0.0;
    stepper.initialize(aStateVector, t, adjustedTimeStep);

    bool conditionSatisfied = false;
    Index count = 0;
    Real previousValue = Real::Undefined();

    while (t < aDurationInSeconds && conditionSatisfied)
    {
        observeNumericalIntegration(stepper.current_state(), stepper.current_time());

        stepper.do_step(aSystemOfEquations);

        const Real currentValue = aCondition.evaluate(stepper.current_state(), stepper.current_time());

        if (count > 0)
        {
            conditionSatisfied = aCondition(currentValue, previousValue);
        }

        // update time, previous value and count
        t = stepper.current_time();
        previousValue = currentValue;
        ++count;
    }

    if (!conditionSatisfied)
    {
        return stepper.current_state();
    }

    // do bisection method
    double previousTime = stepper.previous_time();
    double nextTime = stepper.current_time();

    double midTime;
    NumericalSolver::StateVector midState(stepper.current_state());
    previousValue = aCondition.evaluate(stepper.previous_state(), stepper.previous_time());

    while (std::abs(nextTime - previousTime) > 1e-6)
    {
        midTime = 0.5 * (previousTime + nextTime);
        stepper.calc_state(midTime, midState);

        const Real currentValue = aCondition.evaluate(midState, midTime);

        if (aCondition(currentValue, previousValue))
        {
            nextTime = midTime;  // condition changer lies before midpoint
        }
        else
        {
            previousTime = midTime;  // condition changer lies after midpoint
        }

        previousValue = currentValue;
    }

    // we found the interval of size eps, take it's midpoint as final guess
    midTime = 0.5 * (previousTime + nextTime);
    stepper.calc_state(midTime, midState);
    observeNumericalIntegration(midState, midTime);

    return midState;
}

template <typename ConditionFunction>
NumericalSolver::StateVector NumericalSolver::integrateTimes(
    const NumericalSolver::StateVector& anInitialStateVector,
    const Real& aStartTime,
    const Real& anEndTime,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const ConditionFunction& aCondition
)
{
    return integrateDurations(anInitialStateVector, anEndTime - aStartTime, aSystemOfEquations, aCondition);
}

}  // namespace astro
}  // namespace ostk
