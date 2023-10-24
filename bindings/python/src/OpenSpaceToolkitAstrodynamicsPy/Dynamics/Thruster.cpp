/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics/Thruster/ConstantThrust.cpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Shared;
using ostk::core::ctnr::Array;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::Dynamics;
using ostk::astro::dynamics::Thruster;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;

// Trampoline class for virtual member functions
class PyThruster : public Thruster
{
   public:
    using Thruster::Thruster;

    // Trampoline (need one for each virtual function)

    void print(std::ostream& anOutputStream, bool displayDecorator) const override
    {
        PYBIND11_OVERRIDE(void, Thruster, print, anOutputStream, displayDecorator);
    }

    bool isDefined() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(bool, Thruster, "is_defined", isDefined);
    }

    Array<Shared<const CoordinatesSubset>> getReadCoordinatesSubsets() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<Shared<const CoordinatesSubset>>, Thruster, "get_read_coordinates_subsets", getReadCoordinatesSubsets
        );
    }

    Array<Shared<const CoordinatesSubset>> getWriteCoordinatesSubsets() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<Shared<const CoordinatesSubset>>,
            Thruster,
            "get_write_coordinates_subsets",
            getWriteCoordinatesSubsets
        );
    }

    VectorXd computeContribution(const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr)
        const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            VectorXd, Thruster, "compute_contribution", computeContribution, anInstant, x, aFrameSPtr
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_Thruster(pybind11::module& aModule)
{
    class_<Thruster, PyThruster, Dynamics, Shared<Thruster>>(
        aModule,
        "Thruster",
        R"doc(
            Abstract Thruster Class.

            Base class to derive other thruster classes from. Cannot be instantiated.

            Group:
                dynamics
        )doc"
    )

        .def(
            init<const SatelliteSystem&, const String&>(),
            arg("satellite_system"),
            arg("name") = String::Empty(),
            R"doc(
                Constructor.

                Args:
                    satellite_system (SatelliteSystem): The satellite system.
                    name (str): The name of the thruster.

            )doc"
        )

        .def(
            "get_name",
            &Thruster::getName,
            R"doc(
                Get the name of the thruster.

                Returns:
                    str: The name of the thruster.

            )doc"
        )

        .def(
            "get_satelltite_system",
            &Thruster::getSatelliteSystem,
            R"doc(
                Get the satellite system of the thruster.

                Returns:
                    SatelliteSystem: The satellite system.

            )doc"
        )

        .def("__str__", &(shiftToString<Thruster>))
        .def("__repr__", &(shiftToString<Thruster>))

        .def(
            "is_defined",
            &Thruster::isDefined,
            R"doc(
                Check if the thruster is defined.

                Returns:
                    bool: True if the thruster is defined, False otherwise.

            )doc"
        )

        .def(
            "compute_contribution",
            &Thruster::computeContribution,
            arg("instant"),
            arg("state_vector"),
            arg("frame"),
            R"doc(
                Compute the contribution of the thruster to the state vector.

                Args:
                    instant (Instant): The instant of the state vector.
                    state_vector (numpy.ndarray): The state vector.
                    frame (Frame): The reference frame.

                Returns:
                    numpy.ndarray: The contribution of the thruster to the state vector.

            )doc"
        )

        ;

    // Create "thruster" python submodule
    auto thruster = aModule.def_submodule("thruster");

    // Add __path__ attribute for "thruster" submodule
    thruster.attr("__path__") = "ostk.astrodynamics.dynamics.thruster";

    // Add objects to "thruster" submodule
    OpenSpaceToolkitAstrodynamicsPy_Dynamics_Thruster_ConstantThrust(thruster);
}
