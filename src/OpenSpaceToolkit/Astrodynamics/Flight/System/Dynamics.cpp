/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

using ostk::core::types::Index;

using ostk::physics::time::Duration;

Dynamics::Dynamics(const String& aName)
    : name_(aName)
{
}

Dynamics::~Dynamics() {}

String Dynamics::getName() const
{
    return name_;
}

void Dynamics::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Dynamics") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

NumericalSolver::SystemOfEquationsWrapper Dynamics::GetDynamicalEquations(
    const Array<Shared<Dynamics>>& aDynamicsArray, const Instant& anInstant
)
{
    return std::bind(
        Dynamics::DynamicalEquations,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        aDynamicsArray,
        anInstant
    );
}

void Dynamics::DynamicalEquations(
    const NumericalSolver::StateVector& x,
    NumericalSolver::StateVector& dxdt,
    const double& t,
    const Array<Shared<Dynamics>>& aDynamicsArray,
    const Instant& anInstant
)
{
    for (Index i = 0; i < (Size)dxdt.size(); ++i)
    {
        dxdt[i] = 0;
    }

    const Instant nextInstant = anInstant + Duration::Seconds(t);

    for (const Shared<Dynamics>& dynamics : aDynamicsArray)
    {
        dynamics->applyContribution(x, dxdt, nextInstant);
    }
}

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
