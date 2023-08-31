/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::physics::time::Duration;

using ostk::astro::eventcondition::RealCondition;
using ostk::astro::eventcondition::InstantCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_InstantCondition(pybind11::module& aModule)
{
    {
        class_<InstantCondition, RealCondition, Shared<InstantCondition>>(aModule, "InstantCondition")

            .def(init<const RealCondition::Criterion&, const Duration&>(), arg("criterion"), arg("duration"))

            .def("get_duration", &InstantCondition::getDuration)

            ;
    }
}
