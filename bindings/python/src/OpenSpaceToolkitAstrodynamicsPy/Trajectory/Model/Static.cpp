/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Static.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Model_Static(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::physics::coordinate::Position;

    using ostk::astrodynamics::trajectory::model::Static;

    class_<Static, ostk::astrodynamics::trajectory::Model>(
        aModule,
        "Static",
        R"doc(
            Static trajectory model.

            This model represents a fixed position in a non quasi-inertial frame (e.g. a point on the surface of the Earth).
        )doc"
    )

        .def(
            init<const Position&>(),
            R"doc(
                Construct a `Static` object from a position.

                Args:
                    position (Position): The position. Must be in a non quasi-inertial frame (e.g. ITRF).

                Returns:
                    Static: The `Static` object.
            )doc",
            arg("position")
        )

        .def(self == self)
        .def(self != self)

        .def("__str__", &(shiftToString<Static>))
        .def("__repr__", &(shiftToString<Static>))

        .def(
            "is_defined",
            &Static::isDefined,
            R"doc(
                Check if the model is defined.

                Returns:
                    bool: True if the model is defined, False otherwise.
            )doc"
        )
        .def(
            "calculate_state_at",
            &Static::calculateStateAt,
            R"doc(
                Calculate the state at a given instant, expressed in the frame of the position.

                Args:
                    instant (Instant): The instant.

                Returns:
                    State: The state at the given instant.
            )doc",
            arg("instant")
        )

        ;
}
