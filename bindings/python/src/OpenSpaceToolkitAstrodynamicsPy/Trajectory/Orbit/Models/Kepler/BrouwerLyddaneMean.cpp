/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/BrouwerLyddaneMean.hpp>

using namespace pybind11;

using ostk::core::types::Real;
using ostk::physics::units::Length;
using ostk::physics::units::Angle;

using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::orbit::models::kepler::BrouwerLyddaneMean;

// Trampoline class for virtual member functions
class PyBrouwerLyddaneMean : public BrouwerLyddaneMean
{
   public:
    using BrouwerLyddaneMean::BrouwerLyddaneMean;

    // Trampoline (need one for each virtual function)

    Angle getMeanAnomaly() const
    {
        PYBIND11_OVERRIDE_NAME(Angle, BrouwerLyddaneMean, "get_mean_anomaly", getMeanAnomaly);
    }

    Angle getTrueAnomaly() const
    {
        PYBIND11_OVERRIDE_NAME(Angle, BrouwerLyddaneMean, "get_true_anomaly", getTrueAnomaly);
    }

    Angle getEccentricAnomaly() const
    {
        PYBIND11_OVERRIDE_NAME(Angle, BrouwerLyddaneMean, "get_eccentric_anomaly", getEccentricAnomaly);
    }

    COE toCOE() const
    {
        PYBIND11_OVERRIDE_PURE_NAME(COE, BrouwerLyddaneMean, "to_coe", toCOE);
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMean(pybind11::module& aModule)
{
    class_<BrouwerLyddaneMean, PyBrouwerLyddaneMean, COE> brouwerLyddaneMean(aModule, "BrouwerLyddaneMean");

    brouwerLyddaneMean

        .def(
            init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>(),
            arg("semi_major_axis"),
            arg("eccentricity"),
            arg("inclination"),
            arg("raan"),
            arg("aop"),
            arg("mean_anomaly")
        )

        .def("get_mean_anomaly", &BrouwerLyddaneMean::getMeanAnomaly)
        .def("get_true_anomaly", &BrouwerLyddaneMean::getTrueAnomaly)
        .def("get_eccentric_anomaly", &BrouwerLyddaneMean::getEccentricAnomaly)
        .def(
            "get_cartesian_state", &BrouwerLyddaneMean::getCartesianState, arg("gravitational_parameter"), arg("frame")
        );
}
