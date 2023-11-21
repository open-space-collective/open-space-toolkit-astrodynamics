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

    using ostk::core::ctnr::Array;

    using ostk::astro::Trajectory;
    using ostk::astro::trajectory::State;

    class_<Trajectory>(
        aModule,
        "Trajectory",
        R"doc(
            Path followed by an object through space as a function of time.

            Group:
                trajectory
        )doc"
    )

        .def(
            init<const ostk::astro::trajectory::Model&>(),
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
                Get the states of the trajectory at a given set of instants. This method is preferred over calling `get_state_at` multiple times since there is a performance gain.

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
                    position (Position): The position.

                Returns:
                    Trajectory: The `Trajectory` object representing the position.
            )doc",
            arg("position")
        )

        ;

    // Create "trajectory" python submodule
    auto trajectory = aModule.def_submodule("trajectory");

    // Add __path__ attribute for "trajectory" submodule
    trajectory.attr("__path__") = "ostk.astrodynamics.trajectory";

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
