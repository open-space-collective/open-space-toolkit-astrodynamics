/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanEventCondition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::BooleanEventCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_BooleanEventCondition(pybind11::module& aModule)
{
    class_<BooleanEventCondition, EventCondition, Shared<BooleanEventCondition>>(aModule, "BooleanEventCondition")

        .def(
            init<
                const String&,
                const EventCondition::Criteria&,
                std::function<bool(const VectorXd&, const Real&)>,
                const bool&>(),
            arg("name"),
            arg("criteria"),
            arg("evaluator"),
            arg("is_inverse")
        )

        .def("__str__", &(shiftToString<BooleanEventCondition>))
        .def("__repr__", &(shiftToString<BooleanEventCondition>))

        .def("is_inversed", &BooleanEventCondition::isInversed)

        .def("evaluate", &BooleanEventCondition::evaluate, arg("state_vector"), arg("time"))

        .def(
            "is_satisfied",
            &BooleanEventCondition::isSatisfied,
            arg("current_state_vector"),
            arg("current_time"),
            arg("previous_state_vector"),
            arg("previous_time")
        )

        ;
}
