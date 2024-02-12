/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/AttitudeQuaternion.hpp>

using namespace pybind11;

using ostk::core::type::String;
using ostk::core::type::Shared;

using ostk::astro::trajectory::state::coordinatessubsets::AttitudeQuaternion;
using ostk::astro::trajectory::state::CoordinatesSubset;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion(
    pybind11::module& aModule
)
{
    class_<AttitudeQuaternion, Shared<AttitudeQuaternion>, CoordinatesSubset>(
        aModule,
        "AttitudeQuaternion",
        R"doc(
            Attitude quaternion coordinates subset.

            Defined with respect to a reference frame.

        )doc"
    )

        .def(
            init<const String&>(),
            R"doc(
                Constructor.

                Args:
                    name (str): The name of the subset.

            )doc",
            arg("name")
        )

        .def(
            "in_frame",
            &AttitudeQuaternion::inFrame,
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinates_broker"),
            R"doc(
                Convert a Attitude quaternion from one reference frame to another.

                Args:
                    instant (Instant): The instant of the conversion.
                    coordinates (numpy.ndarray): The Attitude quaternion to convert.
                    from_frame (str): The reference frame of the input Attitude quaternion.
                    to_frame (str): The reference frame of the output Attitude quaternion.
                    coordinates_broker (CoordinatesBroker): The coordinates broker.

                Returns:
                    numpy.ndarray: The Attitude quaternion in the output reference frame.

            )doc"
        )

        .def_static(
            "default",
            &AttitudeQuaternion::Default,
            R"doc(
                Get the default Attitude quaternion subset.

                Returns:
                    AttitudeQuaternion: The default Attitude quaternion subset.
            )doc"
        )

        ;
}
