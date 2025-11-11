/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/HeterogeneousGuidanceLaw.hpp>

using namespace pybind11;

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::guidancelaw::HeterogeneousGuidanceLaw;

inline void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_HeterogeneousGuidanceLaw(pybind11::module& aModule)
{
    class_<HeterogeneousGuidanceLaw, GuidanceLaw, Shared<HeterogeneousGuidanceLaw>>(
        aModule,
        "HeterogeneousGuidanceLaw",
        R"doc(
            A guidance law that sequences multiple guidance laws over specific time intervals.
            
            At each point in time, the applicable guidance law is selected and used to calculate
            the thrust acceleration. Guidance laws don't need to be contiguous, and can be added
            in any order. If the instant does not fall within any of the intervals, the thrust
            acceleration is zero. The guidance law intervals must not intersect each other.
        )doc"
    )

        .def(
            init<const Array<Pair<Shared<const GuidanceLaw>, Interval>>&>(),
            arg_v("guidance_laws_with_intervals", Array<Pair<Shared<const GuidanceLaw>, Interval>>::Empty(), "[]"),
            R"doc(
                Constructor.

                Args:
                    guidance_laws_with_intervals (list[tuple[GuidanceLaw, Interval]], optional): Array of tuples containing the guidance laws and their corresponding intervals. Defaults to empty array.

                Raises:
                    RuntimeError: If any interval is undefined, if the guidance law is null or if the interval intersects with an existing interval.
            )doc"
        )

        .def(
            "get_guidance_laws_with_intervals",
            &HeterogeneousGuidanceLaw::getGuidanceLawsWithIntervals,
            R"doc(
                Get the guidance laws with their corresponding intervals.

                Returns:
                    list[tuple[GuidanceLaw, Interval]]: Array of tuples containing the guidance laws and their corresponding intervals.
            )doc"
        )

        .def(
            "add_guidance_law",
            &HeterogeneousGuidanceLaw::addGuidanceLaw,
            arg("guidance_law"),
            arg("interval"),
            R"doc(
                Add a guidance law with its corresponding interval.

                Args:
                    guidance_law (GuidanceLaw): The guidance law to add.
                    interval (Interval): The interval during which the guidance law is active.

                Raises:
                    RuntimeError: If the interval is undefined, if the guidance law is null or if the interval intersects with an existing interval.
            )doc"
        )

        .def(
            "calculate_thrust_acceleration_at",
            &HeterogeneousGuidanceLaw::calculateThrustAccelerationAt,
            arg("instant"),
            arg("position_coordinates"),
            arg("velocity_coordinates"),
            arg("thrust_acceleration"),
            arg("output_frame"),
            R"doc(
                Calculate thrust acceleration at a given instant and state.

                Args:
                    instant (Instant): The instant.
                    position_coordinates (numpy.ndarray): The position coordinates.
                    velocity_coordinates (numpy.ndarray): The velocity coordinates.
                    thrust_acceleration (float): The thrust acceleration magnitude.
                    output_frame (Frame): The frame in which the acceleration is expressed.

                Returns:
                    numpy.ndarray: The acceleration vector at the provided coordinates.
            )doc"
        )

        .def("__str__", &(shiftToString<HeterogeneousGuidanceLaw>))
        .def("__repr__", &(shiftToString<HeterogeneousGuidanceLaw>))

        ;
}
