/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Segment(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;
    using ostk::core::types::String;
    using ostk::core::types::Shared;

    using ostk::physics::time::Duration;

    using ostk::astro::trajectory::state::NumericalSolver;
    using ostk::astro::trajectory::Segment;
    using ostk::astro::Dynamics;

    class_<Segment::Solution>(aModule, "SegmentSolution")

        .def_readonly("name", &Segment::Solution::name)
        .def_readonly("dynamics", &Segment::Solution::dynamics)
        .def_readonly("states", &Segment::Solution::states)

        ;

    {
        class_<Segment> segment(aModule, "Segment");

        segment

            .def("__str__", &(shiftToString<Segment>))
            .def("__repr__", &(shiftToString<Segment>))

            .def("get_name", &Segment::getName)
            .def("get_event_condition", &Segment::getEventCondition)
            .def("get_dynamics", &Segment::getDynamics)
            .def("get_numerical_solver", &Segment::getNumericalSolver)
            .def("get_type", &Segment::getType)

            .def("solve", &Segment::solve, arg("state"), arg("maximum_propagation_duration") = Duration::Days(30.0))

            .def_static(
                "coast", &Segment::Coast, arg("name"), arg("event_condition"), arg("dynamics"), arg("numerical_solver")
            )

            .def_static(
                "maneuver",
                &Segment::Maneuver,
                arg("name"),
                arg("event_condition"),
                arg("thruster_dynamics"),
                arg("dynamics"),
                arg("numerical_solver")
            )

            ;

        enum_<Segment::Type>(segment, "Type")

            .value("Coast", Segment::Type::Coast)
            .value("Maneuver", Segment::Type::Maneuver)

            ;
    }
}
