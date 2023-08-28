/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesSubsets/CartesianPosition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesSubsets/CartesianVelocity.cpp>

using namespace pybind11;

using ostk::core::types::Shared;
using ostk::core::types::Size;

using ostk::math::obj::VectorXd;

using ostk::astro::trajectory::state::CoordinatesSubset;

// Trampoline class for virtual member functions
class PyCoordinatesSubset : public CoordinatesSubset
{
   public:
    using CoordinatesSubset::CoordinatesSubset;

    // Trampoline (need one for each virtual function)

    VectorXd add(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    ) const override
    {
        PYBIND11_OVERRIDE(
            VectorXd,
            CoordinatesSubset,
            add,
            anInstant,
            aFullCoordinatesVector,
            anotherFullCoordinatesVector,
            aFrameSPtr,
            aCoordinatesBrokerSPtr
        );
    }

    VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    ) const override
    {
        PYBIND11_OVERRIDE(
            VectorXd,
            CoordinatesSubset,
            subtract,
            anInstant,
            aFullCoordinatesVector,
            anotherFullCoordinatesVector,
            aFrameSPtr,
            aCoordinatesBrokerSPtr
        );
    }

    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrameSPtr,
        const Shared<const Frame>& toFrameSPtr,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    ) const override
    {
        PYBIND11_OVERRIDE(
            VectorXd,
            CoordinatesSubset,
            inFrame,
            anInstant,
            aFullCoordinatesVector,
            fromFrameSPtr,
            toFrameSPtr,
            aCoordinatesBrokerSPtr
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubset(pybind11::module& aModule)
{
    class_<CoordinatesSubset, PyCoordinatesSubset, Shared<CoordinatesSubset>>(aModule, "CoordinatesSubset")

        .def(init<const String&, const Size&>(), arg("name"), arg("size"))

        .def("__eq__", &CoordinatesSubset::operator==)
        .def("__ne__", &CoordinatesSubset::operator!=)

        .def("get_id", &CoordinatesSubset::getId)
        .def("get_name", &CoordinatesSubset::getName)
        .def("get_size", &CoordinatesSubset::getSize)

        .def(
            "add",
            &CoordinatesSubset::add,
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinate_broker")
        )
        .def(
            "subtract",
            &CoordinatesSubset::subtract,
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinate_broker")
        )
        .def(
            "in_frame",
            &CoordinatesSubset::inFrame,
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinate_broker")
        )

        .def_static("mass", &CoordinatesSubset::Mass)

        ;

    // Create "coordinates_subset" python submodule
    auto coordinates_subset = aModule.def_submodule("coordinates_subset");

    // Add __path__ attribute for "coordinates_subset" submodule
    coordinates_subset.attr("__path__") = "ostk.astrodynamics.trajectory.state.coordinates_subset";

    // Add objects to "coordinates_subset" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_CartesianPosition(coordinates_subset);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_CartesianVelocity(coordinates_subset);
}
