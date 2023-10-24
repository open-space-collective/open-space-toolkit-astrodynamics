/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::physics::time::Instant;

using ostk::astro::eventcondition::RealCondition;
using ostk::astro::eventcondition::InstantCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_InstantCondition(pybind11::module& aModule)
{
    {
        class_<InstantCondition, RealCondition, Shared<InstantCondition>>(
            aModule,
            "InstantCondition",
            R"doc(
                An Instant Event Condition.

                Group:
                    event-condition
            )doc"
        )

            .def(
                init<const RealCondition::Criterion&, const Instant&>(),
                R"doc(
                    Constructor.

                    Args:
                        criterion (Criterion): The criterion.
                        instant (Instant): The instant.

                    Group:
                        Constructors
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

                    Group:
                        Methods
                )doc"
            )

            ;
    }
}
