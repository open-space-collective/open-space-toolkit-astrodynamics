/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

using namespace pybind11;

using ostk::core::type::String;
using ostk::core::type::Shared;

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_CartesianVelocity(
    pybind11::module& aModule
)
{
    class_<CartesianVelocity, Shared<CartesianVelocity>, CoordinateSubset>(
        aModule,
        "CartesianVelocity",
        R"doc(
            Cartesian velocity coordinate subset.

            Defined with respect to a reference frame and a Cartesian position.

        )doc"
    )

        .def(
            init<const Shared<const CartesianPosition>&, const String&>(),
            R"doc(
                Constructor.

                Args:
                    cartesian_position (CartesianPosition): The Cartesian position.
                    name (str): The name of the subset.

            )doc",
            arg("cartesian_position"),
            arg("name")
        )

        .def(
            "in_frame",
            &CartesianVelocity::inFrame,
            R"doc(
                Convert a Cartesian velocity from one reference frame to another.

                Args:
                    instant (Instant): The instant of the conversion.
                    coordinates (numpy.ndarray): The Cartesian velocity to convert.
                    from_frame (str): The reference frame of the input Cartesian velocity.
                    to_frame (str): The reference frame of the output Cartesian velocity.
                    coordinate_broker (CoordinateBroker): The coordinate broker.

                Returns:
                    numpy.ndarray: The Cartesian velocity in the output reference frame.

            )doc",
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinate_broker")
        )

        .def_static(
            "default",
            &CartesianVelocity::Default,
            R"doc(
                Get the default Cartesian velocity subset.

                Returns:
                    CartesianVelocity: The default Cartesian velocity subset.
            )doc"
        )

        ;
}
