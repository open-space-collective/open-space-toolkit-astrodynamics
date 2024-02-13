/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

PositionDerivative::PositionDerivative()
    : Dynamics("Position Derivative")
{
}

PositionDerivative::~PositionDerivative() {}

bool PositionDerivative::isDefined() const
{
    return true;
}

std::ostream& operator<<(std::ostream& anOutputStream, const PositionDerivative& aPositionDerivative)
{
    aPositionDerivative.print(anOutputStream);

    return anOutputStream;
}

Array<Shared<const CoordinateSubset>> PositionDerivative::getReadCoordinateSubsets() const
{
    return {
        CartesianVelocity::Default(),
    };
}

Array<Shared<const CoordinateSubset>> PositionDerivative::getWriteCoordinateSubsets() const
{
    return {
        CartesianPosition::Default(),
    };
}

VectorXd PositionDerivative::computeContribution(
    [[maybe_unused]] const Instant& anInstant, const VectorXd& x, [[maybe_unused]] const Shared<const Frame>& aFrameSPtr
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
}  // namespace astrodynamics
}  // namespace ostk
