/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Pass(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Integer;

    using ostk::physics::time::Interval;

    using ostk::astro::trajectory::orbit::Pass;

    class_<Pass> pass_class(
        aModule,
        "Pass",
        R"doc(
            A revolution of an orbiting object.

            Group:
                Orbit
        )doc"
    );

    enum_<Pass::Type>(
        pass_class,
        "Type",
        R"doc(
            The type of `Pass`.
        )doc"
    )

        .value("Undefined", Pass::Type::Undefined, "Undefined")
        .value("Complete", Pass::Type::Complete, "Complete")
        .value("Partial", Pass::Type::Partial, "Partial")

        ;

    enum_<Pass::Phase>(
        pass_class,
        "Phase",
        R"doc(
            The phase of the `Pass`.
        )doc"
    )

        .value("Undefined", Pass::Phase::Undefined, "Undefined")
        .value("Ascending", Pass::Phase::Ascending, "Ascending")
        .value("Descending", Pass::Phase::Descending, "Descending")

        ;

    pass_class

        .def(
            init<const Integer&, const Instant&, const Instant&, const Instant&, const Instant&, const Instant&>(),
            arg("revolution_number"),
            arg("instant_at_ascending_node"),
            arg("instant_at_north_point"),
            arg("instant_at_descending_node"),
            arg("instant_at_south_point"),
            arg("instant_at_pass_break"),
            R"doc(
                Constructor.

                Args:
                    revolution_number (int): The revolution number of the pass.
                    instant_at_ascending_node (Instant): The instant at the ascending node of the pass.
                    instant_at_north_point (Instant): The instant at the north point of the pass.
                    instant_at_descending_node (Instant): The instant at the descending node of the pass.
                    instant_at_south_point (Instant): The instant at the south point of the pass.
                    instant_at_pass_break (Instant): The instant at which the pass breaks.

            )doc"
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Pass>))
        .def("__repr__", &(shiftToString<Pass>))

        .def(
            "is_defined",
            &Pass::isDefined,
            R"doc(
                Check if the pass is defined.

                Returns:
                    bool: True if the pass is defined, False otherwise.

            )doc"
        )
        .def(
            "is_complete",
            &Pass::isComplete,
            R"doc(
                Check if the pass is complete.

                Returns:
                    bool: True if the pass is complete, False otherwise.

            )doc"
        )

        .def(
            "get_type",
            &Pass::getType,
            R"doc(
                Get the type of the pass.

                Returns:
                    Pass.Type: The type of the pass.

            )doc"
        )
        .def(
            "get_revolution_number",
            &Pass::getRevolutionNumber,
            R"doc(
                Get the revolution number of the pass.

                Returns:
                    int: The revolution number of the pass.

            )doc"
        )
        .def(
            "get_duration",
            &Pass::getDuration,
            R"doc(
                Get the duration of the pass. Undefined if the pass is not complete.

                Returns:
                    Duration: The duration of the pass.

            )doc"
        )
        .def(
            "get_instant_at_ascending_node",
            &Pass::accessInstantAtAscendingNode,
            R"doc(
                Get the instant at the ascending node of the pass.

                Returns:
                    Instant: The instant at the ascending node of the pass.
            
            )doc"
        )
        .def(
            "get_instant_at_north_point",
            &Pass::accessInstantAtNorthPoint,
            R"doc(
                Get the instant at the north point of the pass.

                Returns:
                    Instant: The instant at the north point of the pass.

            )doc"
        )
        .def(
            "get_instant_at_descending_node",
            &Pass::accessInstantAtDescendingNode,
            R"doc(
                Get the instant at the descending node of the pass.

                Returns:
                    Instant: The instant at the descending node of the pass.

            )doc"
        )
        .def(
            "get_instant_at_south_point",
            &Pass::accessInstantAtSouthPoint,
            R"doc(
                Get the instant at the south point of the pass.

                Returns:
                    Instant: The instant at the south point of the pass.

            )doc"
        )
        .def(
            "get_instant_at_pass_break",
            &Pass::accessInstantAtSouthPoint,
            R"doc(
                Get the instant at the break of the pass.

                Returns:
                    Instant: The instant at the break of the pass.

            )doc"
        )

        .def_static(
            "undefined",
            &Pass::Undefined,
            R"doc(
                Get an undefined pass.

                Returns:
                    Pass: The undefined pass.
            )doc"
        )
        .def_static(
            "string_from_type",
            &Pass::StringFromType,
            R"doc(
                Get the string representation of a pass type.

                Args:
                    type (Pass.Type): The pass type.

                Returns:
                    str: The string representation of the pass type.
            )doc",
            arg("type")
        )
        .def_static(
            "string_from_phase",
            &Pass::StringFromPhase,
            R"doc(
                Get the string representation of a pass phase.

                Args:
                    phase (Pass.Phase): The pass phase.

                Returns:
                    str: The string representation of the pass phase.
            )doc",
            arg("phase")
        )

        ;
}
