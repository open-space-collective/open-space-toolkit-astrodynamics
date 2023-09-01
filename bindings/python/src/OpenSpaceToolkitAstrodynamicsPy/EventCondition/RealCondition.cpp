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

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_RealCondition(pybind11::module& aModule)
{
    {
        class_<RealCondition, EventCondition, Shared<RealCondition>> realCondition(aModule, "RealCondition");

        realCondition

            .def(
                init<
                    const String&,
                    const RealCondition::Criteria&,
                    std::function<Real(const VectorXd&, const Real&)>,
                    const Real&>(),
                arg("name"),
                arg("criteria"),
                arg("evaluator"),
                arg("target") = 0.0
            )

            .def("__str__", &(shiftToString<RealCondition>))
            .def("__repr__", &(shiftToString<RealCondition>))

            .def("get_target", &RealCondition::getTarget)
            .def("get_criteria", &RealCondition::getCriteria)
            .def("get_evaluator", &RealCondition::getEvaluator)

            .def("evaluate", &RealCondition::evaluate, arg("state_vector"), arg("time"))

            .def(
                "is_satisfied",
                &RealCondition::isSatisfied,
                arg("current_state_vector"),
                arg("current_time"),
                arg("previous_state_vector"),
                arg("previous_time")
            )

            .def_static("string_from_criteria", &RealCondition::StringFromCriteria, arg("criteria"))

            ;

        enum_<RealCondition::Criteria>(realCondition, "Criteria")

            .value("PositiveCrossing", RealCondition::Criteria::PositiveCrossing)
            .value("NegativeCrossing", RealCondition::Criteria::NegativeCrossing)
            .value("AnyCrossing", RealCondition::Criteria::AnyCrossing)
            .value("StrictlyPositive", RealCondition::Criteria::StrictlyPositive)
            .value("StrictlyNegative", RealCondition::Criteria::StrictlyNegative)

            ;
    }
}
