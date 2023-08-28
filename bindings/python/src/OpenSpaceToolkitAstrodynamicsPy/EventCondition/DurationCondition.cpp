/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::physics::time::Duration;

using ostk::astro::eventcondition::RealEventCondition;
using ostk::astro::eventcondition::DurationCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_DurationCondition(pybind11::module& aModule)
{
    {
        class_<DurationCondition, RealEventCondition, Shared<DurationCondition>>(aModule, "DurationCondition")

            .def(init<const RealEventCondition::Criteria&, const Duration&>(), arg("criteria"), arg("duration"))

            .def("get_duration", &DurationCondition::getDuration)

            ;
    }
}
