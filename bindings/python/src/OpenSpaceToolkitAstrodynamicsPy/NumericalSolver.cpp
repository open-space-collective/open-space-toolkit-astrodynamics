////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           bindings/python/src/OpenSpaceToolkitAstrodynamicsPy/NumericalSolver.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

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

    typedef std::function<NumericalSolver::StateVector(const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double t)> pythonSystemOfEquationsSignature ;

    {

        class_<NumericalSolver> numerical_solver_class(aModule, "NumericalSolver") ;

        numerical_solver_class
            .def(init<const   NumericalSolver::LogType&, const   NumericalSolver::StepperType&, const   Real&, const   Real&, const   Real&>())
            .def(init<const   NumericalSolver&>())

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<NumericalSolver>))
            .def("__repr__", &(shiftToString<NumericalSolver>))

            .def("is_defined", &NumericalSolver::isDefined)

            .def("get_stepper_type", &NumericalSolver::getStepperType)
            .def("get_log_type", &NumericalSolver::getLogType)
            .def("get_time_step", &NumericalSolver::getTimeStep)
            .def("get_relative_tolerance", &NumericalSolver::getRelativeTolerance)
            .def("get_absolute_tolerance", &NumericalSolver::getAbsoluteTolerance)

            .def("integrate_state_for_duration", +[](NumericalSolver& aNumericalSolver, const object& stateVectorObject, const object& durationObject, const object &aSystemOfEquationsObject)
                                                    {
                                                        const NumericalSolver::StateVector stateVector = pybind11::cast<NumericalSolver::StateVector>(stateVectorObject) ;
                                                        const Duration duration = pybind11::cast<Duration>(durationObject) ;

                                                        const auto pythonDynamicsEquation = pybind11::cast<pythonSystemOfEquationsSignature>(aSystemOfEquationsObject) ;

                                                        const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations = [&pythonDynamicsEquation, &stateVector]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double t) -> void
                                                            {
                                                                dxdt = pythonDynamicsEquation(x, dxdt, t) ;
                                                            } ;

                                                        return aNumericalSolver.integrateStateForDuration(stateVector, duration, aSystemOfEquations) ;
                                                    }
                                                )

            .def("integrate_state_from_instant_to_instant", +[](NumericalSolver& aNumericalSolver, const object& stateVectorObject, const object& startInstantObject, const object& endInstantObject, const object &aSystemOfEquationsObject)
                                                    {
                                                        const NumericalSolver::StateVector stateVector = pybind11::cast<NumericalSolver::StateVector>(stateVectorObject) ;
                                                        const Instant startInstant = pybind11::cast<Instant>(startInstantObject) ;
                                                        const Instant endInstant = pybind11::cast<Instant>(endInstantObject) ;

                                                        const auto pythonDynamicsEquation = pybind11::cast<pythonSystemOfEquationsSignature>(aSystemOfEquationsObject) ;

                                                        const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations = [&pythonDynamicsEquation, &stateVector]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double t) -> void
                                                            {
                                                                dxdt = pythonDynamicsEquation(x, dxdt, t) ;
                                                            } ;

                                                        return aNumericalSolver.integrateStateFromInstantToInstant(stateVector, startInstant, endInstant, aSystemOfEquations) ;
                                                    }
                                                )

            .def_static("string_from_stepper_type", &NumericalSolver::StringFromStepperType)
            .def_static("string_from_log_type", &NumericalSolver::StringFromLogType)

        ;

        enum_<NumericalSolver::StepperType>(numerical_solver_class, "StepperType")

            .value("RungeKuttaCashKarp54", NumericalSolver::StepperType::RungeKuttaCashKarp54)
            .value("RungeKuttaFehlberg78", NumericalSolver::StepperType::RungeKuttaFehlberg78)

        ;

        enum_<NumericalSolver::LogType>(numerical_solver_class, "LogType")

            .value("NoLog", NumericalSolver::LogType::NoLog)
            .value("LogConstant", NumericalSolver::LogType::LogConstant)
            .value("LogAdaptive", NumericalSolver::LogType::LogAdaptive)

        ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
