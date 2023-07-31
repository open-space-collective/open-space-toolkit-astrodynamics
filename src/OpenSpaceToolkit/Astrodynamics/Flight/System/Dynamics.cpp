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

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Index;
using ostk::core::types::Size;

using ostk::physics::time::Duration;

Dynamics::DynamicsInformation::DynamicsInformation(
    const Shared<Dynamics>& aDynamics,
    const Array<Pair<Index, Size>>& aReadIndexes,
    const Array<Pair<Index, Size>>& aWriteIndexes
)
    : dynamics(aDynamics),
      readIndexes(aReadIndexes),
      writeIndexes(aWriteIndexes),
      reducedStateSize(0)
{
    for (const Pair<Index, Size>& pair : readIndexes)
    {
        this->reducedStateSize += pair.second;
    }
}

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
    dxdt.setZero();

    const Instant nextInstant = anInstant + Duration::Seconds(t);

    for (const Dynamics::DynamicsInformation& dynamicsInformation : aDynamicsInformationArray)
    {
        const VectorXd contribution = dynamicsInformation.dynamics->computeContribution(
            nextInstant,
            Dynamics::extractReadState(x, dynamicsInformation.readIndexes, dynamicsInformation.reducedStateSize),
            aFrame
        );

        Dynamics::applyContribution(dxdt, contribution, dynamicsInformation.writeIndexes);
    }
}

VectorXd Dynamics::extractReadState(
    const NumericalSolver::StateVector& x, const Array<Pair<Index, Size>>& readInfo, const Size readSize
)
{
    Index offset = 0;
    VectorXd reduced = VectorXd(readSize);

    for (const Pair<Index, Size>& pair : readInfo)
    {
        const Index subsetOffset = pair.first;
        const Size subsetSize = pair.second;

        reduced.segment(offset, subsetSize) = x.segment(subsetOffset, subsetSize);
        offset += subsetSize;
    }

    return reduced;
}

void Dynamics::applyContribution(
    NumericalSolver::StateVector& dxdt, const VectorXd& contribution, const Array<Pair<Index, Size>>& writeInfo
)
{
    Index offset = 0;
    for (Pair<Index, Size> pair : writeInfo)
    {
        const Index subsetOffset = pair.first;
        const Size subsetSize = pair.second;

        dxdt.segment(subsetOffset, subsetSize) += contribution.segment(offset, subsetSize);
        offset += subsetSize;
    }
}

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
