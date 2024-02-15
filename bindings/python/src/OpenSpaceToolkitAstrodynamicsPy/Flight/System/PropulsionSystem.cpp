/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Unit.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_PropulsionSystem(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::type::Real;

    using ostk::physics::unit::Mass;

    using ostk::astrodynamics::flight::system::PropulsionSystem;

    {
        class_<PropulsionSystem>(
            aModule,
            "PropulsionSystem",
            R"doc(
                A propulsion system.

            )doc"
        )

            .def(
                init<const Real&, const Real&>(),
                R"doc(
                    Construct a propulsion system.

                    Args:
                        thrust (float): Thrust in Newton.
                        specific_impulse (float): Specific impulse in Seconds.
                )doc",
                arg("thrust_si_unit"),
                arg("specific_impulse_si_unit")
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<PropulsionSystem>))
            .def("__repr__", &(shiftToString<PropulsionSystem>))

            .def(
                "is_defined",
                &PropulsionSystem::isDefined,
                R"doc(
                    Check if the propulsion system is defined.

                    Returns:
                        bool: True if the propulsion system is defined, False otherwise.
                )doc"
            )

            .def(
                "get_thrust",
                &PropulsionSystem::getThrust,
                R"doc(
                    Return the thrust.

                    Returns:
                        float: The thrust in Newton.
                )doc"
            )
            .def(
                "get_specific_impulse",
                &PropulsionSystem::getSpecificImpulse,
                R"doc(
                    Return the specific impulse.

                    Returns:
                        float: The specific impulse in Seconds.
                )doc"
            )
            .def(
                "get_mass_flow_rate",
                &PropulsionSystem::getMassFlowRate,
                R"doc(
                    Return the mass flow rate.

                    Returns:
                        float: The mass flow rate in Kilograms per Second.
                )doc"
            )
            .def(
                "get_acceleration",
                &PropulsionSystem::getAcceleration,
                arg("mass"),
                R"doc(
                    Return the acceleration.

                    Args:
                        mass (float): Mass in Kilograms.

                    Returns:
                        float: The acceleration in Meters per Second squared.
                )doc"
            )

            .def_static(
                "undefined",
                &PropulsionSystem::Undefined,
                R"doc(
                    Return an undefined propulsion system.

                    Returns:
                        PropulsionSystem: An undefined propulsion system.
                )doc"
            )
            .def_static(
                "default",
                &PropulsionSystem::Default,
                R"doc(
                    Return a default propulsion system.

                    Returns:
                        PropulsionSystem: A default propulsion system.
                )doc"

            )

            ;
    }
}
