/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/TargetScan.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_TargetScan(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::coordinate::spherical::LLA;
    using ostk::physics::environment::object::Celestial;
    using ostk::physics::environment::object::celestial::Earth;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astrodynamics::trajectory::model::TargetScan;
    using ostk::astrodynamics::trajectory::State;

    class_<TargetScan, ostk::astrodynamics::trajectory::Model>(
        aModule,
        "TargetScan",
        R"doc(
            TargetScan trajectory model.

            This model represents a trajectory that scans between two target locations on a celestial body.
        )doc"
    )

        .def(
            init<const LLA&, const LLA&, const Instant&, const Instant&, const Celestial&, const Duration&>(),
            R"doc(
                Construct a `TargetScan` object.

                Args:
                    start_lla (LLA): The starting location.
                    end_lla (LLA): The ending location.
                    start_instant (Instant): The starting instant.
                    end_instant (Instant): The ending instant.
                    celestial (Celestial): The celestial body. Defaults to Earth.WGS84().
                    step_size (Duration): The step size for the trajectory. Defaults to 1e-2 seconds.

                Returns:
                    TargetScan: The `TargetScan` object.
            )doc",
            arg("start_lla"),
            arg("end_lla"),
            arg("start_instant"),
            arg("end_instant"),
            arg("celestial") = Earth::WGS84(),
            arg("step_size") = Duration::Seconds(1e-2)
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<TargetScan>))
        .def("__repr__", &(shiftToString<TargetScan>))

        .def(
            "is_defined",
            &TargetScan::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.
            )doc"
        )
        .def(
            "calculate_state_at",
            &TargetScan::calculateStateAt,
            R"doc(
                Calculate the state at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state at the given instant.
            )doc",
            arg("instant")
        )
        .def(
            "get_start_lla",
            &TargetScan::getStartLLA,
            R"doc(
                Get the starting LLA of the target scan.

                Returns:
                    LLA: The starting LLA.
            )doc"
        )
        .def(
            "get_end_lla",
            &TargetScan::getEndLLA,
            R"doc(
                Get the ending LLA of the target scan.

                Returns:
                    LLA: The ending LLA.
            )doc"
        )
        .def(
            "get_start_instant",
            &TargetScan::getStartInstant,
            R"doc(
                Get the starting instant of the target scan.

                Returns:
                    Instant: The starting instant.
            )doc"
        )
        .def(
            "get_end_instant",
            &TargetScan::getEndInstant,
            R"doc(
                Get the ending instant of the target scan.

                Returns:
                    Instant: The ending instant.
            )doc"
        )
        .def(
            "get_celestial",
            &TargetScan::getCelestial,
            R"doc(
                Get the celestial object of the target scan.

                Returns:
                    Celestial: The celestial object.
            )doc"
        )
        .def(
            "get_step_size",
            &TargetScan::getStepSize,
            R"doc(
                Get the step size of the target scan.

                Returns:
                    Duration: The step size.
            )doc"
        )

        .def_static(
            "from_ground_speed",
            &TargetScan::FromGroundSpeed,
            R"doc(
                Construct a `TargetScan` object from ground speed.

                Args:
                    start_lla (LLA): The starting location.
                    end_lla (LLA): The ending location.
                    ground_speed (Derived): The ground speed.
                    start_instant (Instant): The starting instant.
                    celestial (Celestial): The celestial body.
                    step_size (Duration): The step size for the trajectory.

                Returns:
                    TargetScan: The `TargetScan` object.
            )doc",
            arg("start_lla"),
            arg("end_lla"),
            arg("ground_speed"),
            arg("start_instant"),
            arg("celestial") = Earth::WGS84(),
            arg("step_size") = Duration::Seconds(1e-2)
        )

        ;
}
