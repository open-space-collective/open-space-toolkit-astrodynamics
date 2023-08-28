/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::RealEventCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_RealEventCondition(pybind11::module& aModule)
{
    {
        class_<RealEventCondition, EventCondition, Shared<RealEventCondition>>(aModule, "RealEventCondition")

            .def(
                init<
                    const String&,
                    const EventCondition::Criteria&,
                    std::function<Real(const VectorXd&, const Real&)>,
                    const Real&>(),
                arg("name"),
                arg("criteria"),
                arg("evaluator"),
                arg("target") = 0.0
            )

            .def("__str__", &(shiftToString<RealEventCondition>))
            .def("__repr__", &(shiftToString<RealEventCondition>))

            .def("get_target", &RealEventCondition::getTarget)

            .def("evaluate", &RealEventCondition::evaluate, arg("state_vector"), arg("time"))

            .def(
                "is_satisfied",
                &RealEventCondition::isSatisfied,
                arg("current_state_vector"),
                arg("current_time"),
                arg("previous_state_vector"),
                arg("previous_time")
            )

            ;
    }
}
