/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/MEOE.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Angle.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>

namespace py = pybind11;

using ostk::astrodynamics::trajectory::orbit::model::MEOE;

using ostk::core::type::Real;
using ostk::core::container::Pair;

using ostk::physics::unit::Length;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::coordinate::Frame; // Required for Shared<Frame> in getCartesianState

// Trampoline class for virtual methods if MEOE were to be inherited in Python and have virtuals
// Not strictly necessary for MEOE as it's a concrete class here.

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_MEOE(pybind11::module& aModule)
{
    using namespace pybind11;

    py::class_<MEOE, std::shared_ptr<MEOE>>(
        aModule,
        "MEOE",
        R"doc(
            Modified Equinoctial Orbital Elements (MEOE).

            The Modified Equinoctial Orbital Elements (MEOE) provide a non-singular representation of an orbit,
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
            arg("p"),
            arg("f"),
            arg("g"),
            arg("h"),
            arg("k"),
            arg("L"),
            R"doc(
                Constructor.

                Args:
                    p (Length): Semi-latus rectum.
                    f (float): x-component of eccentricity vector.
                    g (float): y-component of eccentricity vector.
                    h (float): x-component of node vector.
                    k (float): y-component of node vector.
                    L (Angle): True longitude.
            )doc"
        )

        .def(self == self, "Equality operator.")
        .def(self != self, "Inequality operator.")

        .def(
            "__str__",
            [](const MEOE& self)
            {
                std::ostringstream oss;
                self.print(oss, true);  // Use decorator for __str__
                return oss.str();
            }
        )
        .def(
            "__repr__",
            [](const MEOE& self)
            {
                if (!self.isDefined()) {
                    return "MEOE.undefined()";
                }
                std::ostringstream oss;
                oss << "MEOE(p=" << self.getSemiLatusRectum().toString() << ", f=" << self.getF().toString()
                    << ", g=" << self.getG().toString() << ", h=" << self.getH().toString()
                    << ", k=" << self.getK().toString() << ", L=" << self.getTrueLongitude().toString() << ")";
                return oss.str();
            }
        )

        .def(
            "is_defined",
            &MEOE::isDefined,
            R"doc(
                Check if MEOE is defined.

                Returns:
                    bool: True if MEOE is defined.
            )doc"
        )

        .def(
            "get_semi_latus_rectum",
            &MEOE::getSemiLatusRectum,
            R"doc(
                Get semi-latus rectum (p).

                Returns:
                    Length: Semi-latus rectum.
            )doc"
        )
        .def(
            "get_f",
            &MEOE::getF,
            R"doc(
                Get x-component of eccentricity vector (f).

                Returns:
                    float: f component.
            )doc"
        )
        .def(
            "get_g",
            &MEOE::getG,
            R"doc(
                Get y-component of eccentricity vector (g).

                Returns:
                    float: g component.
            )doc"
        )
        .def(
            "get_h",
            &MEOE::getH,
            R"doc(
                Get x-component of node vector (h).

                Returns:
                    float: h component.
            )doc"
        )
        .def(
            "get_k",
            &MEOE::getK,
            R"doc(
                Get y-component of node vector (k).

                Returns:
                    float: k component.
            )doc"
        )
        .def(
            "get_true_longitude",
            &MEOE::getTrueLongitude,
            R"doc(
                Get true longitude (L).

                Returns:
                    Angle: True longitude.
            )doc"
        )

        .def(
            "get_cartesian_state",
            &MEOE::getCartesianState,
            arg("gravitational_parameter"),
            arg("frame"),
            R"doc(
                Get Cartesian state (position, velocity) from MEOE.

                Args:
                    gravitational_parameter (Derived): Gravitational parameter of the central body.
                    frame (Frame): The reference frame for the output Cartesian state.

                Returns:
                    tuple[Position, Velocity]: Position and Velocity.
            )doc"
        )

        .def(
            "get_si_vector",
            &MEOE::getSIVector,
            R"doc(
                Get MEOE elements as a 6D vector in SI units.
                [p (m), f, g, h, k, L (rad)]

                Returns:
                    numpy.ndarray: 6D vector of elements in SI units.
            )doc"
        )

        .def_static(
            "undefined",
            &MEOE::Undefined,
            R"doc(
                Create an undefined MEOE object.

                Returns:
                    MEOE: Undefined MEOE object.
            )doc"
        )
        .def_static(
            "cartesian",
            &MEOE::Cartesian,
            arg("cartesian_state"),
            arg("gravitational_parameter"),
            R"doc(
                Create MEOE from Cartesian state (position, velocity).

                Args:
                    cartesian_state (tuple[Position, Velocity]): Cartesian state (Position, Velocity).
                    gravitational_parameter (Derived): Gravitational parameter of the central body.

                Returns:
                    MEOE: MEOE object.
            )doc"
        )
        // MEOE::Element and MEOE::StringFromElement are not bound as they are mainly C++ utility
        // The __repr__ and getters provide sufficient Python introspection.
        ;
}
