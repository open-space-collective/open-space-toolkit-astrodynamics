/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/AngularCondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/BooleanCondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/COECondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/InstantCondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/LogicalCondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/RealCondition.cpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::units::Length;
using ostk::physics::units::Angle;

using ostk::astro::EventCondition;
using ostk::astro::trajectory::State;

// Trampoline class for virtual member functions
class PyEventCondition : public EventCondition
{
   public:
    using EventCondition::EventCondition;

    // Trampoline (need one for each virtual function)

    bool isSatisfied(const State& currentState, const State& previousState) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(bool, EventCondition, "is_satisfied", isSatisfied, currentState, previousState);
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition(pybind11::module& aModule)
{
    {
        class_<EventCondition, PyEventCondition, Shared<EventCondition>> eventCondition(
            aModule,
            "EventCondition",
            R"doc(
                An Event Condition defines a criterion that can be evaluated based on a current/previous state vectors and times

                Group:
                    event-condition
            )doc"
        );

        class_<EventCondition::Target> eventConditionTarget(
            eventCondition,
            "EventConditionTarget",
            R"doc(
                The Event Condition Target.

                Group:
                    event-condition
            )doc"
        );

        enum_<EventCondition::Target::Type>(
            eventConditionTarget,
            "Type",
            R"doc(
                Event Condition Target type.
            )doc"
        )

            .value("Absolute", EventCondition::Target::Type::Absolute, "Absolute")
            .value("Relative", EventCondition::Target::Type::Relative, "Relative to the provided State.")

            ;

        eventConditionTarget

            .def(
                init<const Real&, const EventCondition::Target::Type&>(),
                R"doc(
                    Construct a new `EventConditionTarget` object.

                    Args:
                        value (float): The value of the target.
                        type (EventConditionTarget.Type): The type of the target. Defaults to EventConditionTarget.Type.Absolute.

                    Returns:
                        event_condition_target (EventConditionTarget): The new `EventConditionTarget` object.
                )doc",
                arg("value"),
                arg("type") = EventCondition::Target::Type::Absolute
            )

            .def(
                init<const Length&, const EventCondition::Target::Type&>(),
                R"doc(
                    Construct a new `EventConditionTarget` object.

                    Args:
                        length (Length): The value of the target as a `Length`.
                        type (EventConditionTarget.Type): The type of the target. Defaults to EventConditionTarget.Type.Absolute.

                    Returns:
                        event_condition_target (EventConditionTarget): The new `EventConditionTarget` object.
                )doc",
                arg("value"),
                arg("type") = EventCondition::Target::Type::Absolute
            )

            .def(
                init<const Angle&, const EventCondition::Target::Type&>(),
                R"doc(
                    Construct a new `EventConditionTarget` object.

                    Args:
                        angle (Angle): The value of the target as an `Angle`.
                        type (EventConditionTarget.Type): The type of the target. Defaults to EventConditionTarget.Type.Absolute.

                    Returns:
                        event_condition_target (EventConditionTarget): The new `EventConditionTarget` object.
                )doc",
                arg("value"),
                arg("type") = EventCondition::Target::Type::Absolute
            )

            .def_static(
                "StringFromType",
                &EventCondition::Target::StringFromType,
                R"doc(
                    Enum as a string

                    Args:
                        type (EventConditionTarget.Type): The type of the target.

                    Returns:
                        string (str): Name of the enum as a string.
                )doc"
            )

            .def_readonly(
                "value",
                &EventCondition::Target::value,
                R"doc(
                    The value of the target.

                    :type: Real
                )doc"
            )
            .def_readonly(
                "type",
                &EventCondition::Target::type,
                R"doc(
                    The type of the target.

                    :type: Type
                )doc"
            )

            .def("__eq__", &EventCondition::Target::operator==)
            .def("__ne__", &EventCondition::Target::operator!=)

            ;

        eventCondition

            .def(
                init<const String&, const std::function<Real(const State&)>&, const EventCondition::Target&>(),
                R"doc(
                    Construct a new `EventCondition` object.

                    Args:
                        name (str): The name of the event condition.
                        evaluator (callable): The evaluator that accepts a `State` and returns a float value.
                        target (EventConditionTarget): The target of the event condition.

                    Returns:
                        event_condition (EventCondition): The new `EventCondition` object.
                )doc",
                arg("name"),
                arg("evaluator"),
                arg("target")
            )

            .def(
                init<const String&, const std::function<Real(const State&)>&, const Real&>(),
                R"doc(
                    Construct a new `EventCondition` object.

                    Args:
                        name (str): The name of the event condition.
                        evaluator (callable): The evaluator that accepts a `State` and returns a float value.
                        target_value (float): The target of the event condition.

                    Returns:
                        event_condition (EventCondition): The new `EventCondition` object.
                )doc",
                arg("name"),
                arg("evaluator"),
                arg("target_value")
            )

            .def("__str__", &(shiftToString<EventCondition>))
            .def("__repr__", &(shiftToString<EventCondition>))

            .def(
                "get_name",
                &EventCondition::getName,
                R"doc(
                    Get the name of the event condition.

                    Returns:
                       name (str): The name of the event condition.
                )doc"
            )

            .def(
                "get_evaluator",
                &EventCondition::getEvaluator,
                R"doc(
                    Get the evaluator of the event condition.

                    Returns:
                       evaluator (str): The evaluator of the event condition.
                )doc"
            )

            .def(
                "get_target",
                &EventCondition::getTarget,
                R"doc(
                    Get the target of the event condition.

                    Returns:
                       target (EventConditionTarget): The target of the event condition.
                )doc"
            )

            .def(
                "is_satisfied",
                &EventCondition::isSatisfied,
                arg("current_state"),
                arg("previous_state"),
                R"doc(
                    Check if the event condition is satisfied.

                    Args:
                        current_state (State): The current state.
                        previous_state (State): The previous state.

                    Returns:
                       is_satisfied (bool): True if the event condition is satisfied, False otherwise.
                )doc"
            )

            ;
    }

    // Create "event_condition" python submodule
    auto event_condition = aModule.def_submodule("event_condition");

    // Add __path__ attribute for "event_condition" submodule
    event_condition.attr("__path__") = "ostk.astrodynamics.event_condition";

    OpenSpaceToolkitAstrodynamicsPy_EventCondition_RealCondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_BooleanCondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_AngularCondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_InstantCondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_COECondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_LogicalCondition(event_condition);
}
