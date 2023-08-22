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
    class_<CartesianPosition, Shared<CartesianPosition>, CoordinatesSubset>(aModule, "CartesianPosition")

        .def(init<const String&>(), arg("name"))

        .def(
            "add",
            &CartesianPosition::add,
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinates_broker")
        )
        .def(
            "subtract",
            &CartesianPosition::subtract,
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinates_broker")
        )
        .def(
            "in_frame",
            &CartesianPosition::inFrame,
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinates_broker")
        )

        .def_static("default", &CartesianPosition::Default)

        ;
}
