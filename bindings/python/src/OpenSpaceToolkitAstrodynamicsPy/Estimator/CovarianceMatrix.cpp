/// Apache License 2.0

#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Estimator_CovarianceMatrix(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::mathematics::object::MatrixXd;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::time::Instant;

    using ostk::astrodynamics::estimator::CovarianceMatrix;
    using ostk::astrodynamics::trajectory::state::CoordinateSubset;

    class_<CovarianceMatrix>(
        aModule,
        "CovarianceMatrix",
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
                Construct a Covariance Matrix.

                Args:
                    instant (Instant): An instant.
                    coordinates (np.ndarray): The coordinates at the instant in International System of Units.
                    frame (Frame): The reference frame in which the coordinates are referenced to and resolved in.
                    coordinate_subsets (list[CoordinateSubset]): The coordinate subsets associated to the coordinates.
            )doc",
            arg("instant"),
            arg("coordinates"),
            arg("frame"),
            arg("coordinate_subsets")
        )
        .def(init<const CovarianceMatrix&>(), arg("covariance_matrix"))

        .def(self == self)
        .def(self != self)
        .def(self + self)
        .def(self - self)

        .def("__str__", &(shiftToString<CovarianceMatrix>))
        .def("__repr__", &(shiftToString<CovarianceMatrix>))

        .def(
            "access_instant",
            &CovarianceMatrix::accessInstant,
            return_value_policy::reference_internal,
            R"doc(
                Access the instant.

                Returns:
                    Instant: The instant.
            )doc"
        )
        .def(
            "access_frame",
            &CovarianceMatrix::accessFrame,
            R"doc(
                Access the reference frame.

                Returns:
                    Frame: The reference frame.
            )doc"
        )
        .def(
            "access_coordinates",
            &CovarianceMatrix::accessCoordinates,
            R"doc(
                Access the coordinates.

                Returns:
                    np.ndarray: The coordinates.
            )doc"
        )
        .def(
            "get_size",
            &CovarianceMatrix::getSize,
            R"doc(
                Get the size of the Covariance Matrix.

                Returns:
                    int: The size of the Covariance Matrix.
            )doc"
        )
        .def(
            "get_instant",
            &CovarianceMatrix::getInstant,
            R"doc(
                Get the instant associated with the Covariance Matrix.

                Returns:
                    Instant: The instant.
            )doc"
        )
        .def(
            "get_frame",
            &CovarianceMatrix::getFrame,
            R"doc(
                Get the reference frame associated with the Covariance Matrix.

                Returns:
                    Frame: The reference frame.
            )doc"
        )
        .def(
            "get_coordinates",
            &CovarianceMatrix::getCoordinates,
            R"doc(
                Get the coordinates of the Covariance Matrix.

                Returns:
                    np.ndarray: The coordinates.
            )doc"
        )
        .def(
            "get_coordinate_subsets",
            &CovarianceMatrix::getCoordinateSubsets,
            R"doc(
                Get the coordinate subsets of the Covariance Matrix.

                Returns:
                    list[CoordinateSubset]: The coordinate subsets.
            )doc"
        )
        .def(
            "extract_coordinate",
            &CovarianceMatrix::extractCoordinate,
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
            &CovarianceMatrix::extractCoordinates,
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
            &CovarianceMatrix::rotate,
            arg("frame"),
            R"doc(
                Return a new Covariance Matrix rotated into a different reference frame.

                Args:
                    frame (Frame): The reference frame to rotate into.

                Returns:
                    CovarianceMatrix: The rotated Covariance Matrix.
            )doc"
        )
        .def(
            "diagonalize",
            &CovarianceMatrix::diagonalize,
            R"doc(
                Return a new Covariance Matrix retaining only the diagonal variance terms.

                Returns:
                    CovarianceMatrix: The diagonalized Covariance Matrix.
            )doc"
        )
        .def(
            "reduce",
            &CovarianceMatrix::reduce,
            arg("coordinate_subsets"),
            R"doc(
                Return a new Covariance Matrix reduced to the given coordinate subsets.

                Args:
                    coordinate_subsets (list[CoordinateSubset]): The coordinate subsets to retain.

                Returns:
                    CovarianceMatrix: The reduced Covariance Matrix.
            )doc"
        )
        .def(
            "scale",
            &CovarianceMatrix::scale,
            arg("scalar"),
            R"doc(
                Return a new Covariance Matrix whose coordinates are multiplied by a scalar.

                Args:
                    scalar (float): The strictly positive scalar to multiply the covariance coordinates by.

                Returns:
                    CovarianceMatrix: The scaled Covariance Matrix.
            )doc"
        )

        .def_static(
            "from_position_sigmas",
            &CovarianceMatrix::FromPositionSigmas,
            arg("instant"),
            arg("position_sigmas"),
            arg("frame"),
            R"doc(
                Build a 3x3 Covariance Matrix from cartesian position sigmas.

                Args:
                    instant (Instant): An instant.
                    position_sigmas (np.ndarray): The cartesian position sigmas.
                    frame (Frame): The reference frame in which the covariance is referenced to and resolved in.

                Returns:
                    CovarianceMatrix: A diagonal Covariance Matrix.
            )doc"
        )
        .def_static(
            "from_position_velocity_sigmas",
            &CovarianceMatrix::FromPositionVelocitySigmas,
            arg("instant"),
            arg("position_sigmas"),
            arg("velocity_sigmas"),
            arg("frame"),
            R"doc(
                Build a 6x6 Covariance Matrix from cartesian position and velocity sigmas.

                Args:
                    instant (Instant): An instant.
                    position_sigmas (np.ndarray): The cartesian position sigmas.
                    velocity_sigmas (np.ndarray): The cartesian velocity sigmas.
                    frame (Frame): The reference frame in which the covariance is referenced to and resolved in.

                Returns:
                    CovarianceMatrix: A diagonal Covariance Matrix.
            )doc"
        )

        ;
}
