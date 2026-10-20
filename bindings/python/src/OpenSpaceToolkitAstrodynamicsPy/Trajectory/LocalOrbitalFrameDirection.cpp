/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameDirection(nanobind::module_& aModule)
{
    using namespace nanobind;

    using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
    using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;

    class_<LocalOrbitalFrameDirection>(
        aModule,
        "LocalOrbitalFrameDirection",
        R"doc(
            A local orbital frame direction.

        )doc"
    )
        .def(
            init<const ostk::mathematics::object::Vector3d&, const Shared<const LocalOrbitalFrameFactory>&>(),
            arg("vector"),
            arg("local_orbital_frame_factory"),
            R"doc(
                Construct a new `LocalOrbitalFrameDirection` object.

                Args:
                    vector (numpy.ndarray): The vector expressed in the local orbital frame.
                    local_orbital_frame_factory (LocalOrbitalFrameFactory): The local orbital frame factory that defines the frame.

                Returns:
                    LocalOrbitalFrameDirection: The new `LocalOrbitalFrameDirection` object.
            )doc"
        )

        .def(
            "__eq__",
            [](const LocalOrbitalFrameDirection& self, const LocalOrbitalFrameDirection& other)
            {
                return self == other;
            },
            nanobind::is_operator()
        )
        .def(
            "__ne__",
            [](const LocalOrbitalFrameDirection& self, const LocalOrbitalFrameDirection& other)
            {
                return self != other;
            },
            nanobind::is_operator()
        )

        .def(
            "is_defined",
            &LocalOrbitalFrameDirection::isDefined,
            R"doc(
                Check if the local orbital frame direction is defined.

                Returns:
                    bool: True if the local orbital frame direction is defined, False otherwise.

            )doc"
        )

        .def(
            "get_value",
            &LocalOrbitalFrameDirection::getValue,
            R"doc(
                Get the vector expressed in the local orbital frame.

                Returns:
                    Vector3d: The vector expressed in the local orbital frame.

            )doc"
        )
        .def(
            "get_local_orbital_frame_factory",
            &LocalOrbitalFrameDirection::getLocalOrbitalFrameFactory,
            R"doc(
                Get the local orbital frame factory that defines the frame.

                Returns:
                    LocalOrbitalFrameFactory: The local orbital frame factory that defines the frame.

            )doc"
        )

        .def_static(
            "undefined",
            &LocalOrbitalFrameDirection::Undefined,
            R"doc(
                Get an undefined local orbital frame direction.

                Returns:
                    LocalOrbitalFrameDirection: The undefined local orbital frame direction.
            )doc"
        )

        ;
}
