/// Apache License 2.0

#include <optional>

#include <boost/log/trivial.hpp>
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
    if (!timeStep_.isStrictlyPositive())
    {
        throw ostk::core::error::RuntimeError("Time step [{}] must be strictly positive.", timeStep_.toString());
    }

    if (!tolerance_.isStrictlyPositive())
    {
        throw ostk::core::error::RuntimeError("Tolerance [{}] must be strictly positive.", tolerance_.toString());
    }

    if (tolerance_ >= timeStep_)
    {
        throw ostk::core::error::RuntimeError(
            "Tolerance [{}] must be smaller than time step [{}].", tolerance_.toString(), timeStep_.toString()
        );
    }
}

const Duration& TemporalConditionSolver::getTimeStep() const
{
    return this->timeStep_;
}

const Duration& TemporalConditionSolver::getTolerance() const
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
    const Duration intervalDuration = anInterval.getDuration();

    if (timeStep_ > intervalDuration)
    {
        BOOST_LOG_TRIVIAL(warning) << "Time step [" << timeStep_.toString() << "] is larger than interval duration ["
                                   << intervalDuration.toString()
                                   << "]: condition switches within the interval may be missed.";
    }

    Array<Interval> intervals = Array<Interval>::Empty();

    using ostk::physics::time::Scale;

    const Array<Instant> instants = anInterval.generateGrid(timeStep_);

    bool conditionIsMetCache = false;
    std::optional<Instant> conditionStartInstantCache = std::nullopt;
    std::optional<Instant> previousInstantCache = std::nullopt;

    for (const auto& instant : instants)
    {
        const bool conditionIsMet = TemporalConditionSolver::EvaluateConditionAt(instant, aConditionArray);

        // If this is the first iteration
        if (!previousInstantCache.has_value())
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
                    this->findSwitchingInstant(previousInstantCache.value(), instant, aConditionArray);

                if (conditionIsMet)
                {
                    conditionStartInstantCache = switchingInstant;
                }
                else
                {
                    intervals.add(Interval::Closed(conditionStartInstantCache.value(), switchingInstant));
                    conditionStartInstantCache = std::nullopt;
                }

                conditionIsMetCache = conditionIsMet;
            }
        }

        previousInstantCache = instant;
    }

    // Add interval if condition is met on the last iteration
    if (conditionIsMetCache)
    {
        intervals.add(Interval::Closed(conditionStartInstantCache.value(), instants.getLast()));
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
