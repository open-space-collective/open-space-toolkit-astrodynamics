/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::CoordinatesSubset;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_CartesianPosition(
    pybind11::module& aModule
)
{
    class_<CartesianPosition, Shared<CartesianPosition>, CoordinatesSubset>(
        aModule,
        "CartesianPosition",
        R"doc(
            Cartesian position coordinates subset.

            Defined with respect to a reference frame.

            Group:
                CoordinatesSubsets
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
            arg("coordinates_broker"),
            R"doc(
                Convert a Cartesian position from one reference frame to another.

                Args:
                    instant (Instant): The instant of the conversion.
                    coordinates (numpy.ndarray): The Cartesian position to convert.
                    from_frame (str): The reference frame of the input Cartesian position.
                    to_frame (str): The reference frame of the output Cartesian position.
                    coordinates_broker (CoordinatesBroker): The coordinates broker.

                Returns:
                    coordinates (numpy.ndarray): The Cartesian position in the output reference frame.

            )doc"
        )

        .def_static("default", &CartesianPosition::Default, R"doc(
            Get the default Cartesian position subset.

            Returns:
                cartesian_position (CartesianPosition): The default Cartesian position subset.

            Group:
                Static methods
        )doc")

        ;
}
