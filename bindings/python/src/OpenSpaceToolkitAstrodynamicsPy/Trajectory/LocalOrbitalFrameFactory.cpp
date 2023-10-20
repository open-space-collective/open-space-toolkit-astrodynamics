/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameFactory(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::time::Instant;

    using ostk::astro::trajectory::LocalOrbitalFrameFactory;

    class_<LocalOrbitalFrameFactory, Shared<LocalOrbitalFrameFactory>>(aModule, "LocalOrbitalFrameFactory")
        .def(
            "is_defined",
            &LocalOrbitalFrameFactory::isDefined,
            R"doc(
                Check if the local orbital frame factory is defined.

                Returns:
                    Frame: True if the local orbital frame factory is defined, False otherwise.

            )doc"
        )

        .def(
            "generate_frame",
            &LocalOrbitalFrameFactory::generateFrame,
            arg("instant"),
            arg("position_vector"),
            arg("velocity_vector"),
            R"doc(
                Generate a local orbital frame.

                Args:
                    instant (Instant): The instant.
                    position_vector (numpy.ndarray): The position vector.
                    velocity_vector (numpy.ndarray): The velocity vector.

                Returns:
                    Frame: The local orbital frame.

            )doc"
        )

        .def_static(
            "undefined",
            &LocalOrbitalFrameFactory::Undefined,
            R"doc(
                Get an undefined local orbital frame factory.

                Returns:
                    LocalOrbitalFrameFactory: The undefined local orbital frame factory.

                Group:
                    Static methods
            )doc"
        )
        .def_static(
            "NED",
            &LocalOrbitalFrameFactory::NED,
            arg("parent_frame"),
            R"doc(
                Get a North-East-Down (NED) local orbital frame factory.

                Args:
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The NED local orbital frame factory.

                Group:
                    Static methods
            )doc"
        )
        .def_static(
            "LVLH",
            &LocalOrbitalFrameFactory::LVLH,
            arg("parent_frame"),
            R"doc(
                Get a Local Vertical Local Horizontal (LVLH) local orbital frame factory.

                Args:
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The LVLH local orbital frame factory.

                Group:
                    Static methods
            )doc"
        )
        .def_static(
            "VVLH",
            &LocalOrbitalFrameFactory::VVLH,
            arg("parent_frame"),
            R"doc(
                Get a Velocity Local Vertical Local Horizontal (VVLH) local orbital frame factory.

                Args:
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The VVLH local orbital frame factory.

                Group:
                    Static methods
            )doc"
        )
        .def_static(
            "QSW",
            &LocalOrbitalFrameFactory::QSW,
            arg("parent_frame"),
            R"doc(
                Get a Quasi-Satellite World (QSW) local orbital frame factory.

                Args:
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The QSW local orbital frame factory.

                Group:
                    Static methods
            )doc"
        )
        .def_static(
            "TNW",
            &LocalOrbitalFrameFactory::TNW,
            arg("parent_frame"),
            R"doc(
                Get a Topocentric North-West-Up (TNW) local orbital frame factory.

                Args:
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The TNW local orbital frame factory.

                Group:
                    Static methods
            )doc"
        )
        .def_static(
            "VNC",
            &LocalOrbitalFrameFactory::VNC,
            arg("parent_frame"),
            R"doc(
                Get a Velocity-Normal-Co-normal (VNC) local orbital frame factory.

                Args:
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The VNC local orbital frame factory.

                Group:
                    Static methods
            )doc"
        )

        ;
}