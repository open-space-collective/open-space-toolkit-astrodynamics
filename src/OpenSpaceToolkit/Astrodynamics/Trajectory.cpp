/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Static.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::model::Tabulated;

Trajectory::Trajectory(const Model& aModel)
    : modelUPtr_(aModel.clone())
{
}

Trajectory::Trajectory(const Array<State>& aStateArray)
    : modelUPtr_(std::make_unique<Tabulated>(aStateArray))
{
}

Trajectory::Trajectory(const Trajectory& aTrajectory)
    : modelUPtr_((aTrajectory.modelUPtr_) != nullptr ? aTrajectory.modelUPtr_->clone() : nullptr)
{
}

Trajectory& Trajectory::operator=(const Trajectory& aTrajectory)
{
    if (this != &aTrajectory)
    {
        modelUPtr_ = Unique<Model>(aTrajectory.modelUPtr_->clone());
    }

    return *this;
}

bool Trajectory::operator==(const Trajectory& aTrajectory) const
{
    if ((!this->isDefined()) || (!aTrajectory.isDefined()))
    {
        return false;
    }

    return (*modelUPtr_) == (*aTrajectory.modelUPtr_);
}

bool Trajectory::operator!=(const Trajectory& aTrajectory) const
{
    return !((*this) == aTrajectory);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Trajectory& aTrajectory)
{
    aTrajectory.print(anOutputStream);

    return anOutputStream;
}

bool Trajectory::isDefined() const
{
    return (modelUPtr_ != nullptr) && modelUPtr_->isDefined();
}

const Model& Trajectory::accessModel() const
{
    // if (!this->isDefined())
    // {
    //     throw ostk::core::error::runtime::Undefined("Trajectory");
    // }

    if (modelUPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Model");
    }

    return *modelUPtr_;
}

State Trajectory::getStateAt(const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }

    return modelUPtr_->calculateStateAt(anInstant);
}

Array<State> Trajectory::getStatesAt(const Array<Instant>& anInstantArray) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }

    return modelUPtr_->calculateStatesAt(anInstantArray);
}

void Trajectory::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory") : void();

    ostk::core::utils::Print::Separator(anOutputStream, "Model");

    modelUPtr_->print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Trajectory Trajectory::Undefined()
{
    return Trajectory();
}

Trajectory Trajectory::Position(const physics::coordinate::Position& aPosition)
{
    using ostk::astrodynamics::trajectory::model::Static;

    if (!aPosition.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Position");
    }

    return Trajectory(Static(aPosition));
}

Trajectory Trajectory::GroundStrip(
    const LLA& aStartLLA,
    const LLA& anEndLLA,
    const Real& aGroundSpeed,
    const Instant& aStartInstant,
    const Celestial& aCelestial
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

    if ((aStartLLA.getAltitude().inMeters() > Real::Epsilon()) || (anEndLLA.getAltitude().inMeters() > Real::Epsilon()))
    {
        throw ostk::core::error::RuntimeError("LLA altitude must be zero.");
    }

    const Length distance =
        aStartLLA.calculateDistanceTo(anEndLLA, aCelestial.getEquatorialRadius(), aCelestial.getFlattening());

    const Duration duration = Duration::Seconds(distance.inMeters() / aGroundSpeed);

    const Instant endInstant = aStartInstant + duration;

    const Interval interval = Interval::Closed(aStartInstant, endInstant);

    const Array<Instant> instants = interval.generateGrid(Duration::Seconds(1.0));  // TBI: Make a param?

    return GroundStrip(aStartLLA, anEndLLA, instants, aCelestial);
}

Trajectory Trajectory::GroundStrip(
    const LLA& aStartLLA, const LLA& anEndLLA, const Array<Instant>& anInstantArray, const Celestial& aCelestial
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

    if (anInstantArray.getSize() < 2)
    {
        throw ostk::core::error::RuntimeError("Atleast 2 instants must be provided.");
    }

    if ((aStartLLA.getAltitude().inMeters() > Real::Epsilon()) || (anEndLLA.getAltitude().inMeters() > Real::Epsilon()))
    {
        throw ostk::core::error::RuntimeError("LLA altitude must be zero.");
    }

    const Velocity velocity = Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF());

    Array<State> states = Array<State>::Empty();

    const Duration duration = anInstantArray.accessLast() - anInstantArray.accessFirst();

    for (const auto& instant : anInstantArray)
    {
        const Real ratio = (instant - anInstantArray.accessFirst()).inSeconds() / duration.inSeconds();

        const LLA intermediateLLA = aStartLLA.calculateIntermediateTo(
            anEndLLA, ratio, aCelestial.getEquatorialRadius(), aCelestial.getFlattening()
        );

        const physics::coordinate::Position position = physics::coordinate::Position::Meters(
            intermediateLLA.toCartesian(aCelestial.getEquatorialRadius(), aCelestial.getFlattening()), Frame::ITRF()
        );

        const State state = State(instant, position, velocity).inFrame(Frame::GCRF());

        states.add(state);
    }

    return Trajectory(states);
}

Trajectory::Trajectory()
    : modelUPtr_(nullptr)
{
}

}  // namespace astrodynamics
}  // namespace ostk
