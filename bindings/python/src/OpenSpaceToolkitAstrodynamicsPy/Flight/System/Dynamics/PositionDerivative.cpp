/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_PositionDerivative(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::PositionDerivative;

    {
        class_<PositionDerivative, Dynamics, Shared<PositionDerivative>>(aModule, "PositionDerivative")
            .def(init<>())

            .def("__str__", &(shiftToString<PositionDerivative>))
            .def("__repr__", &(shiftToString<PositionDerivative>))

            .def("is_defined", &PositionDerivative::isDefined)

            .def("apply_contribution", &PositionDerivative::applyContribution, arg("x"), arg("dxdt"), arg("instant"));
    }
}
