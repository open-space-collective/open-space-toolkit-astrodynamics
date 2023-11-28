/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::object::VectorXd;

using ostk::astro::eventcondition::RealCondition;
using ostk::astro::eventcondition::BooleanCondition;
using ostk::astro::trajectory::State;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_BooleanCondition(pybind11::module& aModule)
{
    class_<BooleanCondition, RealCondition, Shared<BooleanCondition>>(
        aModule,
        "BooleanCondition",
        R"doc(
            A Boolean Event Condition.

            Group:
                event-condition
        )doc"
    )

        .def(
            init<const String&, const RealCondition::Criterion&, std::function<bool(const State&)>, const bool&>(),
            R"doc(
                Constructor.

                Args:
                    name (str): The name of the condition.
                    criterion (Criterion): The criterion of the condition.
                    evaluator (function): The evaluator of the condition.
                    is_inverse (bool): Whether the condition is inverse.

                Group:
                    Constructors
            )doc",
            arg("name"),
            arg("criterion"),
            arg("evaluator"),
            arg("is_inverse")
        )

        .def("__str__", &(shiftToString<BooleanCondition>))
        .def("__repr__", &(shiftToString<BooleanCondition>))

        .def(
            "is_inversed",
            &BooleanCondition::isInversed,
            R"doc(
                Check if the condition is inverse.

                Returns:
                    bool: True if the condition is inverse, False otherwise.

            )doc"
        )

        .def(
            "evaluate",
            &BooleanCondition::evaluate,
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
            &BooleanCondition::isSatisfied,
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

        ;
}
