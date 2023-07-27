/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::physics::units::Length;
using ostk::physics::units::Derived;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::COECondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_COECondition(pybind11::module& aModule)
{
    {
        class_<COECondition, Shared<COECondition>, EventCondition>(aModule, "COECondition")

            .def(
                init<const EventCondition::Criteria&, const Length&, const Derived&>(),
                arg("criteria"),
                arg("semi_major_axis"),
                arg("gravitational_parameter")
            )

            .def("get_semi_major_axis", &COECondition::getSemiMajorAxis)

            .def("get_gravitational_parameter", &COECondition::getGravitationalParameter)

            ;
    }
}
