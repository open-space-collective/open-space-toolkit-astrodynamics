////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Flight/System/SatelliteDynamics.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics//Flight/System/SatelliteDynamics.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Flight_System_SatelliteDynamics (        pybind11::module& aModule                               )
{

    using namespace pybind11 ;

    using ostk::physics::time::Instant ;
    using ostk::physics::Environment ;

    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::trajectory::State ;

    using ostk::astro::flight::system::SatelliteDynamics ;

    {

        class_<SatelliteDynamics> satellitedynamics_class(aModule, "SatelliteDynamics") ;

        satellitedynamics_class
            .def(init<const Environment&, const   SatelliteSystem&, const   State&, const   SatelliteDynamics::StateVectorType&>())
            .def(init<const   SatelliteDynamics&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<SatelliteDynamics>))
            .def("__repr__", &(shiftToString<SatelliteDynamics>))

            .def("is_defined", &SatelliteDynamics::isDefined)

            .def("get_state_vector_type", &SatelliteDynamics::getStateVectorType)

            .def("get_state", &SatelliteDynamics::getState)
            .def("set_state", &SatelliteDynamics::setState)
            .def("access_dynamical_equations", &SatelliteDynamics::accessDynamicalEquations)

            .def_static("string_from_state_vector_type", &SatelliteDynamics::StringFromStateVectorType)

        ;

        enum_<SatelliteDynamics::StateVectorType>(satellitedynamics_class, "StateVectorType")

            .value("Position_velocity", SatelliteDynamics::StateVectorType::PositionVelocity)
            .value("Position_velocity_dragcoeff", SatelliteDynamics::StateVectorType::PositionVelocitywithDragCoefficient)

        ;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
