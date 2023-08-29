/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/TrajectorySegment.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Segment(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;
    using ostk::core::types::String;
    using ostk::core::types::Shared;

    using ostk::astro::NumericalSolver;
    using ostk::astro::trajectory::TrajectorySegment;
    using ostk::astro::flight::system::Dynamics;

    class_<TrajectorySegment::Solution>(aModule, "SegmentSolution")

        .def_readonly("name", &TrajectorySegment::Solution::name)
        .def_readonly("dynamics", &TrajectorySegment::Solution::dynamics)
        .def_readonly("states", &TrajectorySegment::Solution::states)

        ;

    {
        class_<TrajectorySegment>(aModule, "TrajectorySegment")

            .def("__str__", &(shiftToString<TrajectorySegment>))
            .def("__repr__", &(shiftToString<TrajectorySegment>))

            .def("get_name", &TrajectorySegment::getName)
            .def("get_event_condition", &TrajectorySegment::getEventCondition)
            .def("get_dynamics", &TrajectorySegment::getDynamics)
            .def("get_numerical_solver", &TrajectorySegment::getNumericalSolver)

            .def(
                "solve",
                &TrajectorySegment::solve,
                arg("state"),
                arg("dynamics") = Array<Shared<Dynamics>>::Empty(),
                arg("numerical_solver") = NumericalSolver::Undefined(),
                arg("maximum_propagation_duration") = Duration::Days(30.0)
            )

            .def_static(
                "coast",
                &TrajectorySegment::Coast,
                arg("event_condition"),
                arg("dynamics") = Array<Shared<Dynamics>>::Empty(),
                arg("numerical_solver") = NumericalSolver::Undefined()
            )

            .def_static(
                "maneuver",
                &TrajectorySegment::Maneuver,
                arg("event_condition"),
                arg("thruster_dynamics"),
                arg("dynamics") = Array<Shared<Dynamics>>::Empty(),
                arg("numerical_solver") = NumericalSolver::Undefined()
            )

            ;
    }
}
