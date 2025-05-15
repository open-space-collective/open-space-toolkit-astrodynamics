/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/LocalOrbitalFrameDirection.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/LocalOrbitalFrameFactory.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/LocalOrbitalFrameTransformProvider.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Model.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Propagator.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Segment.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Sequence.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/StateBuilder.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::spherical::LLA;
    using ostk::physics::environment::object::Celestial;
    using ostk::physics::environment::object::celestial::Earth;
    using ostk::physics::time::Duration;
    using ostk::physics::unit::Derived;

    using ostk::astrodynamics::Trajectory;
    using ostk::astrodynamics::trajectory::State;

    class_<Trajectory, Shared<Trajectory>>(
        aModule,
        "Trajectory",
        R"doc(
            Path followed by an object through space as a function of time.

        )doc"
    )

        .def(
            init<const ostk::astrodynamics::trajectory::Model&>(),
            R"doc(
                Construct a `Trajectory` object from a model.

                Args:
                    model (trajectory.Model): The model of the trajectory.

                Returns:
                    Trajectory: The `Trajectory` object.
            )doc",
            arg("model")
        )

        .def(
            init<const Array<State>&>(),
            R"doc(
                Construct a `Trajectory` object from an array of states.

                Args:
                    states (list[State]): The states of the trajectory.

                Returns:
                    Trajectory: The `Trajectory` object.
            )doc",
            arg("states")
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Trajectory>))
        .def("__repr__", &(shiftToString<Trajectory>))

        .def(
            "is_defined",
            &Trajectory::isDefined,
            R"doc(
                Check if the trajectory is defined.

                Returns:
                    bool: True if the trajectory is defined, False otherwise.
            )doc"
        )

        .def(
            "access_model",
            &Trajectory::accessModel,
            return_value_policy::reference_internal,
            R"doc(
                Access the model of the trajectory.

                Returns:
                    Model: The model of the trajectory.
            )doc"
        )

        .def(
            "get_state_at",
            &Trajectory::getStateAt,
            R"doc(
                Get the state of the trajectory at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state of the trajectory at the given instant.
            )doc",
            arg("instant")
        )

        .def(
            "get_states_at",
            &Trajectory::getStatesAt,
            R"doc(
                Get the states of the trajectory at a given set of instants. It can be more performant than looping `calculate_state_at` for multiple instants.

                Args:
                    instants (list[Instant]): The instants.

                Returns:
                    list[State]: The states of the trajectory at the given instants.
            )doc",
            arg("instants")
        )

        .def_static(
            "undefined",
            &Trajectory::Undefined,
            R"doc(
                Create an undefined `Trajectory` object.

                Returns:
                    Trajectory: The undefined `Trajectory` object.
            )doc"
        )

        .def_static(
            "position",
            &Trajectory::Position,
            R"doc(
                Create a `Trajectory` object representing a position.

                Args:
                    position (Position): The position. Must be in the ITRF frame.

                Returns:
                    Trajectory: The `Trajectory` object representing the position.
            )doc",
            arg("position")
        )
        .def_static(
            "ground_strip",
            overload_cast<const LLA&, const LLA&, const Derived&, const Instant&, const Celestial&, const Duration&>(
                &Trajectory::GroundStrip
            ),
            R"doc(
                Create a `Trajectory` object representing a ground strip.
                Computes the duration as the geodetic distance / ground speed.
                Instants are generated at a 1 second interval.

                Args:
                    start_lla (LLA): The start LLA.
                    end_lla (LLA): The end LLA.
                    ground_speed (Derived): The ground speed.
                    start_instant (Instant): The start instant.
                    celestial_object (Celestial): The celestial object. Defaults to Earth.WGS84().
                    step_size (Duration): The step size. Defaults to 1 second.

                Returns:
                    Trajectory: The `Trajectory` object representing the ground strip.

            )doc",
            arg("start_lla"),
            arg("end_lla"),
            arg("ground_speed"),
            arg("start_instant"),
            arg_v("celestial_object", Earth::WGS84(), "Earth.WGS84()"),
            arg_v("step_size", Duration::Seconds(1.0), "Duration.Seconds(1.0)")
        )
        .def_static(
            "ground_strip",
            overload_cast<const LLA&, const LLA&, const Array<Instant>&, const Celestial&>(&Trajectory::GroundStrip),
            R"doc(
                Create a `Trajectory` object representing a ground strip.
                This method computes the duration as the geodetic distance / ground speed.

                Args:
                    start_lla (LLA): The start LLA.
                    end_lla (LLA): The end LLA.
                    instants (list[Instant]): The instants.
                    celestial_object (Celestial): The celestial object. Defaults to Earth.WGS84().

                Returns:
                    Trajectory: The `Trajectory` object representing the ground strip.

            )doc",
            arg("start_lla"),
            arg("end_lla"),
            arg("instants"),
            arg_v("celestial_object", Earth::WGS84(), "Earth.WGS84()")
        )
        .def_static(
            "ground_strip_geodetic_nadir",
            &Trajectory::GroundStripGeodeticNadir,
            R"doc(
                Create a `Trajectory` object representing a ground strip that follows the geodetic nadir of the provided orbit.

                Args:
                    orbit (Orbit): The orbit.
                    instants (list[Instant]): The instants.
                    celestial_object (Celestial): The celestial object. Defaults to Earth.WGS84().

                Returns:
                    Trajectory: The `Trajectory` object representing the ground strip.  
            )doc",
            arg("orbit"),
            arg("instants"),
            arg_v("celestial_object", Earth::WGS84(), "Earth.WGS84()")
        )
        .def_static(
            "target_scan",
            overload_cast<const LLA&, const LLA&, const Instant&, const Instant&, const Celestial&>(
                &Trajectory::TargetScan
            ),
            R"doc(
                Create a `Trajectory` object representing a target scan between two locations at the provided instants.

                Args:
                    start_lla (LLA): The start LLA.
                    end_lla (LLA): The end LLA.
                    start_instant (Instant): The start instant.
                    end_instant (Instant): The end instant.
                    celestial (Celestial): The celestial object. Defaults to Earth.WGS84().

                Returns:
                    Trajectory: The `Trajectory` object representing the target scan.
            )doc",
            arg("start_lla"),
            arg("end_lla"),
            arg("start_instant"),
            arg("end_instant"),
            arg_v("celestial", Earth::WGS84(), "Earth.WGS84()")
        )
        .def_static(
            "target_scan",
            overload_cast<const LLA&, const LLA&, const Derived&, const Instant&, const Celestial&>(
                &Trajectory::TargetScan
            ),
            R"doc(
                Create a `Trajectory` object representing a target scan between two locations with a specified ground speed.

                Args:
                    start_lla (LLA): The start LLA.
                    end_lla (LLA): The end LLA.
                    ground_speed (Derived): The ground speed.
                    start_instant (Instant): The start instant.
                    celestial (Celestial): The celestial object. Defaults to Earth.WGS84().

                Returns:
                    Trajectory: The `Trajectory` object representing the target scan.
            )doc",
            arg("start_lla"),
            arg("end_lla"),
            arg("ground_speed"),
            arg("start_instant"),
            arg_v("celestial", Earth::WGS84(), "Earth.WGS84()")
        );

    // Create "trajectory" python submodule
    auto trajectory = aModule.def_submodule("trajectory");

    // Add objects to python submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameTransformProvider(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameFactory(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameDirection(trajectory);

    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_StateBuilder(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Propagator(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Segment(trajectory);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Sequence(trajectory);
}
