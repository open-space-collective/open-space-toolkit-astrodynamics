/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AttitudeQuaternion.hpp>

using namespace pybind11;

using ostk::core::type::String;
using ostk::core::type::Shared;

using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_AttitudeQuaternion(
    pybind11::module& aModule
)
{
    class_<AttitudeQuaternion, Shared<AttitudeQuaternion>, CoordinateSubset>(
        aModule,
        "AttitudeQuaternion",
        R"doc(
            Attitude quaternion coordinate subset.

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
            arg("coordinate_broker"),
            R"doc(
                Convert a Attitude quaternion from one reference frame to another.

                Args:
                    instant (Instant): The instant of the conversion.
                    coordinates (numpy.ndarray): The Attitude quaternion to convert.
                    from_frame (str): The reference frame of the input Attitude quaternion.
                    to_frame (str): The reference frame of the output Attitude quaternion.
                    coordinate_broker (CoordinateBroker): The coordinate broker.

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
