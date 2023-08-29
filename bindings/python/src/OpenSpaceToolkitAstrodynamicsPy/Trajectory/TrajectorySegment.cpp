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
        class_<TrajectorySegment> trajectorySegment(aModule, "TrajectorySegment");

        trajectorySegment

            .def("__str__", &(shiftToString<TrajectorySegment>))
            .def("__repr__", &(shiftToString<TrajectorySegment>))

            .def("get_name", &TrajectorySegment::getName)
            .def("get_event_condition", &TrajectorySegment::getEventCondition)
            .def("get_dynamics", &TrajectorySegment::getDynamics)
            .def("get_numerical_solver", &TrajectorySegment::getNumericalSolver)
            .def("get_type", &TrajectorySegment::getType)

            .def(
                "solve",
                &TrajectorySegment::solve,
                arg("state"),
                arg("maximum_propagation_duration") = Duration::Days(30.0)
            )

            .def_static(
                "coast",
                &TrajectorySegment::Coast,
                arg("name"),
                arg("event_condition"),
                arg("dynamics"),
                arg("numerical_solver")
            )

            .def_static(
                "maneuver",
                &TrajectorySegment::Maneuver,
                arg("name"),
                arg("event_condition"),
                arg("thruster_dynamics"),
                arg("dynamics"),
                arg("numerical_solver")
            )

            ;

        enum_<TrajectorySegment::Type>(trajectorySegment, "Type")

            .value("Coast", TrajectorySegment::Type::Coast)
            .value("Maneuver", TrajectorySegment::Type::Maneuver)

            ;
    }
}
