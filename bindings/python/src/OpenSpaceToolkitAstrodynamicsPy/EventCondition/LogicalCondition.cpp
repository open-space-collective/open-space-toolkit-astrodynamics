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
            aModule,
            "LogicalCondition",
            R"doc(
                A Logical Event Condition. This class is used to combine multiple event conditions into a single set.

                Group:
                    event-condition
            )doc"
        );

        logicalCondition

            .def(
                init<const String&, const LogicalCondition::Type&, const Array<Shared<EventCondition>>&>(),
                R"doc(
                    Constructor.

                    Args:
                        name (str): The name of the condition.
                        type (Type): The type of the logical condition.
                        event_conditions (List[EventCondition]): The list of event conditions.

                    Group:
                        Constructors
                )doc",
                arg("name"),
                arg("type"),
                arg("event_conditions")
            )

            .def(
                "get_type",
                &LogicalCondition::getType,
                R"doc(
                    Get the type of the logical condition.

                    Returns:
                        Type: The type of the logical condition.

                )doc"
            )

            .def(
                "get_event_conditions",
                &LogicalCondition::getEventConditions,
                R"doc(
                    Get the list of event conditions.

                    Returns:
                        List[EventCondition]: The list of event conditions.

                )doc"
            )

            ;

        enum_<LogicalCondition::Type>(
            logicalCondition,
            "Type",
            R"doc(
                Logical Condition Type.
                    - Disjunctive (Or)
                    - Conjucntive (And)
            )doc"
        )

            .value("And", LogicalCondition::Type::And, "And")
            .value("Or", LogicalCondition::Type::Or, "Or")

            ;
    }
}
