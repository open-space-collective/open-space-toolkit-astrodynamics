/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityGatedConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::container::Tuple;

using ostk::mathematics::object::Matrix3d;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;

using ostk::astrodynamics::trajectory::State;

EffectivityGatedConstantThrust::EffectivityGatedConstantThrust(
    const Vector3d& aThrustDirectionThetaRH, const Shared<const QLaw>& aQLawSPtr
)
    : GuidanceLaw("Effectivity-Gated Constant Thrust"),
      thrustDirectionThetaRH_({0.0, 0.0, 0.0}),
      qLawSPtr_(aQLawSPtr)
{
    const double norm = aThrustDirectionThetaRH.norm();
    if (norm < 1e-12)
    {
        throw ostk::core::error::runtime::Undefined("Thrust direction");
    }

    if (qLawSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("QLaw");
    }

    thrustDirectionThetaRH_ = aThrustDirectionThetaRH / norm;
}

EffectivityGatedConstantThrust::~EffectivityGatedConstantThrust() {}

std::ostream& operator<<(std::ostream& anOutputStream, const EffectivityGatedConstantThrust& aLaw)
{
    aLaw.print(anOutputStream);
    return anOutputStream;
}

void EffectivityGatedConstantThrust::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Effectivity-Gated Constant Thrust Guidance Law")
                     : void();

    GuidanceLaw::print(anOutputStream, false);

    ostk::core::utils::Print::Line(anOutputStream)
        << "Thrust direction (theta-R-H):" << thrustDirectionThetaRH_.transpose();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Shared<const QLaw>& EffectivityGatedConstantThrust::accessQLaw() const
{
    return qLawSPtr_;
}

Vector3d EffectivityGatedConstantThrust::getThrustDirectionThetaRH() const
{
    return thrustDirectionThetaRH_;
}

Vector3d EffectivityGatedConstantThrust::calculateThrustAccelerationAt(
    const Instant& anInstant,
    const Vector3d& aPositionCoordinates,
    const Vector3d& aVelocityCoordinates,
    const Real& aThrustAcceleration,
    [[maybe_unused]] const Shared<const Frame>& outputFrameSPtr
) const
{
    const QLaw::Parameters parameters = qLawSPtr_->getParameters();
    const bool gateActive =
        parameters.relativeEffectivityThreshold.isDefined() || parameters.absoluteEffectivityThreshold.isDefined();

    if (gateActive)
    {
        const State stateGCRF = {
            anInstant,
            Position::Meters(aPositionCoordinates, Frame::GCRF()),
            Velocity::MetersPerSecond(aVelocityCoordinates, Frame::GCRF()),
        };

        double etaRelative = 0.0;
        double etaAbsolute = 0.0;
        std::tie(etaRelative, etaAbsolute) =
            qLawSPtr_->computeEffectivity(stateGCRF, thrustDirectionThetaRH_, aThrustAcceleration);

        if (parameters.relativeEffectivityThreshold.isDefined() &&
            etaRelative < parameters.relativeEffectivityThreshold)
        {
            return Vector3d::Zero();
        }

        if (parameters.absoluteEffectivityThreshold.isDefined() &&
            etaAbsolute < parameters.absoluteEffectivityThreshold)
        {
            return Vector3d::Zero();
        }
    }

    // Rotate the constant theta-R-H direction into GCRF. Mirrors QLaw::calculateThrustAccelerationAt
    // (QLaw.cpp:186-188): inputs and outputs are taken to be in GCRF (same implicit assumption as
    // the sibling class).
    const Matrix3d R_thetaRH_GCRF = QLaw::ThetaRHToGCRF(aPositionCoordinates, aVelocityCoordinates);
    const Vector3d direction_GCRF = R_thetaRH_GCRF * thrustDirectionThetaRH_;

    return aThrustAcceleration * direction_GCRF;
}

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk
