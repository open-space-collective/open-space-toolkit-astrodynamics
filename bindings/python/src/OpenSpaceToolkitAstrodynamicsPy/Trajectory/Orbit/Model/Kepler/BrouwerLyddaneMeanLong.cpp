/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_BrouwerLyddaneMean_BrouwerLyddaneMeanLong(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::type::Real;

    using ostk::physics::unit::Angle;
    using ostk::physics::unit::Length;

    using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMean;
    using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;

    class_<BrouwerLyddaneMeanLong, BrouwerLyddaneMean> brouwerLyddaneMeanLong(
        aModule,
        "BrouwerLyddaneMeanLong",
        R"doc(
            Brouwer-Lyddane Mean (Long) orbit elements. Short periodic variations and secular variations are averaged.

        )doc"
    );

    brouwerLyddaneMeanLong

        .def(
            init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>(),
            R"doc(
                Constructor.

                Args:
                    semi_major_axis (Length): The semi-major axis.
                    eccentricity (float): The eccentricity.
                    inclination (Angle): The inclination.
                    raan (Angle): The right ascension of the ascending node.
                    aop (Angle): The argument of periapsis.
                    mean_anomaly (Angle): The mean anomaly.

            )doc",
            arg("semi_major_axis"),
            arg("eccentricity"),
            arg("inclination"),
            arg("raan"),
            arg("aop"),
            arg("mean_anomaly")
        )

        .def(
            "to_coe",
            &BrouwerLyddaneMeanLong::toCOE,
            R"doc(
                Convert the `BrouwerLyddaneMeanLong` model to classical orbital elements.

                Returns:
                    COE: The classical orbital elements.

            )doc"
        )

        .def_static(
            "COE",
            &BrouwerLyddaneMeanLong::COE,
            R"doc(
                Create a `BrouwerLyddaneMeanLong` model from classical orbital elements.

                Args:
                    coe (COE): The classical orbital elements.

                Returns:
                    BrouwerLyddaneMeanLong: The `BrouwerLyddaneMeanLong` model.
            )doc",
            arg("coe")
        )

        .def_static(
            "cartesian",
            &BrouwerLyddaneMeanLong::Cartesian,
            R"doc(
                Create a `BrouwerLyddaneMeanLong` model from Cartesian state.

                Args:
                    cartesian_state (CartesianState): The Cartesian state.
                    gravitational_parameter (float): The gravitational parameter of the central body.

                Returns:
                    BrouwerLyddaneMeanLong: The `BrouwerLyddaneMeanLong` model.
            )doc",
            arg("cartesian_state"),
            arg("gravitational_parameter")
        )

        .def_static(
            "undefined",
            &BrouwerLyddaneMeanLong::Undefined,
            R"doc(
                Create an undefined `BrouwerLyddaneMeanLong` model.

                Returns:
                    BrouwerLyddaneMeanLong: The undefined `BrouwerLyddaneMeanLong` model.
            )doc"
        )

        ;
}
