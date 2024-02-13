/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/SGP4/TLE.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_SGP4(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::astrodynamics::trajectory::orbit::model::SGP4;
    using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;

    {
        class_<SGP4, ostk::astrodynamics::trajectory::orbit::Model>(
            aModule,
            "SGP4",
            R"doc(
                A SGP4 model.

                Provides the interface for orbit models.

            )doc"
        )

            .def(
                init<TLE>(),
                R"doc(
                    Constructor.

                    Args:
                        tle (TLE): The TLE.

                )doc",
                arg("tle")
            )

            .def(
                "is_defined",
                &SGP4::isDefined,
                R"doc(
                    Check if the `SGP4` model is defined.

                    Returns:
                        bool: True if the `SGP4` model is defined, False otherwise.

                )doc"
            )

            .def(
                "get_tle",
                &SGP4::getTle,
                R"doc(
                    Get the TLE of the `SGP4` model.

                    Returns:
                        TLE: The TLE.

                )doc"
            )

            .def(
                "get_epoch",
                &SGP4::getEpoch,
                R"doc(
                    Get the epoch of the `SGP4` model.

                    Returns:
                        Instant: The epoch.

                )doc"
            )

            .def(
                "get_revolution_number_at_epoch",
                &SGP4::getRevolutionNumberAtEpoch,
                R"doc(
                    Get the revolution number at the epoch of the `SGP4` model.

                    Returns:
                        int: The revolution number.

                )doc"
            )

            .def(
                "calculate_state_at",
                &SGP4::calculateStateAt,
                arg("instant"),
                R"doc(
                    Calculate the state of the `SGP4` model at a given instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        State: The state.

                )doc"
            )

            .def(
                "calculate_revolution_number_at",
                &SGP4::calculateRevolutionNumberAt,
                arg("instant"),
                R"doc(
                    Calculate the revolution number of the `SGP4` model at a given instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        int: The revolution number.

                )doc"
            )

            ;
    }

    // Create "sgp4" python submodule
    auto sgp4 = aModule.def_submodule("sgp4");

    // Add objects to "sgp4" python submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_SGP4_TLE(sgp4);
}
