////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/SatelliteDynamics.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_SatelliteDynamics (        pybind11::module& aModule                               )
{

    using namespace pybind11 ;

    using ostk::physics::time::Instant ;
    using ostk::physics::Environment ;

    using ostk::astro::flight::system::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::Dynamics ;
    using ostk::astro::flight::system::dynamics::SatelliteDynamics ;

    {

        class_<SatelliteDynamics> satellite_dynamics_class(aModule, "SatelliteDynamics") ;

        satellite_dynamics_class
            .def(init<const Environment&, const SatelliteSystem&, const State&>())
            .def(init<const SatelliteDynamics&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<SatelliteDynamics>))
            .def("__repr__", &(shiftToString<SatelliteDynamics>))

            .def("is_defined", &SatelliteDynamics::isDefined)

            .def("get_state", &SatelliteDynamics::getState)
            .def("set_state", &SatelliteDynamics::setState)
            .def("get_dynamical_equations", &SatelliteDynamics::getDynamicalEquations)

        ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
