# Copyright © Loft Orbital Solutions Inc.


from pathlib import Path

import gc
import memray
from memray._stats import Stats
from memray._memray import (  # pylint: disable=no-name-in-module
    compute_statistics,
)
from memray.reporters.stats import StatsReporter


# Decorator to track memory usage
def track_memory_usage_memray(func):
    def wrapper(*args, **kwargs):

        file_dest = memray.FileDestination("memory-profile.bin", overwrite=True)
        # with memray.Tracker(str(Path("app/bindings/python/test/memory-profile.bin"))):
        with memray.Tracker(destination=file_dest, native_traces=True):
            # Execute the function
            result = func(*args, **kwargs)

            # Trigger garbage collection
            gc.collect()

        stats: Stats = compute_statistics(
            file_name="memory-profile.bin",
            report_progress=False,
            num_largest=10,
        )
        reporter: StatsReporter = StatsReporter(stats, num_largest=10)

        reporter.render()

        return result

    return wrapper
