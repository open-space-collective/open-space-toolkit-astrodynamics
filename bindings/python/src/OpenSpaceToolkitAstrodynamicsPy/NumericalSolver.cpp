////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/NumericalSolver.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteDynamics.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void                     OpenSpaceToolkitAstrodynamicsPy_NumericalSolver (        pybind11::module& aModule                               )
{

    using namespace pybind11 ;

    using ostk::core::types::Real ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;
    using ostk::astro::flight::system::SatelliteDynamics ;

    typedef std::function<NumericalSolver::StateVector(const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double t)> pythonDynamicsEquationSignature ;

    {

        class_<NumericalSolver> numericalsolver_class(aModule, "NumericalSolver") ;

        numericalsolver_class
            .def(init<const   NumericalSolver::IntegrationLogType&, const   NumericalSolver::IntegrationStepperType&, const   Real&, const   Real&, const   Real&>())
            .def(init<const   NumericalSolver&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<NumericalSolver>))
            .def("__repr__", &(shiftToString<NumericalSolver>))

            .def("is_defined", &NumericalSolver::isDefined)

            .def("get_integration_stepper_type", &NumericalSolver::getIntegrationStepperType)
            .def("get_integration_log_type", &NumericalSolver::getIntegrationLogType)
            .def("get_time_step", &NumericalSolver::getTimeStep)
            .def("get_relative_tolerance", &NumericalSolver::getRelativeTolerance)
            .def("get_absolute_tolerance", &NumericalSolver::getAbsoluteTolerance)

            .def("integrate_state_for_duration", +[](NumericalSolver& aNumericalSolver, const object& stateVectorObject, const object& durationObject, const object &aDynamicsEquationObject)
                                                    {
                                                        const NumericalSolver::StateVector stateVector = pybind11::cast<NumericalSolver::StateVector>(stateVectorObject) ;
                                                        const Duration duration = pybind11::cast<Duration>(durationObject) ;

                                                        const auto pythonDynamicsEquation = pybind11::cast<pythonDynamicsEquationSignature>(aDynamicsEquationObject) ;

                                                        const SatelliteDynamics::DynamicalEquationFuncCallback aDynamicsEquationCallback = [&pythonDynamicsEquation, &stateVector]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double t) -> void
                                                            {
                                                                dxdt = pythonDynamicsEquation(x, dxdt, t) ;
                                                            } ;

                                                        return aNumericalSolver.integrateStateForDuration(stateVector, duration, aDynamicsEquationCallback) ;
                                                    }
                                                )

            .def("integrate_state_from_instant_to_instant", +[](NumericalSolver& aNumericalSolver, const object& stateVectorObject, const object& startInstantObject, const object& endInstantObject, const object &aDynamicsEquationObject)
                                                    {
                                                        const NumericalSolver::StateVector stateVector = pybind11::cast<NumericalSolver::StateVector>(stateVectorObject) ;
                                                        const Instant startInstant = pybind11::cast<Instant>(startInstantObject) ;
                                                        const Instant endInstant = pybind11::cast<Instant>(endInstantObject) ;

                                                        const auto pythonDynamicsEquation = pybind11::cast<pythonDynamicsEquationSignature>(aDynamicsEquationObject) ;

                                                        const SatelliteDynamics::DynamicalEquationFuncCallback aDynamicsEquationCallback = [&pythonDynamicsEquation, &stateVector]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double t) -> void
                                                            {
                                                                dxdt = pythonDynamicsEquation(x, dxdt, t) ;
                                                            } ;

                                                        return aNumericalSolver.integrateStateFromInstantToInstant(stateVector, startInstant, endInstant, aDynamicsEquationCallback) ;
                                                    }
                                                )

            .def_static("string_from_integration_stepper_type", &NumericalSolver::StringFromIntegrationStepperType)
            .def_static("string_from_integration_log_type", &NumericalSolver::StringFromIntegrationLogType)

        ;

        enum_<NumericalSolver::IntegrationStepperType>(numericalsolver_class, "IntegrationStepperType")

            .value("Runge_Kutta_Cash_Karp_54", NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54)
            .value("Runge_Kutta_Fehlberg_78", NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78)

        ;

        enum_<NumericalSolver::IntegrationLogType>(numericalsolver_class, "IntegrationLogType")

            .value("No_log", NumericalSolver::IntegrationLogType::NoLog)
            .value("Log_constant", NumericalSolver::IntegrationLogType::LogConstant)
            .value("Log_adaptive", NumericalSolver::IntegrationLogType::LogAdaptive)

        ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
