/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/SemiMajorAxisCondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::physics::units::Length;
using ostk::physics::units::Derived;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::SemiMajorAxisCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_SemiMajorAxisCondition(pybind11::module& aModule)
{
    {
        class_<SemiMajorAxisCondition, Shared<SemiMajorAxisCondition>, EventCondition>(
            aModule, "SemiMajorAxisCondition"
        )

            .def(
                init<const EventCondition::Criteria&, const Length&, const Derived&>(),
                arg("criteria"),
                arg("semi_major_axis"),
                arg("gravitational_parameter")
            )

            .def("get_semi_major_axis", &SemiMajorAxisCondition::getSemiMajorAxis)

            .def("get_gravitational_parameter", &SemiMajorAxisCondition::getGravitationalParameter)

            ;
    }
}
