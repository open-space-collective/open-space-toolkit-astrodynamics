/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Data_Provider_OffNadir__
#define __OpenSpaceToolkit_Astrodynamics_Data_Provider_OffNadir__

#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace data
{
namespace provider
{

using ostk::core::container::Tuple;

using ostk::physics::coordinate::Position;
using ostk::physics::unit::Angle;

using ostk::astrodynamics::trajectory::State;

/// @brief Compute the along-track, cross-track and total off-nadir angle between the satellite and the target
///
/// - The along-track angle is the angle between the nadir vector [Z] and the projection of the satellite->target vector
///   onto the plane defined by the satellite local horizontal (velocity vector in circular orbits) [X] and the nadir
///   vector [Z].
/// - The cross-track angle is the angle between the nadir vector [Z] and the projection of the satellite->target vector
///   onto the plane defined by the negative orbital momentum vector [Y] and the nadir vector [Z].
/// - The total off-nadir angle is the angle between the nadir vector [Z] and the satellite->target vector.
///
/// @param aState The state of the satellite
/// @param aTargetPosition The position of the target
/// @return A tuple containing the along-track, cross-track and total off-nadir angles
Tuple<Angle, Angle, Angle> ComputeOffNadirAngles(const State& aState, const Position& aTargetPosition);

}  // namespace provider
}  // namespace data
}  // namespace astrodynamics
}  // namespace ostk

#endif
