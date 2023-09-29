/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_ThirdBodyGravity(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::Dynamics;
    using ostk::astro::dynamics::ThirdBodyGravity;

    {
        class_<ThirdBodyGravity, Dynamics, Shared<ThirdBodyGravity>>(aModule, "ThirdBodyGravity")
            .def(init<const Shared<Celestial>&>(), arg("celestial"))

            .def("__str__", &(shiftToString<ThirdBodyGravity>))
            .def("__repr__", &(shiftToString<ThirdBodyGravity>))

            .def("is_defined", &ThirdBodyGravity::isDefined)

            .def("get_celestial", &ThirdBodyGravity::getCelestial)

            .def(
                "compute_contribution", &ThirdBodyGravity::computeContribution, arg("instant"), arg("x"), arg("frame")
            );
    }
}
