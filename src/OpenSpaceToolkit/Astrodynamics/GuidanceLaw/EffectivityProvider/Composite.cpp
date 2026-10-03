/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider/Composite.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{
namespace effectivityprovider
{

Composite::Composite(const Array<Shared<EffectivityProvider>>& aProviders, const Size& aPrimaryIndex)
    : EffectivityProvider("Composite Effectivity Provider"),
      providers_(aProviders),
      primaryIndex_(aPrimaryIndex)
{
    if (providers_.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Composite effectivity provider requires at least one sub-provider.");
    }

    for (const auto& provider : providers_)
    {
        if (provider == nullptr)
        {
            throw ostk::core::error::runtime::Undefined("Sub-provider");
        }
    }

    if (primaryIndex_ >= providers_.getSize())
    {
        throw ostk::core::error::RuntimeError(
            "Primary provider index [" + std::to_string(primaryIndex_) + "] out of range [0, " +
            std::to_string(providers_.getSize()) + ")."
        );
    }
}

Composite::~Composite() {}

void Composite::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Composite Effectivity Provider") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Provider count:" << std::to_string(providers_.getSize());
    ostk::core::utils::Print::Line(anOutputStream) << "Primary index:" << std::to_string(primaryIndex_);

    for (Size i = 0; i < providers_.getSize(); ++i)
    {
        ostk::core::utils::Print::Line(anOutputStream)
            << ("Provider [" + std::to_string(i) + "]:") << providers_[i]->getName();
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Array<Shared<EffectivityProvider>>& Composite::accessProviders() const
{
    return providers_;
}

Size Composite::getPrimaryIndex() const
{
    return primaryIndex_;
}

bool Composite::isEffective(
    const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration
) const
{
    for (const auto& provider : providers_)
    {
        if (!provider->isEffective(aState, aThrustDirectionThetaRH, aThrustAcceleration))
        {
            return false;
        }
    }

    return true;
}

Real Composite::computeScore(
    const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration
) const
{
    return providers_[primaryIndex_]->computeScore(aState, aThrustDirectionThetaRH, aThrustAcceleration);
}

Array<EffectivityProvider::Evaluation> Composite::evaluate(
    const State& aState, const Array<Vector3d>& aThrustDirectionsThetaRH, const Real& aThrustAcceleration
) const
{
    const Size directionCount = aThrustDirectionsThetaRH.getSize();

    Array<Evaluation> evaluations;
    evaluations.reserve(directionCount);

    if (directionCount == 0)
    {
        return evaluations;
    }

    Array<Array<Evaluation>> subEvaluations;
    subEvaluations.reserve(providers_.getSize());
    for (const auto& provider : providers_)
    {
        subEvaluations.add(provider->evaluate(aState, aThrustDirectionsThetaRH, aThrustAcceleration));
    }

    for (Size i = 0; i < directionCount; ++i)
    {
        bool effective = true;
        for (const auto& providerEvaluations : subEvaluations)
        {
            if (!providerEvaluations[i].effective)
            {
                effective = false;
                break;
            }
        }

        evaluations.add({subEvaluations[primaryIndex_][i].score, effective});
    }

    return evaluations;
}

}  // namespace effectivityprovider
}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
