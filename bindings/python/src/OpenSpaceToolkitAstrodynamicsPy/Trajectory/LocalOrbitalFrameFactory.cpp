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

        .def("is_defined", &LocalOrbitalFrameFactory::isDefined)

        .def(
            "generate_frame",
            &LocalOrbitalFrameFactory::generateFrame,
            arg("instant"),
            arg("position_vector"),
            arg("velocity_vector")
        )

        .def_static("undefined", &LocalOrbitalFrameFactory::Undefined)
        .def_static("NED", &LocalOrbitalFrameFactory::NED, arg("parent_frame"))
        .def_static("LVLH", &LocalOrbitalFrameFactory::LVLH, arg("parent_frame"))
        .def_static("VVLH", &LocalOrbitalFrameFactory::VVLH, arg("parent_frame"))
        .def_static("QSW", &LocalOrbitalFrameFactory::QSW, arg("parent_frame"))
        .def_static("TNW", &LocalOrbitalFrameFactory::TNW, arg("parent_frame"))
        .def_static("VNC", &LocalOrbitalFrameFactory::VNC, arg("parent_frame"))

        ;
}
