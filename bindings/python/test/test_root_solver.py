# Apache License 2.0

import pytest
import math

from ostk.astrodynamics import RootSolver


def quadratic_function(x):
    return x**2 - 4


@pytest.fixture
def maximum_iterations_count() -> int:
    return 100


@pytest.fixture
def number_of_digits() -> int:
    return 27


@pytest.fixture
def root_solver(maximum_iterations_count: int, number_of_digits: int) -> RootSolver:
    return RootSolver(maximum_iterations_count, number_of_digits)


class TestRootSolver:
    def test_constructor(self, maximum_iterations_count: int, number_of_digits: int):
        assert RootSolver(maximum_iterations_count, number_of_digits) is not None

    def test_solve_with_initial_guess(self, root_solver):
        solution = root_solver.solve(quadratic_function, 1.0, True)
        assert pytest.approx(solution.root, abs=1e-15) == 2.0

    def test_default(self):
        assert RootSolver.default() is not None

    def test_getters(
        self,
        root_solver: RootSolver,
        maximum_iterations_count: int,
        number_of_digits: int,
    ):
        assert root_solver.get_maximum_iterations_count() == maximum_iterations_count
        assert root_solver.get_number_of_digits() == number_of_digits
