/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameTransformProvider(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Transform;
    using ostk::physics::time::Instant;

    using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;

    class_<LocalOrbitalFrameTransformProvider, Shared<LocalOrbitalFrameTransformProvider>>
        localOrbitalFrameTransformProviderClass(
            aModule,
            "LocalOrbitalFrameTransformProvider",
            R"doc(
                Local orbital frame transform provider, frame provider.
                Generates a specific transform based on a State (instant, position, velocity) and a LOF type.

            )doc"
        );
    // TBI: can't make this linked with Shared<Provider>

    enum_<LocalOrbitalFrameTransformProvider::Type>(
        localOrbitalFrameTransformProviderClass,
        "Type",
        R"doc(
            The local orbital frame type.
        )doc"
    )

        .value("Undefined", LocalOrbitalFrameTransformProvider::Type::Undefined, "Undefined")
        .value("NED", LocalOrbitalFrameTransformProvider::Type::NED, "North-East-Down")
        .value("LVLH", LocalOrbitalFrameTransformProvider::Type::LVLH, "Local Vertical-Local Horizontal")
        .value("LVLHGD", LocalOrbitalFrameTransformProvider::Type::LVLHGD, "Local Vertical-Local Horizontal Geodetic")
        .value("VVLH", LocalOrbitalFrameTransformProvider::Type::VVLH, "Vertical-Local Horizontal")
        .value("QSW", LocalOrbitalFrameTransformProvider::Type::QSW, "Quasi-Satellite West")
        .value("TNW", LocalOrbitalFrameTransformProvider::Type::TNW, "Tangent-Normal-Wideband")
        .value("VNC", LocalOrbitalFrameTransformProvider::Type::VNC, "Velocity-Normal-Conormal")

        ;

    localOrbitalFrameTransformProviderClass

        .def(
            init<const Transform&>(),
            arg("transform"),
            R"doc(
                Constructs a local orbital frame transform provider.

                Args:
                    transform (Transform): The transform.

                Returns:
                    LocalOrbitalFrameTransformProvider: The provider.
            )doc"
        )

        .def(
            "is_defined",
            &LocalOrbitalFrameTransformProvider::isDefined,
            R"doc(
                Returns true if the provider is defined.

                Returns:
                    bool: True if the provider is defined.
            )doc"
        )

        .def(
            "get_transform_at",
            &LocalOrbitalFrameTransformProvider::getTransformAt,
            R"doc(
                Returns the transform at a given instant.

                Args:
                    instant (Instant): The instant.

                Returns:
                    Transform: The transform at the given instant.
            )doc",
            arg("instant")
        )

        .def_static(
            "construct",
            &LocalOrbitalFrameTransformProvider::Construct,
            R"doc(
                Constructs a local orbital frame transform provider for the provided type.

                Args:
                    type (LocalOrbitalFrameTransformProvider.Type): The local orbital frame provider type.
                    state (State): The state.

                Returns:
                    LocalOrbitalFrameTransformProvider: The provider.
            )doc",
            arg("type"),
            arg("state")
        )

        .def_static(
            "get_transform_generator",
            &LocalOrbitalFrameTransformProvider::GetTransformGenerator,
            R"doc(
                Returns the transform generator function for a given type.

                Args:
                    type (LocalOrbitalFrameTransformProvider.Type): The local orbital frame provider type.

                Returns:
                    callable[[State], Transform]: The transform generator function.
            )doc",
            arg("type")
        )

        ;
}
