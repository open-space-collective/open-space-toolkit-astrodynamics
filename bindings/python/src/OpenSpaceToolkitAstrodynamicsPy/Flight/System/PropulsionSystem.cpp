/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Unit.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_System_PropulsionSystem(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::types::Real;

    using ostk::physics::Unit;
    using ostk::physics::units::Mass;
    using ostk::physics::data::Scalar;

    using ostk::astro::flight::system::PropulsionSystem;

    {
        class_<PropulsionSystem>(aModule, "PropulsionSystem")

            // .def(
            //     init([](const Real& thrust, const Unit& thrustUnit, const Real& specificImpulse, const Unit&
            //     specificImpulseUnit)
            //     {
            //         return PropulsionSystem(Scalar(thrust, thrustUnit), Scalar(specificImpulse,
            //         specificImpulseUnit));
            //     }
            //     ),
            //     arg("thrust"),
            //     arg("thrust_unit"),
            //     arg("specific_impulse"),
            //     arg("specific_unit")
            // )

            .def(init<const Real&, const Real&>(), arg("thrust_si_unit"), arg("specific_impulse_si_unit"))

            .def(self == self)
            .def(self != self)

            // .def("__str__", &(shiftToString<PropulsionSystem>))
            // .def("__repr__", &(shiftToString<PropulsionSystem>))

            .def("is_defined", &PropulsionSystem::isDefined)

            // .def("get_thrust", &PropulsionSystem::getThrust)
            // .def("get_specific_impulse", &PropulsionSystem::getSpecificImpulse)
            // .def("get_mass_flow_rate", &PropulsionSystem::getMassFlowRate) Scalar output
            // .def("get_acceleration", &PropulsionSystem::getAcceleration, arg("mass"))

            .def_static("undefined", &PropulsionSystem::Undefined)
            .def_static("default", &PropulsionSystem::Default)

            ;
    }
}
