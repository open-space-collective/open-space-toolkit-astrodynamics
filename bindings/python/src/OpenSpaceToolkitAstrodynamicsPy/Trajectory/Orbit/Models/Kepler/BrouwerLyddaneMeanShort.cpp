/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_BrouwerLyddaneMean_BrouwerLyddaneMeanShort(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::type::Real;

    using ostk::physics::units::Angle;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMean;
    using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMeanShort;

    class_<BrouwerLyddaneMeanShort, BrouwerLyddaneMean> brouwerLyddaneMeanShort(
        aModule,
        "BrouwerLyddaneMeanShort",
        R"doc(
            Brouwer-Lyddane Mean (Short) orbit elements. Short periodic variations are averaged.

        )doc"
    );

    brouwerLyddaneMeanShort

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
            &BrouwerLyddaneMeanShort::toCOE,
            R"doc(
                Convert the `BrouwerLyddaneMeanShort` model to classical orbital elements.

                Returns:
                    COE: The classical orbital elements.
            )doc"
        )

        .def_static(
            "COE",
            &BrouwerLyddaneMeanShort::COE,
            R"doc(
                Create a `BrouwerLyddaneMeanShort` model from classical orbital elements.

                Args:
                    coe (COE): The classical orbital elements.

                Returns:
                    BrouwerLyddaneMeanShort: The `BrouwerLyddaneMeanShort` model.
            )doc",
            arg("coe")
        )

        .def_static(
            "cartesian",
            &BrouwerLyddaneMeanShort::Cartesian,
            R"doc(
                Create a `BrouwerLyddaneMeanShort` model from Cartesian state.

                Args:
                    cartesian_state (CartesianState): The Cartesian state.
                    gravitational_parameter (float): The gravitational parameter of the central body.

                Returns:
                    BrouwerLyddaneMeanShort: The `BrouwerLyddaneMeanShort` model.
            )doc",
            arg("cartesian_state"),
            arg("gravitational_parameter")
        )

        .def_static(
            "undefined",
            &BrouwerLyddaneMeanShort::Undefined,
            R"doc(
                Create an undefined `BrouwerLyddaneMeanShort` model.

                Returns:
                    BrouwerLyddaneMeanShort: The undefined `BrouwerLyddaneMeanShort` model.
            )doc"
        )

        ;
}
