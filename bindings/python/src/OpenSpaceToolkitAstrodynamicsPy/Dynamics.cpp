/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics/AtmosphericDrag.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics/CentralBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics/PositionDerivative.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics/Tabulated.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics/ThirdBodyGravity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Dynamics/Thruster.cpp>

using namespace pybind11;

using ostk::core::types::Shared;

using ostk::math::object::VectorXd;
using ostk::math::object::Vector3d;

using ostk::physics::Environment;
using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::Dynamics;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;

// Trampoline class for virtual member functions
class PyDynamics : public Dynamics
{
   public:
    using Dynamics::Dynamics;

    // Trampoline (need one for each virtual function)

    void print(std::ostream& anOutputStream, bool displayDecorator) const override
    {
        PYBIND11_OVERRIDE(void, Dynamics, print, anOutputStream, displayDecorator);
    }

    bool isDefined() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(bool, Dynamics, "is_defined", isDefined);
    }

    Array<Shared<const CoordinatesSubset>> getReadCoordinatesSubsets() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<Shared<const CoordinatesSubset>>, Dynamics, "get_read_coordinates_subsets", getReadCoordinatesSubsets
        );
    }

    Array<Shared<const CoordinatesSubset>> getWriteCoordinatesSubsets() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<Shared<const CoordinatesSubset>>,
            Dynamics,
            "get_write_coordinates_subsets",
            getWriteCoordinatesSubsets
        );
    }

    VectorXd computeContribution(const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr)
        const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            VectorXd, Dynamics, "compute_contribution", computeContribution, anInstant, x, aFrameSPtr
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics(pybind11::module& aModule)
{
    class_<Dynamics, PyDynamics, Shared<Dynamics>>(
        aModule,
        "Dynamics",
        R"doc(
            Abstract interface class for dynamics.
            
            Can inherit and provide the virtual methods:
                - is_defined
                - get_read_coordinates_subsets
                - get_write_coordinates_subsets
                - compute_contribution
            to create a custom dynamics class

            Group:
                dynamics
        )doc"
    )

        .def(
            init<const String&>(),
            arg("name"),
            R"doc(
                Construct a new `Dynamics` object.

                Args:
                    name (str): The name of the dynamics.

                Returns:
                    dynamics (Dynamics): The new `Dynamics` object.
            )doc"
        )

        .def(
            "get_name",
            &Dynamics::getName,
            R"doc(
                Get the name of the dynamics.

                Returns:
                    name (str): The name of the dynamics.
            )doc"
        )

        .def("__str__", &(shiftToString<Dynamics>))
        .def("__repr__", &(shiftToString<Dynamics>))

        .def(
            "is_defined",
            &Dynamics::isDefined,
            R"doc(
                Check if the dynamics is defined.

                Returns:
                    is_defined (bool): True if the dynamics is defined, False otherwise.
            )doc"
        )

        .def(
            "get_read_coordinates_subsets",
            &Dynamics::getReadCoordinatesSubsets,
            R"doc(
                Get the coordinates subsets that the dynamics reads.

                Returns:
                    read_coordinates_subsets (Array<CoordinatesSubset>): The coordinates subsets that the dynamics reads.
            )doc"
        )

        .def(
            "get_write_coordinates_subsets",
            &Dynamics::getWriteCoordinatesSubsets,
            R"doc(
                Get the coordinates subsets that the dynamics writes.

                Returns:
                    write_coordinates_subsets (Array<CoordinatesSubset>): The coordinates subsets that the dynamics writes.
            )doc"
        )

        .def(
            "compute_contribution",
            &Dynamics::computeContribution,
            arg("instant"),
            arg("state_vector"),
            arg("frame"),
            R"doc(
                Compute the contribution of the dynamics at a given instant.

                Args:
                    instant (Instant): The instant at which to compute the contribution.
                    state_vector (numpy.ndarray): The state vector at the instant.
                    frame (Frame): The reference frame in which to compute the contribution.

                Returns:
                    contribution (numpy.ndarray): The contribution of the dynamics at the instant.
            )doc"
        )

        .def_static(
            "from_environment",
            &Dynamics::FromEnvironment,
            arg("environment"),
            R"doc(
                Create a `Dynamics` object from an environment.

                Args:
                    environment (Environment): The environment to create the dynamics from.

                Returns:
                    dynamics (Dynamics): The `Dynamics` object created from the environment.
            )doc"
        )

        ;

    // Create "dynamics" python submodule
    auto dynamics = aModule.def_submodule("dynamics");

    // Add objects to "dynamics" submodule
    OpenSpaceToolkitAstrodynamicsPy_Dynamics_PositionDerivative(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Dynamics_CentralBodyGravity(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Dynamics_ThirdBodyGravity(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Dynamics_AtmosphericDrag(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Dynamics_Thruster(dynamics);
    OpenSpaceToolkitAstrodynamicsPy_Dynamics_Tabulated(dynamics);
}
