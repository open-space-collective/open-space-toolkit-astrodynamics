/// Apache License 2.0

#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Uncertainty_Covariance(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::mathematics::object::MatrixXd;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::time::Instant;

    using ostk::astrodynamics::trajectory::state::CoordinateSubset;
    using ostk::astrodynamics::uncertainty::Covariance;

    class_<Covariance>(
        aModule,
        "Covariance",
        R"doc(
            Covariance matrix at a given instant in time.

        )doc"
    )

        .def(
            init<
                const Instant&,
                const MatrixXd&,
                const Shared<const Frame>&,
                const Array<Shared<const CoordinateSubset>>&>(),
            R"doc(
                Construct a Covariance.

                Args:
                    instant (Instant): An instant.
                    coordinates (np.ndarray): The coordinates at the instant in International System of Units.
                    frame (Frame): The reference frame in which the coordinates are referenced to and resolved in.
                    coordinate_subsets (list[CoordinateSubset]): The coordinate subsets associated with the coordinates.

                Raises:
                    RuntimeError: If the coordinates are not square, do not match the coordinate subsets, are not exactly symmetric or are not positive semi-definite.
            )doc",
            arg("instant"),
            arg("coordinates"),
            arg("frame"),
            arg("coordinate_subsets")
        )
        .def(init<const Covariance&>(), arg("covariance"))

        .def(
            self + self,
            R"doc(
                Add two Covariances element-wise.

                Args:
                    covariance (Covariance): The other Covariance.

                Returns:
                    Covariance: The element-wise sum of the two Covariances.
            )doc"
        )
        .def(
            self - self,
            R"doc(
                Subtract two Covariances element-wise.

                Args:
                    covariance (Covariance): The other Covariance.

                Returns:
                    Covariance: The element-wise difference between the two Covariances.
            )doc"
        )

        .def("__str__", &(shiftToString<Covariance>))
        .def("__repr__", &(shiftToString<Covariance>))

        .def(
            "get_size",
            &Covariance::getSize,
            R"doc(
                Get the size of the Covariance.

                Returns:
                    int: The size of the Covariance.
            )doc"
        )
        .def(
            "get_instant",
            &Covariance::getInstant,
            R"doc(
                Get the instant associated with the Covariance.

                Returns:
                    Instant: The instant.
            )doc"
        )
        .def(
            "get_frame",
            &Covariance::getFrame,
            R"doc(
                Get the reference frame associated with the Covariance.

                Returns:
                    Frame: The reference frame.
            )doc"
        )
        .def(
            "get_coordinates",
            &Covariance::getCoordinates,
            R"doc(
                Get the coordinates of the Covariance.

                Returns:
                    np.ndarray: The coordinates.
            )doc"
        )
        .def(
            "get_coordinate_subsets",
            &Covariance::getCoordinateSubsets,
            R"doc(
                Get the coordinate subsets of the Covariance.

                Returns:
                    list[CoordinateSubset]: The coordinate subsets.
            )doc"
        )
        .def(
            "extract_coordinate",
            &Covariance::extractCoordinate,
            arg("coordinate_subset"),
            R"doc(
                Extract the coordinates for a single subset.

                Args:
                    coordinate_subset (CoordinateSubset): The subset to extract the coordinates for.

                Returns:
                    np.ndarray: The coordinates for the subset.
            )doc"
        )
        .def(
            "extract_coordinates",
            &Covariance::extractCoordinates,
            arg("coordinate_subsets"),
            R"doc(
                Extract the coordinates for multiple subsets.

                Args:
                    coordinate_subsets (list[CoordinateSubset]): The subsets to extract the coordinates for.

                Returns:
                    np.ndarray: The coordinates for the subsets.
            )doc"
        )
        .def(
            "rotate",
            &Covariance::rotate,
            arg("frame"),
            R"doc(
                Return a new Covariance rotated into a different reference frame.

                Args:
                    frame (Frame): The reference frame to rotate into.

                Returns:
                    Covariance: The rotated Covariance.
            )doc"
        )
        .def(
            "diagonalize",
            &Covariance::diagonalize,
            R"doc(
                Return a new Covariance retaining only the diagonal variance terms.

                Returns:
                    Covariance: The diagonalized Covariance.
            )doc"
        )
        .def(
            "reduce",
            &Covariance::reduce,
            arg("coordinate_subsets"),
            R"doc(
                Return a new Covariance reduced to the given coordinate subsets.

                Args:
                    coordinate_subsets (list[CoordinateSubset]): The coordinate subsets to retain.

                Returns:
                    Covariance: The reduced Covariance.
            )doc"
        )
        .def(
            "scale",
            &Covariance::scale,
            arg("scalar"),
            R"doc(
                Return a new Covariance whose coordinates are multiplied by a scalar.

                Args:
                    scalar (float): The strictly positive scalar to multiply the covariance coordinates by.

                Returns:
                    Covariance: The scaled Covariance.
            )doc"
        )

        .def_static(
            "from_position_sigmas",
            &Covariance::FromPositionSigmas,
            arg("instant"),
            arg("position_sigmas"),
            arg("frame"),
            R"doc(
                Build a 3x3 Covariance from cartesian position sigmas.

                Args:
                    instant (Instant): An instant.
                    position_sigmas (np.ndarray): The cartesian position sigmas.
                    frame (Frame): The reference frame in which the covariance is referenced to and resolved in.

                Returns:
                    Covariance: A diagonal Covariance.
            )doc"
        )
        .def_static(
            "from_position_velocity_sigmas",
            &Covariance::FromPositionVelocitySigmas,
            arg("instant"),
            arg("position_sigmas"),
            arg("velocity_sigmas"),
            arg("frame"),
            R"doc(
                Build a 6x6 Covariance from cartesian position and velocity sigmas.

                Args:
                    instant (Instant): An instant.
                    position_sigmas (np.ndarray): The cartesian position sigmas.
                    velocity_sigmas (np.ndarray): The cartesian velocity sigmas.
                    frame (Frame): The reference frame in which the covariance is referenced to and resolved in.

                Returns:
                    Covariance: A diagonal Covariance.
            )doc"
        )

        ;
}
