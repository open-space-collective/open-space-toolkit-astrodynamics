/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Conjunctive.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>

using namespace pybind11;

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::Conjunctive;
using ostk::astro::eventcondition::LogicalCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_Conjunctive(pybind11::module& aModule)
{
    {
        class_<Conjunctive, LogicalCondition, Shared<Conjunctive>>(aModule, "Conjunctive")

            .def(init<const Array<Shared<EventCondition>>&>(), arg("event_conditions"))

            ;
    }
}
