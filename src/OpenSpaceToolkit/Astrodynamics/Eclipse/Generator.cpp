/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Utility/Eclipse.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Eclipse/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eclipse
{

using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Position;
using ostk::physics::environment::utilities::Eclipse;
using ostk::physics::environment::utilities::EclipsePhase;
using ostk::physics::environment::utilities::montenbruckGillShadowFunction;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;

Generator::Generator(
    const Environment& anEnvironment, const Duration& aSearchStepSize, const Duration& aSearchTolerance
)
    : environment_(anEnvironment),
      temporalConditionSolver_(aSearchStepSize, aSearchTolerance)
{
}

bool Generator::isDefined() const
{
    return environment_.isDefined();
}

Environment Generator::getEnvironment() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return environment_;
}

Duration Generator::getSearchStepSize() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return temporalConditionSolver_.getTimeStep();
}

Duration Generator::getSearchTolerance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return temporalConditionSolver_.getTolerance();
}

Array<Eclipse> Generator::generate(
    const Trajectory& aTrajectory,
    const Interval& anAnalysisInterval,
    const String& anOccultedCelestialObjectName,
    const String& anOccultingCelestialObjectName
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    if (!aTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }

    if (!anAnalysisInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Analysis interval");
    }

    // Get celestial objects
    const Shared<const Celestial> occultedCelestialObject =
        environment_.accessCelestialObjectWithName(anOccultedCelestialObjectName);
    const Shared<const Celestial> occultingCelestialObject =
        environment_.accessCelestialObjectWithName(anOccultingCelestialObjectName);

    // Define condition for penumbra and umbra (shadow factor < 1.0)
    const auto penumbraAndUmbraCondition =
        [&aTrajectory, &occultedCelestialObject, &occultingCelestialObject](const Instant& anInstant) -> bool
    {
        const State state = aTrajectory.getStateAt(anInstant);
        const Position position = state.getPosition();

        const Real shadowFactor =
            montenbruckGillShadowFunction(anInstant, position, *occultedCelestialObject, *occultingCelestialObject);

        return shadowFactor < 1.0;
    };

    // Find all penumbra and umbra intervals
    const Array<Interval> penumbraAndUmbraIntervals =
        temporalConditionSolver_.solve(penumbraAndUmbraCondition, anAnalysisInterval);

    // Define condition for umbra only (shadow factor == 0.0)
    const auto umbraCondition =
        [&aTrajectory, &occultedCelestialObject, &occultingCelestialObject](const Instant& anInstant) -> bool
    {
        const State state = aTrajectory.getStateAt(anInstant);
        const Position position = state.getPosition();

        const Real shadowFactor =
            montenbruckGillShadowFunction(anInstant, position, *occultedCelestialObject, *occultingCelestialObject);

        return shadowFactor == 0.0;
    };

    Array<Eclipse> eclipses = Array<Eclipse>::Empty();

    // Process each penumbra and umbra interval
    for (const Interval& penumbraAndUmbraInterval : penumbraAndUmbraIntervals)
    {
        // Find umbra intervals within this eclipse interval
        const Array<Interval> umbraIntervals = temporalConditionSolver_.solve(umbraCondition, penumbraAndUmbraInterval);

        // Calculate penumbra intervals as gaps between umbra intervals
        const Array<Interval> penumbraIntervals = Interval::GetGaps(umbraIntervals, penumbraAndUmbraInterval);

        // Create eclipse phases
        Array<EclipsePhase> phases = Array<EclipsePhase>::Empty();

        // Add penumbra phases
        for (const Interval& penumbraInterval : penumbraIntervals)
        {
            const bool isComplete = (penumbraInterval.getStart() != anAnalysisInterval.getStart()) &&
                                    (penumbraInterval.getEnd() != anAnalysisInterval.getEnd());

            phases.add(EclipsePhase(EclipsePhase::Region::Penumbra, penumbraInterval, isComplete));
        }

        // Add umbra phases
        for (const Interval& umbraInterval : umbraIntervals)
        {
            const bool isComplete = (umbraInterval.getStart() != anAnalysisInterval.getStart()) &&
                                    (umbraInterval.getEnd() != anAnalysisInterval.getEnd());

            phases.add(EclipsePhase(EclipsePhase::Region::Umbra, umbraInterval, isComplete));
        }

        // Create and add eclipse
        eclipses.add(Eclipse(*occultedCelestialObject, *occultingCelestialObject, phases));
    }

    // Sort eclipses by start time
    std::sort(
        eclipses.begin(),
        eclipses.end(),
        [](const Eclipse& firstEclipse, const Eclipse& secondEclipse) -> bool
        {
            return firstEclipse.getPhases().accessFirst().getInterval().getStart() <
                   secondEclipse.getPhases().accessFirst().getInterval().getStart();
        }
    );

    return eclipses;
}

}  // namespace eclipse
}  // namespace astrodynamics
}  // namespace ostk
