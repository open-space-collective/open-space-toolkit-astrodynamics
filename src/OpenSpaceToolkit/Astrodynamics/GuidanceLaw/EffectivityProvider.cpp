/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

EffectivityProvider::EffectivityProvider(const String& aName)
    : name_(aName)
{
}

EffectivityProvider::~EffectivityProvider() {}

std::ostream& operator<<(std::ostream& anOutputStream, const EffectivityProvider& aProvider)
{
    aProvider.print(anOutputStream);
    return anOutputStream;
}

String EffectivityProvider::getName() const
{
    return name_;
}

void EffectivityProvider::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Effectivity Provider") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Real EffectivityProvider::computeScore(
    [[maybe_unused]] const State& aState,
    [[maybe_unused]] const Vector3d& aThrustDirectionThetaRH,
    [[maybe_unused]] const Real& aThrustAcceleration
) const
{
    return Real::Undefined();
}

Array<EffectivityProvider::Evaluation> EffectivityProvider::evaluate(
    const State& aState, const Array<Vector3d>& aThrustDirectionsThetaRH, const Real& aThrustAcceleration
) const
{
    Array<Evaluation> evaluations;
    evaluations.reserve(aThrustDirectionsThetaRH.getSize());

    for (const Vector3d& direction : aThrustDirectionsThetaRH)
    {
        evaluations.add(
            {this->computeScore(aState, direction, aThrustAcceleration),
             this->isEffective(aState, direction, aThrustAcceleration)}
        );
    }

    return evaluations;
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
