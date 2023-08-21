/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/AtmosphericDrag.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/CentralBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/PositionDerivative.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/ThirdBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinateSubset.cpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::state::CoordinatesSubset;

// Trampoline class for virtual member functions
class PyEventCondition : public Dynamics
{
   public:
    // Trampoline (need one for each virtual function)

    bool isDefined() const override
    {
        PYBIND11_OVERRIDE(bool, Dynamics::Dynamics, isDefined);
    }

    Array<Shared<const CoordinateSubset>> getReadCoordinatesSubsets() const override
    {
        PYBIND11_OVERRIDE(bool, Dynamics::Dynamics, getReadCoordinatesSubsets);
    }

    Array<Shared<const CoordinateSubset>> getWriteCoordinatesSubsets() const override
    {
        PYBIND11_OVERRIDE(bool, Dynamics::Dynamics, getWriteCoordinatesSubsets);
    }

    VectorXd computeContribution(const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr, )
        const override
    {
        PYBIND11_OVERRIDE(VectorXd, Dynamics::Dynamics, computeContribution, anInstant, x, aFrameSPtr);
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics(pybind11::module& aModule)
{
    class_<Dynamics, PyEventCondition, Shared<Dynamics>>(aModule, "Dynamics")

        .def("get_name", &Dynamics::getName)

        .def("get_read_coordinates_subsets", &Dynamics::getReadCoordinatesSubsets)
        .def("get_write_coordinates_subsets", &Dynamics::getWriteCoordinatesSubsets)
        .def("is_defined", &Dynamics::isDefined)

        .def("compute_contribution", &Dynamics::computeContribution, arg("instant"), arg("state_vector"), arg("frame"))

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
}
