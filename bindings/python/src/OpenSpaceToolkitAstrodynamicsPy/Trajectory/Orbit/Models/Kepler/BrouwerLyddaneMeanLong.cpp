/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_BrouwerLyddaneMean_BrouwerLyddaneMeanLong(
    pybind11::module& aModule
)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::physics::units::Angle;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMean;
    using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMeanLong;

    class_<BrouwerLyddaneMeanLong, BrouwerLyddaneMean> brouwerLyddaneMeanLong(aModule, "BrouwerLyddaneMeanLong");

    brouwerLyddaneMeanLong

        .def(
            init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>(),
            arg("semi_major_axis"),
            arg("eccentricity"),
            arg("inclination"),
            arg("raan"),
            arg("aop"),
            arg("mean_anomaly")
        )

        .def("to_coe", &BrouwerLyddaneMeanLong::toCOE)

        .def_static("COE", &BrouwerLyddaneMeanLong::COE, arg("coe"))

        .def_static(
            "cartesian", &BrouwerLyddaneMeanLong::Cartesian, arg("cartersian_state"), arg("gravitational_parameter")
        )

        .def_static("undefined", &BrouwerLyddaneMeanLong::Undefined)

        ;
}
