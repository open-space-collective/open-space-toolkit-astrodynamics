/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>

using namespace pybind11;

using ostk::core::types::String;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::units::Angle;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::AngularCondition;
using ostk::astro::trajectory::State;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_AngularCondition(pybind11::module& aModule)
{
    {
        class_<AngularCondition, EventCondition, Shared<AngularCondition>> angularCondition(
            aModule, "AngularCondition"
        );

        angularCondition

            .def(
                init<
                    const String&,
                    const AngularCondition::Criterion&,
                    std::function<Real(const State&)>,
                    const Angle&>(),
                arg("name"),
                arg("criterion"),
                arg("evaluator"),
                arg("target_angle")
            )

            .def("__str__", &(shiftToString<AngularCondition>))
            .def("__repr__", &(shiftToString<AngularCondition>))

            .def("get_criterion", &AngularCondition::getCriterion)
            .def("get_evaluator", &AngularCondition::getEvaluator)
            .def("get_target_angle", &AngularCondition::getTargetAngle)
            .def("get_target_range", &AngularCondition::getTargetRange)

            .def("is_satisfied", &AngularCondition::isSatisfied, arg("current_state"), arg("previous_state"))

            .def_static(
                "within_range", &AngularCondition::WithinRange, arg("name"), arg("evaluator"), arg("target_range")
            )
            .def_static("string_from_criterion", &AngularCondition::StringFromCriterion, arg("criterion"))

            ;

        enum_<AngularCondition::Criterion>(angularCondition, "Criterion")

            .value("PositiveCrossing", AngularCondition::Criterion::PositiveCrossing)
            .value("NegativeCrossing", AngularCondition::Criterion::NegativeCrossing)
            .value("AnyCrossing", AngularCondition::Criterion::AnyCrossing)
            .value("WithinRange", AngularCondition::Criterion::WithinRange)

            ;
    }
}
