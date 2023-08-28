/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalConnective.hpp>

using namespace pybind11;

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::LogicalConnective;

// Trampoline class for virtual member functions
class PyLogicalConnective : public LogicalConnective
{
   public:
    using LogicalConnective::LogicalConnective;

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
            LogicalConnective,
            "is_satisfied",
            isSatisfied,
            currentStateVector,
            currentTime,
            previousStateVector,
            previousTime
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_LogicalConnective(pybind11::module& aModule)
{
    {
        class_<LogicalConnective, EventCondition, PyLogicalConnective, Shared<LogicalConnective>>(
            aModule, "LogicalConnective"
        )

            .def(init<const String&, const Array<Shared<EventCondition>>&>(), arg("name"), arg("event_conditions"))

            .def("get_event_conditions", &LogicalConnective::getEventConditions)

            ;
    }
}
