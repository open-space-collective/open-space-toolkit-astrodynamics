/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityGatedGuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
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

EffectivityGatedGuidanceLaw::EffectivityGatedGuidanceLaw(
    const Shared<const GuidanceLaw>& aBaseLawSPtr, const Shared<EffectivityProvider>& aProviderSPtr
)
    : GuidanceLaw("Effectivity-Gated Guidance Law"),
      baseLawSPtr_(aBaseLawSPtr),
      providerSPtr_(aProviderSPtr)
{
    if (baseLawSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Base guidance law");
    }

    if (providerSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Effectivity provider");
    }
}

EffectivityGatedGuidanceLaw::~EffectivityGatedGuidanceLaw() {}

std::ostream& operator<<(std::ostream& anOutputStream, const EffectivityGatedGuidanceLaw& aLaw)
{
    aLaw.print(anOutputStream);
    return anOutputStream;
}

void EffectivityGatedGuidanceLaw::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Effectivity-Gated Guidance Law") : void();

    GuidanceLaw::print(anOutputStream, false);

    ostk::core::utils::Print::Line(anOutputStream) << "Base law:" << baseLawSPtr_->getName();
    ostk::core::utils::Print::Line(anOutputStream) << "Provider:" << providerSPtr_->getName();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Shared<const GuidanceLaw>& EffectivityGatedGuidanceLaw::accessBaseLaw() const
{
    return baseLawSPtr_;
}

const Shared<EffectivityProvider>& EffectivityGatedGuidanceLaw::accessProvider() const
{
    return providerSPtr_;
}

Vector3d EffectivityGatedGuidanceLaw::calculateThrustAccelerationAt(
    const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Real& aThrustAcceleration,
    const Shared<const Frame>& outputFrameSPtr
) const
{
    const Vector3d baseAcceleration = baseLawSPtr_->calculateThrustAccelerationAt(
        anInstant, aPositionCoordinates, aVelocityCoordinates, aThrustAcceleration, outputFrameSPtr
    );

    const double baseNorm = baseAcceleration.norm();
    if (baseNorm < 1e-12)
    {
        return Vector3d::Zero();
    }

    // Express the base law's direction in theta-R-H so that direction-aware providers (e.g. QLaw)
    // can reason about it. Inputs are taken to be GCRF — same convention as QLaw and the sibling
    // gated laws (see EffectivityGatedConstantThrust.cpp / InTrack.cpp).
    const Matrix3d R_thetaRH_GCRF = QLaw::ThetaRHToGCRF(aPositionCoordinates, aVelocityCoordinates);
    const Vector3d direction_thetaRH = R_thetaRH_GCRF.transpose() * (baseAcceleration / baseNorm);

    const State stateGCRF = {
        anInstant,
        Position::Meters(aPositionCoordinates, Frame::GCRF()),
        Velocity::MetersPerSecond(aVelocityCoordinates, Frame::GCRF()),
    };

    if (!providerSPtr_->isEffective(stateGCRF, direction_thetaRH, aThrustAcceleration))
    {
        return Vector3d::Zero();
    }

    return baseAcceleration;
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
