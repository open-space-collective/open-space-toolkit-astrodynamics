/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BooleanCondition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::eventcondition::RealCondition;
using ostk::astro::eventcondition::BooleanCondition;
using ostk::astro::trajectory::State;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_BooleanCondition(pybind11::module& aModule)
{
    class_<BooleanCondition, RealCondition, Shared<BooleanCondition>>(aModule, "BooleanCondition")

        .def(
            init<const String&, const RealCondition::Criterion&, std::function<bool(const State&)>, const bool&>(),
            arg("name"),
            arg("criterion"),
            arg("evaluator"),
            arg("is_inverse")
        )

        .def("__str__", &(shiftToString<BooleanCondition>))
        .def("__repr__", &(shiftToString<BooleanCondition>))

        .def("is_inversed", &BooleanCondition::isInversed)

        .def("evaluate", &BooleanCondition::evaluate, arg("state"))

        .def("is_satisfied", &BooleanCondition::isSatisfied, arg("current_state"), arg("previous_state"))

        ;
}
