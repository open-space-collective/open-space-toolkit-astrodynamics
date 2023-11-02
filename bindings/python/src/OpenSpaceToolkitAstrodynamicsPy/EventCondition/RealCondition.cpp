/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::RealCondition;
using ostk::astro::trajectory::State;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_RealCondition(pybind11::module& aModule)
{
    {
        class_<RealCondition, EventCondition, Shared<RealCondition>> realCondition(
            aModule,
            "RealCondition",
            R"doc(
                A Real Event Condition.

                Group:
                    event-condition
            )doc"
        );

        enum_<RealCondition::Criterion>(
            realCondition,
            "Criterion",
            R"doc(
                The Criterion that defines how the condition is satisfied.
            )doc"
        )

            .value("PositiveCrossing", RealCondition::Criterion::PositiveCrossing, "The positive crossing criterion")
            .value("NegativeCrossing", RealCondition::Criterion::NegativeCrossing, "The negative crossing criterion")
            .value("AnyCrossing", RealCondition::Criterion::AnyCrossing, "The any crossing criterion")
            .value("StrictlyPositive", RealCondition::Criterion::StrictlyPositive, "The strictly positive criterion")
            .value("StrictlyNegative", RealCondition::Criterion::StrictlyNegative, "The strictly negative criterion")

            ;

        realCondition

            .def(
                init<
                    const String&,
                    const RealCondition::Criterion&,
                    const std::function<Real(const State&)>&,
                    const Real&>(),
                R"doc(
                    Constructor.

                    Args:
                        name (str): The name of the condition.
                        criterion (Criterion): The criterion of the condition.
                        evaluator (function): The evaluator of the condition.
                        target_value (float): The target value of the condition.

                )doc",
                arg("name"),
                arg("criterion"),
                arg("evaluator"),
                arg("target_value") = 0.0
            )

            .def(
                init<
                    const String&,
                    const RealCondition::Criterion&,
                    const std::function<Real(const State&)>&,
                    const EventCondition::Target&>(),
                R"doc(
                    Constructor.

                    Args:
                        name (str): The name of the condition.
                        criterion (Criterion): The criterion of the condition.
                        evaluator (function): The evaluator of the condition.
                        target (EventConditionTarget): The target of the condition.

                )doc",
                arg("name"),
                arg("criterion"),
                arg("evaluator"),
                arg("target")
            )

            .def(
                "__str__",
                &(shiftToString<RealCondition>),
                R"doc(
                    Return a string representation of the real condition.

                    Returns:
                        str: The string representation.

                )doc"
            )

            .def(
                "__repr__",
                &(shiftToString<RealCondition>),
                R"doc(
                    Return a string representation of the real condition.

                    Returns:
                        str: The string representation.

                )doc"
            )

            .def(
                "get_criterion",
                &RealCondition::getCriterion,
                R"doc(
                    Get the criterion of the condition.

                    Returns:
                        Criterion: The criterion.

                )doc"
            )

            .def(
                "evaluate",
                &RealCondition::evaluate,
                R"doc(
                    Evaluate the condition.

                    Args:
                        state (State): The state.

                    Returns:
                        bool: True if the condition is satisfied, False otherwise.

                )doc",
                arg("state")
            )

            .def(
                "is_satisfied",
                &RealCondition::isSatisfied,
                R"doc(
                    Check if the condition is satisfied.

                    Args:
                        current_state (State): The current state.
                        previous_state (State): The previous state.

                    Returns:
                        bool: True if the condition is satisfied, False otherwise.

                )doc",
                arg("current_state"),
                arg("previous_state")
            )

            .def_static(
                "string_from_criterion",
                &RealCondition::StringFromCriterion,
                R"doc(
                    Get the string representation of a criterion.

                    Args:
                        criterion (Criterion): The criterion.

                    Returns:
                        str: The string representation.
                )doc",
                arg("criterion")
            )

            .def_static(
                "duration_condition",
                &RealCondition::DurationCondition,
                R"doc(
                    Generate a duration condition.

                    Args:
                        criterion (Criterion): The criterion of the condition.
                        duration (Duration): Duration target.

                    Returns:
                        RealCondition: The duration condition.
                )doc",
                arg("criterion"),
                arg("duration")
            )

            ;
    }
}
