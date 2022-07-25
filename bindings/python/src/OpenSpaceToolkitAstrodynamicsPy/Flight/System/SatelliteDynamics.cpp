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

        class_<SatelliteDynamics> satellite_dynamics_class(aModule, "SatelliteDynamics") ;

        satellite_dynamics_class
            .def(init<const Environment&, const   SatelliteSystem&, const   State&, const   SatelliteDynamics::StateVectorDimension&>())
            .def(init<const   SatelliteDynamics&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<SatelliteDynamics>))
            .def("__repr__", &(shiftToString<SatelliteDynamics>))

            .def("is_defined", &SatelliteDynamics::isDefined)

            .def("get_state_vector_dimension", &SatelliteDynamics::getStateVectorDimension)

            .def("get_state", &SatelliteDynamics::getState)
            .def("set_state", &SatelliteDynamics::setState)
            .def("access_dynamical_equations", &SatelliteDynamics::accessDynamicalEquations)

            .def_static("string_from_state_vector_dimension", &SatelliteDynamics::StringFromStateVectorDimension)

        ;

        enum_<SatelliteDynamics::StateVectorDimension>(satellite_dynamics_class, "StateVectorDimension")

            .value("PositionVelocity", SatelliteDynamics::StateVectorDimension::PositionVelocity)
            .value("PositionVelocityDragCoefficient", SatelliteDynamics::StateVectorDimension::PositionVelocitywithDragCoefficient)

        ;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
