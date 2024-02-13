/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameTransformProvider(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;

    using ostk::physics::time::Instant;

    using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;

    class_<LocalOrbitalFrameTransformProvider, Shared<LocalOrbitalFrameTransformProvider>>
        localOrbitalFrameTransformProviderClass(
            aModule,
            "LocalOrbitalFrameTransformProvider",
            R"doc(
                Local orbital frame transform provider, frame provider.
                Generates a specific transform based on instant, position, velocity and a LOF type.

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
        .value("TNW", LocalOrbitalFrameTransformProvider::Type::TNW, "Topocentric North-West")
        .value("VNC", LocalOrbitalFrameTransformProvider::Type::VNC, "Velocity-Normal-Co-normal")

        ;

    localOrbitalFrameTransformProviderClass

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

        ;
}
