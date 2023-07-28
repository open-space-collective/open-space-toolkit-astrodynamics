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

using ostk::core::ctnr::Pair;
using ostk::core::types::Index;
using ostk::core::types::Size;

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
    const Array<Dynamics::DynamicsInformation>& aDynamicsInformationArray,
    const Instant& anInstant,
    const Shared<const Frame>& aFrame
)
{
    return std::bind(
        Dynamics::DynamicalEquations,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        aDynamicsInformationArray,
        anInstant,
        aFrame
    );
}

void Dynamics::DynamicalEquations(
    const NumericalSolver::StateVector& x,
    NumericalSolver::StateVector& dxdt,
    const double& t,
    const Array<Dynamics::DynamicsInformation>& aDynamicsInformationArray,
    const Instant& anInstant,
    const Shared<const Frame>& aFrame
)
{
    for (int i = 0; i < dxdt.size(); ++i)
    {
        dxdt[i] = 0;
    }

    const Instant nextInstant = anInstant + Duration::Seconds(t);

    for (const Dynamics::DynamicsInformation& dynamicsInformation : aDynamicsInformationArray)
    {
        const VectorXd contribution = dynamicsInformation.dynamics->computeContribution(
            nextInstant,
            Dynamics::ReduceFullStateToReadState(
                x, dynamicsInformation.readIndexes, dynamicsInformation.reducedStateSize
            ),
            aFrame
        );

        Dynamics::AddContributionToFullState(dxdt, contribution, dynamicsInformation.writeIndexes);
    }
}

VectorXd Dynamics::ReduceFullStateToReadState(
    const NumericalSolver::StateVector& x, const Array<Pair<Index, Size>>& readInfo, const Size readSize
)
{
    Index i = 0;
    VectorXd reduced = VectorXd(readSize);

    for (const Pair<Index, Size>& pair : readInfo)
    {
        const Index subsetOffset = pair.first;
        const Size subsetSize = pair.second;
        for (Index j = 0; j < subsetSize; j++)
        {
            reduced[i] = x[subsetOffset + j];
            i++;
        }
    }

    return reduced;
}

void Dynamics::AddContributionToFullState(
    NumericalSolver::StateVector& dxdt, const VectorXd& contribution, const Array<Pair<Index, Size>>& writeInfo
)
{
    Index i = 0;
    for (Pair<Index, Size> pair : writeInfo)
    {
        const Index subsetOffset = pair.first;
        const Size subsetSize = pair.second;
        for (Index j = 0; j < subsetSize; j++)
        {
            dxdt[subsetOffset + j] += contribution[i];
            i++;
        }
    }
}

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
