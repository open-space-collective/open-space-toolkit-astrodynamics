/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>

using namespace pybind11;

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::LogicalCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_LogicalCondition(pybind11::module& aModule)
{
    {
        class_<LogicalCondition, EventCondition, Shared<LogicalCondition>> logicalCondition(
            aModule, "LogicalCondition"
        );

        logicalCondition

            .def(
                init<const String&, const LogicalCondition::Type&, const Array<Shared<EventCondition>>&>(),
                arg("name"),
                arg("type"),
                arg("event_conditions")
            )

            .def("get_type", &LogicalCondition::getType)
            .def("get_event_conditions", &LogicalCondition::getEventConditions)

            ;

        enum_<LogicalCondition::Type>(logicalCondition, "Type")

            .value("And", LogicalCondition::Type::And)
            .value("Or", LogicalCondition::Type::Or)

            .export_values()

            ;
    }
}
