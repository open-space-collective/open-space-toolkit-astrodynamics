/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/TargetScan.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Position;
using ostk::physics::unit::Derived;

TargetScan::TargetScan(
    const LLA& aStartLLA,
    const LLA& anEndLLA,
    const Instant& aStartInstant,
    const Instant& anEndInstant,
    const Celestial& aCelestial,
    const Duration& aStepSize
)
    : Model(),
      startLLA_(aStartLLA),
      endLLA_(anEndLLA),
      startInstant_(aStartInstant),
      endInstant_(anEndInstant),
      celestialSPtr_(std::make_shared<const Celestial>(aCelestial)),
      stepSize_(aStepSize)
{
}

TargetScan* TargetScan::clone() const
{
    return new TargetScan(*this);
}

bool TargetScan::operator==(const TargetScan& aTargetScanModel) const
{
    if ((!this->isDefined()) || (!aTargetScanModel.isDefined()))
    {
        return false;
    }

    return startLLA_ == aTargetScanModel.startLLA_ && endLLA_ == aTargetScanModel.endLLA_ &&
           startInstant_ == aTargetScanModel.startInstant_ && endInstant_ == aTargetScanModel.endInstant_ &&
           celestialSPtr_ == aTargetScanModel.celestialSPtr_ && stepSize_ == aTargetScanModel.stepSize_;
}

bool TargetScan::operator!=(const TargetScan& aTargetScanModel) const
{
    return !((*this) == aTargetScanModel);
}

std::ostream& operator<<(std::ostream& anOutputStream, const TargetScan& aTargetScanModel)
{
    aTargetScanModel.print(anOutputStream);

    return anOutputStream;
}

bool TargetScan::isDefined() const
{
    return startLLA_.isDefined() && endLLA_.isDefined() && startInstant_.isDefined() && endInstant_.isDefined() &&
           celestialSPtr_ != nullptr && celestialSPtr_->isDefined() && stepSize_.isDefined();
}

State TargetScan::calculateStateAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TargetScan");
    }

    if (anInstant < startInstant_ || anInstant > endInstant_)
    {
        throw ostk::core::error::RuntimeError("Instant is outside the interval.");
    }

    // interpolate the velocity using polynomial interpolation

    const Array<Instant> instants = {
        anInstant - stepSize_ * 2,
        anInstant - stepSize_,
        anInstant,
        anInstant + stepSize_,
        anInstant + stepSize_ * 2,
    };

    VectorXd times(instants.getSize());
    MatrixXd positionCoordinates(3, instants.getSize());

    Position position = Position::Undefined();

    const Duration intervalDuration = endInstant_ - startInstant_;
    const Length scanDistance =
        startLLA_.calculateDistanceTo(endLLA_, celestialSPtr_->getEquatorialRadius(), celestialSPtr_->getFlattening());
    const Real scanSpeed = scanDistance.inMeters() / intervalDuration.inSeconds();

    const Pair<Angle, Angle> azimuthAngles =
        startLLA_.calculateAzimuthTo(endLLA_, celestialSPtr_->getEquatorialRadius(), celestialSPtr_->getFlattening());

    Size i = 0;
    for (const auto& instant : instants)
    {
        times(i) = (instant - anInstant).inSeconds();

        const Length forwardDistance = Length::Meters(scanSpeed * (instant - startInstant_).inSeconds());

        const LLA lla = LLA::Forward(
            startLLA_,
            azimuthAngles.first,
            forwardDistance,
            celestialSPtr_->getEquatorialRadius(),
            celestialSPtr_->getFlattening()
        );

        const physics::coordinate::Position positionAtInstant =
            physics::coordinate::Position::FromLLA(lla, celestialSPtr_).inFrame(Frame::GCRF(), instant);

        if (instant == anInstant)
        {
            position = positionAtInstant;
        }

        positionCoordinates.col(i) = positionAtInstant.getCoordinates();

        ++i;
    }

    Array<Shared<const Interpolator>> interpolators = {
        Interpolator::GenerateInterpolator(Interpolator::Type::CubicSpline, times, positionCoordinates.row(0)),
        Interpolator::GenerateInterpolator(Interpolator::Type::CubicSpline, times, positionCoordinates.row(1)),
        Interpolator::GenerateInterpolator(Interpolator::Type::CubicSpline, times, positionCoordinates.row(2)),
    };

    const Vector3d velocityCoordinates = {
        interpolators[0]->computeDerivative(0.0),
        interpolators[1]->computeDerivative(0.0),
        interpolators[2]->computeDerivative(0.0),
    };

    const Velocity velocity = Velocity::MetersPerSecond(velocityCoordinates, Frame::GCRF());

    return State(anInstant, position, velocity);
}

LLA TargetScan::getStartLLA() const
{
    return startLLA_;
}

LLA TargetScan::getEndLLA() const
{
    return endLLA_;
}

Instant TargetScan::getStartInstant() const
{
    return startInstant_;
}

Instant TargetScan::getEndInstant() const
{
    return endInstant_;
}

Celestial TargetScan::getCelestial() const
{
    return *celestialSPtr_;
}

Duration TargetScan::getStepSize() const
{
    return stepSize_;
}

void TargetScan::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::type::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "TargetScan") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Start LLA:" << (startLLA_.isDefined() ? startLLA_.toString() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "End LLA:" << (endLLA_.isDefined() ? endLLA_.toString() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Start Instant:" << (startInstant_.isDefined() ? startInstant_.toString() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "End Instant:" << (endInstant_.isDefined() ? endInstant_.toString() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Celestial:"
        << (celestialSPtr_ != nullptr && celestialSPtr_->isDefined() ? celestialSPtr_->getName() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Step Size:" << (stepSize_.isDefined() ? stepSize_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

bool TargetScan::operator==(const Model& aModel) const
{
    const TargetScan* staticModelPtr = dynamic_cast<const TargetScan*>(&aModel);

    return (staticModelPtr != nullptr) && this->operator==(*staticModelPtr);
}

bool TargetScan::operator!=(const Model& aModel) const
{
    return !((*this) == aModel);
}

TargetScan TargetScan::FromGroundSpeed(
    const LLA& aStartLLA,
    const LLA& anEndLLA,
    const Derived& aGroundSpeed,
    const Instant& aStartInstant,
    const Celestial& aCelestial,
    const Duration& aStepSize
)
{
    if (!aStartLLA.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Start LLA");
    }

    if (!anEndLLA.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("End LLA");
    }

    if (!aGroundSpeed.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Ground speed");
    }

    if (!aStartInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Start instant");
    }

    if (!aStepSize.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Step size");
    }

    if (aStepSize.inSeconds() <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Step size must be positive.");
    }

    const Real groundSpeedMps = aGroundSpeed.in(Derived::Unit::MeterPerSecond());

    if (groundSpeedMps <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Ground speed must be positive.");
    }

    const Length distance =
        aStartLLA.calculateDistanceTo(anEndLLA, aCelestial.getEquatorialRadius(), aCelestial.getFlattening());

    const Duration duration = Duration::Seconds(distance.inMeters() / groundSpeedMps);

    const Instant endInstant = aStartInstant + duration;

    return TargetScan(aStartLLA, anEndLLA, aStartInstant, endInstant, aCelestial, aStepSize);
}

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
