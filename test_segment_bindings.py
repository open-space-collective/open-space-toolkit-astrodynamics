#!/usr/bin/env python3

"""
Test script for Segment ManeuverConstraints Python bindings
"""

from ostk.physics.time import Duration
from ostk.astrodynamics.trajectory import Segment

# Test MaximumManeuverDurationViolationStrategy enum
print("Testing MaximumManeuverDurationViolationStrategy enum...")
assert hasattr(Segment, 'MaximumManeuverDurationViolationStrategy')
strategy_fail = Segment.MaximumManeuverDurationViolationStrategy.Fail
strategy_skip = Segment.MaximumManeuverDurationViolationStrategy.Skip
strategy_slice = Segment.MaximumManeuverDurationViolationStrategy.Slice
strategy_center = Segment.MaximumManeuverDurationViolationStrategy.Center
print("  ✓ All strategies accessible")

# Test ManeuverConstraints class
print("\nTesting ManeuverConstraints class...")
assert hasattr(Segment, 'ManeuverConstraints')

# Test default constructor
constraints1 = Segment.ManeuverConstraints()
assert not constraints1.is_defined()
print("  ✓ Default constructor works")

# Test parameterized constructor
min_dur = Duration.minutes(1.0)
max_dur = Duration.minutes(10.0)
min_sep = Duration.minutes(5.0)
constraints2 = Segment.ManeuverConstraints(
    min_dur,
    max_dur,
    min_sep,
    Segment.MaximumManeuverDurationViolationStrategy.Slice
)
assert constraints2.is_defined()
assert constraints2.minimum_duration == min_dur
assert constraints2.maximum_duration == max_dur
assert constraints2.minimum_separation == min_sep
assert constraints2.maximum_duration_strategy == Segment.MaximumManeuverDurationViolationStrategy.Slice
print("  ✓ Parameterized constructor works")
print("  ✓ All fields accessible")
print("  ✓ is_defined() method works")

# Test field modification
constraints2.minimum_duration = Duration.minutes(2.0)
assert constraints2.minimum_duration == Duration.minutes(2.0)
print("  ✓ Fields are modifiable")

print("\n✅ All Python binding tests passed!")
