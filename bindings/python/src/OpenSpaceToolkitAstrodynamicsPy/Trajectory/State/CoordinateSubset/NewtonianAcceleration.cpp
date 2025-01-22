/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/NewtonianAcceleration.hpp>

using namespace pybind11;

using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::NewtonianAcceleration;

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_NewtonianAcceleration(
    pybind11::module& aModule
)
{
    class_<NewtonianAcceleration, Shared<NewtonianAcceleration>, CoordinateSubset>(
        aModule,
        "NewtonianAcceleration",
        R"doc(
            Newtonian acceleration coordinate subset.

            Defined with respect to a reference frame.
        )doc"
    )

        .def(
            init<const Shared<const CartesianPosition>, const String&>(),
            R"doc(
                Constructor.

                Args:
                    cartesian_position (CartesianPosition): The Cartesian position.
                    name (str): The name of the subset.

            )doc",
            arg("cartesian_position"),
            arg("name")
        )

        .def(
            "in_frame",
            &NewtonianAcceleration::inFrame,
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinate_broker"),
            R"doc(
                Convert a Newtonian acceleration from one reference frame to another.
                Note: Acceleration is transformed similar to position, as like position it is a bound vector.
                Refer to https://elib.dlr.de/142099/1/MA_SpaceEngineering_Huckfeldt_412335.pdf (Page 30) for more information.

                Args:
                    instant (Instant): The instant of the conversion.
                    coordinates (numpy.ndarray): The Newtonian acceleration to convert.
                    from_frame (str): The reference frame of the input Newtonian acceleration.
                    to_frame (str): The reference frame of the output Newtonian acceleration.
                    coordinate_broker (CoordinateBroker): The coordinate broker.

                Returns:
                    numpy.ndarray: The Newtonian acceleration in the output reference frame.

            )doc"
        )

        .def_static(
            "default",
            &NewtonianAcceleration::Default,
            R"doc(
                Get the default Newtonian acceleration subset.

                Returns:
                    NewtonianAcceleration: The default Newtonian acceleration subset.
            )doc"
        )

        ;
}
