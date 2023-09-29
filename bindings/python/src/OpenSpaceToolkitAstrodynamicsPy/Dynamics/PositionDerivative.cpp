/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_PositionDerivative(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::Dynamics;
    using ostk::astro::dynamics::PositionDerivative;

    {
        class_<PositionDerivative, Dynamics, Shared<PositionDerivative>>(aModule, "PositionDerivative")
            .def(init<>())

            .def("__str__", &(shiftToString<PositionDerivative>))
            .def("__repr__", &(shiftToString<PositionDerivative>))

            .def("is_defined", &PositionDerivative::isDefined)

            .def(
                "compute_contribution", &PositionDerivative::computeContribution, arg("instant"), arg("x"), arg("frame")
            );
    }
}
