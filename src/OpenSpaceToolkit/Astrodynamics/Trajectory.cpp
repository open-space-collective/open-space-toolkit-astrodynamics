/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Nadir.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Static.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/TargetScan.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::model::Nadir;
using TargetScanModel = ostk::astrodynamics::trajectory::model::TargetScan;
using ostk::astrodynamics::trajectory::model::Static;
using ostk::astrodynamics::trajectory::model::Tabulated;
using ostk::astrodynamics::trajectory::Orbit;

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
    if (!aPosition.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Position");
    }

    return Trajectory(Static(aPosition));
}

Trajectory Trajectory::GroundStrip(
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

    if ((aStartLLA.getAltitude().inMeters() > Real::Epsilon()) || (anEndLLA.getAltitude().inMeters() > Real::Epsilon()))
    {
        throw ostk::core::error::RuntimeError("LLA altitude must be zero.");
    }

    const Length distance =
        aStartLLA.calculateDistanceTo(anEndLLA, aCelestial.getEquatorialRadius(), aCelestial.getFlattening());

    const Duration duration = Duration::Seconds(distance.inMeters() / aGroundSpeed.in(Derived::Unit::MeterPerSecond()));

    const Instant endInstant = aStartInstant + duration;

    const Interval interval = Interval::Closed(aStartInstant, endInstant);

    const Array<Instant> instants = interval.generateGrid(aStepSize);

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

    const Duration duration = anInstantArray.accessLast() - anInstantArray.accessFirst();

    const Shared<Celestial> celestialSPtr = std::make_shared<Celestial>(aCelestial);

    const auto positionGenerator = [&aStartLLA,
                                    &anEndLLA,
                                    &celestialSPtr,
                                    startInstant = anInstantArray.accessFirst(),
                                    duration = duration](const Instant& anInstant) -> physics::coordinate::Position
    {
        const Real ratio = (anInstant - startInstant).inSeconds() / duration.inSeconds();

        const LLA intermediateLLA = aStartLLA.calculateIntermediateTo(
            anEndLLA, ratio, celestialSPtr->getEquatorialRadius(), celestialSPtr->getFlattening()
        );

        return physics::coordinate::Position::FromLLA(intermediateLLA, celestialSPtr).inFrame(Frame::GCRF(), anInstant);
    };

    const Array<State> states = computeStates(positionGenerator, anInstantArray);

    return Trajectory(states);
}

Trajectory Trajectory::GroundStripGeodeticNadir(
    const trajectory::Orbit& anOrbit,
    [[maybe_unused]] const Array<Instant>& anInstantArray,
    [[maybe_unused]] const Celestial& aCelestial
)
{
    [[deprecated("Use Trajectory::GeodeticNadirGroundTrack instead.")]] return Trajectory(Nadir(anOrbit));
}

Trajectory Trajectory::TargetScan(
    const LLA& aStartLLA,
    const LLA& anEndLLA,
    const Instant& aStartInstant,
    const Instant& anEndInstant,
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

    if (!aStartInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Start instant");
    }

    if (!anEndInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("End instant");
    }

    if (!aCelestial.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Celestial");
    }

    return Trajectory(TargetScanModel(aStartLLA, anEndLLA, aStartInstant, anEndInstant, aCelestial));
}

Trajectory Trajectory::TargetScan(
    const LLA& aStartLLA,
    const LLA& anEndLLA,
    const Derived& aGroundSpeed,
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
        throw ostk::core::error::runtime::Undefined("Ground Speed");
    }

    if (!aStartInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Start instant");
    }

    if (!aCelestial.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Celestial");
    }

    const Real groundSpeedMps = aGroundSpeed.in(Derived::Unit::MeterPerSecond());

    const Length distance =
        aStartLLA.calculateDistanceTo(anEndLLA, aCelestial.getEquatorialRadius(), aCelestial.getFlattening());

    const Duration duration = Duration::Seconds(distance.inMeters() / groundSpeedMps);

    const Instant endInstant = aStartInstant + duration;

    return Trajectory(TargetScanModel(aStartLLA, anEndLLA, aStartInstant, endInstant, aCelestial));
}

Trajectory::Trajectory()
    : modelUPtr_(nullptr)
{
}

Array<State> Trajectory::computeStates(
    const std::function<physics::coordinate::Position(const Instant&)>& aPositionFunction,
    const Array<Instant>& anInstantArray,
    const Duration& aStepSize
)
{
    Array<State> states = Array<State>::Empty();
    states.reserve(anInstantArray.getSize());

    Vector3d velocityCoordinates = Vector3d::Zero();
    for (Size i = 0; i < anInstantArray.getSize(); i++)
    {
        const Instant currentInstant = anInstantArray.at(i);
        physics::coordinate::Position currentPosition = aPositionFunction(currentInstant);

        if (i == 0)
        {
            const Instant nextInstant = currentInstant + aStepSize;
            const physics::coordinate::Position nextPosition = aPositionFunction(nextInstant);

            velocityCoordinates =
                (nextPosition.getCoordinates() - currentPosition.getCoordinates()) / aStepSize.inSeconds();
        }

        else if (i == anInstantArray.getSize() - 1)
        {
            const Instant previousInstant = currentInstant - aStepSize;
            const physics::coordinate::Position previousPosition = aPositionFunction(previousInstant);

            velocityCoordinates =
                (currentPosition.getCoordinates() - previousPosition.getCoordinates()) / aStepSize.inSeconds();
        }

        else
        {
            const Instant nextInstant = currentInstant + aStepSize;
            const Instant previousInstant = currentInstant - aStepSize;
            const physics::coordinate::Position nextPosition = aPositionFunction(nextInstant);
            const physics::coordinate::Position previousPosition = aPositionFunction(previousInstant);

            velocityCoordinates =
                (nextPosition.getCoordinates() - previousPosition.getCoordinates()) / (2.0 * aStepSize.inSeconds());
        }

        const Velocity currentVelocity = Velocity::MetersPerSecond(velocityCoordinates, currentPosition.accessFrame());

        states.add(State(currentInstant, currentPosition, currentVelocity).inFrame(Frame::GCRF()));
    }

    return states;
}

}  // namespace astrodynamics
}  // namespace ostk
