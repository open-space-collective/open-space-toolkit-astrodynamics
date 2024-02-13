/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateBroker(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::mathematics::object::VectorXd;

    using ostk::astrodynamics::trajectory::state::CoordinateBroker;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

    class_<CoordinateBroker, Shared<CoordinateBroker>>(
        aModule,
        "CoordinateBroker",
        R"doc(
            Class to manage the coordinate subsets of a state.
        
        )doc"
    )

        .def(
            init<>(),
            R"doc(
                Default constructor.
            )doc"
        )
        .def(
            init<const Array<Shared<const CoordinateSubset>>&>(),
            arg("coordinate_subsets"),
            R"doc(
                Create a broker for ther provided coordinate subsets.

                Args:
                    list[CoordinateSubset]: The list of coordinate subsets.

            )doc"
        )

        .def(self == self)
        .def(self != self)

        .def(
            "access_subsets",
            &CoordinateBroker::accessSubsets,
            R"doc(
                Access the list of coordinate subsets.

                Returns:
                    list[CoordinateSubset]: The list of coordinate subsets.

            )doc"
        )
        .def(
            "get_number_of_coordinates",
            &CoordinateBroker::getNumberOfCoordinates,
            R"doc(
                Get the total number of coordinates.

                Returns:
                    int: The total number of coordinates.

            )doc"
        )
        .def(
            "get_number_of_subsets",
            &CoordinateBroker::getNumberOfSubsets,
            R"doc(
                Get the number of coordinate subsets.

                Returns:
                    int: The number of coordinate subsets.

            )doc"
        )
        .def(
            "get_subsets",
            &CoordinateBroker::getSubsets,
            R"doc(
                Get the list of coordinate subsets.

                Returns:
                    list[CoordinateSubset]: The list of coordinate subsets.

            )doc"
        )
        .def(
            "add_subset",
            &CoordinateBroker::addSubset,
            R"doc(
                Add a coordinate subset.

                Args:
                    coordinate_subset (CoordinateSubset): The coordinate subset to add.

            )doc",
            arg("coordinate_subset")
        )
        .def(
            "has_subset",
            overload_cast<const Shared<const CoordinateSubset>&>(&CoordinateBroker::hasSubset, const_),
            arg("coordinate_subset"),
            R"doc(
                Check if the coordinate broker has a given coordinate subset.

                Args:
                    coordinate_subset (CoordinateSubset): The coordinate subset to check.

                Returns:
                    bool: True if the coordinate broker has the coordinate subset, False otherwise.

            )doc"
        )

        .def(
            "extract_coordinate",
            overload_cast<const VectorXd&, const Shared<const CoordinateSubset>&>(
                &CoordinateBroker::extractCoordinate, const_
            ),
            arg("coordinates"),
            arg("coordinate_subset"),
            R"doc(
                Extract the coordinates of a subset from a full coordinates vector.

                Args:
                    coordinates (numpy.ndarray): The full coordinates vector.
                    coordinate_subset (CoordinateSubset): The coordinate subset.

                Returns:
                    numpy.ndarray: The coordinates of the subset.

            )doc"
        )
        .def(
            "extract_coordinates",
            &CoordinateBroker::extractCoordinates,
            arg("coordinates"),
            arg("coordinate_subsets"),
            R"doc(
                Extract the coordinates of multiple subsets from a full coordinates vector.

                Args:
                    coordinates (numpy.ndarray): The full coordinates vector.
                    coordinate_subsets (list[CoordinateSubset]): The coordinate subsets.

                Returns:
                    numpy.ndarray: The coordinates of the subsets.

            )doc"
        )

        ;
}
