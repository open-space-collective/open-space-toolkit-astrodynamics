/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Dynamics/Thrust.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_Thrust(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::trajectory::Dynamics;
    using ostk::astro::trajectory::dynamics::Thrust;

    {
        class_<Thrust, Dynamics, Shared<Thrust>>(aModule, "Thrust")
            // .def(init<const Shared<Celestial>&>(), arg("celestial"))

            // .def("__str__", &(shiftToString<Thrust>))
            // .def("__repr__", &(shiftToString<Thrust>))

            .def("is_defined", &Thrust::isDefined)

            // getters

            .def("apply_contribution", &Thrust::applyContribution, arg("x"), arg("dxdt"), arg("instant"));
    }
}
