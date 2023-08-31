/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::LogicalCondition;

// Trampoline class for virtual member functions
class PyLogicalCondition : public LogicalCondition
{
   public:
    using LogicalCondition::LogicalCondition;

    // Trampoline (need one for each virtual function)

    bool isSatisfied(
        const VectorXd& currentStateVector,
        const Real& currentTime,
        const VectorXd& previousStateVector,
        const Real& previousTime
    ) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            bool,
            LogicalCondition,
            "is_satisfied",
            isSatisfied,
            currentStateVector,
            currentTime,
            previousStateVector,
            previousTime
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_LogicalCondition(pybind11::module& aModule)
{
    {
        class_<LogicalCondition, EventCondition, PyLogicalCondition, Shared<LogicalCondition>>(
            aModule, "LogicalCondition"
        )

            .def(init<const String&, const Array<Shared<EventCondition>>&>(), arg("name"), arg("event_conditions"))

            .def("get_event_conditions", &LogicalCondition::getEventConditions)

            ;
    }
}
