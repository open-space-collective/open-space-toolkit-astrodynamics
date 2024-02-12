/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesBroker(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::mathematics::object::VectorXd;

    using ostk::astro::trajectory::state::CoordinatesBroker;
    using ostk::astro::trajectory::state::CoordinatesSubset;

    class_<CoordinatesBroker, Shared<CoordinatesBroker>>(
        aModule,
        "CoordinatesBroker",
        R"doc(
            Class to manage the coordinates subsets of a state.
        
        )doc"
    )

        .def(
            init<>(),
            R"doc(
                Default constructor.
            )doc"
        )
        .def(
            init<const Array<Shared<const CoordinatesSubset>>&>(),
            arg("coordinates_subsets"),
            R"doc(
                Create a broker for ther provided coordinate subsets.

                Args:
                    list[CoordinatesSubset]: The list of coordinates subsets.

            )doc"
        )

        .def(self == self)
        .def(self != self)

        .def(
            "access_subsets",
            &CoordinatesBroker::accessSubsets,
            R"doc(
                Access the list of coordinates subsets.

                Returns:
                    list[CoordinatesSubset]: The list of coordinates subsets.

            )doc"
        )
        .def(
            "get_number_of_coordinates",
            &CoordinatesBroker::getNumberOfCoordinates,
            R"doc(
                Get the total number of coordinates.

                Returns:
                    int: The total number of coordinates.

            )doc"
        )
        .def(
            "get_number_of_subsets",
            &CoordinatesBroker::getNumberOfSubsets,
            R"doc(
                Get the number of coordinates subsets.

                Returns:
                    int: The number of coordinates subsets.

            )doc"
        )
        .def(
            "get_subsets",
            &CoordinatesBroker::getSubsets,
            R"doc(
                Get the list of coordinates subsets.

                Returns:
                    list[CoordinatesSubset]: The list of coordinates subsets.

            )doc"
        )
        .def(
            "add_subset",
            &CoordinatesBroker::addSubset,
            R"doc(
                Add a coordinates subset.

                Args:
                    coordinates_subset (CoordinatesSubset): The coordinates subset to add.

            )doc",
            arg("coordinates_subset")
        )
        .def(
            "has_subset",
            overload_cast<const Shared<const CoordinatesSubset>&>(&CoordinatesBroker::hasSubset, const_),
            arg("coordinates_subset"),
            R"doc(
                Check if the coordinates broker has a given coordinates subset.

                Args:
                    coordinates_subset (CoordinatesSubset): The coordinates subset to check.

                Returns:
                    bool: True if the coordinates broker has the coordinates subset, False otherwise.

            )doc"
        )

        .def(
            "extract_coordinate",
            overload_cast<const VectorXd&, const Shared<const CoordinatesSubset>&>(
                &CoordinatesBroker::extractCoordinate, const_
            ),
            arg("coordinates"),
            arg("coordinates_subset"),
            R"doc(
                Extract the coordinates of a subset from a full coordinates vector.

                Args:
                    coordinates (numpy.ndarray): The full coordinates vector.
                    coordinates_subset (CoordinatesSubset): The coordinates subset.

                Returns:
                    numpy.ndarray: The coordinates of the subset.

            )doc"
        )
        .def(
            "extract_coordinates",
            &CoordinatesBroker::extractCoordinates,
            arg("coordinates"),
            arg("coordinates_subsets"),
            R"doc(
                Extract the coordinates of multiple subsets from a full coordinates vector.

                Args:
                    coordinates (numpy.ndarray): The full coordinates vector.
                    coordinates_subsets (list[CoordinatesSubset]): The coordinates subsets.

                Returns:
                    numpy.ndarray: The coordinates of the subsets.

            )doc"
        )

        ;
}
