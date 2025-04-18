/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameFactory(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::coordinate::Transform;
    using ostk::physics::time::Instant;

    using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
    using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;

    class_<LocalOrbitalFrameFactory, Shared<LocalOrbitalFrameFactory>>(
        aModule,
        "LocalOrbitalFrameFactory",
        R"doc(
            The local orbital frame factory.

        )doc"
    )
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
            "access_parent_frame",
            &LocalOrbitalFrameFactory::accessParentFrame,
            R"doc(
                Get the parent frame.

                Returns:
                    Frame: The parent frame.
            )doc"
        )

        .def(
            "get_provider_type",
            &LocalOrbitalFrameFactory::getProviderType,
            R"doc(
                Get the provider type.

                Returns:
                    LocalOrbitalFrameTransformProvider.Type: The provider type.
            )doc"
        )

        .def(
            "generate_frame",
            &LocalOrbitalFrameFactory::generateFrame,
            arg("state"),
            R"doc(
                Generate a local orbital frame.

                Args:
                    state (State): The state.

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
            )doc"
        )
        .def_static(
            "TNW",
            &LocalOrbitalFrameFactory::TNW,
            arg("parent_frame"),
            R"doc(
                Get a Tangent-Normal-Wideband (TNW) local orbital frame factory.

                Args:
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The TNW local orbital frame factory.
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
            )doc"
        )

        .def_static(
            "construct",
            overload_cast<const LocalOrbitalFrameTransformProvider::Type&, const Shared<const Frame>&>(
                &LocalOrbitalFrameFactory::Construct
            ),
            arg("type"),
            arg("parent_frame"),
            R"doc(
                Construct a local orbital frame factory for the provided type.

                Args:
                    type (LocalOrbitalFrameTransformProvider.Type): The type of local orbital frame transform provider.
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The local orbital frame factory.
            )doc"
        )
        .def_static(
            "construct",
            overload_cast<const std::function<Transform(const State&)>&, const Shared<const Frame>&>(
                &LocalOrbitalFrameFactory::Construct
            ),
            arg("transform_generator"),
            arg("parent_frame"),
            R"doc(
                Construct a local orbital frame factory for a custom type, using the provided transform generator.

                Args:
                    transform_generator (callable[[State], Transform]): The transform generator.
                    parent_frame (Frame): The parent frame.

                Returns:
                    LocalOrbitalFrameFactory: The local orbital frame factory.
            )doc"
        )

        ;
}
