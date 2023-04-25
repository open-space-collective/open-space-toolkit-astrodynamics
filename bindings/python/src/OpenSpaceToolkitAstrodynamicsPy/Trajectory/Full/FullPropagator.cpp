////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/FullPropagator.cpp
/// @author         Vishwa Shah <vishwa@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Full/FullPropagator.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Full_Propagator (        pybind11::module&         aModule                                     )
{

    using namespace pybind11 ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;

    using ostk::astro::NumericalSolver ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::trajectory::FullPropagator ;
    using ostk::astro::flight::system::dynamics::FullSatelliteDynamics ;

    class_<FullPropagator>(aModule, "FullPropagator")

        .def
        (
            init<const FullSatelliteDynamics&, const NumericalSolver&>(),
            arg("satellite_dynamics"),
            arg("numerical_solver")
        )

        .def("__str__", &(shiftToString<FullPropagator>))
        .def("__repr__", &(shiftToString<FullPropagator>))

        .def("is_defined", &FullPropagator::isDefined)

        .def
        (
            "calculate_state_at",
            &FullPropagator::calculateStateAt,
            arg("state"),
            arg("instant")
        )

        .def
        (
            "calculate_states_at",
            &FullPropagator::calculateStatesAt,
            arg("state"),
            arg("instant_array")
        )

        .def_static
        (
            "medium_fidelity",
            &FullPropagator::MediumFidelity
        )

        .def_static
        (
            "high_fidelity",
            &FullPropagator::HighFidelity
        )

    ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
