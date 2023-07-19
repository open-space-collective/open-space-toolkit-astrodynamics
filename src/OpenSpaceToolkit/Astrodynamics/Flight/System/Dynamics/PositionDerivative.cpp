/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianVelocity.hpp>

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

using ostk::astro::trajectory::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::coordinatessubsets::CartesianVelocity;

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

Array<Shared<const CoordinatesSubset>> PositionDerivative::getReadCoordinateSubsets() const
{
    return {CartesianVelocity::ThreeDimensional()};
}

Array<Shared<const CoordinatesSubset>> PositionDerivative::getWriteCoordinateSubsets() const
{
    return {CartesianPosition::ThreeDimensional()};
}

VectorXd PositionDerivative::computeContribution(
    [[maybe_unused]] const Instant& anInstant, const VectorXd& x, [[maybe_unused]] const Shared<const Frame>& aFrame
) const
{
    VectorXd contribution(3);
    contribution << x[0], x[1], x[2];

    return contribution;
}

void PositionDerivative::declareCoordinates(const Shared<CoordinatesBroker>& coordinatesBroker)
{
    this->positionIndex_ = coordinatesBroker->addSubset(CartesianPosition::ThreeDimensional());
    this->velocityIndex_ = coordinatesBroker->addSubset(CartesianVelocity::ThreeDimensional());
}

void PositionDerivative::applyContribution(
    const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant
) const
{
    (void)anInstant;

    // Integrate position states
    dxdt[positionIndex_] += x[velocityIndex_];
    dxdt[positionIndex_ + 1] += x[velocityIndex_ + 1];
    dxdt[positionIndex_ + 2] += x[velocityIndex_ + 2];
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
