/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/RANSAC.hpp>

// Trampoline so Python subclasses can override `filter`.
class PyObservationFilter : public ostk::astrodynamics::estimator::ObservationFilter
{
   public:
    using ostk::astrodynamics::estimator::ObservationFilter::ObservationFilter;

    ostk::core::type::Shared<const ostk::astrodynamics::estimator::ObservationFilter::Analysis> filter(
        const ostk::core::container::Array<ostk::astrodynamics::trajectory::State>& anObservationStateArray
    ) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ostk::core::type::Shared<const ostk::astrodynamics::estimator::ObservationFilter::Analysis>,
            ostk::astrodynamics::estimator::ObservationFilter,
            "filter",
            filter,
            anObservationStateArray
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator_ObservationFilter(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;
    using ostk::core::type::Size;

    using ostk::mathematics::object::VectorXi;

    using ostk::physics::Environment;

    using ostk::astrodynamics::estimator::ObservationFilter;
    using ostk::astrodynamics::estimator::observationfilter::RANSAC;
    using ostk::astrodynamics::trajectory::Propagator;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::NumericalSolver;

    class_<ObservationFilter, PyObservationFilter, Shared<ObservationFilter>> observationFilter(
        aModule,
        "ObservationFilter",
        R"doc(
            Abstract base class for observation filters.

            Subclass and override :py:meth:`filter` to provide a custom filter strategy.
            The override must return an :py:class:`ObservationFilter.Analysis` (or subclass).
        )doc"
    );

    class_<ObservationFilter::Analysis, Shared<ObservationFilter::Analysis>>(
        observationFilter,
        "Analysis",
        R"doc(
            Common analysis results returned by any observation filter.
        )doc"
    )
        .def(
            init<const VectorXi&>(),
            arg("inlier_mask"),
            R"doc(
                Construct an Analysis.

                Args:
                    inlier_mask (numpy.ndarray[int]): Integer mask aligned with the original
                        observation array (1 = inlier, 0 = outlier).
            )doc"
        )
        .def("__str__", &(shiftToString<ObservationFilter::Analysis>))
        .def("__repr__", &(shiftToString<ObservationFilter::Analysis>))
        .def(
            "get_inlier_count",
            &ObservationFilter::Analysis::getInlierCount,
            R"doc(
                Number of observations classified as inliers.
            )doc"
        )
        .def(
            "get_outlier_count",
            &ObservationFilter::Analysis::getOutlierCount,
            R"doc(
                Number of observations classified as outliers (filtered out).
            )doc"
        )
        .def(
            "get_filtered_observations",
            &ObservationFilter::Analysis::getFilteredObservations,
            arg("observations"),
            R"doc(
                Apply the inlier mask to an observation array and return the inlier subset.

                Args:
                    observations (list[State]): Original observation array (same size as the mask).

                Returns:
                    list[State]: Inlier subset, preserving original order.
            )doc"
        )
        .def_readonly(
            "inlier_mask",
            &ObservationFilter::Analysis::inlierMask,
            R"doc(
                Eigen integer mask aligned with the original observation array; 1 = inlier, 0 = outlier.
            )doc"
        );

    observationFilter.def(init<>()).def(
        "filter",
        &ObservationFilter::filter,
        arg("observations"),
        R"doc(
            Filter an array of observation states.

            Args:
                observations (list[State]): Observations to filter.

            Returns:
                ObservationFilter.Analysis: Analysis carrying the inlier mask, filtered
                observations, and any filter-specific metrics (dynamic type depends on
                the concrete filter).
        )doc"
    );

    class_<RANSAC, ObservationFilter, Shared<RANSAC>> ransac(
        aModule,
        "RANSAC",
        R"doc(
            RANSAC-based observation filter.

            Each iteration samples one observation, propagates it to every observation epoch, and
            counts observations whose position residual is below the inlier threshold. The
            iteration with the largest inlier set wins; its inliers are returned.
        )doc"
    );

    class_<RANSAC::Analysis, ObservationFilter::Analysis, Shared<RANSAC::Analysis>>(
        ransac,
        "Analysis",
        R"doc(
            RANSAC-specific analysis, extending :py:class:`ObservationFilter.Analysis`.
        )doc"
    )
        .def(
            init<const VectorXi&, const Size&, const bool, const Size&, const Size&>(),
            arg("inlier_mask"),
            arg("iteration_count"),
            arg("early_termination"),
            arg("best_inlier_count"),
            arg("seed_failure_count"),
            R"doc(
                Construct a RANSAC Analysis.
            )doc"
        )
        .def("__str__", &(shiftToString<RANSAC::Analysis>))
        .def("__repr__", &(shiftToString<RANSAC::Analysis>))
        .def_readonly(
            "iteration_count",
            &RANSAC::Analysis::iterationCount,
            R"doc(
                Number of RANSAC iterations actually executed.
            )doc"
        )
        .def_readonly(
            "early_termination",
            &RANSAC::Analysis::earlyTermination,
            R"doc(
                True if the loop terminated after reaching the inlier-ratio target.
            )doc"
        )
        .def_readonly(
            "best_inlier_count",
            &RANSAC::Analysis::bestInlierCount,
            R"doc(
                Inlier count for the winning iteration.
            )doc"
        )
        .def_readonly(
            "seed_failure_count",
            &RANSAC::Analysis::seedFailureCount,
            R"doc(
                Number of iterations whose seed propagation threw.
            )doc"
        );

    ransac
        .def(
            init<const Propagator&, const Real&, const Size&, const Real&, const Size&>(),
            arg("propagator"),
            arg_v("inlier_threshold", DEFAULT_RANSAC_INLIER_THRESHOLD, "1000.0"),
            arg_v("max_iteration_count", DEFAULT_RANSAC_MAX_ITERATION_COUNT, "50"),
            arg_v("min_inlier_ratio", DEFAULT_RANSAC_MIN_INLIER_RATIO, "0.5"),
            arg_v("random_seed", DEFAULT_RANSAC_RANDOM_SEED, "0"),
            R"doc(
                Construct a RANSAC filter from a Propagator.

                Args:
                    propagator (Propagator): Propagator used to fit each sampled observation.
                    inlier_threshold (float, optional): Position-residual threshold (meters).
                    max_iteration_count (int, optional): Maximum number of RANSAC iterations.
                    min_inlier_ratio (float, optional): Inlier-fraction early-termination threshold.
                    random_seed (int, optional): RNG seed (0 = random_device-derived seed).
            )doc"
        )
        .def(
            init<const Environment&, const NumericalSolver&, const Real&, const Size&, const Real&, const Size&>(),
            arg("environment"),
            arg_v("numerical_solver", NumericalSolver::Default(), "NumericalSolver.default()"),
            arg_v("inlier_threshold", DEFAULT_RANSAC_INLIER_THRESHOLD, "1000.0"),
            arg_v("max_iteration_count", DEFAULT_RANSAC_MAX_ITERATION_COUNT, "50"),
            arg_v("min_inlier_ratio", DEFAULT_RANSAC_MIN_INLIER_RATIO, "0.5"),
            arg_v("random_seed", DEFAULT_RANSAC_RANDOM_SEED, "0"),
            R"doc(
                Construct a RANSAC filter, building the propagator from an Environment.

                Args:
                    environment (Environment): Environment for the propagator.
                    numerical_solver (NumericalSolver, optional): Numerical solver.
                    inlier_threshold (float, optional): Position-residual threshold (meters).
                    max_iteration_count (int, optional): Maximum number of RANSAC iterations.
                    min_inlier_ratio (float, optional): Inlier-fraction early-termination threshold.
                    random_seed (int, optional): RNG seed (0 = random_device-derived seed).
            )doc"
        )
        .def(
            "access_propagator",
            &RANSAC::accessPropagator,
            return_value_policy::reference_internal,
            R"doc(
                Access the underlying Propagator.

                Returns:
                    Propagator: The propagator.
            )doc"
        )
        .def(
            "get_inlier_threshold",
            &RANSAC::getInlierThreshold,
            R"doc(
                Get the inlier threshold (meters).
            )doc"
        )
        .def(
            "get_max_iteration_count",
            &RANSAC::getMaxIterationCount,
            R"doc(
                Get the maximum iteration count.
            )doc"
        )
        .def(
            "get_min_inlier_ratio",
            &RANSAC::getMinInlierRatio,
            R"doc(
                Get the minimum inlier ratio for early termination.
            )doc"
        )
        .def(
            "get_random_seed",
            &RANSAC::getRandomSeed,
            R"doc(
                Get the configured RNG seed (0 indicates a random_device-derived seed).
            )doc"
        )
        .def(
            "filter",
            &RANSAC::filter,
            arg("observations"),
            R"doc(
                Filter observations.

                Args:
                    observations (list[State]): Observations to filter.

                Returns:
                    RANSAC.Analysis: Analysis with the inlier mask, filtered observations,
                    iteration count, early-termination flag, best inlier count, and
                    seed-failure count.
            )doc"
        );
}
