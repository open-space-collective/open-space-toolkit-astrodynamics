/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Access/Generator.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Access(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::physics::time::Instant;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::Access;

    {
        class_<Access> access_class(
            aModule,
            "Access",
            R"doc(
                Object-to-object visibility
                
                This class encapsulates the concept of visibility access between two trajectories.
                
            )doc"
        );

        enum_<Access::Type>(
            access_class,
            "Type",
            R"doc(
                Access type.
            )doc"
        )

            .value("Undefined", Access::Type::Undefined, "Undefined")
            .value("Complete", Access::Type::Complete, "Complete")
            .value("Partial", Access::Type::Partial, "Partial")

            ;

        access_class

            .def(
                init<const Access::Type&, const Instant&, const Instant&, const Instant&, const Angle&>(),
                R"doc(
                    Constructs an Access object.
                    
                    Args:
                        type (Access.Type): Type of the access (Complete, Partial, Undefined)
                        acquisition_of_signal (Instant): The instant when the signal is first acquired
                        time_of_closest_approach (Instant): The time of closest approach between objects
                        loss_of_signal (Instant): The instant when the signal is lost
                        max_elevation (Angle): The maximum elevation angle during the access
                )doc",
                arg("type"),
                arg("acquisition_of_signal"),
                arg("time_of_closest_approach"),
                arg("loss_of_signal"),
                arg("max_elevation")
            )

            .def(self == self)
            .def(self != self)

            .def("__str__", &(shiftToString<Access>))
            .def("__repr__", &(shiftToString<Access>))

            .def(
                "is_defined",
                &Access::isDefined,
                R"doc(
                    Check if the Access object is defined.
                    
                    Returns:
                       bool: True if defined, False otherwise.
                 )doc"
            )

            .def(
                "is_complete",
                &Access::isComplete,
                R"doc(
                    Check if the access is complete.
                    
                    Returns:
                        bool: True if complete, False otherwise.
                 )doc"
            )

            .def(
                "get_type",
                &Access::getType,
                R"doc(
                    Get the type of the access.

                    Returns:
                        Access.Type: The type of the access.
                )doc"
            )

            .def(
                "get_acquisition_of_signal",
                &Access::getAcquisitionOfSignal,
                R"doc(
                    Get the acquisition of signal of the access.

                    Returns:
                        Instant: The acquisition of signal of the access.
                )doc"
            )

            .def(
                "get_time_of_closest_approach",
                &Access::getTimeOfClosestApproach,
                R"doc(
                    Get the time of closest approach of the access.

                    Returns:
                        Instant: The time of closest approach of the access.
                )doc"
            )

            .def(
                "get_loss_of_signal",
                &Access::getLossOfSignal,
                R"doc(
                    Get the loss of signal of the access.

                    Returns:
                       Instant: The loss of signal of the access.
                )doc"
            )

            .def(
                "get_interval",
                &Access::getInterval,
                R"doc(
                    Get the interval of the access.

                    Returns:
                       Interval: The interval of the access.
                )doc"
            )

            .def(
                "get_duration",
                &Access::getDuration,
                R"doc(
                    Get the duration of the access.

                    Returns:
                       Duration: The duration of the access.
                )doc"
            )

            .def(
                "get_max_elevation",
                &Access::getMaxElevation,
                R"doc(
                    Get the maximum elevation of the access.

                    Returns:
                      Angle: The maximum elevation of the access.
                )doc"
            )

            .def_static(
                "undefined",
                &Access::Undefined,
                R"doc(
                    Creates an undefined Access object.
                    
                    Returns:
                        Access: An undefined Access object.
                )doc"
            )

            .def_static(
                "string_from_type",
                &Access::StringFromType,
                R"doc(
                    Returns a string representation of the Access type.
    
                    Args:
                        type (Access.Type): The type of the access.
                    
                    Returns:
                        str: A string representation of the type.
                )doc",
                arg("type")
            )

            ;
    }

    // Create "access" python submodule
    auto access = aModule.def_submodule("access");

    // Add elements to "access" module
    OpenSpaceToolkitAstrodynamicsPy_Access_Generator(access);
}
