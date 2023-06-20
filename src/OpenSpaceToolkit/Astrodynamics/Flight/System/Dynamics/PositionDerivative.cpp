/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>

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

void PositionDerivative::applyContribution(
    const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant
) const
{
    (void)anInstant;

    // Integrate position states
    dxdt[0] = x[3];
    dxdt[1] = x[4];
    dxdt[2] = x[5];
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
