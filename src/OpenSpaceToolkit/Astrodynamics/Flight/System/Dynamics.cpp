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

Dynamics::Dynamics() {}

Dynamics::~Dynamics() {}

Dynamics::DynamicalEquationWrapper Dynamics::GetDynamicalEquations(
    const Instant& anInstant, const Array<Shared<Dynamics>>& dynamics
)
{
    return std::bind(
        Dynamics::DynamicalEquations,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        dynamics,
        anInstant
    );
}

void Dynamics::DynamicalEquations(
    const Dynamics::StateVector& x,
    Dynamics::StateVector& dxdt,
    const double t,
    const Array<Shared<Dynamics>>& dynamics,
    const Instant& anInstant
)
{
    for (Index i = 0; i < dxdt.size(); ++i)
    {
        dxdt[i] = 0;
    }

    const Instant nextInstant = anInstant + Duration::Seconds(t);

    for (const Shared<Dynamics>& dynamic : dynamics)
    {
        dynamic->update(x, dxdt, nextInstant);
    }
}

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
