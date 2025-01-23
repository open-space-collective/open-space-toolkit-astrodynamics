/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>

using namespace pybind11;

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_CartesianAcceleration(
    pybind11::module& aModule
)
{
    class_<CartesianAcceleration, Shared<CartesianAcceleration>, CoordinateSubset>(
        aModule,
        "CartesianAcceleration",
        R"doc(
            Cartesian acceleration coordinate subset.

            Defined with respect to a reference frame.
        )doc"
    )

        .def(
            init<const Shared<const CartesianPosition>&, const Shared<const CartesianVelocity>&, const String&>(),
            R"doc(
                Constructor.

                Args:
                    cartesian_position (CartesianPosition): The Cartesian position.
                    cartesian_velocity (CartesianVelocity): The Cartesian velocity.
                    name (str): The name of the subset.

            )doc",
            arg("cartesian_position"),
            arg("cartesian_velocity"),
            arg("name")
        )

        .def(
            "in_frame",
            &CartesianAcceleration::inFrame,
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinate_broker"),
            R"doc(
                Convert a Cartesian acceleration from one reference frame to another.

                Args:
                    instant (Instant): The instant of the conversion.
                    coordinates (np.ndarray): The Cartesian acceleration to convert.
                    from_frame (Frame): The reference frame of the input Cartesian acceleration.
                    to_frame (Frame): The reference frame of the output Cartesian acceleration.
                    coordinate_broker (CoordinateBroker): The coordinate broker.

                Returns:
                    numpy.ndarray: The Cartesian acceleration in the output reference frame.

            )doc"
        )

        .def_static(
            "default",
            &CartesianAcceleration::Default,
            R"doc(
                Get the default Cartesian acceleration subset.

                Returns:
                    CartesianAcceleration: The default Cartesian acceleration subset.
            )doc"
        )

        .def_static(
            "thrust_acceleration",
            &CartesianAcceleration::ThrustAcceleration,
            R"doc(
                Get the Cartesian acceleration subset for thrust acceleration.

                Returns:
                    CartesianAcceleration: The Cartesian acceleration subset for thrust acceleration.
            )doc"
        )

        ;
}
