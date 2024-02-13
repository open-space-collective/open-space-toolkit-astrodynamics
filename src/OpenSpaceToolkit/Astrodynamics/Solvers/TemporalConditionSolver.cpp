/// Apache License 2.0

#include <boost/math/tools/roots.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/TemporalConditionSolver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace solver
{

TemporalConditionSolver::TemporalConditionSolver(
    const Duration& aTimeStep, const Duration& aTolerance, const Size& aMaximumIterationCount
)
    : timeStep_(aTimeStep),
      tolerance_(aTolerance),
      maximumIterationCount_(aMaximumIterationCount)
{
}

Duration TemporalConditionSolver::getTimeStep() const
{
    return this->timeStep_;
}

Duration TemporalConditionSolver::getTolerance() const
{
    return this->tolerance_;
}

Size TemporalConditionSolver::getMaximumIterationCount() const
{
    return this->maximumIterationCount_;
}

Array<Interval> TemporalConditionSolver::solve(
    const TemporalConditionSolver::Condition& aCondition, const Interval& anInterval
) const
{
    return this->solve(Array<TemporalConditionSolver::Condition>({aCondition}), anInterval);
}

Array<Interval> TemporalConditionSolver::solve(
    const Array<TemporalConditionSolver::Condition>& aConditionArray, const Interval& anInterval
) const
{
    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    Array<Interval> intervals = Array<Interval>::Empty();

    using ostk::physics::time::Scale;

    const Array<Instant> instants = anInterval.generateGrid(timeStep_);

    bool conditionIsMetCache = false;
    Instant conditionStartInstantCache = Instant::Undefined();
    Instant previousInstantCache = Instant::Undefined();

    for (const auto& instant : instants)
    {
        const bool conditionIsMet = TemporalConditionSolver::EvaluateConditionAt(instant, aConditionArray);

        // If this is the first iteration
        if (!previousInstantCache.isDefined())
        {
            conditionIsMetCache = conditionIsMet;
            if (conditionIsMet)
            {
                conditionStartInstantCache = instant;
            }
        }
        else
        {
            const bool conditionIsSwitching = (conditionIsMet != conditionIsMetCache);

            if (conditionIsSwitching)
            {
                const Instant switchingInstant =
                    this->findSwitchingInstant(previousInstantCache, instant, aConditionArray);

                if (conditionIsMet)
                {
                    conditionStartInstantCache = switchingInstant;
                }
                else
                {
                    intervals.add(Interval::Closed(conditionStartInstantCache, switchingInstant));
                    conditionStartInstantCache = Instant::Undefined();
                }

                conditionIsMetCache = conditionIsMet;
            }
        }

        previousInstantCache = instant;
    }

    // Add interval if condition is met on the last iteration
    if (conditionIsMetCache)
    {
        intervals.add(Interval::Closed(conditionStartInstantCache, instants.accessLast()));
    }

    return intervals;
}

Instant TemporalConditionSolver::findSwitchingInstant(
    const Instant& aPreviousInstant,
    const Instant& aNextInstant,
    const Array<TemporalConditionSolver::Condition>& aConditionArray
) const
{
    const RootSolver rootSolver = RootSolver(this->maximumIterationCount_, this->tolerance_.inSeconds());

    const auto result = rootSolver.solve(
        [&aPreviousInstant, &aConditionArray](double aDurationInSeconds) -> double
        {
            return TemporalConditionSolver::EvaluateConditionAt(
                       aPreviousInstant + Duration::Seconds(aDurationInSeconds), aConditionArray
                   )
                     ? +1.0
                     : -1.0;
        },
        0.0,
        Duration::Between(aPreviousInstant, aNextInstant).inSeconds()
    );

    return aPreviousInstant + Duration::Seconds(result.root);
}

bool TemporalConditionSolver::EvaluateConditionAt(
    const Instant& anInstant, const Array<TemporalConditionSolver::Condition>& aConditionArray
)
{
    return std::all_of(
        aConditionArray.begin(),
        aConditionArray.end(),
        [&anInstant](const TemporalConditionSolver::Condition& aCondition)
        {
            return aCondition(anInstant);
        }
    );
}

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk
