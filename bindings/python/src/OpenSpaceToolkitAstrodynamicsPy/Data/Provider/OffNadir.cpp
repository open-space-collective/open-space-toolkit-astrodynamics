/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Data/Provider/OffNadir.hpp>

inline void OpenSpaceToolkitAstrodynamicsPy_Data_Provider_OffNadir(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::astrodynamics::data::provider::ComputeOffNadirAngles;

    aModule.def(
        "compute_off_nadir_angles",
        &ComputeOffNadirAngles,
        arg("state"),
        arg("target_position"),
        R"doc(
            Compute the along-track, cross-track and total off-nadir angle between the satellite and the target.

            - The along-track angle is the angle between the nadir vector [Z] and the projection of the satellite->target vector 
              onto the plane defined by the satellite local horizontal (velocity vector in circular orbits) [X] and the nadir vector [Z].
            - The cross-track angle is the angle between the nadir vector [Z] and the projection of the satellite->target vector 
              onto the plane defined by the negative orbital momentum vector [Y] and the nadir vector [Z].
            - The total off-nadir angle is the angle between the nadir vector [Z] and the satellite->target vector.

            Args:
                state (State): The state of the satellite.
                target_position (Position): The position of the target.

            Returns:
                tuple[Angle, Angle, Angle]: The along-track, cross-track and total off-nadir angle between the satellite and the target.
        )doc"
    );
}
