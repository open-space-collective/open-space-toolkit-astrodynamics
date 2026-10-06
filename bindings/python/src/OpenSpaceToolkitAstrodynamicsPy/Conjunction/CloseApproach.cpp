/// Apache License 2.0

#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Conjunction/CloseApproach/Generator.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Conjunction_CloseApproach(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Tuple;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::time::Instant;
    using ostk::physics::unit::Derived;
    using ostk::physics::unit::Length;

    using ostk::astrodynamics::conjunction::CloseApproach;
    using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
    using ostk::astrodynamics::trajectory::State;

    class_<CloseApproach>(
        aModule,
        "CloseApproach",
        R"doc(
            Close approach between two objects.

            This class represents a close approach event between two objects, providing access to the states of both
            objects at the time of closest approach, the miss distance, the relative state, and the relative speed.
            The covariances of both objects, if any, are carried by their respective states.
        )doc"
    )

        .def(
            init<const State&, const State&>(),
            R"doc(
                Constructor.

                Args:
                    object_1_state (State): The state of Object 1.
                    object_2_state (State): The state of Object 2.
            )doc",
            arg("object_1_state"),
            arg("object_2_state")
        )

        .def("__str__", &(shiftToString<CloseApproach>))
        .def("__repr__", &(shiftToString<CloseApproach>))

        .def(
            "is_defined",
            &CloseApproach::isDefined,
            R"doc(
                Check if the close approach is defined.

                Returns:
                    bool: True if close approach is defined, False otherwise.
            )doc"
        )

        .def(
            "get_object_1_state",
            &CloseApproach::getObject1State,
            R"doc(
                Get the state of Object 1.

                Returns:
                    State: The state of Object 1.
            )doc"
        )

        .def(
            "get_object_2_state",
            &CloseApproach::getObject2State,
            R"doc(
                Get the state of Object 2.

                Returns:
                    State: The state of Object 2.
            )doc"
        )

        .def(
            "get_object_1_covariance",
            &CloseApproach::getObject1Covariance,
            R"doc(
                Get the covariance of Object 1.

                Returns:
                    Covariance | None: The covariance of Object 1, or None if the state of Object 1 has no covariance attached.
            )doc"
        )

        .def(
            "get_object_2_covariance",
            &CloseApproach::getObject2Covariance,
            R"doc(
                Get the covariance of Object 2.

                Returns:
                    Covariance | None: The covariance of Object 2, or None if the state of Object 2 has no covariance attached.
            )doc"
        )

        .def(
            "scale",
            &CloseApproach::scale,
            R"doc(
                Return a new Close Approach with the covariances of Object 1 and Object 2 scaled by the given factors.

                A covariance is only scaled if its scale factor is provided and the corresponding state has a
                covariance attached; otherwise the corresponding state is left unchanged.

                Args:
                    scale_factor_1 (float | None, optional): The scale factor for Object 1 covariance. Defaults to None.
                    scale_factor_2 (float | None, optional): The scale factor for Object 2 covariance. Defaults to None.

                Returns:
                    CloseApproach: A new Close Approach with scaled covariances.
            )doc",
            arg("scale_factor_1") = none(),
            arg("scale_factor_2") = none()
        )

        .def(
            "swap",
            &CloseApproach::swap,
            R"doc(
                Return a new Close Approach with Object 1 and Object 2 swapped.

                The states and covariances of Object 1 and Object 2 are exchanged.

                Returns:
                    CloseApproach: A new Close Approach with Object 1 and Object 2 swapped.
            )doc"
        )

        .def(
            "get_instant",
            &CloseApproach::getInstant,
            R"doc(
                Get the instant of the close approach.

                Returns:
                    Instant: The instant of closest approach.
            )doc"
        )

        .def(
            "get_miss_distance",
            &CloseApproach::getMissDistance,
            R"doc(
                Get the miss distance.

                Returns:
                    Length: The miss distance between the two objects.
            )doc"
        )

        .def(
            "get_relative_state",
            &CloseApproach::getRelativeState,
            R"doc(
                Get the relative state (Object 2 relative to Object 1).

                Returns:
                    State: The relative state.
            )doc"
        )

        .def(
            "get_relative_speed",
            &CloseApproach::getRelativeSpeed,
            R"doc(
                Get the relative speed (i.e. the magnitude of the relative velocity).

                Returns:
                    Derived: The relative speed in meters per second.
            )doc"
        )

        .def(
            "get_encounter_frame",
            &CloseApproach::getEncounterFrame,
            R"doc(
                Get the "default" convention of the encounter frame centered on Object 1.

                The encounter frame is a local orbital frame centered on Object 1:
                - z-axis: Normalized relative velocity (Object 2 velocity - Object 1 velocity)
                - y-axis: Normalized cross product of the z-axis and the relative position (Object 2 position - Object 1 position)
                - x-axis: Completes the right-handed coordinate system

                Args:
                    frame (Frame, optional): The inertial (or quasi-inertial) frame in which relative position and velocity are computed. Defaults to GCRF.

                Returns:
                    Frame: The encounter frame.
            )doc",
            arg_v("frame", Frame::GCRF(), "GCRF")
        )

        .def(
            "compute_miss_distance_components_in_frame",
            &CloseApproach::computeMissDistanceComponentsInFrame,
            R"doc(
                Compute the miss distance components in the desired frame.

                Args:
                    frame (Frame): The frame in which to resolve the miss distance components.

                Returns:
                    tuple[Length, Length, Length]: The miss distance components (x, y, z).
            )doc",
            arg("frame")
        )

        .def(
            "compute_miss_distance_components_in_local_orbital_frame",
            &CloseApproach::computeMissDistanceComponentsInLocalOrbitalFrame,
            R"doc(
                Compute the miss distance components in a local orbital frame (generated from Object 1 state).

                Args:
                    local_orbital_frame_factory (LocalOrbitalFrameFactory): The local orbital frame factory.

                Returns:
                    tuple[Length, Length, Length]: The miss distance components (radial, in-track, cross-track or similar depending on the factory).
            )doc",
            arg("local_orbital_frame_factory")
        )

        .def_static(
            "undefined",
            &CloseApproach::Undefined,
            R"doc(
                Construct an undefined close approach.

                Returns:
                    CloseApproach: An undefined close approach.
            )doc"
        )

        ;

    // Create "close_approach" python submodule
    auto closeApproach = aModule.def_submodule("close_approach");

    // Add objects to "close_approach" submodule
    OpenSpaceToolkitAstrodynamicsPy_Conjunction_CloseApproach_Generator(closeApproach);
}
