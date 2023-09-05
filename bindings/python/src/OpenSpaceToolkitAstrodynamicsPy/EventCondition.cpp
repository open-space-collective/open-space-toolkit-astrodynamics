/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

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
        class_<EventCondition, PyEventCondition, Shared<EventCondition>> eventCondition_class(
            aModule, "EventCondition"
        );

        eventCondition_class

            .def(init<const String&>(), arg("name"))

            .def("__str__", &(shiftToString<EventCondition>))
            .def("__repr__", &(shiftToString<EventCondition>))

            .def("get_name", &EventCondition::getName)

            .def("is_satisfied", &EventCondition::isSatisfied, arg("current_state"), arg("previous_state"))

            ;
    }

    // Create "event_condition" python submodule
    auto event_condition = aModule.def_submodule("event_condition");

    // Add __path__ attribute for "event_condition" submodule
    event_condition.attr("__path__") = "ostk.astrodynamics.event_condition";

    OpenSpaceToolkitAstrodynamicsPy_EventCondition_RealCondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_BooleanCondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_InstantCondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_COECondition(event_condition);
    OpenSpaceToolkitAstrodynamicsPy_EventCondition_LogicalCondition(event_condition);
}
