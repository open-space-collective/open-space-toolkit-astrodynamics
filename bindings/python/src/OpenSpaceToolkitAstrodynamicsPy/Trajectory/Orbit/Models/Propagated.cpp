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
                init<const SatelliteDynamics&, const NumericalSolver&>(),
                arg("satellite_dynamics"),
                arg("numerical_solver")
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
                overload_cast<const Instant&>(&Propagated::calculateStateAt, const_),
                arg("instant")
            )

            .def
            (
                "calculate_state_at",
                overload_cast<const State&, const Instant&>(&Propagated::calculateStateAt, const_),
                arg("state"),
                arg("instant")
            )

            .def
            (
                "calculate_states_at",
                overload_cast<const Array<Instant>&>(&Propagated::calculateStatesAt, const_),
                arg("instant_array")
            )

            .def
            (
                "calculate_states_at",
                overload_cast<const State&, const Array<Instant>&>(&Propagated::calculateStatesAt, const_),
                arg("state"),
                arg("instant_array")
            )

            .def
            (
                "calculate_revolution_number_at",
                &Propagated::calculateRevolutionNumberAt,
                arg("instant")
            )

            .def("access_cached_state_array", &Propagated::accessCachedStateArray)

            .def_static
            (
                "medium_fidelity",
                &Propagated::MediumFidelity,
                arg("state")
            )

            .def_static
            (
                "high_fidelity",
                &Propagated::HighFidelity,
                arg("state")
            )

        ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
