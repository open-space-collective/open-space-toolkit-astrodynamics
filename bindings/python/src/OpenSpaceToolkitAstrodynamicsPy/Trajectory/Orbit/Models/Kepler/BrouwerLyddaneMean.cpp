/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMean.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/BrouwerLyddaneMeanLong.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Kepler/BrouwerLyddaneMeanShort.cpp>

using namespace pybind11;

using ostk::core::types::Real;
using ostk::physics::units::Length;
using ostk::physics::units::Angle;

using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMean;

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

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_BrouwerLyddaneMean(pybind11::module& aModule)
{
    {
        class_<BrouwerLyddaneMean, PyBrouwerLyddaneMean, COE> brouwerLyddaneMean(
            aModule,
            "BrouwerLyddaneMean",
            R"doc(
                Brouwer-Lyddane mean orbit elements. This is a parent class, please use the Short or Long child classes as appropriate.

                Group:
                    Kepler
            )doc"
        );

        brouwerLyddaneMean

            .def(
                init<const Length&, const Real&, const Angle&, const Angle&, const Angle&, const Angle&>(),
                R"doc(
                    Constructor.

                    Args:
                        semi_major_axis (Length): The semi-major axis.
                        eccentricity (Real): The eccentricity.
                        inclination (Angle): The inclination.
                        raan (Angle): The right ascension of the ascending node.
                        aop (Angle): The argument of periapsis.
                        mean_anomaly (Angle): The mean anomaly.

                    Group:
                        Constructors
                )doc",
                arg("semi_major_axis"),
                arg("eccentricity"),
                arg("inclination"),
                arg("raan"),
                arg("aop"),
                arg("mean_anomaly")
            )

            .def(
                "get_mean_anomaly",
                &BrouwerLyddaneMean::getMeanAnomaly,
                R"doc(
                    Get the mean anomaly of the `BrouwerLyddaneMean` model.

                    Returns:
                        Angle: The mean anomaly.

                )doc"
            )

            .def(
                "get_true_anomaly",
                &BrouwerLyddaneMean::getTrueAnomaly,
                R"doc(
                    Get the true anomaly of the `BrouwerLyddaneMean` model.

                    Returns:
                        Angle: The true anomaly.

                )doc"
            )

            .def(
                "get_eccentric_anomaly",
                &BrouwerLyddaneMean::getEccentricAnomaly,
                R"doc(
                    Get the eccentric anomaly of the `BrouwerLyddaneMean` model.

                    Returns:
                        Angle: The eccentric anomaly.

                )doc"
            )

            .def(
                "to_coe",
                &BrouwerLyddaneMean::toCOE,
                R"doc(
                    Convert the `BrouwerLyddaneMean` model to classical orbital elements.

                    Returns:
                        COE: The classical orbital elements.

                )doc"
            )

            .def(
                "get_cartesian_state",
                &BrouwerLyddaneMean::getCartesianState,
                arg("gravitational_parameter"),
                arg("frame"),
                R"doc(
                    Get the Cartesian state of the `BrouwerLyddaneMean` model.

                    Args:
                        gravitational_parameter (Real): The gravitational parameter of the central body.
                        frame (str): The reference frame in which the state is expressed.

                    Returns:
                        CartesianState: The Cartesian state.

                )doc"
            )

            ;
    }

    // Create "brouwerLyddaneMean" python submodule
    auto brouwerLyddaneMean = aModule.def_submodule("brouwerLyddaneMean");

    // Add __path__ attribute for "brouwerLyddaneMean" submodule
    brouwerLyddaneMean.attr("__path__") = "ostk.astrodynamics.trajectory.orbit.models.brouwerLyddaneMean";

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_BrouwerLyddaneMean_BrouwerLyddaneMeanShort(
        brouwerLyddaneMean
    );
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_BrouwerLyddaneMean_BrouwerLyddaneMeanLong(brouwerLyddaneMean
    );
}
