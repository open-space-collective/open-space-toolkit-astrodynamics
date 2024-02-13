/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

using namespace pybind11;

using ostk::core::type::String;
using ostk::core::type::Shared;

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_CartesianPosition(
    pybind11::module& aModule
)
{
    class_<CartesianPosition, Shared<CartesianPosition>, CoordinateSubset>(
        aModule,
        "CartesianPosition",
        R"doc(
            Cartesian position coordinate subset.

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
            &CartesianPosition::inFrame,
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinate_broker"),
            R"doc(
                Convert a Cartesian position from one reference frame to another.

                Args:
                    instant (Instant): The instant of the conversion.
                    coordinates (numpy.ndarray): The Cartesian position to convert.
                    from_frame (str): The reference frame of the input Cartesian position.
                    to_frame (str): The reference frame of the output Cartesian position.
                    coordinate_broker (CoordinateBroker): The coordinate broker.

                Returns:
                    numpy.ndarray: The Cartesian position in the output reference frame.

            )doc"
        )

        .def_static(
            "default",
            &CartesianPosition::Default,
            R"doc(
                Get the default Cartesian position subset.

                Returns:
                    CartesianPosition: The default Cartesian position subset.
            )doc"
        )

        ;
}
