/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Conjunctive.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalConnective.hpp>

using namespace pybind11;

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::Conjunctive;
using ostk::astro::eventcondition::LogicalConnective;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_Conjunctive(pybind11::module& aModule)
{
    {
        class_<Conjunctive, Shared<Conjunctive>, LogicalConnective>(aModule, "Conjunctive")

            .def(init<const Array<Shared<EventCondition>>&>(), arg("event_conditions"))

            .def("is_satisfied", &Conjunctive::isSatisfied)

            ;
    }
}
