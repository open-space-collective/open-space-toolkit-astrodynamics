/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/ModifiedEquinoctial.hpp>

namespace py = pybind11;

using ostk::astrodynamics::trajectory::orbit::model::kepler::ModifiedEquinoctial;

using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial(pybind11::module& aModule)
{
    using namespace pybind11;

    py::class_<ModifiedEquinoctial, Shared<ModifiedEquinoctial>>(
        aModule,
        "ModifiedEquinoctial",
        R"doc(
            Modified Equinoctial Orbital Elements (ModifiedEquinoctial).

            The Modified Equinoctial Orbital Elements (ModifiedEquinoctial) provide a non-singular representation of an orbit,
            useful for a wide range of eccentricities and inclinations (except for i = 180 deg).

            Elements:
            p: semi-latus rectum (m)
            f: x-component of eccentricity vector (e * cos(RAAN + AOP))
            g: y-component of eccentricity vector (e * sin(RAAN + AOP))
            h: x-component of node vector (tan(i/2) * cos(RAAN))
            k: y-component of node vector (tan(i/2) * sin(RAAN))
            L: true longitude (RAAN + AOP + True Anomaly) (rad)

        )doc"
    )
        .def(
            init<const Length&, const Real&, const Real&, const Real&, const Real&, const Angle&>(),
            arg("semi_latus_rectum"),
            arg("eccentricity_x"),
            arg("eccentricity_y"),
            arg("node_x"),
            arg("node_y"),
            arg("true_longitude"),
            R"doc(
                Constructor.

                Args:
                    semi_latus_rectum (Length): Semi-latus rectum.
                    eccentricity_x (float): x-component of eccentricity vector.
                    eccentricity_y (float): y-component of eccentricity vector.
                    node_x (float): x-component of node vector.
                    node_y (float): y-component of node vector.
                    true_longitude (Angle): True longitude.
            )doc"
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<ModifiedEquinoctial>))
        .def("__repr__", &(shiftToString<ModifiedEquinoctial>))

        .def(
            "is_defined",
            &ModifiedEquinoctial::isDefined,
            R"doc(
                Check if ModifiedEquinoctial is defined.

                Returns:
                    bool: True if ModifiedEquinoctial is defined.
            )doc"
        )

        .def(
            "get_semi_latus_rectum",
            &ModifiedEquinoctial::getSemiLatusRectum,
            R"doc(
                Get semi-latus rectum (p).

                Returns:
                    Length: Semi-latus rectum.
            )doc"
        )
        .def(
            "get_eccentricity_x",
            &ModifiedEquinoctial::getEccentricityX,
            R"doc(
                Get x-component of eccentricity vector (f).

                Returns:
                    float: f component.
            )doc"
        )
        .def(
            "get_eccentricity_y",
            &ModifiedEquinoctial::getEccentricityY,
            R"doc(
                Get y-component of eccentricity vector (g).

                Returns:
                    float: g component.
            )doc"
        )
        .def(
            "get_node_x",
            &ModifiedEquinoctial::getNodeX,
            R"doc(
                Get x-component of node vector (h).

                Returns:
                    float: h component.
            )doc"
        )
        .def(
            "get_node_y",
            &ModifiedEquinoctial::getNodeY,
            R"doc(
                Get y-component of node vector (k).

                Returns:
                    float: k component.
            )doc"
        )
        .def(
            "get_true_longitude",
            &ModifiedEquinoctial::getTrueLongitude,
            R"doc(
                Get true longitude (L).

                Returns:
                    Angle: True longitude.
            )doc"
        )

        .def(
            "get_cartesian_state",
            &ModifiedEquinoctial::getCartesianState,
            arg("gravitational_parameter"),
            arg("frame"),
            R"doc(
                Get Cartesian state (position, velocity) from ModifiedEquinoctial.

                Args:
                    gravitational_parameter (Derived): Gravitational parameter of the central body.
                    frame (Frame): The reference frame for the output Cartesian state. Must be an inertial frame.

                Returns:
                    tuple[Position, Velocity]: Position and Velocity.
            )doc"
        )

        .def(
            "get_si_vector",
            &ModifiedEquinoctial::getSIVector,
            R"doc(
                Get ModifiedEquinoctial elements as a 6D vector in SI units.
                [p (m), f, g, h, k, L (rad)]

                Returns:
                    numpy.ndarray: 6D vector of elements in SI units.
            )doc"
        )

        .def_static(
            "undefined",
            &ModifiedEquinoctial::Undefined,
            R"doc(
                Create an undefined ModifiedEquinoctial object.

                Returns:
                    ModifiedEquinoctial: Undefined ModifiedEquinoctial object.
            )doc"
        )
        .def_static(
            "cartesian",
            &ModifiedEquinoctial::Cartesian,
            arg("cartesian_state"),
            arg("gravitational_parameter"),
            R"doc(
                Create ModifiedEquinoctial from Cartesian state (position, velocity).

                Args:
                    cartesian_state (tuple[Position, Velocity]): Cartesian state (Position, Velocity). Must be in an inertial frame.
                    gravitational_parameter (Derived): Gravitational parameter of the central body.

                Returns:
                    ModifiedEquinoctial: ModifiedEquinoctial object.
            )doc"
        )

        .def_static(
            "coe",
            &ModifiedEquinoctial::COE,
            arg("coe"),
            R"doc(
                Create Modified Equinoctial elements from Classical Orbital Elements (COE).

                Args:
                    coe (COE): Classical Orbital Elements.

                Returns:
                    ModifiedEquinoctial: Modified Equinoctial Elements.
            )doc"
        )

        ;
}
