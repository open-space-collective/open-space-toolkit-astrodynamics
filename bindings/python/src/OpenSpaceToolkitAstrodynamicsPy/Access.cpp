/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Access/Generator.cpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Access(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::physics::time::Instant;
    using ostk::physics::units::Angle;

    using ostk::astro::Access;

    {
        class_<Access> access_class(
            aModule,
            "Access",
            R"docs(
                Object-to-object visibility
                
                This class encapsulates the concept of visibility access between two trajectories.
                
                Group:
                    astrodynamics
            )docs"
        );

        access_class

            .def(
                init<const Access::Type&, const Instant&, const Instant&, const Instant&, const Angle&>(),
                R"docs(
                    Constructs an Access object.
                    
                    Args:
                        type (Access.Type): Type of the access (Complete, Partial, Undefined)
                        acquisition_of_signal (Instant): The instant when the signal is first acquired
                        time_of_closest_approach (Instant): The time of closest approach between objects
                        loss_of_signal (Instant): The instant when the signal is lost
                        max_elevation (Angle): The maximum elevation angle during the access
                )docs",
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
                R"docs(
                    Check if the Access object is defined.
                    
                    Returns:
                       bool: True if defined, False otherwise.
                 )docs"
            )

            .def(
                "is_complete",
                &Access::isComplete,
                R"docs(
                    Check if the access is complete.
                    
                    Returns:
                        bool: True if complete, False otherwise.
                 )docs"
            )

            .def(
                "get_type",
                &Access::getType,
                R"docs(
                    Get the type of the access.

                    Returns:
                        Access.Type: The type of the access.
                )docs"
            )

            .def(
                "get_acquisition_of_signal",
                &Access::getAcquisitionOfSignal,
                R"docs(
                    Get the acquisition of signal of the access.

                    Returns:
                        Instant: The acquisition of signal of the access.
                )docs"
            )

            .def(
                "get_time_of_closest_approach",
                &Access::getTimeOfClosestApproach,
                R"docs(
                    Get the time of closest approach of the access.

                    Returns:
                        Instant: The time of closest approach of the access.
                )docs"
            )

            .def(
                "get_loss_of_signal",
                &Access::getLossOfSignal,
                R"docs(
                    Get the loss of signal of the access.

                    Returns:
                       Instant: The loss of signal of the access.
                )docs"
            )

            .def(
                "get_interval",
                &Access::getInterval,
                R"docs(
                    Get the interval of the access.

                    Returns:
                       Interval: The interval of the access.
                )docs"
            )

            .def(
                "get_duration",
                &Access::getDuration,
                R"docs(
                    Get the duration of the access.

                    Returns:
                       Duration: The duration of the access.
                )docs"
            )

            .def(
                "get_max_elevation",
                &Access::getMaxElevation,
                R"docs(
                    Get the maximum elevation of the access.

                    Returns:
                      Angle: The maximum elevation of the access.
                )docs"
            )

            .def_static(
                "undefined",
                &Access::Undefined,
                R"docs(
                    Creates an undefined Access object.
                    
                    Returns:
                        Access: An undefined Access object.
                    
                    Group:
                        Static methods
                )docs"
            )

            .def_static(
                "string_from_type",
                &Access::StringFromType,
                R"docs(
                    Returns a string representation of the Access type.
    
                    Args:
                        type (Access.Type): The type of the access.
                    
                    Returns:
                        str: A string representation of the type.
                    
                    Group:
                        Static methods
                )docs",
                arg("type")
            )

            ;

        enum_<Access::Type>(
            access_class,
            "Type",
            R"docs(
                Access type.
            )docs"
        )

            .value("Undefined", Access::Type::Undefined, "Undefined")
            .value("Complete", Access::Type::Complete, "Complete")
            .value("Partial", Access::Type::Partial, "Partial")

            ;
    }

    // Create "access" python submodule
    auto access = aModule.def_submodule("access");

    // Add __path__ attribute for "access" submodule
    access.attr("__path__") = "ostk.astrodynamics.access";

    // Add elements to "access" module
    OpenSpaceToolkitAstrodynamicsPy_Access_Generator(access);
}
