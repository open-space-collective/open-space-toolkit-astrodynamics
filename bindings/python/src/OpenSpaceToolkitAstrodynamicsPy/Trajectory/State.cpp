/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::time::Instant;

    using ostk::astro::trajectory::State;

    class_<State>(aModule, "State")

        .def(init<const Instant&, const Position&, const Velocity&>(), arg("instant"), arg("position"), arg("velocity"))

        .def(self == self)
        .def(self != self)
        .def(self + self)
        .def(self - self)

        .def("__str__", &(shiftToString<State>))
        .def("__repr__", &(shiftToString<State>))

        .def("is_defined", &State::isDefined)

        .def("get_instant", &State::getInstant)
        .def("get_position", &State::getPosition)
        .def("get_velocity", &State::getVelocity)
        .def("get_coordinates", &State::getCoordinates)
        .def("in_frame", &State::inFrame, arg("frame"))

        .def_static("undefined", &State::Undefined)

        ;
}
