/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider/QLaw.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{
namespace effectivityprovider
{

using ostk::core::container::Tuple;

QLaw::QLaw(
    const Shared<const guidancelaw::QLaw>& aQLawSPtr,
    const Real& anAbsoluteEffectivityThreshold,
    const Real& aRelativeEffectivityThreshold
)
    : EffectivityProvider("QLaw Effectivity Provider"),
      qLawSPtr_(aQLawSPtr),
      absoluteEffectivityThreshold_(anAbsoluteEffectivityThreshold),
      relativeEffectivityThreshold_(aRelativeEffectivityThreshold)
{
    if (qLawSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("QLaw");
    }

    if (absoluteEffectivityThreshold_.isDefined() &&
        (absoluteEffectivityThreshold_ < 0.0 || absoluteEffectivityThreshold_ > 1.0))
    {
        throw ostk::core::error::RuntimeError("Absolute effectivity threshold must be within range [0.0, 1.0].");
    }

    if (relativeEffectivityThreshold_.isDefined() &&
        (relativeEffectivityThreshold_ < 0.0 || relativeEffectivityThreshold_ > 1.0))
    {
        throw ostk::core::error::RuntimeError("Relative effectivity threshold must be within range [0.0, 1.0].");
    }

    // Transitional fallback: if neither threshold is supplied directly, inherit from the
    // (deprecated) thresholds carried on QLaw::Parameters so existing call sites keep working
    // when migrating to the provider model.
    if (!absoluteEffectivityThreshold_.isDefined() && !relativeEffectivityThreshold_.isDefined())
    {
        const guidancelaw::QLaw::Parameters parameters = qLawSPtr_->getParameters();
        absoluteEffectivityThreshold_ = parameters.absoluteEffectivityThreshold;
        relativeEffectivityThreshold_ = parameters.relativeEffectivityThreshold;
    }
}

QLaw::~QLaw() {}

void QLaw::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "QLaw Effectivity Provider") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Absolute effectivity threshold:"
        << (absoluteEffectivityThreshold_.isDefined() ? absoluteEffectivityThreshold_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Relative effectivity threshold:"
        << (relativeEffectivityThreshold_.isDefined() ? relativeEffectivityThreshold_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Shared<const guidancelaw::QLaw>& QLaw::accessQLaw() const
{
    return qLawSPtr_;
}

Real QLaw::getAbsoluteEffectivityThreshold() const
{
    return absoluteEffectivityThreshold_;
}

Real QLaw::getRelativeEffectivityThreshold() const
{
    return relativeEffectivityThreshold_;
}

bool QLaw::isEffective(const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration)
    const
{
    if (!absoluteEffectivityThreshold_.isDefined() && !relativeEffectivityThreshold_.isDefined())
    {
        return true;
    }

    double etaRelative = 0.0;
    double etaAbsolute = 0.0;
    std::tie(etaRelative, etaAbsolute) =
        qLawSPtr_->computeEffectivity(aState, aThrustDirectionThetaRH, aThrustAcceleration);

    if (relativeEffectivityThreshold_.isDefined() && etaRelative < relativeEffectivityThreshold_)
    {
        return false;
    }

    if (absoluteEffectivityThreshold_.isDefined() && etaAbsolute < absoluteEffectivityThreshold_)
    {
        return false;
    }

    return true;
}

Real QLaw::computeScore(const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration)
    const
{
    double etaRelative = 0.0;
    double etaAbsolute = 0.0;
    std::tie(etaRelative, etaAbsolute) =
        qLawSPtr_->computeEffectivity(aState, aThrustDirectionThetaRH, aThrustAcceleration);

    return Real(etaRelative);
}

Array<EffectivityProvider::Evaluation> QLaw::evaluate(
    const State& aState, const Array<Vector3d>& aThrustDirectionsThetaRH, const Real& aThrustAcceleration
) const
{
    Array<Evaluation> evaluations;
    evaluations.reserve(aThrustDirectionsThetaRH.getSize());

    if (aThrustDirectionsThetaRH.isEmpty())
    {
        return evaluations;
    }

    const Array<Tuple<double, double>> effectivities =
        qLawSPtr_->computeEffectivities(aState, aThrustDirectionsThetaRH, aThrustAcceleration);

    const bool absoluteGateActive = absoluteEffectivityThreshold_.isDefined();
    const bool relativeGateActive = relativeEffectivityThreshold_.isDefined();

    for (const auto& effectivity : effectivities)
    {
        const double etaRelative = std::get<0>(effectivity);
        const double etaAbsolute = std::get<1>(effectivity);

        bool effective = true;
        if (relativeGateActive && etaRelative < relativeEffectivityThreshold_)
        {
            effective = false;
        }
        else if (absoluteGateActive && etaAbsolute < absoluteEffectivityThreshold_)
        {
            effective = false;
        }

        evaluations.add({Real(etaRelative), effective});
    }

    return evaluations;
}

}  // namespace effectivityprovider
}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
