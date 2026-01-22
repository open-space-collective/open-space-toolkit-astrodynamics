/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>

using namespace pybind11;

using ostk::core::type::Shared;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::eventcondition::DurationCondition;
using ostk::astrodynamics::eventcondition::RealCondition;

inline void OpenSpaceToolkitAstrodynamicsPy_EventCondition_DurationCondition(pybind11::module& aModule)
{
    {
        class_<DurationCondition, RealCondition, Shared<DurationCondition>>(
            aModule,
            "DurationCondition",
            R"doc(
                A Duration Event Condition.

                This condition is satisfied when a specified duration has elapsed from a reference instant.
                The reference instant can be updated dynamically using the `update_target` method.

            )doc"
        )

            .def(
                init<const RealCondition::Criterion&, const Duration&, const Instant&>(),
                R"doc(
                    Constructor.

                    Args:
                        criterion (Criterion): The criterion.
                        duration (Duration): The duration threshold.
                        reference_instant (Instant): The reference instant from which the duration is measured. Defaults to J2000.

                )doc",
                arg("criterion"),
                arg("duration"),
                arg("reference_instant") = Instant::J2000()
            )

            .def(
                "get_duration",
                &DurationCondition::getDuration,
                R"doc(
                    Get the duration.

                    Returns:
                        Duration: The duration threshold.

                )doc"
            )

            .def(
                "get_reference_instant",
                &DurationCondition::getReferenceInstant,
                R"doc(
                    Get the reference instant.

                    Returns:
                        Instant: The reference instant from which the duration is measured.

                )doc"
            )

            ;
    }
}
