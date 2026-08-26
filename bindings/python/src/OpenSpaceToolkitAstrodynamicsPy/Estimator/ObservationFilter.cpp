/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/MeanElementConsistency.hpp>

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

    using ostk::mathematics::object::MatrixXd;
    using ostk::mathematics::object::VectorXd;
    using ostk::mathematics::object::VectorXi;

    using ostk::physics::unit::Derived;

    using ostk::astrodynamics::estimator::ObservationFilter;
    using ostk::astrodynamics::estimator::observationfilter::MeanElementConsistency;
    using ostk::astrodynamics::trajectory::State;

    using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

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

    class_<MeanElementConsistency, ObservationFilter, Shared<MeanElementConsistency>> meanElementConsistency(
        aModule,
        "MeanElementConsistency",
        R"doc(
            Mean-orbital-element consistency observation filter.

            Converts each observation (position, velocity, time) to Brouwer-Lyddane mean
            elements in closed form, robustly fits each element channel's slow secular trend
            (Theil-Sen seed plus outlier-excluding polynomial refits), and classifies an
            observation as an outlier if any channel residual deviates from the channel median
            by more than gate_factor x max(1.4826 x MAD, channel noise floor).

            No propagation, environment, or random sampling is involved: the filter is
            deterministic, runs in milliseconds on thousands of observations, and has exactly
            two tuning parameters (gate factor and observation noise floor).
        )doc"
    );

    class_<MeanElementConsistency::Analysis, ObservationFilter::Analysis, Shared<MeanElementConsistency::Analysis>>(
        meanElementConsistency,
        "Analysis",
        R"doc(
            Mean-element-consistency-specific analysis, extending :py:class:`ObservationFilter.Analysis`.
        )doc"
    )
        .def(
            init<const VectorXi&, const MatrixXd&, const VectorXd&>(),
            arg("inlier_mask"),
            arg("channel_residuals"),
            arg("applied_channel_gates"),
            R"doc(
                Construct a MeanElementConsistency Analysis.
            )doc"
        )
        .def("__str__", &(shiftToString<MeanElementConsistency::Analysis>))
        .def("__repr__", &(shiftToString<MeanElementConsistency::Analysis>))
        .def_readonly(
            "channel_residuals",
            &MeanElementConsistency::Analysis::channelResiduals,
            R"doc(
                (N x 6) per-observation channel residuals, aligned with the original observation
                array: [semi-major axis (m), ex (-), ey (-), inclination (rad), node (rad),
                phase (rad)].
            )doc"
        )
        .def_readonly(
            "applied_channel_gates",
            &MeanElementConsistency::Analysis::appliedChannelGates,
            R"doc(
                (6) gate actually applied to each channel, same order and units as the residuals.
            )doc"
        );

    meanElementConsistency
        .def(
            init<const Real&, const Real&, const Derived&>(),
            arg_v("gate_factor", DEFAULT_MEAN_ELEMENT_CONSISTENCY_GATE_FACTOR, "10.0"),
            arg_v("noise_floor", DEFAULT_MEAN_ELEMENT_CONSISTENCY_NOISE_FLOOR, "10.0"),
            arg_v(
                "gravitational_parameter",
                EarthGravitationalModel::EGM2008.gravitationalParameter_,
                "EarthGravitationalModel.EGM2008.gravitational_parameter"
            ),
            R"doc(
                Construct a MeanElementConsistency filter.

                Args:
                    gate_factor (float, optional): Multiplier on the robust per-channel scale
                        (max of 1.4826 x MAD and the channel noise floor) beyond which an
                        observation is classified as an outlier. Defaults to 10.0.
                    noise_floor (float, optional): Assumed observation position-noise floor
                        (meters); deviations below this scale are never classified as outliers.
                        Defaults to 10.0.
                    gravitational_parameter (Derived, optional): Gravitational parameter used for
                        the mean element conversion and the secular rates. Defaults to
                        EGM2008.
            )doc"
        )
        .def(
            "get_gate_factor",
            &MeanElementConsistency::getGateFactor,
            R"doc(
                Get the gate factor.
            )doc"
        )
        .def(
            "get_noise_floor",
            &MeanElementConsistency::getNoiseFloor,
            R"doc(
                Get the observation noise floor (meters).
            )doc"
        )
        .def(
            "get_gravitational_parameter",
            &MeanElementConsistency::getGravitationalParameter,
            R"doc(
                Get the gravitational parameter.
            )doc"
        );
}
