////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <OpenSpaceToolkitAstrodynamicsPy/Utilities/IterableConverter.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_State (        pybind11::module&         aModule                                     )
{

    using namespace pybind11 ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;

    using ostk::astro::trajectory::State ;

    class_<State>(aModule, "State")

        .def(init<const Instant&, const Position&, const Velocity&>())

        .def(self == self)
        .def(self != self)

        // .def(self_ns::str(self_ns::self))
        // .def(self_ns::repr(self_ns::self))

        .def("is_defined", &State::isDefined)

        .def("get_instant", &State::getInstant)
        .def("get_position", &State::getPosition)
        .def("get_velocity", &State::getVelocity)
        .def("in_frame", &State::inFrame)

        .def_static("undefined", &State::Undefined)

    ;

    // using ostk::core::ctnr::Array ;

    // IterableConverter()

    //     .from_python<Array<State>>()
    //     .to_python<Array<State>>()

    // ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
