/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesSubsets/AngularVelocity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesSubsets/AttitudeQuaternion.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesSubsets/CartesianPosition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinatesSubsets/CartesianVelocity.cpp>

using namespace pybind11;

using ostk::core::types::Shared;
using ostk::core::types::Size;

using ostk::math::object::VectorXd;

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
    class_<CoordinatesSubset, PyCoordinatesSubset, Shared<CoordinatesSubset>>(
        aModule,
        "CoordinatesSubset",
        R"doc(
            State coordinates subset. It contains information related to a particular group of coordinates. It does not
            contain the coordinate values.

        )doc"
    )

        .def(
            init<const String&, const Size&>(),
            R"doc(
                Constructor.

                Args:
                    name (str): The name of the coordinates subset.
                    size (int): The size of the coordinates subset.

            )doc",
            arg("name"),
            arg("size")
        )

        .def("__eq__", &CoordinatesSubset::operator==)
        .def("__ne__", &CoordinatesSubset::operator!=)

        .def(
            "get_id",
            &CoordinatesSubset::getId,
            R"doc(
                Get the identifier of the coordinates subset.

                Returns:
                    str: The identifier of the coordinates subset.
            )doc"
        )
        .def(
            "get_name",
            &CoordinatesSubset::getName,
            R"doc(
                Get the name of the coordinates subset.

                Returns:
                    str: The name of the coordinates subset.
            )doc"
        )
        .def(
            "get_size",
            &CoordinatesSubset::getSize,
            R"doc(
                Get the size of the coordinates subset.

                Returns:
                    int: The size of the coordinates subset.
            )doc"
        )

        .def(
            "add",
            &CoordinatesSubset::add,
            R"doc(
                Add the coordinates of another state to the coordinates of this state.

                Args:
                    instant (Instant): The instant of the state.
                    coordinates (numpy.ndarray): The coordinates of this state.
                    another_coordinates (numpy.ndarray): The coordinates of the other state.
                    frame (Frame): The reference frame of the coordinates.
                    coordinate_broker (CoordinatesBroker): The coordinates broker.

                Returns:
                    numpy.ndarray: The sum of the coordinates.

            )doc",
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinate_broker")
        )
        .def(
            "subtract",
            &CoordinatesSubset::subtract,
            R"doc(
                Subtract the coordinates of another state from the coordinates of this state.

                Args:
                    instant (Instant): The instant of the state.
                    coordinates (numpy.ndarray): The coordinates of this state.
                    another_coordinates (numpy.ndarray): The coordinates of the other state.
                    frame (Frame): The reference frame of the coordinates.
                    coordinate_broker (CoordinatesBroker): The coordinates broker.

                Returns:
                    numpy.ndarray: The difference of the coordinates.

            )doc",
            arg("instant"),
            arg("coordinates"),
            arg("another_coordinates"),
            arg("frame"),
            arg("coordinate_broker")
        )
        .def(
            "in_frame",
            &CoordinatesSubset::inFrame,
            R"doc(
                Convert the coordinates of this state from one frame to another.

                Args:
                    instant (Instant): The instant of the state.
                    coordinates (numpy.ndarray): The coordinates of this state.
                    from_frame (Frame): The reference frame of the input coordinates.
                    to_frame (Frame): The reference frame of the output coordinates.
                    coordinate_broker (CoordinatesBroker): The coordinates broker.

                Returns:
                    numpy.ndarray: The coordinates in the output frame.

            )doc",
            arg("instant"),
            arg("coordinates"),
            arg("from_frame"),
            arg("to_frame"),
            arg("coordinate_broker")
        )

        .def_static(
            "mass",
            &CoordinatesSubset::Mass,
            R"doc(
                Get the mass coordinates subset.

                Returns:
                    CoordinatesSubset: The mass coordinates subset.
            )doc"
        )
        .def_static(
            "surface_area",
            &CoordinatesSubset::SurfaceArea,
            R"doc(
                Get the surface area coordinates subset.

                Returns:
                    CoordinatesSubset: The surface area coordinates subset.
            )doc"
        )
        .def_static(
            "drag_coefficient",
            &CoordinatesSubset::DragCoefficient,
            R"doc(
                Get the drag coefficient coordinates subset.

                Returns:
                    CoordinatesSubset: The drag coefficient coordinates subset.
            )doc"
        )

        ;

    // Create "coordinates_subset" python submodule
    auto coordinates_subset = aModule.def_submodule("coordinates_subset");

    // Add objects to "coordinates_subset" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_AngularVelocity(coordinates_subset);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_AttitudeQuaternion(coordinates_subset);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_CartesianPosition(coordinates_subset);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinatesSubsets_CartesianVelocity(coordinates_subset);
}
