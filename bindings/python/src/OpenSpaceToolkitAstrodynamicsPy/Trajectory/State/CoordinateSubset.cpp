/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/AtmosphericDrag.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/CentralBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/PositionDerivative.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/ThirdBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinateSubset.cpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::state::CoordinatesSubset;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubset(pybind11::module& aModule)
{
    class_<CoordinatesSubset, Shared<CoordinatesSubset>>(aModule, "CoordinatesSubset")

        .def(init<const String&, const Size&>(), arg("name"), arg("size"))

        .def(self == self)
        .def(self != self)

        .def("get_id", &CoordinatesSubset::getId)
        .def("get_name", &CoordinatesSubset::getName)
        .def("get_size", &CoordinatesSubset::getSize)

        .def(
            "add",
            &CoordinatesSubset::add,
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinate_broker")
        )

        .def("get_read_coordinates_subsets", &Dynamics::getReadCoordinatesSubsets)
        .def("get_write_coordinates_subsets", &Dynamics::getWriteCoordinatesSubsets)
        .def("is_defined", &Dynamics::isDefined)

        .def("compute_contribution", &Dynamics::computeContribution, arg("instant"), arg("state_vector"), arg("frame"))

        ;

    // Create "coordinates_subset" python submodule
    auto coordinates_subset = aModule.def_submodule("coordinates_subset");

    // Add __path__ attribute for "coordinates_subset" submodule
    coordinates_subset.attr("__path__") = "ostk.astrodynamics.trajectory.state.coordinates_subset";

    // Add objects to "coordinates_subset" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_PositionDerivative(coordinates_subset);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_CentralBodyGravity(coordinates_subset);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_ThirdBodyGravity(coordinates_subset);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_AtmosphericDrag(coordinates_subset);
}
