/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Models/Tabulated.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Flight_Profile_Models_Tabulated(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::ctnr::Array;

    using ostk::astro::flight::profile::Model;
    using ostk::astro::flight::profile::State;
    using ostk::astro::flight::profile::models::Tabulated;

    class_<Tabulated, Model>(aModule, "Tabulated")

        .def(init<Array<State>&>(), arg("states"))

        .def("__str__", &(shiftToString<State>))
        .def("__repr__", &(shiftToString<State>))

        .def("is_defined", &Tabulated::isDefined)

        .def("get_interval", &Tabulated::getInterval)
        .def("calculate_state_at", &Tabulated::calculateStateAt, arg("instant"))
        .def("get_axes_at", &Tabulated::getAxesAt, arg("instant"))
        .def("get_body_frame", &Tabulated::getBodyFrame, arg("frame_name"))

        // .def_static("load", &Tabulated::Load, arg("file"))

        ;
}
