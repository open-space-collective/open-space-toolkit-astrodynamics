/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>

#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/Orbit/Model/SGP4/TLE.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_SGP4(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Array;
    using ostk::core::container::Pair;
    using ostk::core::type::Shared;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;

    using ostk::astrodynamics::trajectory::orbit::model::SGP4;
    using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
    using ostk::astrodynamics::trajectory::State;

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
                init<TLE, Shared<const Frame>>(),
                R"doc(
                    Constructor with output frame. If TEME, the runtime is faster as no frame
                    transformations are needed. In other frames, the runtime will be slower as frame transformations are needed.

                    Args:
                        tle (TLE): The TLE.
                        output_frame (Frame): The output frame for state calculations.

                )doc",
                arg("tle"),
                arg("output_frame")
            )

            .def(
                init<Array<TLE>, Shared<const Frame>>(),
                R"doc(
                    Constructor with output frame. If TEME, the runtime is faster as no frame
                    transformations are needed. In other frames, the runtime will be slower as frame transformations are needed.

                    Args:
                        tles (list[TLE]): The array of TLEs.
                        output_frame (Frame): The output frame for state calculations. Defaults to TEME.
                )doc",
                arg("tles"),
                arg_v("output_frame", Frame::TEME(), "Frame.TEME()")
            )

            .def(
                init<Array<Pair<TLE, Interval>>, Shared<const Frame>>(),
                R"doc(
                    Constructor with output frame. If TEME, the runtime is faster as no frame
                    transformations are needed. In other frames, the runtime will be slower as frame transformations are needed.

                    Args:
                        tles_with_intervals (list[tuple[TLE, Interval]]): The array of TLE-Interval pairs.
                        output_frame (Frame): The output frame for state calculations. Defaults to TEME.
                )doc",
                arg("tles_with_intervals"),
                arg_v("output_frame", Frame::TEME(), "Frame.TEME()")
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
                    Get the TLE of the `SGP4` model. For multi-TLE models, returns the TLE with the earliest epoch.

                    Returns:
                        TLE: The TLE.

                )doc"
            )

            .def(
                "get_tles",
                [](const SGP4& self) -> list
                {
                    list result;
                    for (const auto& tle : self.getTles())
                    {
                        result.append(tle);
                    }
                    return result;
                },
                R"doc(
                    Get the array of TLEs of the `SGP4` model.

                    Returns:
                        list[TLE]: The array of TLEs.

                )doc"
            )

            .def(
                "get_validity_intervals",
                [](const SGP4& self) -> list
                {
                    list result;
                    for (const auto& interval : self.getValidityIntervals())
                    {
                        result.append(interval);
                    }
                    return result;
                },
                R"doc(
                    Get the validity intervals of the `SGP4` model.

                    Returns:
                        list[Interval]: The validity intervals.

                )doc"
            )

            .def(
                "get_output_frame",
                &SGP4::getOutputFrame,
                R"doc(
                    Get the output frame of the `SGP4` model.

                    Returns:
                        Frame: The output frame.

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

                    When multiple TLEs are provided, uses the TLE whose epoch is closest to the instant.

                    Args:
                        instant (Instant): The instant.

                    Returns:
                        State: The state.

                )doc"
            )

            .def(
                "calculate_states_at",
                [](const SGP4& self, const list& anInstantList) -> list
                {
                    Array<Instant> instantArray = Array<Instant>::Empty();
                    for (auto item : anInstantList)
                    {
                        instantArray.add(item.cast<Instant>());
                    }
                    const Array<State> states = self.calculateStatesAt(instantArray);
                    list result;
                    for (const auto& state : states)
                    {
                        result.append(state);
                    }
                    return result;
                },
                arg("instant_array"),
                R"doc(
                    Calculate the states of the `SGP4` model at given instants.

                    When multiple TLEs are provided, uses the TLE whose epoch is closest to each instant.

                    Args:
                        instant_array (list[Instant]): The instants.

                    Returns:
                        list[State]: The states.

                )doc"
            )

            ;
    }

    // Create "sgp4" python submodule
    auto sgp4 = aModule.def_submodule("sgp4");

    // Add objects to "sgp4" python submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_Orbit_Model_SGP4_TLE(sgp4);
}
