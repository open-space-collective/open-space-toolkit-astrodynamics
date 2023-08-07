/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/COECondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/Conjunctive.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/Disjunctive.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/DurationCondition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/EventCondition/LogicalConnective.cpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;

// Trampoline class for virtual member functions
class PyEventCondition : public EventCondition
{
   public:
    using EventCondition::EventCondition;

    // Trampoline (need one for each virtual function)

    bool isSatisfied(const Real& currentValue, const Real& previousValue) const override
    {
        PYBIND11_OVERRIDE(bool, EventCondition, isSatisfied, currentValue, previousValue);
    }

    bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const override
    {
        PYBIND11_OVERRIDE(
            bool, EventCondition, isSatisfied, currentStateVector, currentTime, previousStateVector, previousTime
        );
    }

    Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override
    {
        PYBIND11_OVERRIDE_PURE(Real, EventCondition, evaluate, aStateVector, aTime);
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition(pybind11::module& aModule)
{
    {
        class_<EventCondition, PyEventCondition, Shared<EventCondition>> eventCondition_class(
            aModule, "EventCondition"
        );

        eventCondition_class

            .def(init<const String&, const EventCondition::Criteria&>(), arg("name"), arg("criteria"))

            .def("__str__", &(shiftToString<EventCondition>))
            .def("__repr__", &(shiftToString<EventCondition>))

            .def(
                "is_satisfied",
                overload_cast<const Real&, const Real&>(&EventCondition::isSatisfied, const_),
                arg("current_value"),
                arg("previous_value")
            )
            .def(
                "is_satisfied",
                overload_cast<const VectorXd&, const Real&, const VectorXd&, const Real&>(
                    &EventCondition::isSatisfied, const_
                ),
                arg("current_state_vector"),
                arg("current_time"),
                arg("previous_state_vector"),
                arg("previous_time")
            )

            .def("get_name", &EventCondition::getName)
            .def("get_criteria", &EventCondition::getCriteria)

            .def("evaluate", &EventCondition::evaluate, arg("state_vector"), arg("time"))

            .def_static("string_from_criteria", &EventCondition::StringFromCriteria, arg("criteria"))

            ;

        enum_<EventCondition::Criteria>(eventCondition_class, "Criteria")

            .value("PositiveCrossing", EventCondition::Criteria::PositiveCrossing)
            .value("NegativeCrossing", EventCondition::Criteria::NegativeCrossing)
            .value("AnyCrossing", EventCondition::Criteria::AnyCrossing)
            .value("StrictlyPositive", EventCondition::Criteria::StrictlyPositive)
            .value("StrictlyNegative", EventCondition::Criteria::StrictlyNegative)
            .value("Undefined", EventCondition::Criteria::Undefined)

            ;
    }

    // Create "event_condition" python submodule
    auto event_condition = aModule.def_submodule("event_condition");

    // Add __path__ attribute for "event_condition" submodule
    event_condition.attr("__path__") = "ostk.astrodynamics.event_condition";

    OpenSpaceToolkitAstrodynamicsPy_EventCondition_DurationCondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_COECondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_LogicalConnective(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_Conjunctive(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_Disjunctive(event_condition);
}
