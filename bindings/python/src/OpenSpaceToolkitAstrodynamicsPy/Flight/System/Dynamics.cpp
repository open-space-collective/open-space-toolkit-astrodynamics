/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/AtmosphericDrag.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/CentralBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/PositionDerivative.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/ThirdBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/Thruster.cpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::Environment;
using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;

// Trampoline class for virtual member functions
class PyDynamics : public Dynamics
{
   public:
    using Dynamics::Dynamics;

    // Trampoline (need one for each virtual function)

    void print(std::ostream& anOutputStream, bool displayDecorator) const override
    {
        PYBIND11_OVERRIDE(void, Dynamics, print, anOutputStream, displayDecorator);
    }

    bool isDefined() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(bool, Dynamics, "is_defined", isDefined);
    }

    Array<Shared<const CoordinatesSubset>> getReadCoordinatesSubsets() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<Shared<const CoordinatesSubset>>, Dynamics, "get_read_coordinates_subsets", getReadCoordinatesSubsets
        );
    }

    Array<Shared<const CoordinatesSubset>> getWriteCoordinatesSubsets() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<Shared<const CoordinatesSubset>>,
            Dynamics,
            "get_write_coordinates_subsets",
            getWriteCoordinatesSubsets
        );
    }

    VectorXd computeContribution(const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr)
        const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            VectorXd, Dynamics, "compute_contribution", computeContribution, anInstant, x, aFrameSPtr
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics(pybind11::module& aModule)
{
    class_<Dynamics, PyDynamics, Shared<Dynamics>>(aModule, "Dynamics")

        .def(init<const String&>(), arg("name"))

        .def("get_name", &Dynamics::getName)

        .def("__str__", &(shiftToString<Dynamics>))
        .def("__repr__", &(shiftToString<Dynamics>))

        .def("is_defined", &Dynamics::isDefined)
        .def("get_read_coordinates_subsets", &Dynamics::getReadCoordinatesSubsets)
        .def("get_write_coordinates_subsets", &Dynamics::getWriteCoordinatesSubsets)
        .def("compute_contribution", &Dynamics::computeContribution, arg("instant"), arg("state_vector"), arg("frame"))

        .def_static(
            "from_environment",
            &Dynamics::FromEnvironment,
            arg("environment"),
            arg("satellite_system") = SatelliteSystem::Undefined()
        )

        ;

    // Create "dynamics" python submodule
    auto dynamics = aModule.def_submodule("dynamics");

    // Add __path__ attribute for "dynamics" submodule
    dynamics.attr("__path__") = "ostk.astrodynamics.flight.system.dynamics";

    // Add objects to "dynamics" submodule
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_PositionDerivative(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_CentralBodyGravity(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_ThirdBodyGravity(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_AtmosphericDrag(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_Thruster(dynamics);
}
