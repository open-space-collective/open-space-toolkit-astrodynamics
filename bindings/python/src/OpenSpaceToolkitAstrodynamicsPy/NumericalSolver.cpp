/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_NumericalSolver(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    typedef std::function<NumericalSolver::StateVector(
        const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const double t
    )>
        pythonSystemOfEquationsSignature;

    {
        class_<NumericalSolver> numericalSolver(aModule, "NumericalSolver");

        numericalSolver

            .def(
                init<
                    const NumericalSolver::LogType&,
                    const NumericalSolver::StepperType&,
                    const Real&,
                    const Real&,
                    const Real&>(),
                arg("log_type"),
                arg("stepper_type"),
                arg("time_step"),
                arg("relative_tolerance"),
                arg("absolute_tolerance")
            )
            .def(init<const NumericalSolver&>())

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

            .def(
                "integrate_state_for_duration",
                +[](NumericalSolver& aNumericalSolver,
                    const NumericalSolver::StateVector& aStateVector,
                    const Duration& aDuration,
                    const object& aSystemOfEquationsObject)
                {
                    const auto pythonDynamicsEquation =
                        pybind11::cast<pythonSystemOfEquationsSignature>(aSystemOfEquationsObject);

                    const NumericalSolver::SystemOfEquationsWrapper& systemOfEquations =
                        [&](const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const double t
                        ) -> void
                    {
                        dxdt = pythonDynamicsEquation(x, dxdt, t);
                    };

                    return aNumericalSolver.integrateStateForDuration(aStateVector, aDuration, systemOfEquations);
                }
            )

            .def(
                "integrate_state_from_instant_to_instant",
                +[](NumericalSolver& aNumericalSolver,
                    const NumericalSolver::StateVector& aStateVector,
                    const Instant& aStartInstant,
                    const Instant& anEndInstant,
                    const object& aSystemOfEquationsObject)
                {
                    const auto pythonDynamicsEquation =
                        pybind11::cast<pythonSystemOfEquationsSignature>(aSystemOfEquationsObject);

                    const NumericalSolver::SystemOfEquationsWrapper& systemOfEquations =
                        [&](const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const double t
                        ) -> void
                    {
                        dxdt = pythonDynamicsEquation(x, dxdt, t);
                    };

                    return aNumericalSolver.integrateStateFromInstantToInstant(
                        aStateVector, aStartInstant, anEndInstant, systemOfEquations
                    );
                }
            )

            .def_static("string_from_stepper_type", &NumericalSolver::StringFromStepperType, arg("stepper_type"))
            .def_static("string_from_log_type", &NumericalSolver::StringFromLogType, arg("log_type"))
            .def_static("default", &NumericalSolver::Default)

            ;

        enum_<NumericalSolver::StepperType>(numericalSolver, "StepperType")

            .value("RungeKutta4", NumericalSolver::StepperType::RungeKutta4)
            .value("RungeKuttaCashKarp54", NumericalSolver::StepperType::RungeKuttaCashKarp54)
            .value("RungeKuttaFehlberg78", NumericalSolver::StepperType::RungeKuttaFehlberg78)

            ;

        enum_<NumericalSolver::LogType>(numericalSolver, "LogType")

            .value("NoLog", NumericalSolver::LogType::NoLog)
            .value("LogConstant", NumericalSolver::LogType::LogConstant)
            .value("LogAdaptive", NumericalSolver::LogType::LogAdaptive)

            ;
    }
}
