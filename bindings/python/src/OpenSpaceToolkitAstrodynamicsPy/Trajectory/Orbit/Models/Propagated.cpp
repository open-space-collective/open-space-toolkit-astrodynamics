////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Models/Propagated.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Models_Propagated (        pybind11::module& aModule                               )
{

    using namespace pybind11 ;

    using ostk::core::types::Real ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Instant ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::trajectory::State ;
    using ostk::astro::trajectory::orbit::models::Propagated ;

    {

        class_<Propagated, ostk::astro::trajectory::orbit::Model> propagated_class(aModule, "Propagated") ;

        propagated_class.def(init<const State&, const Instant&, const Derived&, const Length&, const Propagated::GravitationalPerturbationType&, const Propagated::AtmosphericPerturbationType&, const Propagated::ThirdBodyPerturbationType&>())
            .def(init<const State&, const Instant&, const Derived&, const Length&, const Propagated::GravitationalPerturbationType&, const Propagated::AtmosphericPerturbationType&, const Propagated::ThirdBodyPerturbationType&, const Propagated::IntegrationStepperType&, const Propagated::IntegrationLogType&, const Real&, const Real&>())
            .def(init<const State&, const Instant&, const   Celestial&, const Propagated::GravitationalPerturbationType&, const Propagated::AtmosphericPerturbationType&, const Propagated::ThirdBodyPerturbationType&>())
            .def(init<const State&, const Instant&, const   Celestial&, const Propagated::GravitationalPerturbationType&, const Propagated::AtmosphericPerturbationType&, const Propagated::ThirdBodyPerturbationType&, const Propagated::IntegrationStepperType&, const Propagated::IntegrationLogType&, const Real&, const Real&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<Propagated>))
            .def("__repr__", &(shiftToString<Propagated>))

            .def("is_defined", &Propagated::isDefined)

            .def("get_epoch", &Propagated::getEpoch)
            .def("get_revolution_number_at_epoch", &Propagated::getRevolutionNumberAtEpoch)
            .def("get_gravitational_parameter", &Propagated::getGravitationalParameter)
            .def("get_equatorial_radius", &Propagated::getEquatorialRadius)

            .def("get_gravitational_perturbation_type", &Propagated::getGravitationalPerturbationType)
            .def("get_atmospheric_perturbation_type", &Propagated::getAtmosphericPerturbationType)
            .def("get_thirdbody_perturbation_type", &Propagated::getThirdBodyPerturbationType)
            .def("get_integration_stepper_type", &Propagated::getIntegrationStepperType)
            .def("get_integration_log_type", &Propagated::getIntegrationLogType)


            .def("calculate_state_at", &Propagated::calculateStateAt)
            .def("calculate_revolution_number_at", &Propagated::calculateRevolutionNumberAt)

            .def_static("string_from_gravitational_perturbation_type", &Propagated::StringFromGravitationalPerturbationType)
            .def_static("string_from_atmospheric_perturbation_type", &Propagated::StringFromAtmosphericPerturbationType)
            .def_static("string_from_thirdbody_perturbation_type", &Propagated::StringFromThirdBodyPerturbationType)
            .def_static("string_from_integration_stepper_type", &Propagated::StringFromIntegrationStepperType)
            .def_static("string_from_integration_log_type", &Propagated::StringFromIntegrationLogType)

        ;

        enum_<Propagated::GravitationalPerturbationType>(propagated_class, "GravitationalPerturbationType")

            .value("No", Propagated::GravitationalPerturbationType::None)
            .value("J2", Propagated::GravitationalPerturbationType::J2)
            .value("Ten_by_ten", Propagated::GravitationalPerturbationType::TenByTen)
            .value("Fourty_by_fourty", Propagated::GravitationalPerturbationType::FourtyByFourty)

        ;

        enum_<Propagated::AtmosphericPerturbationType>(propagated_class, "AtmosphericPerturbationType")

            .value("No", Propagated::AtmosphericPerturbationType::None)
            .value("Exponential", Propagated::AtmosphericPerturbationType::Exponential)
            .value("Jacchia_roberts", Propagated::AtmosphericPerturbationType::JacchiaRoberts)
            .value("NRLMISIS00", Propagated::AtmosphericPerturbationType::NRLMISIS00)

        ;

        enum_<Propagated::ThirdBodyPerturbationType>(propagated_class, "ThirdBodyPerturbationType")

            .value("No", Propagated::ThirdBodyPerturbationType::None)
            .value("Luni", Propagated::ThirdBodyPerturbationType::Luni)
            .value("Solar", Propagated::ThirdBodyPerturbationType::Solar)
            .value("Luni_solar", Propagated::ThirdBodyPerturbationType::LuniSolar)

        ;

        enum_<Propagated::IntegrationStepperType>(propagated_class, "IntegrationStepperType")

            .value("Runge_Kutta_Cash_Karp_54", Propagated::IntegrationStepperType::RungeKuttaCashKarp54)

        ;

        enum_<Propagated::IntegrationLogType>(propagated_class, "IntegrationLogType")

            .value("No_log", Propagated::IntegrationLogType::NoLog)
            .value("Log_constant", Propagated::IntegrationLogType::LogConstant)
            .value("Log_adaptive", Propagated::IntegrationLogType::LogAdaptive)

        ;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
