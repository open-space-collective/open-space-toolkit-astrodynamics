/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::object::VectorXd;

using ostk::physics::units::Angle;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::AngularCondition;
using ostk::astro::trajectory::State;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_AngularCondition(pybind11::module& aModule)
{
    {
        class_<AngularCondition, EventCondition, Shared<AngularCondition>> angularCondition(
            aModule,
            "AngularCondition",
            R"doc(
                An Angular Event Condition.

                Group:
                    event-condition
            )doc"
        );

        enum_<AngularCondition::Criterion>(
            angularCondition,
            "Criterion",
            R"doc(
                Angular condition criterion.

            )doc"
        )

            .value("PositiveCrossing", AngularCondition::Criterion::PositiveCrossing)
            .value("NegativeCrossing", AngularCondition::Criterion::NegativeCrossing)
            .value("AnyCrossing", AngularCondition::Criterion::AnyCrossing)
            .value("WithinRange", AngularCondition::Criterion::WithinRange)

            ;

        angularCondition

            .def(
                init<
                    const String&,
                    const AngularCondition::Criterion&,
                    std::function<Real(const State&)>,
                    const Angle&>(),
                R"doc(
                    Constructor.

                    Args:
                        name (str): The name of the condition.
                        criterion (ostk.astrodynamics.event_condition.AngularCondition.Criterion): The criterion of the condition.
                        evaluator (function): The evaluator of the condition.
                        target_angle (Angle): The target angle of the condition.

                )doc",
                arg("name"),
                arg("criterion"),
                arg("evaluator"),
                arg("target_angle")
            )

            .def(
                init<
                    const String&,
                    const AngularCondition::Criterion&,
                    std::function<Real(const State&)>,
                    const EventCondition::Target&>(),
                R"doc(
                    Constructor.

                    Args:
                        name (str): The name of the condition.
                        criterion (ostk.astrodynamics.event_condition.AngularCondition.Criterion): The criterion of the condition.
                        evaluator (function): The evaluator of the condition.
                        target (EventConditionTarget): The target of the condition.

                )doc",
                arg("name"),
                arg("criterion"),
                arg("evaluator"),
                arg("target")
            )

            .def("__str__", &(shiftToString<AngularCondition>))
            .def("__repr__", &(shiftToString<AngularCondition>))

            .def(
                "get_criterion",
                &AngularCondition::getCriterion,
                R"doc(
                    Get the criterion of the condition.

                    Returns:
                        ostk.astrodynamics.event_condition.AngularCondition.Criterion: The criterion of the condition.

                )doc"
            )

            .def(
                "get_target_angle",
                &AngularCondition::getTargetAngle,
                R"doc(
                    Get the target angle of the condition.

                    Returns:
                        Angle: The target angle of the condition.

                )doc"
            )

            .def(
                "get_target_range",
                &AngularCondition::getTargetRange,
                R"doc(
                    Get the target range of the condition.

                    Returns:
                        tuple: The target range of the condition.

                )doc"
            )

            .def(
                "is_satisfied",
                &AngularCondition::isSatisfied,
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
                "within_range",
                &AngularCondition::WithinRange,
                R"doc(
                    Create an angular condition that is satisfied when the angle is within a range.

                    Args:
                        name (str): The name of the condition.
                        evaluator (function): The evaluator of the condition.
                        target_range (tuple): The target range of the condition.

                    Returns:
                        AngularCondition: The angular condition.

                )doc",
                arg("name"),
                arg("evaluator"),
                arg("target_range")
            )

            .def_static(
                "string_from_criterion",
                &AngularCondition::StringFromCriterion,
                R"doc(
                    Get the string representation of a criterion.

                    Args:
                        criterion (ostk.astrodynamics.event_condition.AngularCondition.Criterion): The criterion.

                    Returns:
                        str: The string representation of the criterion.

                )doc",
                arg("criterion")
            )

            ;
    }
}
