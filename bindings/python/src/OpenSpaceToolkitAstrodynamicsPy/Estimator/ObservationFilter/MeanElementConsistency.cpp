/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/MeanElementConsistency.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator_ObservationFilter_MeanElementConsistency(pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::mathematics::object::MatrixXd;
    using ostk::mathematics::object::VectorXd;
    using ostk::mathematics::object::VectorXi;

    using ostk::physics::unit::Length;

    using ostk::astrodynamics::estimator::ObservationFilter;
    using ostk::astrodynamics::estimator::observationfilter::MeanElementConsistency;

    class_<MeanElementConsistency, ObservationFilter, Shared<MeanElementConsistency>> meanElementConsistency(
        aModule,
        "MeanElementConsistency",
        R"doc(
            Observation filter based on the consistency of Brouwer-Lyddane mean orbital elements along the arc.

            Good observations of a single spacecraft, once converted to mean orbital elements, trace slow and
            smooth trends, while corrupted observations or coherent bad segments (e.g. a wrong orbit plane) jump
            away from them. The filter needs no propagation, force model, initial guess or random numbers.

            Algorithm, after converting each observation to Brouwer-Lyddane Mean (Long) elements:

            1. six channels expressed in metres: a, a·e·cos(Ω+ω), a·e·sin(Ω+ω), a·i, a·sin(i)·(Ω − Ω̇·t) and
               a·(Ω+ω+M − λ̇·t), where the J2 secular rates come from the median elements;
            2. per channel, a robust linear trend is removed (Theil-Sen slope over observation pairs separated by at
               least a quarter of the arc, so that dense high-rate bursts cannot dominate) and the residual is centred
               on its median;
            3. an observation is an outlier when, in any channel, ``|residual| > max(gate_factor * sigma,
               minimum_deviation)`` with ``sigma = 1.4826 * MAD``.

            ``gate_factor`` (default 10, any value in [3, 30] behaves identically on the reference datasets) and
            ``minimum_deviation`` (default 2 km, any value in [1.5, 5] km) are the only tuning parameters. The floor is
            also the smallest error the filter can catch on short arcs. Requires a majority (about 75 % or more) of
            good observations; Earth orbits only; observations are converted to GCRF internally.

            Example::

                filter = MeanElementConsistency()
                inliers = filter.apply(observations)
                analysis = solver.estimate(initial_guess, inliers)
        )doc"
    );

    class_<MeanElementConsistency::Analysis, ObservationFilter::Analysis, Shared<MeanElementConsistency::Analysis>>(
        meanElementConsistency,
        "Analysis",
        R"doc(
            Analysis results of the mean element consistency filter.
        )doc"
    )
        .def(
            init<const VectorXi&, const MatrixXd&, const VectorXd&>(),
            arg("inlier_mask"),
            arg("deviations"),
            arg("gates"),
            R"doc(
                Construct an Analysis.

                Args:
                    inlier_mask (numpy.ndarray[int]): Mask aligned with the observation array (1 = inlier, 0 = outlier).
                    deviations (numpy.ndarray[float]): [N x 6] detrended mean-element residuals in metres.
                    gates (numpy.ndarray[float]): [6] gate applied to each channel in metres.
            )doc"
        )
        .def("__str__", &(shiftToString<MeanElementConsistency::Analysis>))
        .def("__repr__", &(shiftToString<MeanElementConsistency::Analysis>))
        .def_readonly(
            "deviations",
            &MeanElementConsistency::Analysis::deviations,
            R"doc(
                [N x 6] detrended mean-element residuals in metres, columns ordered as
                :py:meth:`MeanElementConsistency.channel_names`. NaN when an observation could not be converted.

                :type: numpy.ndarray[float]
            )doc"
        )
        .def_readonly(
            "gates",
            &MeanElementConsistency::Analysis::gates,
            R"doc(
                [6] gate applied to each channel in metres.

                :type: numpy.ndarray[float]
            )doc"
        );

    meanElementConsistency
        .def(
            init<const Real&, const Length&>(),
            arg("gate_factor") = DEFAULT_MEAN_ELEMENT_GATE_FACTOR,
            arg_v("minimum_deviation", DEFAULT_MEAN_ELEMENT_MINIMUM_DEVIATION, "Length.kilometers(2.0)"),
            R"doc(
                Construct a MeanElementConsistency filter.

                Args:
                    gate_factor (float): Number of robust sigmas beyond which a deviation is an outlier. Defaults to 10.
                    minimum_deviation (Length): Deviation (in mean-element metres) below which an observation is never
                        rejected. Defaults to 2 km.
            )doc"
        )
        .def(
            "get_gate_factor",
            &MeanElementConsistency::getGateFactor,
            R"doc(
                Get the gate factor.

                Returns:
                    float: Gate factor.
            )doc"
        )
        .def(
            "get_minimum_deviation",
            &MeanElementConsistency::getMinimumDeviation,
            R"doc(
                Get the minimum deviation.

                Returns:
                    Length: Minimum deviation.
            )doc"
        )
        .def(
            "filter",
            &MeanElementConsistency::filter,
            arg("observations"),
            R"doc(
                Classify an array of observations.

                Args:
                    observations (list[State]): Observations to classify (Cartesian position and velocity required).

                Returns:
                    MeanElementConsistency.Analysis: Analysis with inlier mask, per-channel deviations and gates.
            )doc"
        )
        .def_static(
            "channel_names",
            &MeanElementConsistency::ChannelNames,
            R"doc(
                Names of the six channels, in the column order of :py:attr:`MeanElementConsistency.Analysis.deviations`.

                Returns:
                    list[str]: Channel names.
            )doc"
        );
}
