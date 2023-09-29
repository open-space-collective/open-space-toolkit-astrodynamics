/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_AtmosphericDrag(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::env::obj::Celestial;

    using ostk::astro::Dynamics;
    using ostk::astro::dynamics::AtmosphericDrag;

    {
        class_<AtmosphericDrag, Dynamics, Shared<AtmosphericDrag>>(aModule, "AtmosphericDrag")
            .def(init<const Shared<Celestial>&>(), arg("celestial"))

            .def("__str__", &(shiftToString<AtmosphericDrag>))
            .def("__repr__", &(shiftToString<AtmosphericDrag>))

            .def("is_defined", &AtmosphericDrag::isDefined)

            .def("get_celestial", &AtmosphericDrag::getCelestial)

            .def("compute_contribution", &AtmosphericDrag::computeContribution, arg("instant"), arg("x"), arg("frame"));
    }
}
