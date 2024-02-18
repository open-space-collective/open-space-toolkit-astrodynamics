/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_Propagated(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Shared;
    using ostk::core::type::Integer;
    using ostk::core::container::Array;

    using ostk::physics::time::Instant;

    using ostk::astrodynamics::trajectory::State;
    using ostk::astrodynamics::trajectory::Propagator;
    using ostk::astrodynamics::trajectory::orbit::model::Propagated;

    {
        class_<Propagated, ostk::astrodynamics::trajectory::orbit::Model> propagated_class(
            aModule,
            "Propagated",
            R"doc(
                A Propagated orbit model.

                Provides the interface for orbit models.

            )doc"
        );

        propagated_class

            .def(
                init<const Propagator&, const State&, const Integer&>(),
                R"doc(
                    Constructor.

                    Args:
                        propagator (Propagator): The propagator.
                        state (State): The initial state.
                        initial_revolution_number (int, optional): The initial revolution number. Defaults to 1.

                )doc",
                arg("propagator"),
                arg("state"),
                arg("initial_revolution_number") = 1
            )

            .def(
                init<const Propagator&, const Array<State>&, const Integer&>(),
                R"doc(
                    Constructor.

                    Args:
                        propagator (Propagator): The propagator.
                        state_array (list[State]): The initial state array.
                        initial_revolution_number (int, optional): The initial revolution number. Defaults to 1.

                )doc",
                arg("propagator"),
                arg("state_array"),
                arg("initial_revolution_number") = 1
            )

            .def(self == self)

            .def(self != self)

            .def("__str__", &(shiftToString<Propagated>))

            .def("__repr__", &(shiftToString<Propagated>))

            .def(
                "is_defined",
                &Propagated::isDefined,
                R"doc(
                    Check if the `Propagated` model is defined.

                    Returns:
                        bool: True if the `Propagated` model is defined, False otherwise.

                )doc"
            )

            .def(
                "get_epoch",
                &Propagated::getEpoch,
                R"doc(
                    Get the epoch of the `Propagated` model.

                    Returns:
                        Instant: The epoch.

                )doc"
            )

            .def(
                "get_revolution_number_at_epoch",
                &Propagated::getRevolutionNumberAtEpoch,
                R"doc(
                    Get the revolution number at the epoch of the `Propagated` model.

                    Returns:
                        int: The revolution number.

                )doc"
            )

            .def(
                "calculate_state_at",
                &Propagated::calculateStateAt,
                arg("instant"),
                R"doc(
                    Calculate the state of the `Propagated` model at a given instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        State: The state.

                )doc"
            )

            .def(
                "calculate_states_at",
                &Propagated::calculateStatesAt,
                arg("instants"),
                R"doc(
                    Calculate the states of the `Propagated` model at given instants.

                    Args:
                        instants (list[Instant]): The instants.

                    Returns:
                        list[State]: The states.

                )doc"
            )

            .def(
                "calculate_revolution_number_at",
                &Propagated::calculateRevolutionNumberAt,
                arg("instant"),
                R"doc(
                    Calculate the revolution number of the `Propagated` model at a given instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        int: The revolution number.

                )doc"
            )

            .def(
                "access_cached_state_array",
                &Propagated::accessCachedStateArray,
                R"doc(
                    Access the cached state array of the `Propagated` model.

                    Returns:
                        list[State]: The cached state array.

                )doc"
            )

            .def(
                "access_propagator",
                &Propagated::accessPropagator,
                R"doc(
                    Access the propagator of the `Propagated` model.

                    Returns:
                        Propagator: The propagator.

                )doc"
            )

            .def(
                "set_cached_state_array",
                &Propagated::setCachedStateArray,
                arg("state_array"),
                R"doc(
                    Set the cached state array of the `Propagated` model.

                    Args:
                        state_array (list[State]): The state array.

                )doc"
            )

            ;
    }
}
