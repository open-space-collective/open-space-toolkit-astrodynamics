# Sequence Constraint Logic Refactoring Implementation Plan

## Overview

This plan outlines a comprehensive refactoring of the Sequence constraint logic to improve readability, maintainability, and testability. The refactoring preserves the current sound architecture (keeping constraint logic in Sequence, not moving to Segment) while making internal improvements through helper method extraction, utility class creation, enhanced documentation, and improved logging.

## Current State Analysis

### What Exists Now

The constraint enforcement logic resides in `Sequence::solveSegment_()` at `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:501-726` (225 lines). This method handles three types of maneuver constraints:

1. **Minimum Maneuver Duration** (lines 597-609) - Skips maneuvers shorter than threshold
2. **Maximum Maneuver Duration** (lines 612-683) - Handles maneuvers exceeding limits with four strategies:
   - **Fail** - Throw exception
   - **Skip** - Skip entire maneuver
   - **Slice** - Slice into multiple chunks
   - **Center** - Take centered portion
3. **Minimum Maneuver Separation** (lines 537-543, 706-709) - Enforces recovery time between maneuvers

### Key Constraints Discovered

From research document and code analysis:

- The architecture is **sound** - constraint logic rightfully belongs in Sequence
- The 225-line method is complex by necessity but can be improved for readability
- Helper method `buildThrusterDynamicsWithManeuverIntervals_` already exists (lines 728-744)
- Excellent test coverage exists (test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp:1379-1817)
- Python bindings expose all constraint API (bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Sequence.cpp:309-399)
- No architectural changes needed - internal refactoring only

### Key File References

**Implementation Files**:
- `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp:289-328` - Header with constraint members and methods
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:501-744` - Implementation with solveSegment_ and helper

**Test Files**:
- `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp:1379-1817` - Comprehensive constraint tests
- `bindings/python/test/trajectory/test_sequence.py:431-480` - Python constraint API tests

**Validation Files**:
- `validation/OpenSpaceToolkit/Astrodynamics/Framework.validation.cpp` - End-to-end validation
- `validation/data/scenarios/020-sequence-multiple-*.yaml` - Scenario definitions

## Desired End State

After this refactoring is complete:

1. **Readability**: The `solveSegment_` method is broken down into well-named helper methods with clear responsibilities
2. **Testability**: Constraint checking logic is encapsulated in a utility class that can be tested independently
3. **Documentation**: Comprehensive algorithm documentation with state machine diagrams and examples
4. **Observability**: Enhanced logging for constraint violations with statistics and clear reasoning
5. **Maintainability**: Code organization follows established patterns in the codebase
6. **Verification**: All existing tests pass; new tests added for edge cases

### How to Verify End State

#### Automated Verification:
- [ ] All existing C++ unit tests pass: `docker run --rm --workdir=/app --volume="$PWD:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root bin/open-space-toolkit-astrodynamics.test`
- [ ] All existing Python tests pass: `docker run --rm --workdir=/app --volume="$PWD:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-install-python && ostk-test-python`
- [ ] All validation tests pass: `docker run --rm --workdir=/app --volume="$PWD:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root bin/open-space-toolkit-astrodynamics.validation`
- [ ] Code formatting is correct: `docker run --rm --workdir=/app --volume="$PWD:/app:delegated" openspacecollective/open-space-toolkit-astrodynamics-development-non-root ostk-check-format-cpp`
- [ ] New unit tests for helper methods pass

#### Manual Verification:
- [ ] Code review confirms improved readability of `solveSegment_` method
- [ ] Documentation is comprehensive and includes state machine diagram
- [ ] Logging output during test runs shows enhanced constraint violation messages
- [ ] No performance regression (run benchmarks if available)

## What We're NOT Doing

**Explicitly out of scope** to prevent scope creep:

1. **NOT moving constraint logic to Segment** - Research concluded this would violate separation of concerns
2. **NOT changing public API** - All refactoring is internal; external interface unchanged
3. **NOT modifying Python bindings** - Since API is unchanged, bindings remain as-is
4. **NOT changing constraint semantics** - Constraint behavior remains identical
5. **NOT adding new constraint types** - Focus is on refactoring existing logic
6. **NOT optimizing performance** - Unless profiling reveals specific issues
7. **NOT changing validation scenarios** - Existing YAML scenarios remain unchanged

## Implementation Approach

### High-Level Strategy

Follow the **Extract Method** refactoring pattern extensively used in the codebase (see patterns in Segment::solve_, COE::FrozenOrbit). The refactoring will be done in phases, with each phase independently testable:

1. **Phase 1**: Extract inline constraint checking into private helper methods
2. **Phase 2**: Create ManeuverConstraintChecker utility class to encapsulate validation logic
3. **Phase 3**: Enhance documentation with algorithm overview and state machine diagrams
4. **Phase 4**: Add enhanced logging and statistics tracking
5. **Phase 5**: Update test coverage for new helper methods and edge cases

### Architectural Decisions

**Decision 1: Keep Constraint Logic in Sequence**
- **Rationale**: Research document (RESEARCH_sequence_constraint_simplification.md) concluded constraints are sequence-level concerns requiring cross-segment state tracking
- **Impact**: No changes to Segment class; all refactoring within Sequence

**Decision 2: Create Internal Utility Class**
- **Rationale**: Encapsulates constraint evaluation; can be tested independently
- **Choice**: ManeuverConstraintChecker as private nested class or separate utility
- **Decision**: Separate utility class in Sequence namespace for testability

**Decision 3: No API Changes**
- **Rationale**: Existing API is well-designed; Python bindings work correctly
- **Impact**: All changes are implementation-only; no header changes to public methods

---

## Phase 1: Extract Private Helper Methods

### Overview

Extract constraint checking logic from the monolithic `solveSegment_` method into focused private helper methods. This phase improves readability without changing behavior.

### Changes Required

#### 1. Add Private Helper Method Declarations to Header

**File**: `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp`
**Location**: After line 319 (after `buildThrusterDynamicsWithManeuverIntervals_` declaration)

```cpp
private:
    // Existing method
    Shared<Thruster> buildThrusterDynamicsWithManeuverIntervals_(...) const;

    // NEW: Constraint checking helper methods

    /// @brief Check if a maneuver satisfies the minimum duration constraint.
    ///
    /// @param aManeuver The maneuver to check
    /// @return True if maneuver meets minimum duration or constraint is undefined, false otherwise
    bool satisfiesMinimumDurationConstraint_(const Maneuver& aManeuver) const;

    /// @brief Check if a maneuver violates the maximum duration constraint and determine handling.
    ///
    /// @param aManeuver The maneuver to check
    /// @param aSegment The segment containing the maneuver (used for strategy application)
    /// @param outModifiedSegment [Output] Modified segment if strategy requires it (Slice/Center)
    /// @param outNextCoastInstant [Output] Instant to coast to after handling
    /// @return True if maneuver is accepted (within max or undefined), false if rejected/modified
    bool checkMaximumDurationConstraint_(
        const Maneuver& aManeuver,
        const Segment& aSegment,
        Segment& outModifiedSegment,
        Instant& outNextCoastInstant
    ) const;

    /// @brief Calculate the instant to coast to for minimum maneuver separation enforcement.
    ///
    /// @param aLastManeuverInterval The interval of the last maneuver
    /// @param aCurrentInstant The current instant
    /// @return Instant to coast to, or Undefined if no separation needed
    Instant calculateSeparationCoastTarget_(
        const Interval& aLastManeuverInterval,
        const Instant& aCurrentInstant
    ) const;

    /// @brief Apply the maximum maneuver duration violation strategy to create a modified segment.
    ///
    /// @param aManeuver The maneuver that violates the constraint
    /// @param aSegment The original segment
    /// @param aStrategy The strategy to apply
    /// @return Modified segment with strategy applied (shortened maneuver)
    Segment applyMaximumDurationStrategy_(
        const Maneuver& aManeuver,
        const Segment& aSegment,
        const MaximumManeuverDurationViolationStrategy& aStrategy
    ) const;
```

#### 2. Implement Helper Methods in Source File

**File**: `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp`
**Location**: After line 744 (after `buildThrusterDynamicsWithManeuverIntervals_` implementation)

```cpp
bool Sequence::satisfiesMinimumDurationConstraint_(const Maneuver& aManeuver) const
{
    if (!minimumManeuverDuration_.isDefined())
    {
        return true;
    }

    const bool satisfies = aManeuver.getInterval().getDuration() >= minimumManeuverDuration_;

    if (!satisfies)
    {
        BOOST_LOG_TRIVIAL(debug)
            << "Maneuver at " << aManeuver.getInterval().toString()
            << " duration (" << aManeuver.getInterval().getDuration().toString() << ") "
            << "is less than minimum (" << minimumManeuverDuration_.toString() << "). "
            << "Skipping maneuver.";
    }

    return satisfies;
}

bool Sequence::checkMaximumDurationConstraint_(
    const Maneuver& aManeuver,
    const Segment& aSegment,
    Segment& outModifiedSegment,
    Instant& outNextCoastInstant
) const
{
    if (!maximumManeuverDuration_.isDefined())
    {
        return true;  // No constraint, accept maneuver
    }

    if (aManeuver.getInterval().getDuration() <= maximumManeuverDuration_)
    {
        return true;  // Within limit, accept maneuver
    }

    // Maneuver exceeds maximum duration - apply strategy
    BOOST_LOG_TRIVIAL(debug)
        << "Maneuver at " << aManeuver.getInterval().toString()
        << " duration (" << aManeuver.getInterval().getDuration().toString() << ") "
        << "exceeds maximum (" << maximumManeuverDuration_.toString() << "). "
        << "Applying " << [this]() {
            switch (maximumManeuverDurationStrategy_)
            {
                case MaximumManeuverDurationViolationStrategy::Fail: return "Fail";
                case MaximumManeuverDurationViolationStrategy::Skip: return "Skip";
                case MaximumManeuverDurationViolationStrategy::Slice: return "Slice";
                case MaximumManeuverDurationViolationStrategy::Center: return "Center";
                default: return "Unknown";
            }
        }() << " strategy.";

    switch (maximumManeuverDurationStrategy_)
    {
        case MaximumManeuverDurationViolationStrategy::Fail:
        {
            throw ostk::core::error::RuntimeError(
                "Maneuver duration (" + aManeuver.getInterval().getDuration().toString() +
                ") exceeds maximum maneuver duration (" + maximumManeuverDuration_.toString() +
                "). Change the maximum maneuver duration strategy to prevent failure."
            );
        }

        case MaximumManeuverDurationViolationStrategy::Skip:
        {
            outModifiedSegment = aSegment.toCoastSegment(
                aSegment.getName() + " (Coast - Skipping Maneuver due to Maximum Duration)"
            );
            outNextCoastInstant = aManeuver.getInterval().getEnd() + Duration::Seconds(10.0);
            return false;  // Maneuver rejected
        }

        case MaximumManeuverDurationViolationStrategy::Slice:
        {
            const Shared<Thruster> shortenedThruster = this->buildThrusterDynamicsWithManeuverIntervals_(
                aSegment,
                {Interval::Closed(
                    aManeuver.getInterval().getStart(),
                    aManeuver.getInterval().getStart() + maximumManeuverDuration_
                )},
                "(Sliced Maneuver)"
            );
            outModifiedSegment = aSegment.toManeuverSegment(
                shortenedThruster,
                aSegment.getName() + " (Sliced Maneuver)"
            );
            outNextCoastInstant = aManeuver.getInterval().getStart() + maximumManeuverDuration_;
            return false;  // Maneuver modified, need to continue iterating
        }

        case MaximumManeuverDurationViolationStrategy::Center:
        {
            const Shared<Thruster> centeredThruster = this->buildThrusterDynamicsWithManeuverIntervals_(
                aSegment,
                {Interval::Centered(
                    aManeuver.getInterval().getCenter(),
                    maximumManeuverDuration_,
                    Interval::Type::Closed
                )},
                "(Centered Maneuver)"
            );
            outModifiedSegment = aSegment.toManeuverSegment(
                centeredThruster,
                aSegment.getName() + " (Centered Maneuver)"
            );
            outNextCoastInstant = aManeuver.getInterval().getEnd() + Duration::Seconds(10.0);
            return false;  // Maneuver modified, need to continue
        }

        default:
        {
            throw ostk::core::error::runtime::ToBeImplemented(
                "Maximum maneuver duration strategy: " +
                String::Format("{}", static_cast<int>(maximumManeuverDurationStrategy_))
            );
        }
    }
}

Instant Sequence::calculateSeparationCoastTarget_(
    const Interval& aLastManeuverInterval,
    const Instant& aCurrentInstant
) const
{
    if (!aLastManeuverInterval.isDefined() || !minimumManeuverSeparation_.isDefined())
    {
        return Instant::Undefined();
    }

    const Instant earliestNextManeuver = aLastManeuverInterval.getEnd() + minimumManeuverSeparation_;

    if (aCurrentInstant >= earliestNextManeuver)
    {
        return Instant::Undefined();  // Already past separation requirement
    }

    BOOST_LOG_TRIVIAL(debug)
        << "Minimum maneuver separation enforced. "
        << "Last maneuver ended at " << aLastManeuverInterval.getEnd().toString() << ", "
        << "coasting to " << earliestNextManeuver.toString()
        << " (separation: " << minimumManeuverSeparation_.toString() << ").";

    return earliestNextManeuver;
}
```

#### 3. Refactor `solveSegment_` to Use Helper Methods

**File**: `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp`
**Location**: Lines 501-726 (replace existing implementation)

```cpp
Tuple<Segment::Solution, Interval> Sequence::solveSegment_(
    const State& aState,
    const Segment& aSegment,
    const Duration& aMaximumPropagationDuration,
    const Interval& aLastManeuverInterval
) const
{
    // Fast path: If no constraints apply, solve directly
    if (aSegment.getType() != Segment::Type::Maneuver ||
        (!maximumManeuverDuration_.isDefined() && !minimumManeuverDuration_.isDefined() &&
         !minimumManeuverSeparation_.isDefined()))
    {
        return {aSegment.solve(aState, aMaximumPropagationDuration), aLastManeuverInterval};
    }

    // Initialize unified solution accumulators
    Array<State> unifiedStates = Array<State>::Empty();
    Array<Interval> unifiedManeuverIntervals = Array<Interval>::Empty();
    const Shared<HeterogeneousGuidanceLaw> unifiedGuidanceLaw = std::make_shared<HeterogeneousGuidanceLaw>();

    unifiedStates.add(aState);
    bool subsegmentConditionIsSatisfied = false;
    const Instant maximumInstant = aState.accessInstant() + aMaximumPropagationDuration;
    Instant nextInstantToCoastTo = Instant::Undefined();
    Segment nextSubsegmentToSolve = aSegment;

    // Helper lambda to update next coast instant
    const auto updateNextCoastInstant = [&maximumInstant, &nextInstantToCoastTo](
        const Instant& candidateInstant
    ) -> Instant
    {
        const Instant clampedCandidate = std::min(candidateInstant, maximumInstant);
        return nextInstantToCoastTo.isDefined() ? std::max(nextInstantToCoastTo, clampedCandidate)
                                                 : clampedCandidate;
    };

    // Check if we need to coast first due to minimum separation from previous maneuver
    const Instant separationTarget = this->calculateSeparationCoastTarget_(
        aLastManeuverInterval,
        aState.accessInstant()
    );
    if (separationTarget.isDefined())
    {
        nextInstantToCoastTo = updateNextCoastInstant(separationTarget);
        nextSubsegmentToSolve = aSegment.toCoastSegment(
            aSegment.getName() + " (Coast - Minimum Maneuver Separation Constraint)"
        );
    }

    const Duration subsegmentManeuverMargin = Duration::Seconds(10.0);

    // Main iteration loop: solve maneuver by maneuver, applying constraints
    while (maximumInstant > unifiedStates.accessLast().accessInstant())
    {
        // STATE: Coasting phase
        if (nextSubsegmentToSolve.getType() == Segment::Type::Coast)
        {
            const Duration coastDuration = std::min(
                nextInstantToCoastTo - unifiedStates.accessLast().accessInstant(),
                maximumInstant - unifiedStates.accessLast().accessInstant()
            );

            Segment::Solution coastSegmentSolution = nextSubsegmentToSolve.solve(
                unifiedStates.accessLast(),
                coastDuration
            );

            subsegmentConditionIsSatisfied = coastSegmentSolution.conditionIsSatisfied;
            unifiedStates.add(Array<State>(
                coastSegmentSolution.states.begin() + 1,
                coastSegmentSolution.states.end()
            ));

            if (subsegmentConditionIsSatisfied)
            {
                break;
            }

            // Transition back to maneuver segment after coast
            nextSubsegmentToSolve = aSegment;
            continue;
        }

        // STATE: Maneuvering phase - solve to next maneuver candidate
        Segment::Solution maneuverSubsegmentSolution = nextSubsegmentToSolve.solveToNextManeuver(
            unifiedStates.accessLast(),
            maximumInstant - unifiedStates.accessLast().accessInstant()
        );
        subsegmentConditionIsSatisfied = maneuverSubsegmentSolution.conditionIsSatisfied;

        const Array<Maneuver> subsegmentManeuvers = maneuverSubsegmentSolution.extractManeuvers(
            aState.accessFrame()
        );

        // No maneuvers found - add states and exit
        if (subsegmentManeuvers.isEmpty())
        {
            unifiedStates.add(Array<State>(
                maneuverSubsegmentSolution.states.begin() + 1,
                maneuverSubsegmentSolution.states.end()
            ));
            break;
        }

        if (subsegmentManeuvers.getSize() > 1)
        {
            throw ostk::core::error::RuntimeError(
                "More than one maneuver found when solving maneuver segment with constraints. "
                "This indicates an issue with solveToNextManeuver() behavior."
            );
        }

        const Maneuver nextManeuverCandidate = subsegmentManeuvers.accessFirst();

        // CONSTRAINT CHECK 1: Minimum maneuver duration
        if (!this->satisfiesMinimumDurationConstraint_(nextManeuverCandidate))
        {
            nextSubsegmentToSolve = aSegment.toCoastSegment(
                aSegment.getName() + " (Skipping Maneuver - Minimum Duration Constraint)"
            );
            nextInstantToCoastTo = updateNextCoastInstant(
                nextManeuverCandidate.getInterval().getEnd() + subsegmentManeuverMargin
            );
            continue;
        }

        // CONSTRAINT CHECK 2: Maximum maneuver duration with strategy
        Segment modifiedSegment;
        Instant maxDurationCoastTarget = Instant::Undefined();

        if (!this->checkMaximumDurationConstraint_(
            nextManeuverCandidate,
            aSegment,
            modifiedSegment,
            maxDurationCoastTarget
        ))
        {
            // Maneuver rejected or modified by strategy
            nextSubsegmentToSolve = modifiedSegment;
            if (maxDurationCoastTarget.isDefined())
            {
                nextInstantToCoastTo = updateNextCoastInstant(maxDurationCoastTarget);
            }
            continue;
        }

        // MANEUVER ACCEPTED - Add to unified solution
        BOOST_LOG_TRIVIAL(debug)
            << "Maneuver accepted at " << nextManeuverCandidate.getInterval().toString()
            << " (duration: " << nextManeuverCandidate.getInterval().getDuration().toString() << ").";

        unifiedStates.add(Array<State>(
            maneuverSubsegmentSolution.states.begin() + 1,
            maneuverSubsegmentSolution.states.end()
        ));
        unifiedManeuverIntervals.add(nextManeuverCandidate.getInterval());
        unifiedGuidanceLaw->addGuidanceLaw(
            maneuverSubsegmentSolution.getThrusterDynamics()->getGuidanceLaw(),
            nextManeuverCandidate.getInterval()
        );

        if (subsegmentConditionIsSatisfied)
        {
            break;
        }

        // Prepare for next iteration: enforce minimum separation after this maneuver
        const Instant nextSeparationTarget = this->calculateSeparationCoastTarget_(
            nextManeuverCandidate.getInterval(),
            unifiedStates.accessLast().accessInstant()
        );

        if (nextSeparationTarget.isDefined())
        {
            nextInstantToCoastTo = updateNextCoastInstant(nextSeparationTarget);
            nextSubsegmentToSolve = aSegment.toCoastSegment(
                aSegment.getName() + " (Coast - Minimum Maneuver Separation Constraint)"
            );
        }
    }

    // Build final unified segment solution
    Array<Shared<Dynamics>> unifiedDynamics = aSegment.getFreeDynamics();
    unifiedDynamics.add(std::make_shared<Thruster>(
        aSegment.getThrusterDynamics()->getSatelliteSystem(),
        unifiedGuidanceLaw,
        aSegment.getName() + " (Maneuvering Constraints Applied)"
    ));

    return {
        Segment::Solution(
            aSegment.getName(),
            unifiedDynamics,
            unifiedStates,
            subsegmentConditionIsSatisfied,
            Segment::Type::Maneuver
        ),
        unifiedManeuverIntervals.isEmpty() ? aLastManeuverInterval : unifiedManeuverIntervals.accessLast()
    };
}
```

### Success Criteria

#### Automated Verification:
- [x] All existing C++ unit tests pass: `bin/open-space-toolkit-astrodynamics.test`
- [x] All constraint-specific tests pass: `bin/open-space-toolkit-astrodynamics.test --gtest_filter=*ManeuverConstraint*`
- [x] Code compiles without warnings
- [x] Code formatting passes: `ostk-check-format-cpp`

#### Manual Verification:
- [x] Code review confirms `solveSegment_` is more readable
- [x] Helper methods have clear, single responsibilities
- [x] No behavioral changes (same test outputs)
- [x] Logging output shows constraint reasoning

---

## Phase 2: Create ManeuverConstraintChecker Utility Class

### Overview

Encapsulate constraint validation logic into a dedicated utility class. This improves testability and provides a clear interface for constraint checking.

### Changes Required

#### 1. Create ManeuverConstraintChecker Header

**File**: `include/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.hpp` (NEW FILE)

```cpp
/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_ManeuverConstraintChecker__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_ManeuverConstraintChecker__

#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::String;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::flight::Maneuver;

/// @brief Utility class to check maneuver constraints and provide violation information.
///
/// This class encapsulates the logic for validating maneuvers against minimum/maximum duration
/// and minimum separation constraints. It does not modify maneuvers or segments; it only evaluates
/// whether constraints are satisfied and provides information for handling violations.
class ManeuverConstraintChecker
{
   public:
    /// @brief Constraint check result with violation details.
    struct ConstraintViolation
    {
        bool isViolated;                  ///< True if constraint is violated
        String violationType;             ///< Type of violation (e.g., "Minimum Duration", "Maximum Duration")
        String description;               ///< Human-readable description
        Duration expectedValue;           ///< Expected value for the constraint
        Duration actualValue;             ///< Actual value from the maneuver
        Instant suggestedCoastTarget;     ///< Suggested instant to coast to (if applicable)
    };

    /// @brief Constructor.
    ///
    /// @param aMinimumDuration Minimum maneuver duration constraint (Undefined for no constraint)
    /// @param aMaximumDuration Maximum maneuver duration constraint (Undefined for no constraint)
    /// @param aMinimumSeparation Minimum separation between maneuvers (Undefined for no constraint)
    /// @param aMaximumDurationStrategy Strategy for handling maximum duration violations
    ManeuverConstraintChecker(
        const Duration& aMinimumDuration,
        const Duration& aMaximumDuration,
        const Duration& aMinimumSeparation,
        const Sequence::MaximumManeuverDurationViolationStrategy& aMaximumDurationStrategy
    );

    /// @brief Check if a maneuver satisfies the minimum duration constraint.
    ///
    /// @param aManeuver The maneuver to check
    /// @return ConstraintViolation result (isViolated = false if satisfied)
    ConstraintViolation checkMinimumDuration(const Maneuver& aManeuver) const;

    /// @brief Check if a maneuver violates the maximum duration constraint.
    ///
    /// @param aManeuver The maneuver to check
    /// @return ConstraintViolation result (isViolated = true if exceeds maximum)
    ConstraintViolation checkMaximumDuration(const Maneuver& aManeuver) const;

    /// @brief Check if minimum separation is satisfied between two maneuvers.
    ///
    /// @param aLastManeuverInterval The interval of the previous maneuver
    /// @param aCurrentInstant The start instant of the next maneuver candidate
    /// @return ConstraintViolation result (isViolated = true if separation insufficient)
    ConstraintViolation checkMinimumSeparation(
        const Interval& aLastManeuverInterval,
        const Instant& aCurrentInstant
    ) const;

    /// @brief Get the maximum duration violation strategy.
    ///
    /// @return The strategy for handling maximum duration violations
    Sequence::MaximumManeuverDurationViolationStrategy getMaximumDurationStrategy() const;

    /// @brief Check if any constraints are defined.
    ///
    /// @return True if at least one constraint is defined
    bool hasConstraints() const;

    /// @brief Generate a summary string of all configured constraints.
    ///
    /// @return Human-readable summary of constraints
    String toString() const;

   private:
    Duration minimumDuration_;
    Duration maximumDuration_;
    Duration minimumSeparation_;
    Sequence::MaximumManeuverDurationViolationStrategy maximumDurationStrategy_;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
```

#### 2. Create ManeuverConstraintChecker Implementation

**File**: `src/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.cpp` (NEW FILE)

```cpp
/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

ManeuverConstraintChecker::ManeuverConstraintChecker(
    const Duration& aMinimumDuration,
    const Duration& aMaximumDuration,
    const Duration& aMinimumSeparation,
    const Sequence::MaximumManeuverDurationViolationStrategy& aMaximumDurationStrategy
)
    : minimumDuration_(aMinimumDuration),
      maximumDuration_(aMaximumDuration),
      minimumSeparation_(aMinimumSeparation),
      maximumDurationStrategy_(aMaximumDurationStrategy)
{
}

ManeuverConstraintChecker::ConstraintViolation ManeuverConstraintChecker::checkMinimumDuration(
    const Maneuver& aManeuver
) const
{
    if (!minimumDuration_.isDefined())
    {
        return {false, "Minimum Duration", "No constraint defined", Duration::Undefined(), Duration::Undefined(), Instant::Undefined()};
    }

    const Duration maneuverDuration = aManeuver.getInterval().getDuration();
    const bool isViolated = maneuverDuration < minimumDuration_;

    return {
        isViolated,
        "Minimum Duration",
        isViolated ? String::Format(
            "Maneuver duration ({}) is less than minimum ({})",
            maneuverDuration.toString(),
            minimumDuration_.toString()
        ) : "Satisfied",
        minimumDuration_,
        maneuverDuration,
        isViolated ? aManeuver.getInterval().getEnd() + Duration::Seconds(10.0) : Instant::Undefined()
    };
}

ManeuverConstraintChecker::ConstraintViolation ManeuverConstraintChecker::checkMaximumDuration(
    const Maneuver& aManeuver
) const
{
    if (!maximumDuration_.isDefined())
    {
        return {false, "Maximum Duration", "No constraint defined", Duration::Undefined(), Duration::Undefined(), Instant::Undefined()};
    }

    const Duration maneuverDuration = aManeuver.getInterval().getDuration();
    const bool isViolated = maneuverDuration > maximumDuration_;

    String strategyName;
    switch (maximumDurationStrategy_)
    {
        case Sequence::MaximumManeuverDurationViolationStrategy::Fail:
            strategyName = "Fail";
            break;
        case Sequence::MaximumManeuverDurationViolationStrategy::Skip:
            strategyName = "Skip";
            break;
        case Sequence::MaximumManeuverDurationViolationStrategy::Slice:
            strategyName = "Slice";
            break;
        case Sequence::MaximumManeuverDurationViolationStrategy::Center:
            strategyName = "Center";
            break;
        default:
            strategyName = "Unknown";
    }

    return {
        isViolated,
        "Maximum Duration",
        isViolated ? String::Format(
            "Maneuver duration ({}) exceeds maximum ({}). Strategy: {}",
            maneuverDuration.toString(),
            maximumDuration_.toString(),
            strategyName
        ) : "Satisfied",
        maximumDuration_,
        maneuverDuration,
        Instant::Undefined()
    };
}

ManeuverConstraintChecker::ConstraintViolation ManeuverConstraintChecker::checkMinimumSeparation(
    const Interval& aLastManeuverInterval,
    const Instant& aCurrentInstant
) const
{
    if (!minimumSeparation_.isDefined() || !aLastManeuverInterval.isDefined())
    {
        return {false, "Minimum Separation", "No constraint defined or no previous maneuver", Duration::Undefined(), Duration::Undefined(), Instant::Undefined()};
    }

    const Instant earliestNextManeuver = aLastManeuverInterval.getEnd() + minimumSeparation_;
    const bool isViolated = aCurrentInstant < earliestNextManeuver;
    const Duration actualSeparation = aCurrentInstant - aLastManeuverInterval.getEnd();

    return {
        isViolated,
        "Minimum Separation",
        isViolated ? String::Format(
            "Separation ({}) from previous maneuver is less than minimum ({})",
            actualSeparation.toString(),
            minimumSeparation_.toString()
        ) : "Satisfied",
        minimumSeparation_,
        actualSeparation,
        isViolated ? earliestNextManeuver : Instant::Undefined()
    };
}

Sequence::MaximumManeuverDurationViolationStrategy ManeuverConstraintChecker::getMaximumDurationStrategy() const
{
    return maximumDurationStrategy_;
}

bool ManeuverConstraintChecker::hasConstraints() const
{
    return minimumDuration_.isDefined() || maximumDuration_.isDefined() || minimumSeparation_.isDefined();
}

String ManeuverConstraintChecker::toString() const
{
    String result = "ManeuverConstraintChecker:\n";

    result += "  Minimum Duration: ";
    result += minimumDuration_.isDefined() ? minimumDuration_.toString() : "Undefined";
    result += "\n";

    result += "  Maximum Duration: ";
    result += maximumDuration_.isDefined() ? maximumDuration_.toString() : "Undefined";
    if (maximumDuration_.isDefined())
    {
        result += " (Strategy: ";
        switch (maximumDurationStrategy_)
        {
            case Sequence::MaximumManeuverDurationViolationStrategy::Fail:
                result += "Fail";
                break;
            case Sequence::MaximumManeuverDurationViolationStrategy::Skip:
                result += "Skip";
                break;
            case Sequence::MaximumManeuverDurationViolationStrategy::Slice:
                result += "Slice";
                break;
            case Sequence::MaximumManeuverDurationViolationStrategy::Center:
                result += "Center";
                break;
            default:
                result += "Unknown";
        }
        result += ")";
    }
    result += "\n";

    result += "  Minimum Separation: ";
    result += minimumSeparation_.isDefined() ? minimumSeparation_.toString() : "Undefined";
    result += "\n";

    return result;
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
```

#### 3. Integrate ManeuverConstraintChecker into Sequence

**File**: `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp`
**Location**: Add include at top (around line 20)

```cpp
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.hpp>
```

**Modify private helper methods** to use ManeuverConstraintChecker (lines after 319):

```cpp
private:
    // ... existing methods ...

    /// @brief Create a constraint checker from current sequence configuration.
    ///
    /// @return ManeuverConstraintChecker with current constraint settings
    ManeuverConstraintChecker createConstraintChecker_() const;
```

**File**: `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp`
**Add implementation** (after buildThrusterDynamicsWithManeuverIntervals_):

```cpp
ManeuverConstraintChecker Sequence::createConstraintChecker_() const
{
    return ManeuverConstraintChecker(
        minimumManeuverDuration_,
        maximumManeuverDuration_,
        minimumManeuverSeparation_,
        maximumManeuverDurationStrategy_
    );
}
```

**Refactor helper methods** (lines 745+) to use ManeuverConstraintChecker:

```cpp
bool Sequence::satisfiesMinimumDurationConstraint_(const Maneuver& aManeuver) const
{
    const ManeuverConstraintChecker checker = this->createConstraintChecker_();
    const ManeuverConstraintChecker::ConstraintViolation violation = checker.checkMinimumDuration(aManeuver);

    if (violation.isViolated)
    {
        BOOST_LOG_TRIVIAL(debug) << violation.description;
    }

    return !violation.isViolated;
}

// Similar updates for checkMaximumDurationConstraint_ and calculateSeparationCoastTarget_
```

#### 4. Update CMakeLists.txt to Include New Files

**File**: `CMakeLists.txt` or appropriate build configuration
**Add**:

```cmake
# In source file list
src/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.cpp

# In header file list
include/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.hpp
```

### Success Criteria

#### Automated Verification:
- [ ] All existing tests pass
- [ ] New unit tests for ManeuverConstraintChecker pass
- [ ] Code compiles without warnings
- [ ] Code formatting passes

#### Manual Verification:
- [ ] ManeuverConstraintChecker can be instantiated independently
- [ ] Helper methods use ManeuverConstraintChecker consistently
- [ ] No behavioral changes to sequence solving

---

## Phase 3: Enhance Documentation with State Machine

### Overview

Add comprehensive documentation explaining the constraint application algorithm, including ASCII state machine diagrams and examples.

### Changes Required

#### 1. Add Algorithm Overview to Header

**File**: `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp`
**Location**: Update solveSegment_ docstring (around line 290)

```cpp
/// @brief Solve an individual segment with maneuver constraint enforcement.
///
/// This method iteratively solves a segment, applying maneuver-related constraints (minimum/maximum
/// duration, minimum separation) to produce a compliant trajectory. The algorithm uses a state machine
/// approach to handle coasting and maneuvering phases.
///
/// # Algorithm Overview
///
/// The constraint enforcement uses an iterative state machine:
///
///                     ┌─────────────────────┐
///                     │  INITIALIZATION     │
///                     │  - Check fast path  │
///                     │  - Setup accumulators│
///                     └──────────┬──────────┘
///                                │
///                                ▼
///                     ┌─────────────────────┐
///           ┌────────▶│   COAST PHASE       │◀───────────┐
///           │         │  - Propagate coast  │            │
///           │         │  - Check condition  │            │
///           │         └──────────┬──────────┘            │
///           │                    │                       │
///           │                    ▼                       │
///           │         ┌─────────────────────┐            │
///           │         │  MANEUVER PHASE     │            │
///           │         │ - Solve to next     │            │
///           │         │ - Extract maneuver  │            │
///           │         └──────────┬──────────┘            │
///           │                    │                       │
///           │                    ▼                       │
///           │         ┌─────────────────────┐            │
///           │         │ CHECK MIN DURATION  │            │
///           │         │ Skip if too short   │───┐        │
///           │         └──────────┬──────────┘   │        │
///           │                    │  Passed      │ Failed │
///           │                    ▼              ▼        │
///           │         ┌─────────────────────┐  Coast     │
///           │         │ CHECK MAX DURATION  │  Instead   │
///           │         │ Apply strategy      │────────────┘
///           │         └──────────┬──────────┘
///           │                    │  Passed
///           │                    ▼
///           │         ┌─────────────────────┐
///           │         │  ACCEPT MANEUVER    │
///           │         │ - Add to solution   │
///           │         │ - Update guidance   │
///           │         └──────────┬──────────┘
///           │                    │
///           │                    ▼
///           │         ┌─────────────────────┐
///           │         │ CHECK SEPARATION    │
///           │         │ Coast if needed     │
///           │         └──────────┬──────────┘
///           │                    │
///           │                    ▼
///           │         ┌─────────────────────┐
///           └─────────│  Continue or Exit?  │
///                     │ Max time reached?   │
///                     └─────────────────────┘
///
/// # Constraint Application Order
///
/// For each maneuver candidate:
/// 1. **Minimum Duration Check** - Reject if duration < minimum (skip maneuver, coast instead)
/// 2. **Maximum Duration Check** - Apply strategy if duration > maximum:
///    - Fail: Throw exception
///    - Skip: Reject maneuver, coast instead
///    - Slice: Accept shortened portion, continue for remainder
///    - Center: Accept centered window of maximum duration
/// 3. **Separation Enforcement** - After accepting, ensure gap before next maneuver
///
/// # State Tracking
///
/// - **Unified States**: Accumulates all propagated states across subsegments
/// - **Unified Maneuver Intervals**: Tracks accepted maneuver time windows
/// - **Unified Guidance Law**: HeterogeneousGuidanceLaw applied only during accepted intervals
/// - **Last Maneuver Interval**: Carried forward for cross-segment separation
///
/// # Examples
///
/// **Example 1: Minimum Duration Constraint**
/// ```
/// Input:  Maneuver at [0-5min] (5-minute duration)
/// Config: minimumManeuverDuration = 10 minutes
/// Result: Maneuver skipped, coast segment created
/// ```
///
/// **Example 2: Maximum Duration with Slice Strategy**
/// ```
/// Input:  Maneuver at [0-30min] (30-minute duration)
/// Config: maximumManeuverDuration = 10 minutes, strategy = Slice, minimumSeparation = 3 minutes
/// Result: Three maneuvers: [0-10min], [13-23min], [26-30min]
/// ```
///
/// **Example 3: Separation Enforcement Across Segments**
/// ```
/// Segment 1: Maneuver ends at T+10min
/// Segment 2: Attempted maneuver at T+12min
/// Config: minimumManeuverSeparation = 5 minutes
/// Result: Segment 2 coasts to T+15min before starting maneuver
/// ```
///
/// @param aState Initial state
/// @param aSegment Segment to solve
/// @param aMaximumPropagationDuration Maximum propagation duration
/// @param aLastManeuverInterval Last maneuver interval prior to this segment (Undefined if no previous maneuvers)
/// @return Tuple of segment solution and last maneuver interval after solving
Tuple<Segment::Solution, Interval> solveSegment_(...) const;
```

#### 2. Add Inline Documentation to Implementation

**File**: `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp`
**Location**: Throughout solveSegment_ implementation (lines 501-726)

Add state transition comments at key points:

```cpp
// === STATE MACHINE: INITIALIZATION ===
// Fast path check and accumulator setup

// === STATE MACHINE: SEPARATION PRE-CHECK ===
// Before entering main loop, check if we need to coast due to previous maneuver

// === STATE MACHINE: MAIN ITERATION LOOP ===
while (maximumInstant > unifiedStates.accessLast().accessInstant())
{
    // === STATE: COASTING ===
    // Propagate coast segment until reaching target instant or condition satisfied

    // === STATE: MANEUVERING ===
    // Solve to next maneuver candidate and extract maneuver

    // === CONSTRAINT GATE 1: MINIMUM DURATION ===
    // Check if maneuver meets minimum duration requirement

    // === CONSTRAINT GATE 2: MAXIMUM DURATION ===
    // Apply strategy if maneuver exceeds maximum

    // === ACCEPTANCE: MANEUVER PASSED CONSTRAINTS ===
    // Add to unified solution and update guidance law

    // === POST-ACCEPTANCE: SEPARATION ENFORCEMENT ===
    // Ensure minimum gap before next maneuver
}

// === FINALIZATION: BUILD UNIFIED SOLUTION ===
// Construct segment solution with all accepted maneuvers
```

#### 3. Create Documentation File

**File**: `docs/constraint-algorithm.md` (NEW FILE)

(Content omitted for brevity - would include detailed algorithm walkthrough, examples, and diagrams)

### Success Criteria

#### Automated Verification:
- [ ] Documentation builds without errors
- [ ] Doxygen generates docs correctly
- [ ] No broken links in documentation

#### Manual Verification:
- [ ] State machine diagram is clear and accurate
- [ ] Examples are correct and helpful
- [ ] Inline comments improve code navigation
- [ ] Code review confirms documentation quality

---

## Phase 4: Add Logging and Validation Enhancements

### Overview

Add enhanced logging for constraint violations with statistics and create validation helpers.

### Changes Required

#### 1. Add Constraint Statistics Tracking

**File**: `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp`
**Add nested struct** (around line 139, after Solution struct):

```cpp
/// @brief Statistics about constraint enforcement during solving.
struct ConstraintStatistics
{
    Size totalManeuverCandidates = 0;      ///< Total maneuvers evaluated
    Size acceptedManeuvers = 0;            ///< Maneuvers that passed all constraints
    Size rejectedMinDuration = 0;          ///< Maneuvers rejected for minimum duration
    Size rejectedMaxDurationFail = 0;      ///< Maneuvers that caused Fail strategy
    Size rejectedMaxDurationSkip = 0;      ///< Maneuvers skipped for maximum duration
    Size modifiedMaxDurationSplit = 0;     ///< Maneuvers split due to maximum duration
    Size modifiedMaxDurationCenter = 0;    ///< Maneuvers centered due to maximum duration
    Size rejectedSeparation = 0;           ///< Maneuvers rejected for separation

    /// @brief Print statistics
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Convert to string
    String toString() const;
};
```

**Modify solveSegment_** to track and return statistics:

```cpp
// Change return type
Tuple<Segment::Solution, Interval, ConstraintStatistics> solveSegment_(...) const;

// Add statistics tracking throughout method
ConstraintStatistics statistics;

// Increment counters at each constraint check
statistics.totalManeuverCandidates++;
if (!satisfiesMinimumDurationConstraint_(...)) {
    statistics.rejectedMinDuration++;
}
// ... similar for other constraints

// Return statistics
return {solution, lastInterval, statistics};
```

#### 2. Enhanced Logging with Structured Messages

**File**: `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp`
**Add helper for structured logging** (around line 745):

```cpp
namespace
{
    void logConstraintViolation(
        const String& constraintType,
        const String& reason,
        const Maneuver& aManeuver,
        const String& action
    )
    {
        BOOST_LOG_TRIVIAL(debug)
            << "[CONSTRAINT] Type: " << constraintType << " | "
            << "Interval: " << aManeuver.getInterval().toString() << " | "
            << "Duration: " << aManeuver.getInterval().getDuration().toString() << " | "
            << "Reason: " << reason << " | "
            << "Action: " << action;
    }

    void logManeuverAccepted(const Maneuver& aManeuver, const Size& acceptedCount)
    {
        BOOST_LOG_TRIVIAL(debug)
            << "[ACCEPTED] Maneuver #" << acceptedCount << " | "
            << "Interval: " << aManeuver.getInterval().toString() << " | "
            << "Duration: " << aManeuver.getInterval().getDuration().toString();
    }

    void logConstraintStatistics(const Sequence::ConstraintStatistics& statistics)
    {
        BOOST_LOG_TRIVIAL(info)
            << "[STATISTICS] "
            << "Total: " << statistics.totalManeuverCandidates << " | "
            << "Accepted: " << statistics.acceptedManeuvers << " | "
            << "Rejected (MinDur): " << statistics.rejectedMinDuration << " | "
            << "Rejected (MaxDur): " << (statistics.rejectedMaxDurationSkip + statistics.rejectedMaxDurationFail) << " | "
            << "Modified (Slice): " << statistics.modifiedMaxDurationSplit << " | "
            << "Modified (Center): " << statistics.modifiedMaxDurationCenter << " | "
            << "Rejected (Sep): " << statistics.rejectedSeparation;
    }
}
```

**Update constraint checking methods** to use structured logging:

```cpp
if (!satisfiesMinimumDurationConstraint_(nextManeuverCandidate))
{
    logConstraintViolation(
        "Minimum Duration",
        "Duration below threshold",
        nextManeuverCandidate,
        "Skip and coast"
    );
    statistics.rejectedMinDuration++;
    // ... rest of handling
}
```

#### 3. Add Constraint Validation Helper

**File**: `include/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.hpp` (NEW FILE)

```cpp
/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_SequenceConstraintValidator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_SequenceConstraintValidator__

#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::String;
using ostk::physics::time::Duration;

/// @brief Validates constraint configurations for Sequence.
class SequenceConstraintValidator
{
   public:
    /// @brief Validation result with error details.
    struct ValidationResult
    {
        bool isValid;           ///< True if configuration is valid
        Array<String> errors;   ///< List of validation errors (empty if valid)
        Array<String> warnings; ///< List of warnings (non-fatal issues)
    };

    /// @brief Validate constraint configuration.
    ///
    /// @param aMinimumDuration Minimum maneuver duration
    /// @param aMaximumDuration Maximum maneuver duration
    /// @param aMinimumSeparation Minimum separation between maneuvers
    /// @param aStrategy Maximum duration violation strategy
    /// @return ValidationResult with errors and warnings
    static ValidationResult Validate(
        const Duration& aMinimumDuration,
        const Duration& aMaximumDuration,
        const Duration& aMinimumSeparation,
        const Sequence::MaximumManeuverDurationViolationStrategy& aStrategy
    );

    /// @brief Check if constraint values are consistent.
    ///
    /// @param aMinimumDuration Minimum maneuver duration
    /// @param aMaximumDuration Maximum maneuver duration
    /// @return True if consistent (max >= min when both defined)
    static bool AreConsistent(const Duration& aMinimumDuration, const Duration& aMaximumDuration);

   private:
    static void validateDuration_(
        const Duration& aDuration,
        const String& aName,
        Array<String>& errors,
        Array<String>& warnings
    );
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
```

**Implementation** in corresponding .cpp file with validation logic.

### Success Criteria

#### Automated Verification:
- [ ] All tests pass with new logging
- [ ] Statistics are correctly tracked and logged
- [ ] SequenceConstraintValidator tests pass
- [ ] No performance degradation

#### Manual Verification:
- [ ] Log output shows clear constraint violation messages
- [ ] Statistics summary is helpful for debugging
- [ ] Validator catches invalid configurations
- [ ] Logging verbosity is appropriate

---

## Phase 5: Update Test Coverage

### Overview

Add unit tests for new helper classes and methods, plus edge case tests identified in research.

### Changes Required

#### 1. Unit Tests for ManeuverConstraintChecker

**File**: `test/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.test.cpp` (NEW FILE)

```cpp
/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_ManeuverConstraintChecker, Constructor)
{
    // Test default construction
    // Test with various constraint combinations
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_ManeuverConstraintChecker, CheckMinimumDuration)
{
    // Test with constraint undefined
    // Test with maneuver below minimum
    // Test with maneuver above minimum
    // Test with maneuver exactly at minimum
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_ManeuverConstraintChecker, CheckMaximumDuration)
{
    // Test each strategy
    // Test violation detection
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_ManeuverConstraintChecker, CheckMinimumSeparation)
{
    // Test with no previous maneuver
    // Test with sufficient separation
    // Test with insufficient separation
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_ManeuverConstraintChecker, HasConstraints)
{
    // Test with no constraints
    // Test with one constraint
    // Test with all constraints
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_ManeuverConstraintChecker, ToString)
{
    // Test string representation
}
```

#### 2. Unit Tests for SequenceConstraintValidator

**File**: `test/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.test.cpp` (NEW FILE)

```cpp
/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_SequenceConstraintValidator, Validate)
{
    // Test valid configurations
    // Test inconsistent min/max durations
    // Test negative durations
    // Test zero durations
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_SequenceConstraintValidator, AreConsistent)
{
    // Test max >= min
    // Test max < min
    // Test with undefined values
}
```

#### 3. Edge Case Tests for Sequence Constraints

**File**: `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp`
**Add new test cases** (after existing constraint tests around line 1817):

```cpp
// Edge Case 1: Very short maneuvers (millisecond scale)
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_WithMillisecondConstraints)
{
    // Setup with millisecond-scale constraints
    // Verify behavior with very short maneuvers
}

// Edge Case 2: Maximum duration with Fail strategy and multiple constraints
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_WithMaxDurationFailAndOtherConstraints)
{
    // Setup with minimum duration + maximum duration (Fail) + separation
    // Verify correct exception thrown
}

// Edge Case 3: Constraint at sequence boundaries
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_WithConstraintsAtBoundaries)
{
    // Maneuver starting at T=0
    // Maneuver ending at sequence max time
    // Verify correct handling
}

// Edge Case 4: Center strategy with very short maneuver
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_WithCenterStrategyShortManeuver)
{
    // Maneuver shorter than maximum duration
    // Verify maneuver accepted as-is (not modified)
}

// Edge Case 5: Multiple consecutive segments with constraints
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_WithConstraintsAcrossThreeSegments)
{
    // Three sequential segments, each with maneuvers
    // Verify separation tracked correctly across all segments
}
```

#### 4. Update CMakeLists.txt for Test Files

**File**: `CMakeLists.txt` or test configuration
**Add**:

```cmake
# New test files
test/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.test.cpp
test/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.test.cpp
```

#### 5. Verify Python Tests Still Pass

**File**: `bindings/python/test/trajectory/test_sequence.py`
**No changes needed** - verify existing tests pass:

```bash
docker run --rm --workdir=/app --volume="$PWD:/app:delegated" \
  openspacecollective/open-space-toolkit-astrodynamics-development-non-root \
  ostk-install-python && ostk-test-python -k "test_sequence"
```

### Success Criteria

#### Automated Verification:
- [ ] All new unit tests pass: `bin/open-space-toolkit-astrodynamics.test --gtest_filter=*ManeuverConstraintChecker*`
- [ ] All new edge case tests pass: `bin/open-space-toolkit-astrodynamics.test --gtest_filter=*Sequence*`
- [ ] All existing tests still pass
- [ ] Python tests pass: `ostk-test-python -k test_sequence`
- [ ] Validation tests pass: `bin/open-space-toolkit-astrodynamics.validation`
- [ ] Test coverage remains high (can check with coverage tools if available)

#### Manual Verification:
- [ ] Edge cases are comprehensively covered
- [ ] Test names are descriptive
- [ ] Test failures provide clear error messages
- [ ] Tests document expected behavior through assertions

---

## Testing Strategy

### Unit Tests

**For ManeuverConstraintChecker**:
- Constructor with various constraint combinations
- Each constraint check method (minimum duration, maximum duration, separation)
- Edge cases: undefined constraints, boundary values, extreme durations
- String representation and utility methods

**For SequenceConstraintValidator**:
- Valid configurations
- Invalid configurations (negative durations, inconsistent min/max)
- Boundary cases (zero durations, undefined values)

**For Sequence Helper Methods**:
- Tests already exist for public API (Sequence.test.cpp:1379-1817)
- Private helper methods tested indirectly through existing tests
- New edge case tests added for comprehensive coverage

### Integration Tests

**Existing Integration Tests** (no changes needed):
- Parameterized constraint tests (test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp:1516-1602)
- Cross-segment separation tests (lines 1680-1874)
- All constraint strategies (Skip, Slice, Center, Fail)

**New Integration Tests**:
- Multiple consecutive segments (3+) with constraints
- Constraint interactions at sequence boundaries
- Very short/long maneuver durations
- Edge cases identified in research

### Validation Tests

**End-to-End Validation** (no changes needed):
- Existing YAML scenarios (020-sequence-multiple-*.yaml)
- Cross-validation against GMAT/Orekit reference data
- Framework.validation.cpp tests

**Verification**:
- All validation tests must continue to pass
- Reference data matches within tolerance

### Manual Testing Steps

1. **Build and Run Tests**:
   ```bash
   docker run --rm --workdir=/app --volume="$PWD:/app:delegated" \
     openspacecollective/open-space-toolkit-astrodynamics-development-non-root \
     ostk-build && bin/open-space-toolkit-astrodynamics.test
   ```

2. **Test Specific Constraint Scenarios**:
   ```bash
   bin/open-space-toolkit-astrodynamics.test --gtest_filter=*ManeuverConstraint*
   ```

3. **Run Python Tests**:
   ```bash
   docker run --rm --workdir=/app --volume="$PWD:/app:delegated" \
     openspacecollective/open-space-toolkit-astrodynamics-development-non-root \
     ostk-install-python && ostk-test-python -k test_sequence
   ```

4. **Run Validation Tests**:
   ```bash
   docker run --rm --workdir=/app --volume="$PWD:/app:delegated" \
     openspacecollective/open-space-toolkit-astrodynamics-development-non-root \
     bin/open-space-toolkit-astrodynamics.validation
   ```

5. **Verify Logging Output**:
   - Run tests with debug logging enabled
   - Check for structured constraint violation messages
   - Verify statistics are logged correctly

6. **Code Review**:
   - Confirm readability improvements in `solveSegment_`
   - Verify documentation is comprehensive
   - Check that helper methods have single responsibilities

## Performance Considerations

### Expected Performance Impact

**Minimal to None** - This refactoring focuses on code organization, not algorithm changes:

- **Helper method calls**: Inlined by compiler at optimization levels (-O2/-O3)
- **ManeuverConstraintChecker creation**: Lightweight object with 4 member variables
- **Enhanced logging**: Only active at debug level; production builds can disable
- **Statistics tracking**: Simple counters, negligible overhead

### Performance Verification

1. **Run Existing Benchmarks** (if available):
   ```bash
   docker run --rm --workdir=/app --volume="$PWD:/app:delegated" \
     openspacecollective/open-space-toolkit-astrodynamics-development-non-root \
     bin/open-space-toolkit-astrodynamics.benchmark
   ```

2. **Compare Validation Test Timing**:
   - Before refactoring: Run validation tests and record time
   - After refactoring: Run same tests and compare
   - Acceptable tolerance: ±5% variation

3. **Profile if Needed**:
   - If performance regression detected, use profiler (e.g., gprof, valgrind)
   - Identify hotspots and optimize accordingly

### Optimization Opportunities (If Needed)

- **Lazy initialization** of ManeuverConstraintChecker if creation overhead is measurable
- **Pre-computed values** in constraint checker (e.g., cache string representations)
- **Conditional compilation** for logging (use `#ifdef BOOST_LOG_DYN_LINK`)

---

## Migration Notes

### For Users

**No migration required** - This is an internal refactoring with no API changes:

- All public methods remain unchanged
- Python bindings unchanged
- Sequence constructor parameters unchanged
- Constraint setters/getters unchanged
- Behavior identical (all tests pass)

### For Developers

**Internal changes only**:

1. **New Files**:
   - `include/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.hpp`
   - `src/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.cpp`
   - `include/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.hpp`
   - `src/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.cpp`
   - `test/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.test.cpp`
   - `test/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.test.cpp`

2. **Modified Files**:
   - `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp` - New private helper methods
   - `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp` - Refactored implementation
   - `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp` - New edge case tests

3. **Build System**:
   - Update CMakeLists.txt to include new source and test files
   - No changes to dependencies

### Backward Compatibility

**100% backward compatible**:
- Binary compatibility: No ABI changes
- Source compatibility: No API changes
- Behavior compatibility: All tests pass

---

## References

### Original Research
- Research document: `RESEARCH_sequence_constraint_simplification.md` (2025-11-11)
- Key conclusion: Preserve architecture, improve readability through internal refactoring

### Related Implementation
- Current implementation: `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:501-726`
- Helper method: `buildThrusterDynamicsWithManeuverIntervals_` (lines 728-744)

### Test Coverage
- C++ unit tests: `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp:1379-1817`
- Python tests: `bindings/python/test/trajectory/test_sequence.py:431-480`
- Validation: `validation/OpenSpaceToolkit/Astrodynamics/Framework.validation.cpp`

### Code Patterns
- Similar refactoring: `Segment::solve_()` and `Segment::solveWithDynamics_()`
- Validation pattern: `COE::FrozenOrbit()` parameter validation
- Builder pattern: `SatelliteSystemBuilder`, `StateBuilder`

### Documentation
- Algorithm overview: Will be added to `Sequence.hpp:290-307`
- State machine: Will be documented with ASCII diagrams
- Examples: Will be added inline in implementation

---

## Appendix: File Change Summary

### New Files (8)
1. `include/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.hpp`
2. `src/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.cpp`
3. `include/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.hpp`
4. `src/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.cpp`
5. `test/OpenSpaceToolkit/Astrodynamics/Trajectory/ManeuverConstraintChecker.test.cpp`
6. `test/OpenSpaceToolkit/Astrodynamics/Trajectory/SequenceConstraintValidator.test.cpp`
7. `docs/constraint-algorithm.md` (optional documentation)
8. Updated `CMakeLists.txt`

### Modified Files (3)
1. `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp`
   - Add private helper method declarations (lines ~320-360)
   - Add ConstraintStatistics struct (lines ~140-165)
   - Add enhanced documentation to solveSegment_ (lines ~290-380)

2. `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp`
   - Refactor solveSegment_ implementation (lines 501-726)
   - Add helper method implementations (lines ~745-900)
   - Add structured logging helpers (~745-770)

3. `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp`
   - Add edge case tests (lines ~1820-2050)

### Unchanged Files (Critical Verification)
- All Python binding files (no API changes)
- All validation scenario files (behavior unchanged)
- All reference data files (results identical)

---

## Conclusion

This comprehensive refactoring plan improves the readability, maintainability, and testability of the Sequence constraint logic while preserving the sound architectural design. The phased approach allows incremental implementation with verification at each step, minimizing risk and ensuring all tests continue to pass.

**Key Benefits**:
1. ✅ **Improved Readability**: 225-line method broken into focused helpers with clear responsibilities
2. ✅ **Better Testability**: ManeuverConstraintChecker can be tested independently
3. ✅ **Enhanced Documentation**: State machine diagrams and comprehensive examples
4. ✅ **Better Observability**: Structured logging with statistics
5. ✅ **Zero Breaking Changes**: 100% backward compatible, no API modifications
6. ✅ **Comprehensive Testing**: New tests for edge cases, all existing tests pass

**Risk Mitigation**:
- Phased implementation with verification after each phase
- All existing tests must pass before proceeding
- No behavior changes (verified through test suite)
- Code review at each phase
- Performance verification through validation tests

**Estimated Effort**:
- Phase 1: 16-24 hours (helper method extraction)
- Phase 2: 20-32 hours (ManeuverConstraintChecker creation and integration)
- Phase 3: 12-16 hours (documentation enhancement)
- Phase 4: 16-24 hours (logging and validation helpers)
- Phase 5: 20-32 hours (test coverage updates)
- **Total: 84-128 hours (2-3 weeks)**

This plan follows the research recommendation to preserve the current architecture and make only internal improvements for readability and maintainability.