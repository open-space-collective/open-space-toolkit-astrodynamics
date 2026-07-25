/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/InTrack.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::mathematics::object::Matrix3d;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;

using ostk::astrodynamics::trajectory::State;

InTrack::InTrack(const Shared<EffectivityProvider>& aProviderSPtr)
    : GuidanceLaw("In-Track"),
      providerSPtr_(aProviderSPtr)
{
    if (providerSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Effectivity provider");
    }
}

InTrack::InTrack(const Shared<const QLaw>& aQLawSPtr)
    : GuidanceLaw("In-Track"),
      providerSPtr_(nullptr)
{
    if (aQLawSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("QLaw");
    }

    // Inherit thresholds from QLaw::Parameters (deprecated path) so the legacy two-arg form
    // remains behaviourally equivalent without callers having to construct the provider.
    providerSPtr_ = std::make_shared<effectivityprovider::QLaw>(aQLawSPtr);
}

InTrack::~InTrack() {}

std::ostream& operator<<(std::ostream& anOutputStream, const InTrack& aLaw)
{
    aLaw.print(anOutputStream);
    return anOutputStream;
}

void InTrack::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "In-Track Guidance Law") : void();

    GuidanceLaw::print(anOutputStream, false);

    ostk::core::utils::Print::Line(anOutputStream) << "Direction:"
                                                   << "auto-selected per step (±along-track in theta-R-H)";
    ostk::core::utils::Print::Line(anOutputStream) << "Provider:" << providerSPtr_->getName();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Shared<EffectivityProvider>& InTrack::accessProvider() const
{
    return providerSPtr_;
}

Shared<const QLaw> InTrack::accessQLaw() const
{
    const auto qLawProviderSPtr = std::dynamic_pointer_cast<effectivityprovider::QLaw>(providerSPtr_);
    if (qLawProviderSPtr == nullptr)
    {
        return nullptr;
    }
    return qLawProviderSPtr->accessQLaw();
}

Vector3d InTrack::calculateThrustAccelerationAt(
    const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Real& aThrustAcceleration,
    [[maybe_unused]] const Shared<const Frame>& outputFrameSPtr
) const
{
    static const Vector3d prograde = {1.0, 0.0, 0.0};
    static const Vector3d retrograde = {-1.0, 0.0, 0.0};

    const State stateGCRF = {
        anInstant,
        Position::Meters(aPositionCoordinates, Frame::GCRF()),
        Velocity::MetersPerSecond(aVelocityCoordinates, Frame::GCRF()),
    };

    // One batched provider call evaluates both candidate directions, sharing any per-state
    // precomputation (e.g. the QLaw provider's per-anomaly thrust-vector grid) and folding the
    // gate decision into the result so we do not repeat the work in a separate isEffective().
    const Array<Vector3d> candidates = {prograde, retrograde};
    const Array<EffectivityProvider::Evaluation> evaluations =
        providerSPtr_->evaluate(stateGCRF, candidates, aThrustAcceleration);

    const Real& scorePrograde = evaluations[0].score;
    const Real& scoreRetrograde = evaluations[1].score;

    if (!scorePrograde.isDefined() || !scoreRetrograde.isDefined())
    {
        throw ostk::core::error::RuntimeError(
            "InTrack requires an effectivity provider that supplies a directional score; "
            "got Undefined() for at least one of ±along-track."
        );
    }

    // Tie defaults to prograde.
    const bool usePrograde = scorePrograde >= scoreRetrograde;
    const Vector3d& direction = usePrograde ? prograde : retrograde;
    const bool gatePass = usePrograde ? evaluations[0].effective : evaluations[1].effective;

    if (!gatePass)
    {
        return Vector3d::Zero();
    }

    // Rotate the chosen theta-R-H direction into GCRF. Inputs are taken to be GCRF (existing
    // convention shared with QLaw).
    const Matrix3d R_thetaRH_GCRF = QLaw::ThetaRHToGCRF(aPositionCoordinates, aVelocityCoordinates);
    const Vector3d direction_GCRF = R_thetaRH_GCRF * direction;

    return aThrustAcceleration * direction_GCRF;
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
