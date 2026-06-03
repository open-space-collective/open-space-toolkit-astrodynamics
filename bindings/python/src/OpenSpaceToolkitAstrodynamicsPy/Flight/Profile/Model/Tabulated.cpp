/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Model_Tabulated(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Map;
    using ostk::core::type::Shared;

    using ostk::mathematics::curvefitting::Interpolator;

    using ostk::astrodynamics::flight::profile::Model;
    using ostk::astrodynamics::flight::profile::model::Tabulated;
    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

    class_<Tabulated, Model>(
        aModule,
        "Tabulated",
        R"doc(
            A flight profile model defined by a set of states.

        )doc"
    )

        .def(
            init<const Array<State>&, const Interpolator::Type&>(),
            R"doc(
                Constructor.

                Args:
                    states (Array[State]): The states of the model.
                    interpolator_type (Interpolator.Type, optional): The type of interpolator to use for all but the AttitudeQuaternion subset. Attitude quaternions will be interpolated using spherical linear interpolation (SLERP). Defaults to Barycentric Rational.
             )doc",
            arg("states"),
            arg_v("interpolator_type", Interpolator::Type::BarycentricRational, "Interpolator.Type.BarycentricRational")
        )

        .def(
            init<const Array<State>&, const Map<Shared<const CoordinateSubset>, Interpolator::Type>&>(),
            R"doc(
                Constructor with per-coordinate-subset interpolation types.

                Each coordinate subset is interpolated using the interpolation type associated with it, except the
                attitude quaternion, which is always interpolated using spherical linear interpolation (SLERP).

                Args:
                    states (Array[State]): The states of the model.
                    interpolation_types (dict[CoordinateSubset, Interpolator.Type]): A mapping from coordinate subset to the interpolation type to use for that subset's coordinates. Every coordinate subset present in the states (other than the attitude quaternion) must have an entry. Entries for coordinate subsets not present in the states (including the attitude quaternion) are ignored.
             )doc",
            arg("states"),
            arg("interpolation_types")
        )

        .def(
            "__str__",
            &(shiftToString<Tabulated>),
            R"doc(
                Convert the model to a string.

                Returns:
                    str: The string representation of the model.
             )doc"
        )

        .def(
            "__repr__",
            &(shiftToString<Tabulated>),
            R"doc(
                Convert the model to a string.

                Returns:
                    str: The string representation of the model.
             )doc"
        )

        .def(
            "is_defined",
            &Tabulated::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.
             )doc"
        )

        .def(
            "get_interval",
            &Tabulated::getInterval,
            R"doc(
                Get the interval of the model.

                Returns:
                    Interval: The interval of the model.
             )doc"
        )
        .def(
            "get_interpolator_type",
            &Tabulated::getInterpolatorType,
            R"doc(
                Get the type of interpolator used in the model.

                Returns:
                    Interpolator.Type: The type of interpolator used in the model.
             )doc"
        )

        .def(
            "calculate_state_at",
            &Tabulated::calculateStateAt,
            R"doc(
                Calculate the state of the model at a specific instant.

                Args:
                    instant (Instant): The instant at which to calculate the state.

                Returns:
                    State: The state of the model at the specified instant.
             )doc",
            arg("instant")
        )

        .def(
            "get_axes_at",
            &Tabulated::getAxesAt,
            R"doc(
                Get the axes of the model at a specific instant.

                Args:
                    instant (Instant): The instant at which to get the axes.

                Returns:
                    numpy.ndarray: The axes of the model at the specified instant.
             )doc",
            arg("instant")
        )

        .def(
            "construct_body_frame",
            &Tabulated::constructBodyFrame,
            R"doc(
                Construct the body frame of the model with the specified name.

                Args:
                    frame_name (str): The name of the body frame.

                Returns:
                    Frame: The body frame of the model with the specified name.
             )doc",
            arg("frame_name")
        )

        .def_static(
            "default",
            &Tabulated::Default,
            R"doc(
                Construct a tabulated profile model using the default per-coordinate-subset interpolation types.

                Each coordinate subset present in the states (other than the attitude quaternion, which is always
                interpolated using SLERP) is interpolated using its default interpolation type (barycentric rational
                for position, velocity, acceleration, angular velocity and mass; zero-order for drag coefficient,
                surface area, mass flow rate and ballistic coefficient).

                Args:
                    states (Array[State]): The states of the model.

                Returns:
                    Tabulated: A tabulated profile model using the default interpolation types.
             )doc",
            arg("states")
        )

        ;
}
