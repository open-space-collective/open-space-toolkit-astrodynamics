/// Apache License 2.0

#include <boost/math/tools/roots.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solvers/TemporalConditionSolver.hpp>

namespace ostk
{
namespace astro
{
namespace solvers
{

TemporalConditionSolver::TemporalConditionSolver(
    const Duration& aTimeStep, const Duration& aTolerance, const Size& aMaximumIterationCount
)
    : timeStep_(aTimeStep),
      tolerance_(aTolerance),
      maximumIterationCount_(aMaximumIterationCount)
{
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

    bool inCondition = false;
    Instant conditionStartCache = Instant::Undefined();
    Instant previousInstantCache = Instant::Undefined();

    for (const auto& instant : instants)
    {
        const bool conditionIsTrue = TemporalConditionSolver::EvaluateConditionAt(instant, aConditionArray);

        // If this is the first iteration
        if (!previousInstantCache.isDefined())
        {
            inCondition = conditionIsTrue;
            if (conditionIsTrue)
            {
                conditionStartCache = instant;
            }
        }
        else
        {
            const bool conditionIsSwitching = (conditionIsTrue != inCondition);

            if (conditionIsSwitching)
            {
                const Instant switchingInstant =
                    this->findSwitchingInstant(previousInstantCache, instant, !conditionIsTrue, aConditionArray);

                if (conditionIsTrue)
                {
                    conditionStartCache = switchingInstant;
                }
                else
                {
                    intervals.add(Interval::Closed(conditionStartCache, switchingInstant));
                    conditionStartCache = Instant::Undefined();
                }

                inCondition = conditionIsTrue;
            }
        }

        previousInstantCache = instant;
    }

    // Add interval if condition is true on the last iteration
    if (inCondition)
    {
        intervals.add(Interval::Closed(conditionStartCache, instants.accessLast()));
    }

    return intervals;
}

// Instant TemporalConditionSolver::findSwitchingInstant(
//     const Instant& aPreviousInstant,
//     const Instant& aNextInstant,
//     const bool isConditionTrueAtPreviousInstant,
//     const Array<TemporalConditionSolver::Condition>& aConditionArray
// ) const
// {
//     const Duration step = Duration::Between(aPreviousInstant, aNextInstant);

//     if (step <= this->tolerance_)
//     {
//         return aNextInstant;
//     }

//     const Instant midInstant = aPreviousInstant + (step / 2.0);

//     const bool conditionIsTrueInMidInstant = TemporalConditionSolver::EvaluateConditionAt(midInstant,
//     aConditionArray);

//     if (isConditionTrueAtPreviousInstant != conditionIsTrueInMidInstant)
//     {
//         return this->findSwitchingInstant(
//             aPreviousInstant, midInstant, isConditionTrueAtPreviousInstant, aConditionArray
//         );
//     }

//     return this->findSwitchingInstant(midInstant, aNextInstant, isConditionTrueAtPreviousInstant, aConditionArray);
// }

// Instant TemporalConditionSolver::findSwitchingInstant(
//     const Instant& aPreviousInstant,
//     const Instant& aNextInstant,
//     const bool isConditionTrueAtPreviousInstant,
//     const Array<TemporalConditionSolver::Condition>& aConditionArray
// ) const
// {
//     const RootSolver rootSolver = RootSolver(this->maximumIterationCount_, this->tolerance_.inSeconds());

//     const auto result = rootSolver.bisection(
//         [&aPreviousInstant, &aConditionArray](double aDurationInSeconds) -> double {
//             return TemporalConditionSolver::EvaluateConditionAt(aPreviousInstant +
//             Duration::Seconds(aDurationInSeconds), aConditionArray) ? +1.0 : -1.0;
//         },
//         0.0,
//         Duration::Between(aPreviousInstant, aNextInstant).inSeconds()
//     ) ;

//     return aPreviousInstant + Duration::Seconds(result.root);
// }

Instant TemporalConditionSolver::findSwitchingInstant(
    const Instant& aPreviousInstant,
    const Instant& aNextInstant,
    const bool isConditionTrueAtPreviousInstant,
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

}  // namespace solvers
}  // namespace astro
}  // namespace ostk
