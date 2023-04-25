////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/System/Dynamics/FullSatelliteDynamics.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/FullSatelliteDynamics.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_System_Dynamics_FullSatelliteDynamics ( pybind11::module& aModule                            )
{

    using namespace pybind11 ;

    using ostk::physics::time::Instant ;
    using ostk::physics::Environment ;

    using ostk::astro::flight::system::SatelliteSystem ;

    using ostk::astro::flight::system::Dynamics ;
    using ostk::astro::flight::system::dynamics::FullSatelliteDynamics ;

    {

        class_<FullSatelliteDynamics>(aModule, "FullSatelliteDynamics")

            .def
            (
                init<const Environment&, const SatelliteSystem&>(),
                arg("environment"),
                arg("satellite_system")
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<FullSatelliteDynamics>))
            .def("__repr__", &(shiftToString<FullSatelliteDynamics>))

            .def("is_defined", &FullSatelliteDynamics::isDefined)

            .def("get_instant", &FullSatelliteDynamics::getInstant)
            .def("set_instant", &FullSatelliteDynamics::setInstant, arg("instant"))
            .def("get_dynamical_equations", &FullSatelliteDynamics::getDynamicalEquations)

        ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
