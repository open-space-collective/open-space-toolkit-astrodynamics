/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinateSubset/AngularVelocity.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinateSubset/AttitudeQuaternion.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinateSubset/CartesianPosition.cpp>
#include <OpenSpaceToolkitAstrodynamicsPy/Trajectory/State/CoordinateSubset/CartesianVelocity.cpp>

using namespace pybind11;

using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::VectorXd;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;

// Trampoline class for virtual member functions
class PyCoordinateSubset : public CoordinateSubset
{
   public:
    using CoordinateSubset::CoordinateSubset;

    // Trampoline (need one for each virtual function)

    VectorXd add(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const override
    {
        PYBIND11_OVERRIDE(
            VectorXd,
            CoordinateSubset,
            add,
            anInstant,
            aFullCoordinatesVector,
            anotherFullCoordinatesVector,
            aFrameSPtr,
            aCoordinateBrokerSPtr
        );
    }

    VectorXd subtract(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const VectorXd& anotherFullCoordinatesVector,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const override
    {
        PYBIND11_OVERRIDE(
            VectorXd,
            CoordinateSubset,
            subtract,
            anInstant,
            aFullCoordinatesVector,
            anotherFullCoordinatesVector,
            aFrameSPtr,
            aCoordinateBrokerSPtr
        );
    }

    VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& aFullCoordinatesVector,
        const Shared<const Frame>& fromFrameSPtr,
        const Shared<const Frame>& toFrameSPtr,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    ) const override
    {
        PYBIND11_OVERRIDE(
            VectorXd,
            CoordinateSubset,
            inFrame,
            anInstant,
            aFullCoordinatesVector,
            fromFrameSPtr,
            toFrameSPtr,
            aCoordinateBrokerSPtr
        );
    }
};

inline void OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset(pybind11::module& aModule)
{
    class_<CoordinateSubset, PyCoordinateSubset, Shared<CoordinateSubset>>(
        aModule,
        "CoordinateSubset",
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

        .def("__eq__", &CoordinateSubset::operator==)
        .def("__ne__", &CoordinateSubset::operator!=)

        .def(
            "get_id",
            &CoordinateSubset::getId,
            R"doc(
                Get the identifier of the coordinates subset.

                Returns:
                    str: The identifier of the coordinates subset.
            )doc"
        )
        .def(
            "get_name",
            &CoordinateSubset::getName,
            R"doc(
                Get the name of the coordinates subset.

                Returns:
                    str: The name of the coordinates subset.
            )doc"
        )
        .def(
            "get_size",
            &CoordinateSubset::getSize,
            R"doc(
                Get the size of the coordinates subset.

                Returns:
                    int: The size of the coordinates subset.
            )doc"
        )

        .def(
            "add",
            &CoordinateSubset::add,
            R"doc(
                Add the coordinates of another state to the coordinates of this state.

                Args:
                    instant (Instant): The instant of the state.
                    coordinates (numpy.ndarray): The coordinates of this state.
                    another_coordinates (numpy.ndarray): The coordinates of the other state.
                    frame (Frame): The reference frame of the coordinates.
                    coordinate_broker (CoordinateBroker): The coordinates broker.

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
            &CoordinateSubset::subtract,
            R"doc(
                Subtract the coordinates of another state from the coordinates of this state.

                Args:
                    instant (Instant): The instant of the state.
                    coordinates (numpy.ndarray): The coordinates of this state.
                    another_coordinates (numpy.ndarray): The coordinates of the other state.
                    frame (Frame): The reference frame of the coordinates.
                    coordinate_broker (CoordinateBroker): The coordinates broker.

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
            &CoordinateSubset::inFrame,
            R"doc(
                Convert the coordinates of this state from one frame to another.

                Args:
                    instant (Instant): The instant of the state.
                    coordinates (numpy.ndarray): The coordinates of this state.
                    from_frame (Frame): The reference frame of the input coordinates.
                    to_frame (Frame): The reference frame of the output coordinates.
                    coordinate_broker (CoordinateBroker): The coordinates broker.

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
            &CoordinateSubset::Mass,
            R"doc(
                Get the mass coordinates subset.

                Returns:
                    CoordinateSubset: The mass coordinates subset.
            )doc"
        )
        .def_static(
            "surface_area",
            &CoordinateSubset::SurfaceArea,
            R"doc(
                Get the surface area coordinates subset.

                Returns:
                    CoordinateSubset: The surface area coordinates subset.
            )doc"
        )
        .def_static(
            "drag_coefficient",
            &CoordinateSubset::DragCoefficient,
            R"doc(
                Get the drag coefficient coordinates subset.

                Returns:
                    CoordinateSubset: The drag coefficient coordinates subset.
            )doc"
        )

        ;

    // Create "coordinate_subset" python submodule
    auto coordinate_subset = aModule.def_submodule("coordinate_subset");

    // Add objects to "coordinate_subset" submodule
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_AngularVelocity(coordinate_subset);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_AttitudeQuaternion(coordinate_subset);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_CartesianPosition(coordinate_subset);
    OpenSpaceToolkitAstrodynamicsPy_Trajectory_State_CoordinateSubset_CartesianVelocity(coordinate_subset);
}
