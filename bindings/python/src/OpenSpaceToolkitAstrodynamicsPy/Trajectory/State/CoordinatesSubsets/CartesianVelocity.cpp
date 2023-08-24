/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::trajectory::state::CoordinatesSubset;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_CartesianVelocity(
    pybind11::module& aModule
)
{
    class_<CartesianVelocity, Shared<CartesianVelocity>, CoordinatesSubset>(aModule, "CartesianVelocity")

        .def(init<const Shared<const CartesianPosition>&, const String&>(), arg("cartesian_position"), arg("name"))

        .def(
            "add",
            &CartesianVelocity::add,
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinates_broker")
        )
        .def(
            "subtract",
            &CartesianVelocity::subtract,
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinates_broker")
        )
        .def(
            "in_frame",
            &CartesianVelocity::inFrame,
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinates_broker")
        )

        .def_static("default", &CartesianVelocity::Default)

        ;
}
