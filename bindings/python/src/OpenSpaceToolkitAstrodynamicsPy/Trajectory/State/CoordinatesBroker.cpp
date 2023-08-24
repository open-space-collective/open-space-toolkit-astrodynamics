/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesBroker(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::math::obj::VectorXd;

    using ostk::astro::trajectory::state::CoordinatesBroker;
    using ostk::astro::trajectory::state::CoordinatesSubset;

    class_<CoordinatesBroker, Shared<CoordinatesBroker>>(aModule, "CoordinatesBroker")

        .def(init<>())
        .def(init<const Array<Shared<const CoordinatesSubset>>&>(), arg("coordinates_subsets"))

        .def(self == self)
        .def(self != self)

        .def("access_subsets", &CoordinatesBroker::accessSubsets)
        .def("get_number_of_coordinates", &CoordinatesBroker::getNumberOfCoordinates)
        .def("get_number_of_subsets", &CoordinatesBroker::getNumberOfSubsets)
        .def("get_subsets", &CoordinatesBroker::getSubsets)
        .def("add_subset", &CoordinatesBroker::addSubset, arg("coordinates_subset"))
        .def(
            "has_subset",
            overload_cast<const Shared<const CoordinatesSubset>&>(&CoordinatesBroker::hasSubset, const_),
            arg("coordinates_subset")
        )
        .def(
            "extract_coordinates",
            overload_cast<const VectorXd&, const CoordinatesSubset&>(&CoordinatesBroker::extractCoordinates, const_),
            arg("coordinates"),
            arg("coordinates_subset")
        )
        .def(
            "extract_coordinates",
            overload_cast<const VectorXd&, const Shared<const CoordinatesSubset>&>(
                &CoordinatesBroker::extractCoordinates, const_
            ),
            arg("coordinates"),
            arg("coordinates_subset")
        )

        ;
}
