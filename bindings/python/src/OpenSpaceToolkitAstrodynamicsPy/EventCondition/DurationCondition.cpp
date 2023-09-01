/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::physics::time::Duration;

using ostk::astro::eventcondition::RealCondition;
using ostk::astro::eventcondition::DurationCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_DurationCondition(pybind11::module& aModule)
{
    {
        class_<DurationCondition, RealCondition, Shared<DurationCondition>>(aModule, "DurationCondition")

            .def(init<const RealCondition::Criterion&, const Duration&>(), arg("criterion"), arg("duration"))

            .def("get_duration", &DurationCondition::getDuration)

            ;
    }
}
