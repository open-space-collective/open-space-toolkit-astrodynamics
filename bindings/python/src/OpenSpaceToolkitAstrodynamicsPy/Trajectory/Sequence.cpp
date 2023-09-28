/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Sequence(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;
    using ostk::core::types::String;
    using ostk::core::types::Shared;
    using ostk::core::types::Size;

    using ostk::physics::time::Duration;

    using ostk::astro::trajectory::state::NumericalSolver;
    using ostk::astro::trajectory::Sequence;
    using ostk::astro::trajectory::Segment;
    using ostk::astro::Dynamics;
    using ostk::astro::flight::system::SatelliteSystem;

    class_<Sequence::Solution>(aModule, "SequenceSolution")

        .def_readonly("segment_solutions", &Sequence::Solution::segmentSolutions)
        .def("get_states", &Sequence::Solution::getStates)

        ;

    {
        class_<Sequence>(aModule, "Sequence")

            .def(
                init<
                    const Array<Segment>&,
                    const Size&,
                    const NumericalSolver&,
                    const Array<Shared<Dynamics>>&,
                    const Duration&>(),
                arg("segments") = Array<Segment>::Empty(),
                arg("repetition_count") = 1,
                arg("numerical_solver") = NumericalSolver::DefaultConditional(),
                arg("dynamics") = Array<Shared<Dynamics>>::Empty(),
                arg("maximum_propagation_duration") = Duration::Days(30.0)
            )

            .def("__str__", &(shiftToString<Sequence>))
            .def("__repr__", &(shiftToString<Sequence>))

            .def("get_segments", &Sequence::getSegments)
            .def("get_numerical_solver", &Sequence::getNumericalSolver)
            .def("get_dynamics", &Sequence::getDynamics)
            .def("get_maximum_propagation_duration", &Sequence::getMaximumPropagationDuration)

            .def("add_segment", overload_cast<const Segment&>(&Sequence::addSegment), arg("segment"))
            .def("add_segment", overload_cast<const Array<Segment>&>(&Sequence::addSegment), arg("segments"))
            .def("add_coast_segment", &Sequence::addCoastSegment, arg("event_condition"))
            .def(
                "add_maneuver_segment", &Sequence::addManeuverSegment, arg("event_condition"), arg("thruster_dynamics")
            )

            .def("solve", &Sequence::solve, arg("state"))

            ;
    }
}
