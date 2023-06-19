/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_CentralBodyGravity(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::flight::system::Dynamics;
    using ostk::astro::flight::system::dynamics::CentralBodyGravity;

    {
        class_<CentralBodyGravity, Shared<CentralBodyGravity>, Dynamics>(aModule, "CentralBodyGravity")
            .def(init<const Shared<Celestial>&>(), arg("celestial"))

            .def("__str__", &(shiftToString<CentralBodyGravity>))
            .def("__repr__", &(shiftToString<CentralBodyGravity>))

            .def("is_defined", &CentralBodyGravity::isDefined)

            .def("get_celestial", &CentralBodyGravity::getCelestial)

            .def("update", &CentralBodyGravity::update, arg("x"), arg("dxdt"), arg("instant"))

            ;
    }
}
