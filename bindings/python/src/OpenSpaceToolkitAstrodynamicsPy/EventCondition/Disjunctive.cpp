/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Disjunctive.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>

using namespace pybind11;

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::Disjunctive;
using ostk::astro::eventcondition::LogicalCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_Disjunctive(pybind11::module& aModule)
{
    {
        class_<Disjunctive, LogicalCondition, Shared<Disjunctive>>(aModule, "Disjunctive")

            .def(init<const Array<Shared<EventCondition>>&>(), arg("event_conditions"))

            ;
    }
}
