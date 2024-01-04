/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/AngularVelocity.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::astro::trajectory::state::coordinatessubsets::AngularVelocity;
using ostk::astro::trajectory::state::coordinatessubsets::AttitudeQuaternion;
using ostk::astro::trajectory::state::CoordinatesSubset;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_AngularVelocity(
    pybind11::module& aModule
)
{
    class_<AngularVelocity, Shared<AngularVelocity>, CoordinatesSubset>(
        aModule,
        "AngularVelocity",
        R"doc(
            Angular velocity coordinates subset.

            Defined with respect to a reference frame and a Attitude quaternion.

            Group:
                state
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
                    coordinates_broker (CoordinatesBroker): The coordinates broker.

                Returns:
                    numpy.ndarray: The Angular velocity in the output reference frame.

            )doc",
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinates_broker")
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
