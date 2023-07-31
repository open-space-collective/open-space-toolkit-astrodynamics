/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{

using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

PositionDerivative::PositionDerivative()
    : Dynamics("Position Derivative")
{
}

PositionDerivative::~PositionDerivative() {}

PositionDerivative* PositionDerivative::clone() const
{
    return new PositionDerivative(*this);
}

bool PositionDerivative::isDefined() const
{
    return true;
}

std::ostream& operator<<(std::ostream& anOutputStream, const PositionDerivative& aPositionDerivative)
{
    aPositionDerivative.print(anOutputStream);

    return anOutputStream;
}

Array<Shared<const CoordinatesSubset>> PositionDerivative::getReadCoordinatesSubsets() const
{
    return {CartesianVelocity::Default()};
}

Array<Shared<const CoordinatesSubset>> PositionDerivative::getWriteCoordinatesSubsets() const
{
    return {CartesianPosition::Default()};
}

VectorXd PositionDerivative::computeContribution(
    [[maybe_unused]] const Instant& anInstant, const VectorXd& x, [[maybe_unused]] const Shared<const Frame>& aFrame
) const
{
    VectorXd contribution(3);
    contribution << x[0], x[1], x[2];

    return contribution;
}

void PositionDerivative::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Position Derivative Dynamics") : void();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
