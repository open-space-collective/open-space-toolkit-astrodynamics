/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Real;

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

    Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override
    {
        PYBIND11_OVERRIDE_PURE(Real, EventCondition, evaluate, aStateVector, aTime);
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition(pybind11::module& aModule)
{
    {
        class_<EventCondition, Shared<EventCondition>, PyEventCondition> eventCondition_class(
            aModule, "EventCondition"
        );

        eventCondition_class

            .def(init<const String&, const EventCondition::Criteria&>(), arg("name"), arg("criteria"))

            .def("__str__", &(shiftToString<EventCondition>))
            .def("__repr__", &(shiftToString<EventCondition>))

            .def("is_satisfied", &EventCondition::isSatisfied, arg("current_value"), arg("previous_value"))

            .def("get_name", &EventCondition::getName)
            .def("get_criteria", &EventCondition::getCriteria)

            .def("evaluate", &EventCondition::evaluate, arg("state_vector"), arg("time"))

            .def_static("string_from_criteria", &EventCondition::StringFromCriteria, arg("criteria"));

        enum_<EventCondition::Criteria>(eventCondition_class, "Criteria")

            .value("PositiveCrossing", EventCondition::Criteria::PositiveCrossing)
            .value("NegativeCrossing", EventCondition::Criteria::NegativeCrossing)
            .value("AnyCrossing", EventCondition::Criteria::AnyCrossing)
            .value("PositiveOnly", EventCondition::Criteria::PositiveOnly)
            .value("NegativeOnly", EventCondition::Criteria::NegativeOnly)
            .value("Undefined", EventCondition::Criteria::Undefined)

            ;
    }
}
