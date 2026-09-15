/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Estimator/ObservationFilter/MeanElementConsistency.cpp>

// Trampoline allowing Python subclasses to override `filter`.
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

    using ostk::core::type::Shared;

    using ostk::mathematics::object::VectorXi;

    using ostk::astrodynamics::estimator::ObservationFilter;

    class_<ObservationFilter, PyObservationFilter, Shared<ObservationFilter>> observationFilter(
        aModule,
        "ObservationFilter",
        R"doc(
            Abstract base class for observation filters.

            An observation filter classifies each observation of an arc as an inlier or an outlier, so that
            outliers can be removed before orbit determination. Subclass it and override :py:meth:`filter`,
            returning an :py:class:`ObservationFilter.Analysis` (or a subclass of it).

            Example::

                class KeepFirstHalf(ObservationFilter):
                    def filter(self, observations):
                        mask = np.zeros(len(observations), dtype=int)
                        mask[: len(observations) // 2] = 1
                        return ObservationFilter.Analysis(mask)

                inliers = KeepFirstHalf().apply(observations)
        )doc"
    );

    class_<ObservationFilter::Analysis, Shared<ObservationFilter::Analysis>>(
        observationFilter,
        "Analysis",
        R"doc(
            Classification produced by an observation filter.
        )doc"
    )
        .def(
            init<const VectorXi&>(),
            arg("inlier_mask"),
            R"doc(
                Construct an Analysis.

                Args:
                    inlier_mask (numpy.ndarray[int]): Mask aligned with the observation array (1 = inlier, 0 = outlier).
            )doc"
        )
        .def("__str__", &(shiftToString<ObservationFilter::Analysis>))
        .def("__repr__", &(shiftToString<ObservationFilter::Analysis>))
        .def(
            "get_inlier_count",
            &ObservationFilter::Analysis::getInlierCount,
            R"doc(
                Get the number of observations classified as inliers.

                Returns:
                    int: Inlier count.
            )doc"
        )
        .def(
            "get_outlier_count",
            &ObservationFilter::Analysis::getOutlierCount,
            R"doc(
                Get the number of observations classified as outliers.

                Returns:
                    int: Outlier count.
            )doc"
        )
        .def(
            "get_filtered_observations",
            &ObservationFilter::Analysis::getFilteredObservations,
            arg("observations"),
            R"doc(
                Apply the inlier mask to the observation array the analysis was computed from.

                Args:
                    observations (list[State]): The original observation array (same size as the mask).

                Returns:
                    list[State]: The inlier observations, in their original order.
            )doc"
        )
        .def_readonly(
            "inlier_mask",
            &ObservationFilter::Analysis::inlierMask,
            R"doc(
                Mask aligned with the observation array (1 = inlier, 0 = outlier).

                :type: numpy.ndarray[int]
            )doc"
        );

    observationFilter.def(init<>())
        .def(
            "filter",
            &ObservationFilter::filter,
            arg("observations"),
            R"doc(
                Classify an array of observations.

                Args:
                    observations (list[State]): Observations to classify.

                Returns:
                    ObservationFilter.Analysis: Analysis holding the inlier mask (dynamic type may be a
                    filter-specific subclass).
            )doc"
        )
        .def(
            "apply",
            &ObservationFilter::apply,
            arg("observations"),
            R"doc(
                Classify an array of observations and return the inliers.

                Args:
                    observations (list[State]): Observations to filter.

                Returns:
                    list[State]: The inlier observations, in their original order.
            )doc"
        );

    // Create "observation_filter" python submodule
    auto observation_filter = aModule.def_submodule("observation_filter");

    OpenSpaceToolkitAstrodynamicsPy_Estimator_ObservationFilter_MeanElementConsistency(observation_filter);
}
