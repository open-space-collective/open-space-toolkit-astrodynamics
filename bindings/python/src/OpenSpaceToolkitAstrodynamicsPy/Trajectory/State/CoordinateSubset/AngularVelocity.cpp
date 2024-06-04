/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>

using namespace pybind11;

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_AngularVelocity(pybind11::module& aModule)
{
    class_<AngularVelocity, Shared<AngularVelocity>, CoordinateSubset>(
        aModule,
        "AngularVelocity",
        R"doc(
            Angular velocity coordinate subset.

            Defined with respect to a reference frame and a Attitude quaternion.

        )doc"
    )

        .def(
            init<const Shared<const AttitudeQuaternion>&, const String&>(),
            R"doc(
                Constructor.

                Args:
                    attitude_quaternion (AttitudeQuaternion): The Attitude quaternion.
                    name (str): The name of the subset.

            )doc",
            arg("attitude_quaternion"),
            arg("name")
        )

        .def(
            "in_frame",
            &AngularVelocity::inFrame,
            R"doc(
                Convert a Angular velocity from one reference frame to another.

                Args:
                    instant (Instant): The instant of the conversion.
                    coordinates (numpy.ndarray): The Angular velocity to convert.
                    from_frame (str): The reference frame of the input Angular velocity.
                    to_frame (str): The reference frame of the output Angular velocity.
                    coordinate_broker (CoordinateBroker): The coordinate broker.

                Returns:
                    numpy.ndarray: The Angular velocity in the output reference frame.

            )doc",
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinate_broker")
        )

        .def_static(
            "default",
            &AngularVelocity::Default,
            R"doc(
                Get the default Angular velocity subset.

                Returns:
                    AngularVelocity: The default Angular velocity subset.
            )doc"
        )

        ;
}
