/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::RealCondition;
using ostk::astro::trajectory::State;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_RealCondition(pybind11::module& aModule)
{
    {
        class_<RealCondition, EventCondition, Shared<RealCondition>> realCondition(aModule, "RealCondition");

        realCondition

            .def(
                init<const String&, const RealCondition::Criterion&, std::function<Real(const State&)>, const Real&>(),
                arg("name"),
                arg("criterion"),
                arg("evaluator"),
                arg("target") = 0.0
            )

            .def("__str__", &(shiftToString<RealCondition>))
            .def("__repr__", &(shiftToString<RealCondition>))

            .def("get_target", &RealCondition::getTarget)
            .def("get_criterion", &RealCondition::getCriterion)
            .def("get_evaluator", &RealCondition::getEvaluator)

            .def("evaluate", &RealCondition::evaluate, arg("state"))

            .def("is_satisfied", &RealCondition::isSatisfied, arg("current_state"), arg("previous_state"))

            .def_static("string_from_criterion", &RealCondition::StringFromCriterion, arg("criterion"))

            ;

        enum_<RealCondition::Criterion>(realCondition, "Criterion")

            .value("PositiveCrossing", RealCondition::Criterion::PositiveCrossing)
            .value("NegativeCrossing", RealCondition::Criterion::NegativeCrossing)
            .value("AnyCrossing", RealCondition::Criterion::AnyCrossing)
            .value("StrictlyPositive", RealCondition::Criterion::StrictlyPositive)
            .value("StrictlyNegative", RealCondition::Criterion::StrictlyNegative)

            ;
    }
}
