/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Maneuver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Maneuver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::physics::coord::Velocity;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Duration;

    using ostk::astro::trajectory::Maneuver;

    class_<Maneuver>(aModule, "Maneuver")

        .def(init<const Instant&, const Duration&, const Velocity&>(), arg("instant"), arg("duration"), arg("velocity_delta"))

        .def(self == self)
        .def(self != self)

        // .def("__str__", &(shiftToString<Maneuver>))
        // .def("__repr__", &(shiftToString<Maneuver>))

        .def("is_defined", &Maneuver::isDefined)

        .def("get_instant", &Maneuver::getInstant)
        .def("get_duration", &Maneuver::getDuration)
        .def("get_velocity_delta", &Maneuver::getVelocityDelta)

        .def_static("undefined", &Maneuver::Undefined)

        ;
}
