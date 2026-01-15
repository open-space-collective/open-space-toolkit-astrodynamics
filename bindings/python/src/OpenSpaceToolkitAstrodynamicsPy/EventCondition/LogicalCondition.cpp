/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>

using namespace pybind11;

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::LogicalCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_LogicalCondition(pybind11::module& aModule)
{
    {
        class_<LogicalCondition, EventCondition, Shared<LogicalCondition>> logicalCondition(
            aModule,
            "LogicalCondition",
            R"doc(
                A Logical Event Condition. This class is used to combine multiple event conditions into a single set.

            )doc"
        );

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

        logicalCondition

            .def(
                init<const String&, const LogicalCondition::Type&, const Array<Shared<EventCondition>>&>(),
                R"doc(
                    Constructor.

                    Args:
                        name (str): The name of the condition.
                        type (Type): The type of the logical condition.
                        event_conditions (list[EventCondition]): The list of event conditions.

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
                        list[EventCondition]: The list of event conditions.

                )doc"
            )

            .def(
                "evaluate",
                &LogicalCondition::evaluate,
                arg("state"),
                R"doc(
                    Evaluate the logical condition.

                    For OR conditions, returns the maximum evaluation value across all sub-conditions.
                    For AND conditions, returns the minimum evaluation value across all sub-conditions.

                    Args:
                        state (State): The state at which to evaluate the logical condition.

                    Returns:
                        Real: The value of the logical condition at the given state.
                )doc"
            )

            ;
    }
}
