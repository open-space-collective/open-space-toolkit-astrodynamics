/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Pass(nanobind::module_& aModule)
{
    using namespace nanobind;

    using ostk::physics::time::Interval;

    using ostk::astrodynamics::trajectory::orbit::Pass;

    class_<Pass> pass_class(
        aModule,
        "Pass",
        R"doc(
            A revolution of an orbiting object.

        )doc"
    );

    enum_<Pass::Type>(
        pass_class,
        "Type",
        R"doc(
            The type of `Pass`.
        )doc"
    )

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

        .value("Ascending", Pass::Phase::Ascending, "Ascending")
        .value("Descending", Pass::Phase::Descending, "Descending")

        ;

    pass_class

        .def(
            init<
                const std::optional<int>&,
                const std::optional<Instant>&,
                const std::optional<Instant>&,
                const std::optional<Instant>&,
                const std::optional<Instant>&,
                const std::optional<Instant>&>(),
            arg("revolution_number"),
            arg("instant_at_ascending_node"),
            arg("instant_at_north_point"),
            arg("instant_at_descending_node"),
            arg("instant_at_south_point"),
            arg("instant_at_pass_break"),
            R"doc(
                Constructor.

                Args:
                    revolution_number (int | None): The revolution number of the pass. None if unknown.
                    instant_at_ascending_node (Instant | None): The instant at the ascending node of the pass. None if unknown.
                    instant_at_north_point (Instant | None): The instant at the north point of the pass. None if unknown.
                    instant_at_descending_node (Instant | None): The instant at the descending node of the pass. None if unknown.
                    instant_at_south_point (Instant | None): The instant at the south point of the pass. None if unknown.
                    instant_at_pass_break (Instant | None): The instant at break of the pass. None if unknown.

            )doc"
        )

        .def(
            "__eq__",
            [](const Pass& self, const Pass& other)
            {
                return self == other;
            },
            is_operator()
        )
        .def(
            "__ne__",
            [](const Pass& self, const Pass& other)
            {
                return self != other;
            },
            is_operator()
        )

        .def("__str__", &(shiftToString<Pass>))
        .def("__repr__", &(shiftToString<Pass>))

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
                    int | None: The revolution number of the pass, or None if it is unknown.

            )doc"
        )
        .def(
            "get_duration",
            &Pass::getDuration,
            R"doc(
                Get the duration of the pass. None if the pass is not complete.

                Returns:
                    Duration | None: The duration of the pass.

            )doc"
        )
        .def(
            "get_start_instant",
            &Pass::getStartInstant,
            R"doc(
                Get the start instant of the pass. For partial passes, this is the minimum known instant.

                Returns:
                    Instant | None: The start instant of the pass, or None if none of the instants of the pass is known.
            
            )doc"
        )
        .def(
            "get_end_instant",
            &Pass::getEndInstant,
            R"doc(
                Get the end instant of the pass. For partial passes, this is the maximum known instant.

                Returns:
                    Instant | None: The end instant of the pass, or None if none of the instants of the pass is known.

            )doc"
        )
        .def(
            "get_interval",
            &Pass::getInterval,
            R"doc(
                Get the interval of the pass. None if the pass is not complete.

                Returns:
                    Interval | None: The interval of the pass.
            )doc"
        )
        .def(
            "get_instant_at_ascending_node",
            &Pass::getInstantAtAscendingNode,
            R"doc(
                Get the instant at the ascending node of the pass.
                i.e. z = 0 & vz > 0 in an ECI frame.

                Returns:
                    Instant | None: The instant at the ascending node of the pass. None if unknown.
            
            )doc"
        )
        .def(
            "get_instant_at_north_point",
            &Pass::getInstantAtNorthPoint,
            R"doc(
                Get the instant at the north point of the pass.
                i.e. z = maximum and vz = 0 in an ECI frame.

                Returns:
                    Instant | None: The instant at the north point of the pass. None if unknown.

            )doc"
        )
        .def(
            "get_instant_at_descending_node",
            &Pass::getInstantAtDescendingNode,
            R"doc(
                Get the instant at the descending node of the pass.
                i.e. z = 0 and vz < 0 in an ECI frame.

                Returns:
                    Instant | None: The instant at the descending node of the pass. None if unknown.

            )doc"
        )
        .def(
            "get_instant_at_south_point",
            &Pass::getInstantAtSouthPoint,
            R"doc(
                Get the instant at the south point of the pass.
                i.e. z = minimum and vz = 0 in an ECI frame.

                Returns:
                    Instant | None: The instant at the south point of the pass. None if unknown.

            )doc"
        )
        .def(
            "get_instant_at_pass_break",
            &Pass::getInstantAtPassBreak,
            R"doc(
                Get the instant at the break of the pass,
                i.e. the ascending node of the next pass.

                Returns:
                    Instant | None: The instant at the break of the pass. None if unknown.

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
