////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Propagated.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Propagated (        pybind11::module& aModule                           )
{

    using namespace pybind11 ;

    using ostk::core::ctnr::Array ;

    using ostk::physics::time::Instant ;

    using ostk::astro::NumericalSolver ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;
    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        class_<Propagated, ostk::astro::trajectory::orbit::Model> propagated_class(aModule, "Propagated") ;

        propagated_class

            .def
            (
                init<const SatelliteDynamics&, const NumericalSolver&, const State&>(),
                arg("satellite_dynamics"),
                arg("numerical_solver"),
                arg("state")
            )

            .def
            (
                init<const SatelliteDynamics&, const NumericalSolver&, const Array<State>&>(),
                arg("satellite_dynamics"),
                arg("numerical_solver"),
                arg("state_array")
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<Propagated>))
            .def("__repr__", &(shiftToString<Propagated>))

            .def("is_defined", &Propagated::isDefined)

            .def("get_epoch", &Propagated::getEpoch)
            .def("get_revolution_number_at_epoch", &Propagated::getRevolutionNumberAtEpoch)

            .def
            (
                "calculate_state_at",
                &Propagated::calculateStateAt,
                arg("instant")
            )

            .def
            (
                "calculate_states_at",
                &Propagated::calculateStatesAt,
                arg("instant_array")
            )

            .def
            (
                "calculate_revolution_number_at",
                &Propagated::calculateRevolutionNumberAt,
                arg("instant")
            )

            .def("access_cached_state_array", &Propagated::accessCachedStateArray)
            .def("access_propagator", &Propagated::accessPropagator)

            .def(
                "set_cached_state_array",
                &Propagated::setCachedStateArray,
                arg("state_array")
            )

        ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
