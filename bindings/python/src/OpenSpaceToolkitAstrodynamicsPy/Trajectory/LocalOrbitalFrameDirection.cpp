/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_LocalOrbitalFrameDirection(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::astro::trajectory::LocalOrbitalFrameDirection;
    using ostk::astro::trajectory::LocalOrbitalFrameFactory;

    class_<LocalOrbitalFrameDirection>(aModule, "LocalOrbitalFrameDirection")

        .def(
            init<const ostk::math::obj::Vector3d&, const Shared<const LocalOrbitalFrameFactory>&>(),
            arg("vector"),
            arg("local_orbital_frame_factory")
        )

        .def(self == self)
        .def(self != self)

        .def("is_defined", &LocalOrbitalFrameDirection::isDefined)

        .def("get_value", &LocalOrbitalFrameDirection::getValue)
        .def("get_local_orbital_frame_factory", &LocalOrbitalFrameDirection::getLocalOrbitalFrameFactory)

        .def_static("undefined", &LocalOrbitalFrameDirection::Undefined)

        ;
}
