/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_StateBuilder(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Shared;
    using ostk::core::ctnr::Array;

    using ostk::physics::coord::Frame;

    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::StateBuilder;
    using ostk::astro::trajectory::state::CoordinatesBroker;
    using ostk::astro::trajectory::state::CoordinatesSubset;

    class_<StateBuilder>(aModule, "StateBuilder")

        .def(
            init<const Shared<const Frame>&, const Array<Shared<const CoordinatesSubset>>&>(),
            arg("frame"),
            arg("coordinates_subsets")
        )
        .def(
            init<const Shared<const Frame>&, const Shared<const CoordinatesBroker>&>(),
            arg("frame"),
            arg("coordinates_broker")
        )
        .def(init<const State&>(), arg("state"))

        .def(self == self)
        .def(self != self)
        .def(
            "__add__",
            [](const StateBuilder& aStateBuilder, const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr)
            {
                return aStateBuilder + aCoordinatesSubsetSPtr;
            },
            is_operator()
        )
        .def(
            "__sub__",
            [](const StateBuilder& aStateBuilder, const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr)
            {
                return aStateBuilder - aCoordinatesSubsetSPtr;
            },
            is_operator()
        )

        .def("__str__", &(shiftToString<StateBuilder>))
        .def("__repr__", &(shiftToString<StateBuilder>))

        .def("is_defined", &StateBuilder::isDefined)

        .def("build", &StateBuilder::build, arg("instant"), arg("coordinates"))
        .def("reduce", &StateBuilder::reduce, arg("state"))
        .def("expand", &StateBuilder::expand, arg("state"), arg("default_state"))

        .def("get_coordinates_subsets", &StateBuilder::getCoordinatesSubsets)
        .def("get_frame", &StateBuilder::getFrame)

        .def_static("undefined", &StateBuilder::Undefined)

        ;
}
