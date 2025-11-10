---
date: 2025-11-11T02:49:33+00:00
researcher: Claude
git_commit: 4b4f2457c51737092fdf5bcd7c4887bcf05ef4b7
branch: feat/add-sequence-solving-capabilities
repository: open-space-toolkit-astrodynamics
topic: "Simplifying Sequence Constraint Logic: Analysis and Refactoring Opportunities"
tags: [research, codebase, sequence, segment, constraints, refactoring, architecture]
status: complete
last_updated: 2025-11-11
last_updated_by: Claude
---

# Research: Simplifying Sequence Constraint Logic and Potential Migration to Segment

**Date**: 2025-11-11T02:49:33+00:00
**Researcher**: Claude
**Git Commit**: 4b4f2457c51737092fdf5bcd7c4887bcf05ef4b7
**Branch**: feat/add-sequence-solving-capabilities
**Repository**: open-space-toolkit-astrodynamics

## Research Question

How can the logic in `Sequence.solveSegment_` that handles minimum duration, maximum duration, and minimum separation constraints be simplified? Can this logic be transferred from the `Sequence.solveSegment_` method to a `solve` method within the `Segment` class that applies the constraints directly?

## Executive Summary

After comprehensive analysis of the constraint handling architecture in OSTk Astrodynamics, I've identified that **the current design is well-architected and should largely be preserved**, with only minor simplification opportunities. Moving constraint logic from `Sequence` to `Segment` would **violate separation of concerns** and create architectural problems.

### Key Findings:

1. **Current Architecture is Sound**: The separation between `Segment` (atomic propagation unit) and `Sequence` (mission orchestrator with constraints) follows established design principles.

2. **Constraint Logic Belongs in Sequence**: The three constraint types (minimum/maximum maneuver duration, minimum separation) are fundamentally **sequence-level concerns** requiring state tracking across multiple segments.

3. **Limited Simplification Opportunities**: The 225-line `solveSegment_` method is complex by necessity due to its iterative constraint-checking algorithm. Minor refactoring could improve readability without architectural changes.

4. **Moving Logic to Segment Would Be Harmful**: This would require `Segment` to know about sequence-level state, violate single responsibility principle, and break the clean abstraction layers.

### Recommended Approach:

**Preserve the current architecture** with these minor improvements:
- Extract helper methods from `solveSegment_` for better readability
- Add documentation explaining the constraint application algorithm
- Consider a `ConstraintChecker` utility class to encapsulate constraint evaluation logic
- Keep the constraint enforcement in `Sequence` where it belongs

## Detailed Findings

### 1. Current Architecture Analysis

#### Class Responsibilities

**Segment** (`Segment.hpp:59`, `Segment.cpp:464`)
- Represents a single atomic propagation unit
- Has an event condition that determines when the segment ends
- Can be either Coast (no thrust) or Maneuver (with thrust)
- Stateless: given initial state + configuration → produces solution
- Key methods:
  - `solve()` - propagate until event condition satisfied
  - `solveToNextManeuver()` - propagate until next maneuver ends (for constraint checking)
  - `toCoastSegment()` / `toManeuverSegment()` - create modified variants

**Sequence** (`Sequence.hpp:46`, `Sequence.cpp:199`)
- Orchestrates execution of multiple segments in order
- Manages segment repetitions and global termination conditions
- **Enforces maneuver constraints** (min/max duration, separation)
- Tracks state across segments (last maneuver interval for separation)
- Key constraint-related members:
  - `minimumManeuverDuration_` - filter out maneuvers too short
  - `maximumManeuverDuration_` - handle maneuvers too long
  - `minimumManeuverSeparation_` - enforce gaps between maneuvers
  - `maximumManeuverDurationStrategy_` - Fail/Skip/Slice/Center

#### Constraint Handling Logic Location

The constraint enforcement lives entirely in `Sequence::solveSegment_()` at `Sequence.cpp:501-726`.

**High-Level Algorithm**:
```cpp
1. Check if segment is maneuver type and constraints are defined
2. If coast or no constraints → call simple segment.solve() and return
3. For maneuver segments with constraints:
   a. Initialize unified solution accumulators (states, maneuvers, guidance law)
   b. Check if need to coast first (minimum separation from previous maneuver)
   c. While not at maximum propagation time:
      - If coasting: solve coast subsegment
      - If maneuvering: solve to next maneuver candidate
      - Extract maneuver from subsegment
      - Check minimum duration constraint → skip if too short
      - Check maximum duration constraint → apply strategy (Fail/Skip/Slice/Center)
      - If accepted: add maneuver to unified solution
      - Insert coast for minimum separation before next maneuver
   d. Build unified segment solution with heterogeneous guidance law
   e. Return unified solution + last maneuver interval
```

This iterative approach allows inspecting and modifying maneuvers before accepting them into the final trajectory.

### 2. Constraint Handling Logic Deep Dive

#### Location and Structure

**File**: `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:501-726`
**Method Signature**:
```cpp
Tuple<Segment::Solution, Interval> Sequence::solveSegment_(
    const State& aState,
    const Segment& aSegment,
    const Duration& aMaximumPropagationDuration,
    const Interval& aLastManeuverInterval
) const
```

#### Constraint Types Implemented

**1. Minimum Maneuver Duration** (`Sequence.cpp:597-609`)
- **Purpose**: Skip maneuvers shorter than threshold (e.g., impractically short burns)
- **Implementation**: After detecting maneuver candidate, check duration against `minimumManeuverDuration_`
- **Action if violated**: Convert segment to coast, skip maneuver, continue solving
- **Example**: 30-second maneuver with 2-minute minimum → skipped

**2. Maximum Maneuver Duration** (`Sequence.cpp:612-683`)
- **Purpose**: Handle maneuvers exceeding operational limits
- **Implementation**: Check duration against `maximumManeuverDuration_` with four strategies:
  - **Fail** (lines 620-626): Throw exception, terminate sequence
  - **Skip** (lines 628-636): Skip entire maneuver, coast to next opportunity
  - **Slice** (lines 638-656): Slice maneuver into chunks ≤ max duration
  - **Center** (lines 658-676): Take center portion of maneuver with max duration
- **Example**: 30-minute maneuver with 20-minute max + Slice → two 20-min + one 10-min burns

**3. Minimum Maneuver Separation** (`Sequence.cpp:537-543, 706-709`)
- **Purpose**: Ensure recovery/cooldown time between consecutive maneuvers
- **Implementation**: Track `lastManeuverInterval`, force coast if too close
- **Scope**: Applies across segment boundaries and sequence repetitions
- **Example**: Maneuver ends at T+10min, next starts at T+12min with 5min separation → forced coast to T+15min

#### Key Implementation Details

**Iterative Subsegment Solving** (`Sequence.cpp:547-711`)
- Uses `segment.solveToNextManeuver()` instead of full `solve()`
- Allows inspecting each maneuver before accepting it
- Builds unified solution incrementally across iterations

**HeterogeneousGuidanceLaw Pattern** (`Sequence.cpp:520, 693-695`)
- Instead of re-solving entire segment, modifies guidance law timing
- Each accepted maneuver adds time interval to heterogeneous guidance
- Final unified dynamics applies original guidance only during accepted intervals
- Maintains state continuity, improves performance

**Coast Insertion** (`Sequence.cpp:542, 603, 630, 705`)
- Creates temporary coast segments on-the-fly via `toCoastSegment()`
- Used for: separation enforcement, skipping rejected maneuvers
- Descriptive names added: "(Coast - Minimum Maneuver Separation Constraint)"

**Helper Method**: `buildThrusterDynamicsWithManeuverIntervals_` (`Sequence.cpp:728-744`)
- Constructs modified thruster with time-windowed guidance law
- Used by Slice/Center strategies to create shortened maneuvers
- Takes original guidance law, wraps in HeterogeneousGuidanceLaw with specific intervals

### 3. Coupling and Dependency Analysis

#### What Sequence Calls on Segment

**Primary Solving Methods**:
- `Segment::solve()` - full propagation (`Sequence.cpp:513`)
- `Segment::solveToNextManeuver()` - incremental propagation (`Sequence.cpp:571`)

**Transformation Methods**:
- `Segment::toCoastSegment()` - create coast variant (`Sequence.cpp:542, 603, 630, 705`)
- `Segment::toManeuverSegment()` - create maneuver variant (`Sequence.cpp:649, 670`)

**Accessors**:
- `Segment::getType()` - check Coast vs Maneuver (`Sequence.cpp:509, 550`)
- `Segment::getName()` - for logging and naming modified segments
- `Segment::getThrusterDynamics()` - extract thruster for modification (`Sequence.cpp:732`)
- `Segment::getFreeDynamics()` - extract non-thruster dynamics (`Sequence.cpp:713`)

**From Segment::Solution**:
- `states` - trajectory array for state extraction and continuation
- `conditionIsSatisfied` - check if segment event condition met
- `extractManeuvers()` - get maneuver intervals to check constraints (`Sequence.cpp:576`)
- `getThrusterDynamics()` - access guidance law (`Sequence.cpp:694`)

#### What Segment Would Need if Logic Moved

If constraint logic moved from `Sequence::solveSegment_` to `Segment::solve()`, the `Segment` class would need:

**Constraint Configuration** (currently in Sequence):
- `minimumManeuverDuration_`
- `maximumManeuverDuration_`
- `minimumManeuverSeparation_`
- `maximumManeuverDurationStrategy_`

**Sequence-Level State**:
- `lastManeuverInterval` - tracks previous maneuver end time across segments
- This carries state across multiple segment solves within a sequence

**Iterative Solving Infrastructure**:
- Unified states accumulation across subsegments
- Unified maneuver intervals tracking
- Unified guidance law building (HeterogeneousGuidanceLaw)
- State machine for coast vs. maneuver transitions
- Next instant to coast to (coordination logic)

**Problems with This Approach**:
1. **Breaks Statelessness**: Segment would need to track sequence state
2. **Violates SRP**: Segment becomes responsible for both propagation AND constraint enforcement
3. **Tight Coupling**: Segment would depend on sequence-level concepts
4. **Reusability Loss**: Can't use Segment standalone without constraint machinery
5. **Testing Complexity**: Would need to test Segment with all constraint combinations
6. **Architectural Confusion**: Blurs the clean layer separation

### 4. Usage Patterns from Codebase

#### Test Coverage

**C++ Unit Tests**:
- `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp` - comprehensive constraint testing
- `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.test.cpp` - segment functionality
- Parameterized tests covering all constraint scenarios and strategies

**Python Tests**:
- `bindings/python/test/trajectory/test_sequence.py` - Python API validation
- `bindings/python/test/trajectory/test_segment.py` - bindings verification

**Validation Tests**:
- `validation/OpenSpaceToolkit/Astrodynamics/Framework.validation.cpp` - cross-validation vs GMAT/Orekit
- YAML-based scenario definitions for end-to-end mission validation

#### Real-World Usage Examples

**Constraint Configuration Pattern**:
```cpp
Sequence sequence = {segments, solver, dynamics, maxPropDuration};

// Configure constraints
sequence.setMinimumManeuverDuration(Duration::Minutes(2.0));
sequence.setMaximumManeuverDuration(Duration::Minutes(20.0));
sequence.setMinimumManeuverSeparation(Duration::Minutes(10.0));
sequence.setMaximumManeuverDurationStrategy(
    Sequence::MaximumManeuverDurationViolationStrategy::Slice
);

// Solve with constraints applied
Sequence::Solution solution = sequence.solve(initialState);
```

**Constraint Behavior Examples** (from tests):

1. **Minimum Duration** - 10-minute threshold:
   - Input: Maneuvers at [0-7min], [10-21min], [25-30min], [50-70min]
   - Output: [10-21min], [50-70min] (short maneuvers skipped)

2. **Maximum Duration Slice** - 10-minute limit:
   - Input: Maneuver at [0-19min]
   - Output: [0-10min], [13-19min] (split with 3-min separation)

3. **Maximum Duration Center** - 10-minute limit:
   - Input: Maneuver at [10-30min] (center = 20min)
   - Output: [15-25min] (centered 10-min window)

4. **Separation Across Segments**:
   - Segment 1: Maneuver at [0-12min]
   - Segment 2: Attempted maneuvers at [16-20min], [23-28min]
   - With 10-min separation: [16-20min] skipped, [23-28min] accepted

### 5. Architectural Design Philosophy

#### Design Principles Observed

**1. Layered Architecture**
- **Propagator Layer**: Numerical integration (RK45, etc.)
- **Segment Layer**: Event-based propagation units
- **Sequence Layer**: Mission orchestration with constraints
- Each layer builds on the previous without leaking concerns upward

**2. Separation of Concerns**
- `Segment`: "How do I propagate until an event occurs?"
- `Sequence`: "How do I execute a mission with operational constraints?"
- Clear boundary: propagation physics vs. mission operations

**3. Composition Over Inheritance**
- Segments composed into Sequences
- Dynamics composed into Segments
- Guidance laws composed via HeterogeneousGuidanceLaw
- Flexible, testable, modular

**4. Immutability Pattern**
- Segment/Sequence are configuration objects
- Solutions are separate, immutable result objects
- Original segments unchanged by solving
- Enables reuse and predictable behavior

**5. Strategy Pattern**
- MaximumManeuverDurationViolationStrategy selects behavior at configuration time
- Clean extensibility for new strategies
- No runtime conditionals scattered through code

#### Similar Patterns in Codebase

**Propagator** (`Trajectory/Propagator.hpp`):
- Also uses stateless design
- Configuration (solver, dynamics) separate from execution (state propagation)
- Solutions returned as separate objects

**EventCondition Hierarchy** (`EventCondition.hpp`):
- Encapsulates termination logic separate from propagation
- Composed into Segments, not baked in
- Similar separation of concerns

**No Architectural Debt Found**:
- No TODO/FIXME comments related to constraint refactoring
- Design is intentional and well-tested
- Constraint architecture considered stable

## Simplification Opportunities

### Minor Refactoring Options (Preserve Architecture)

While the overall architecture should be maintained, the 225-line `solveSegment_` method could benefit from **internal refactoring for readability**:

#### 1. Extract Constraint Checker Methods

**Current**: Inline constraint checking logic (lines 597-609, 612-683)

**Proposed**: Extract to private methods
```cpp
private:
    // Returns true if maneuver should be accepted, false if should be skipped
    bool checkMinimumDurationConstraint_(const Maneuver& maneuver) const;

    // Returns action to take: Accept, Skip, or ModifiedManeuver
    ManeuverConstraintAction checkMaximumDurationConstraint_(
        const Maneuver& maneuver,
        const Segment& segment
    ) const;

    // Returns instant to coast to, or Undefined if no separation needed
    Instant calculateMinimumSeparationCoastTarget_(
        const Interval& lastManeuverInterval,
        const Instant& currentInstant
    ) const;
```

**Benefits**:
- Improves readability of `solveSegment_` main loop
- Makes constraint logic more testable in isolation
- Reduces cognitive load per method

#### 2. Create ConstraintChecker Utility Class (Optional)

**Proposed Structure**:
```cpp
class ManeuverConstraintChecker
{
public:
    ManeuverConstraintChecker(
        const Duration& minimumDuration,
        const Duration& maximumDuration,
        const Duration& minimumSeparation,
        const MaximumManeuverDurationViolationStrategy& strategy
    );

    bool satisfiesMinimumDuration(const Maneuver& maneuver) const;
    ConstraintViolation checkMaximumDuration(const Maneuver& maneuver) const;
    bool satisfiesMinimumSeparation(
        const Instant& proposedStart,
        const Interval& lastManeuverInterval
    ) const;

private:
    Duration minimumDuration_;
    Duration maximumDuration_;
    Duration minimumSeparation_;
    MaximumManeuverDurationViolationStrategy strategy_;
};
```

**Usage in Sequence**:
```cpp
// In solveSegment_
ManeuverConstraintChecker checker(
    minimumManeuverDuration_,
    maximumManeuverDuration_,
    minimumManeuverSeparation_,
    maximumManeuverDurationStrategy_
);

if (!checker.satisfiesMinimumDuration(maneuverCandidate)) {
    // Skip maneuver
}
```

**Benefits**:
- Encapsulates constraint evaluation logic
- Could be reused in other contexts (e.g., pre-validation)
- Testable independently of Sequence

**Concerns**:
- Adds another class to maintain
- May be overkill for current needs
- Constraint logic still fundamentally sequence-level

#### 3. Add Documentation

**Current**: Limited inline comments

**Proposed**: Comprehensive documentation
- High-level algorithm overview in method docstring
- State machine diagram for coast/maneuver transitions
- Examples showing how each constraint type affects solving
- Complexity analysis (why 225 lines is necessary)

### Why NOT to Move Logic to Segment

#### Architectural Arguments

**1. Violates Single Responsibility Principle**
- Segment's responsibility: Propagate from state A to state B given event condition
- Sequence's responsibility: Orchestrate multiple segments with mission constraints
- Constraints are mission-level concerns, not propagation concerns

**2. Breaks Stateless Design**
- Segment is currently stateless: f(initial_state, config) → solution
- Constraints require tracking state across segments (last maneuver interval)
- Making Segment stateful would break this clean abstraction

**3. Creates Tight Coupling**
- Segment would need to know about sequence-level concepts
- Segment standalone use cases (single-segment propagation) don't need constraints
- Would complicate Segment API and testing

**4. Reduces Reusability**
- Current design: Can use Segment independently for simple propagation
- With constraints in Segment: Would need to provide constraint config even for simple cases
- Forces all Segment users to understand constraint machinery

**5. Testing Explosion**
- Segment tests would need to cover all constraint combinations
- Currently: Segment tests focus on propagation, Sequence tests focus on constraints
- Clean separation makes testing tractable

#### Implementation Complexity Concerns

**The Iterative Algorithm Cannot Be Fully Encapsulated in Segment**:

The constraint enforcement algorithm fundamentally requires:
1. Solving to next maneuver
2. Inspecting maneuver properties
3. Deciding whether to accept/reject/modify
4. Continuing propagation with modified dynamics
5. Tracking state for next iteration

This is inherently a **multi-pass algorithm** that operates **above** the segment level. It can't be pushed down to Segment without:
- Giving Segment knowledge of how to modify itself
- Creating complex callback mechanisms
- Implementing a state machine in Segment
- Breaking the clean propagation abstraction

**Example**: The Slice strategy requires:
1. Detect maneuver exceeds maximum
2. Create new thruster with shortened interval
3. Solve with shortened thruster
4. Continue solving for remaining duration
5. Repeat if next maneuver also exceeds maximum

This multi-step process requires orchestration that belongs in Sequence, not Segment.

## Recommendations

### Primary Recommendation: **Preserve Current Architecture**

The current design is sound and should be maintained with only minor improvements:

1. **Keep Constraint Logic in Sequence**
   - Constraints are mission-level concerns
   - Sequence is the appropriate orchestration layer
   - Separation of concerns is correct

2. **Improve Readability via Internal Refactoring**
   - Extract constraint checking methods from `solveSegment_`
   - Add comprehensive documentation
   - Consider ConstraintChecker utility class if complexity grows

3. **Maintain Segment Statelessness**
   - Do not add sequence-level state to Segment
   - Keep Segment focused on atomic propagation
   - Preserve clean abstraction layers

### Secondary Recommendations

**1. Documentation Improvements**
- Add algorithm overview diagram to `Sequence.hpp` docstring
- Document constraint application order and interaction
- Provide examples in comments showing constraint behavior

**2. Consider Constraint Validation Helper**
If constraint logic grows more complex, consider extracting validation logic:
```cpp
class SequenceConstraintValidator {
    bool isConsistent(const Duration& min, const Duration& max);
    void validate(const Duration& duration, const String& name);
};
```

**3. Logging Enhancements**
The existing BOOST_LOG_TRIVIAL usage is good, but could be enhanced:
- Log when maneuvers are skipped with specific constraint violation reason
- Log statistics: X maneuvers accepted, Y skipped, Z split

**4. Performance Profiling**
If performance is a concern, profile the iterative solving loop:
- Is `solveToNextManeuver()` called unnecessarily?
- Can unified solution building be optimized?
- Are there redundant state transformations?

## Alternatives Considered

### Alternative 1: Segment with Constraint Parameters

**Idea**: Add optional constraint parameters to `Segment::solve()`
```cpp
Segment::Solution solve(
    const State& initialState,
    const Duration& maxPropagationDuration,
    const ManeuverConstraints& constraints = ManeuverConstraints::None()
) const;
```

**Pros**:
- Keeps constraints optional for simple use cases
- Doesn't require stateful Segment

**Cons**:
- Still requires Segment to implement constraint checking logic
- Can't handle cross-segment separation constraint (no last maneuver interval)
- Doesn't simplify `solveSegment_` complexity (just moves it)

**Verdict**: Not recommended - doesn't solve the stated problem and breaks separation of concerns.

### Alternative 2: Constraint as Event Conditions

**Idea**: Implement constraints as special EventCondition types
```cpp
class MaximumManeuverDurationCondition : public EventCondition { ... };
```

**Pros**:
- Uses existing EventCondition abstraction
- Could be composed with other conditions

**Cons**:
- EventConditions are for termination, not for modifying/accepting/rejecting maneuvers
- Can't handle Slice/Center strategies
- Can't track state across segments (separation)
- Semantic mismatch

**Verdict**: Not recommended - constraints are not event conditions.

### Alternative 3: Constraint as Dynamics

**Idea**: Implement constraints as a Dynamics type that modifies contributions
```cpp
class ConstraintDynamics : public Dynamics { ... };
```

**Cons**:
- Dynamics compute accelerations, not make accept/reject decisions
- Completely wrong abstraction layer
- Would require dynamics to modify other dynamics
- Semantic mismatch

**Verdict**: Strongly not recommended - misuse of Dynamics abstraction.

### Alternative 4: Post-Processing Solution

**Idea**: Let Segment solve freely, then post-process solution to apply constraints
```cpp
Segment::Solution rawSolution = segment.solve(state, maxDuration);
Segment::Solution constrained = applyConstraints(rawSolution, constraints);
```

**Cons**:
- Can't retroactively apply constraints to a solved trajectory
- Would need to re-solve with modified dynamics anyway
- Doesn't reduce complexity
- May require multiple full propagations (inefficient)

**Verdict**: Not recommended - constraints must be applied during solving, not after.

## Implementation Impact Analysis

If constraint logic were moved to Segment (not recommended), the impact would be:

### Files Requiring Changes

**Major Changes**:
- `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp` - add constraint parameters and methods
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.cpp` - implement constraint logic (copy from Sequence)
- `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp` - remove constraint members
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp` - simplify `solveSegment_` (but how to handle cross-segment state?)

**Testing Impact**:
- `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.test.cpp` - add constraint tests
- `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp` - update to test through Segment
- `validation/**/*.cpp` - verify no regression in end-to-end validation

**Python Bindings**:
- `bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Segment.cpp` - expose new constraint API
- `bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Sequence.cpp` - remove constraint API
- `bindings/python/test/trajectory/test_segment.py` - add constraint tests

**Breaking Changes**:
- Existing Sequence constraint API would be removed
- Users would need to configure constraints on each Segment instead of once on Sequence
- Less intuitive: constraints are mission-level, not segment-level

### Estimated Effort

If this were pursued (again, not recommended):
- Implementation: 40-80 hours
- Testing: 20-40 hours
- Documentation: 8-16 hours
- Code review and refinement: 16-32 hours
- **Total: 84-168 hours (2-4 weeks)**

And for what gain? The logic complexity remains the same, just in a different place. The architecture becomes worse.

## Code References

### Key Implementation Files

- `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp:59-380` - Segment class definition
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.cpp:464-859` - Segment implementation
- `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp:46-335` - Sequence class definition
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:199-789` - Sequence implementation

### Constraint Logic Locations

- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:501-726` - `solveSegment_` method
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:537-543` - minimum separation check (before segment)
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:597-609` - minimum duration constraint
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:612-683` - maximum duration constraint with strategies
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:706-709` - minimum separation check (after maneuver)
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:728-744` - `buildThrusterDynamicsWithManeuverIntervals_` helper

### Constraint Configuration

- `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp:51-58` - MaximumManeuverDurationViolationStrategy enum
- `include/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp:325-328` - constraint member variables
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.cpp:307-368` - constraint setter methods with validation

### Test Coverage

- `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp:1379-1817` - parameterized constraint tests
- `test/OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.test.cpp:733-970` - getter/setter tests
- `bindings/python/test/trajectory/test_sequence.py:328-480` - Python constraint API tests

### Supporting Infrastructure

- `include/OpenSpaceToolkit/Astrodynamics/GuidanceLaw/HeterogeneousGuidanceLaw.hpp` - time-windowed guidance law composition
- `include/OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp:56` - thruster getGuidanceLaw() method
- `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.cpp:173-277` - extractManeuvers() implementation

## Conclusion

The research question asked how to simplify constraint logic and whether to move it from `Sequence.solveSegment_` to `Segment`. After comprehensive analysis:

**On Simplification**:
- The 225-line method is complex because the algorithm is inherently complex
- Minor refactoring for readability is recommended (extract helper methods, improve documentation)
- The complexity cannot be eliminated, only reorganized

**On Moving to Segment**:
- **This is not recommended** - it would violate architectural principles
- Constraints are sequence-level concerns requiring cross-segment state tracking
- Moving logic would break Segment's clean, stateless design
- The current separation of concerns is appropriate and well-executed

**Final Recommendation**:
**Preserve the current architecture**. The design is sound. Make only minor improvements:
1. Extract constraint checking helper methods for readability
2. Add comprehensive documentation explaining the algorithm
3. Consider a ConstraintChecker utility if complexity grows further

The constraint logic belongs in `Sequence` where it is. The architectural layers are correct. Don't fix what isn't broken.
