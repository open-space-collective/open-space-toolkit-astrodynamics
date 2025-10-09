/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/SequentialGuidanceLaw.hpp>

using namespace pybind11;

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::guidancelaw::SequentialGuidanceLaw;

inline void OpenSpaceToolkitAstrodynamicsPy_GuidanceLaw_SequentialGuidanceLaw(pybind11::module& aModule)
{
    class_<SequentialGuidanceLaw, GuidanceLaw, Shared<SequentialGuidanceLaw>>(
        aModule,
        "SequentialGuidanceLaw",
        R"doc(
            A guidance law that sequences multiple guidance laws over specific time intervals.
            
            At each point in time, the applicable guidance law is selected and used to calculate
            the thrust acceleration. Guidance laws don't need to be contiguous, and can be added
            in any order. If the instant does not fall within any of the intervals, the thrust
            acceleration is zero. The guidance law intervals must not intersect each other.
        )doc"
    )

        .def(
            init<const Array<Tuple<Shared<GuidanceLaw>, Interval>>&>(),
            arg_v("guidance_laws_with_intervals", Array<Tuple<Shared<GuidanceLaw>, Interval>>::Empty(), "[]"),
            R"doc(
                Constructor.

                Args:
                    guidance_laws_with_intervals (List[Tuple[GuidanceLaw, Interval]], optional): Array of tuples containing the guidance laws and their corresponding intervals. Defaults to empty array.

                Raises:
                    RuntimeError: If any interval is undefined, if the guidance law is null or if the interval intersects with an existing interval.
            )doc"
        )

        .def(
            "get_guidance_laws_with_intervals",
            &SequentialGuidanceLaw::getGuidanceLawsWithIntervals,
            R"doc(
                Get the guidance laws with their corresponding intervals.

                Returns:
                    List[Tuple[GuidanceLaw, Interval]]: Array of tuples containing the guidance laws and their corresponding intervals.
            )doc"
        )

        .def(
            "add_guidance_law",
            &SequentialGuidanceLaw::addGuidanceLaw,
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
            &SequentialGuidanceLaw::calculateThrustAccelerationAt,
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

        .def("__str__", &(shiftToString<SequentialGuidanceLaw>))
        .def("__repr__", &(shiftToString<SequentialGuidanceLaw>))

        ;
}
