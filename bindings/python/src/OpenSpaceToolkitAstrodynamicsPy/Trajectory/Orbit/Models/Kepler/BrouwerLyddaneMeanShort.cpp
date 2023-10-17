/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_BrouwerLyddaneMean_BrouwerLyddaneMeanShort(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::physics::units::Angle;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMean;
    using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMeanShort;

    class_<BrouwerLyddaneMeanShort, BrouwerLyddaneMean> brouwerLyddaneMeanShort(aModule, "BrouwerLyddaneMeanShort");

    brouwerLyddaneMeanShort

        .def(
            init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>(),
            arg("semi_major_axis"),
            arg("eccentricity"),
            arg("inclination"),
            arg("raan"),
            arg("aop"),
            arg("mean_anomaly")
        )

        .def("to_coe", &BrouwerLyddaneMeanShort::toCOE)

        .def_static("COE", &BrouwerLyddaneMeanShort::COE, arg("coe"))

        .def_static(
            "cartesian", &BrouwerLyddaneMeanShort::Cartesian, arg("cartersian_state"), arg("gravitational_parameter")
        )

        .def_static("undefined", &BrouwerLyddaneMeanShort::Undefined);
}
