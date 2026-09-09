/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>

using namespace nanobind;

using ostk::core::type::Shared;

using ostk::physics::time::Instant;

using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::eventcondition::RealCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_InstantCondition(nanobind::module_& aModule)
{
    {
        class_<InstantCondition, RealCondition>(
            aModule,
            "InstantCondition",
            R"doc(
                An Instant Event Condition.

            )doc"
        )

            .def(
                init<const RealCondition::Criterion&, const Instant&>(),
                R"doc(
                    Constructor.

                    Args:
                        criterion (Criterion): The criterion.
                        instant (Instant): The instant.

                )doc",
                arg("criterion"),
                arg("instant")
            )

            .def(
                "get_instant",
                &InstantCondition::getInstant,
                R"doc(
                    Get the instant.

                    Returns:
                        Instant: The instant.

                )doc"
            )

            ;
    }
}
