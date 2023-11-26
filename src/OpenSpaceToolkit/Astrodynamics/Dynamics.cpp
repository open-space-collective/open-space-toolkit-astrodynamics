/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Index;
using ostk::core::types::Size;

using ostk::physics::time::Duration;
using ostk::physics::environment::object::Celestial;

using ostk::astro::dynamics::CentralBodyGravity;
using ostk::astro::dynamics::ThirdBodyGravity;
using ostk::astro::dynamics::AtmosphericDrag;
using ostk::astro::dynamics::PositionDerivative;

Dynamics::Context::Context(
    const Shared<Dynamics>& aDynamicsSPtr,
    const Array<Pair<Index, Size>>& aReadIndexes,
    const Array<Pair<Index, Size>>& aWriteIndexes
)
    : dynamics(aDynamicsSPtr),
      readIndexes(aReadIndexes),
      writeIndexes(aWriteIndexes),
      readStateSize(0)
{
    for (const Pair<Index, Size>& pair : readIndexes)
    {
        this->readStateSize += pair.second;
    }
}

Dynamics::Dynamics(const String& aName)
    : name_(aName)
{
}

Dynamics::~Dynamics() {}

std::ostream& operator<<(std::ostream& anOutputStream, const Dynamics& aDynamics)
{
    aDynamics.print(anOutputStream);

    return anOutputStream;
}

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

NumericalSolver::SystemOfEquationsWrapper Dynamics::GetSystemOfEquations(
    const Array<Dynamics::Context>& aContextArray, const Instant& anInstant, const Shared<const Frame>& aFrameSPtr
)
{
    return std::bind(
        Dynamics::DynamicalEquations,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        aContextArray,
        anInstant,
        aFrameSPtr
    );
}

void Dynamics::DynamicalEquations(
    const NumericalSolver::StateVector& x,
    NumericalSolver::StateVector& dxdt,
    const double& t,
    const Array<Dynamics::Context>& aContextArray,
    const Instant& anInstant,
    const Shared<const Frame>& aFrameSPtr
)
{
    dxdt.setZero();

    const Instant nextInstant = anInstant + Duration::Seconds(t);

    for (const Dynamics::Context& dynamicsContext : aContextArray)
    {
        const VectorXd contribution = dynamicsContext.dynamics->computeContribution(
            nextInstant,
            Dynamics::extractReadState(x, dynamicsContext.readIndexes, dynamicsContext.readStateSize),
            aFrameSPtr
        );

        Dynamics::applyContribution(dxdt, contribution, dynamicsContext.writeIndexes);
    }
}

VectorXd Dynamics::extractReadState(
    const NumericalSolver::StateVector& x, const Array<Pair<Index, Size>>& readInfo, const Size readSize
)
{
    Index offset = 0;
    VectorXd reducedState = VectorXd(readSize);

    for (const Pair<Index, Size>& pair : readInfo)
    {
        const Index subsetOffset = pair.first;
        const Size subsetSize = pair.second;

        reducedState.segment(offset, subsetSize) = x.segment(subsetOffset, subsetSize);
        offset += subsetSize;
    }

    return reducedState;
}

void Dynamics::applyContribution(
    NumericalSolver::StateVector& dxdt, const VectorXd& contribution, const Array<Pair<Index, Size>>& writeInfo
)
{
    Index offset = 0;
    for (const Pair<Index, Size>& pair : writeInfo)
    {
        const Index subsetOffset = pair.first;
        const Size subsetSize = pair.second;

        dxdt.segment(subsetOffset, subsetSize) += contribution.segment(offset, subsetSize);
        offset += subsetSize;
    }
}

Array<Shared<Dynamics>> Dynamics::FromEnvironment(const Environment& anEnvironment)
{
    const auto getDynamics = [](const Shared<const Celestial>& aCelestial) -> Array<Shared<Dynamics>>
    {
        Array<Shared<Dynamics>> dynamics = Array<Shared<Dynamics>>::Empty();

        if (aCelestial->gravitationalModelIsDefined())
        {
            if (aCelestial->getName() == "Earth")
            {
                dynamics.add(std::make_shared<CentralBodyGravity>(aCelestial));
            }
            else
            {
                dynamics.add(std::make_shared<ThirdBodyGravity>(aCelestial));
            }
        }

        if (aCelestial->atmosphericModelIsDefined())
        {
            dynamics.add(std::make_shared<AtmosphericDrag>(aCelestial));
        }

        return dynamics;
    };

    Array<Shared<Dynamics>> dynamicsArray = Array<Shared<Dynamics>>::Empty();

    for (const String& name : anEnvironment.getObjectNames())
    {
        const Shared<const Celestial> celestialSPtr = anEnvironment.accessCelestialObjectWithName(name);

        dynamicsArray.add(getDynamics(celestialSPtr));
    }

    dynamicsArray.add(std::make_shared<PositionDerivative>());

    return dynamicsArray;
}

}  // namespace astro
}  // namespace ostk
