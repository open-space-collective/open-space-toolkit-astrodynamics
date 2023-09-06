/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameTransformProvider(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;

    using ostk::physics::time::Instant;

    using ostk::astro::trajectory::LocalOrbitalFrameTransformProvider;

    class_<LocalOrbitalFrameTransformProvider, Shared<LocalOrbitalFrameTransformProvider>>
        localOrbitalFrameTransformProviderClass(aModule, "LocalOrbitalFrameTransformProvider");
    // TBI: can't make this linked with Shared<Provider>

    localOrbitalFrameTransformProviderClass

        .def("is_defined", &LocalOrbitalFrameTransformProvider::isDefined)

        .def("get_transform_at", &LocalOrbitalFrameTransformProvider::getTransformAt, arg("instant"))

        ;

    enum_<LocalOrbitalFrameTransformProvider::Type>(localOrbitalFrameTransformProviderClass, "Type")

        .value("Undefined", LocalOrbitalFrameTransformProvider::Type::Undefined)
        .value("NED", LocalOrbitalFrameTransformProvider::Type::NED)
        .value("LVLH", LocalOrbitalFrameTransformProvider::Type::LVLH)
        .value("LVLHGD", LocalOrbitalFrameTransformProvider::Type::LVLHGD)
        .value("VVLH", LocalOrbitalFrameTransformProvider::Type::VVLH)
        .value("QSW", LocalOrbitalFrameTransformProvider::Type::QSW)
        .value("TNW", LocalOrbitalFrameTransformProvider::Type::TNW)
        .value("VNC", LocalOrbitalFrameTransformProvider::Type::VNC)

        ;
}
