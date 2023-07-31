/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalConnective.hpp>

using namespace pybind11;

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::LogicalConnective;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_LogicalConnective(pybind11::module& aModule)
{
    {
        class_<LogicalConnective, Shared<LogicalConnective>, EventCondition>(aModule, "LogicalConnective")

            .def(init<const String&, const Array<Shared<EventCondition>>&>(), arg("name"), arg("event_conditions"))

            .def("get_event_conditions", &LogicalConnective::getEventConditions)

            ;
    }
}
