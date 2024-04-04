/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>

using namespace pybind11;

using ostk::core::type::String;
using ostk::core::type::Shared;
using ostk::core::container::Array;

using ostk::mathematics::object::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;

// Trampoline class for virtual member functions
class PyThruster : public Thruster
{
   public:
    using Thruster::Thruster;

    // Trampoline (need one for each virtual function)

    void print(std::ostream& anOutputStream, bool displayDecorator) const override
    {
        PYBIND11_OVERRIDE(void, Thruster, print, anOutputStream, displayDecorator);
    }

    bool isDefined() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(bool, Thruster, "is_defined", isDefined);
    }

    Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<Shared<const CoordinateSubset>>, Thruster, "get_read_coordinate_subsets", getReadCoordinateSubsets
        );
    }

    Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            Array<Shared<const CoordinateSubset>>, Thruster, "get_write_coordinate_subsets", getWriteCoordinateSubsets
        );
    }

    VectorXd computeContribution(const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr)
        const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            VectorXd, Thruster, "compute_contribution", computeContribution, anInstant, x, aFrameSPtr
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Dynamics_Thruster(pybind11::module& aModule)
{
    class_<Thruster, PyThruster, Dynamics, Shared<Thruster>>(
        aModule,
        "Thruster",
        R"doc(
            Abstract Thruster Class.

            Base class to derive other thruster classes from. Cannot be instantiated.

        )doc"
    )

        .def(
            init<const SatelliteSystem&, const Shared<const GuidanceLaw>&, const String&>(),
            arg("satellite_system"),
            arg("guidance_law"),
            arg("name") = String::Empty(),
            R"doc(
                Constructor.

                Args:
                    satellite_system (SatelliteSystem): The satellite system.
                    guidance_law (GuidanceLaw): The guidance law used to compute the acceleration vector.
                    name (str): The name of the thruster.

            )doc"
        )

        .def(
            "get_satellite_system",
            &Thruster::getSatelliteSystem,
            R"doc(
                Get the satellite system of the thruster.

                Returns:
                    SatelliteSystem: The satellite system.

            )doc"
        )
        .def(
            "get_guidance_law",
            &Thruster::getGuidanceLaw,
            R"doc(
                Get the guidance law of the thruster.

                Returns:
                    GuidanceLaw: The guidance law.

            )doc"
        )

        .def("__str__", &(shiftToString<Thruster>))
        .def("__repr__", &(shiftToString<Thruster>))

        .def(
            "is_defined",
            &Thruster::isDefined,
            R"doc(
                Check if the thruster is defined.

                Returns:
                    bool: True if the thruster is defined, False otherwise.

            )doc"
        )

        .def(
            "compute_contribution",
            &Thruster::computeContribution,
            arg("instant"),
            arg("state_vector"),
            arg("frame"),
            R"doc(
                Compute the contribution of the thruster to the state vector.

                Args:
                    instant (Instant): The instant of the state vector.
                    state_vector (numpy.ndarray): The state vector.
                    frame (Frame): The reference frame.

                Returns:
                    numpy.ndarray: The contribution of the thruster to the state vector.

            )doc"
        )

        ;
}
