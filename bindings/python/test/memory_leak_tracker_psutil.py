# Copyright © Loft Orbital Solutions Inc.

import psutil
import os
import time
import gc


# Function to monitor memory usage
def monitor_memory_usage():
    process = psutil.Process(os.getpid())
    mem_info = process.memory_info()
    return mem_info.rss / (1024 * 1024)  # Return memory usage in MB


# Decorator to track memory usage
def track_memory_usage_psutil(func):
    def wrapper(*args, **kwargs):
        # Memory usage before function execution
        mem_before = monitor_memory_usage()
        print(f"Memory usage before {func.__name__}: {mem_before:.2f} MB")

        # Execute the function
        result = func(*args, **kwargs)

        # Trigger garbage collection
        gc.collect()

        # Memory usage after function execution
        mem_after = monitor_memory_usage()
        print(f"Memory usage after {func.__name__}: {mem_after:.2f} MB")

        # Memory leak
        mem_diff = mem_after - mem_before
        print(f"Memory change in {func.__name__}: {mem_diff:.2f} MB")

        return result

    return wrapper


# Example function to test memory usage
@track_memory_usage
def example_function():
    # Simulate some memory usage
    data = [x**2 for x in range(1000000)]

    # Clear the data to release memory
    # del data


if __name__ == "__main__":
    result = example_function()
