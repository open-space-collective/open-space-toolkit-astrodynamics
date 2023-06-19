/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThirdBodyGravity.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_ThirdBodyGravity(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::ThirdBodyGravity;

    {
        class_<ThirdBodyGravity, Shared<ThirdBodyGravity>, Dynamics>(aModule, "ThirdBodyGravity")
            .def(init<const Shared<Celestial>&>(), arg("celestial"))

            .def("__str__", &(shiftToString<ThirdBodyGravity>))
            .def("__repr__", &(shiftToString<ThirdBodyGravity>))

            .def("is_defined", &ThirdBodyGravity::isDefined)

            .def("get_celestial", &ThirdBodyGravity::getCelestial)

            .def("update", &ThirdBodyGravity::update, arg("x"), arg("dxdt"), arg("instant"))

            ;
    }
}
