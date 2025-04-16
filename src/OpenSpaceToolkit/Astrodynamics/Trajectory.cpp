/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Static.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::unit::Length;

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

    Array<physics::coordinate::Position> positions = Array<physics::coordinate::Position>::Empty();

    for (const auto& instant : anInstantArray)
    {
        const Real ratio = (instant - anInstantArray.accessFirst()).inSeconds() / duration.inSeconds();

        const LLA intermediateLLA = aStartLLA.calculateIntermediateTo(
            anEndLLA, ratio, aCelestial.getEquatorialRadius(), aCelestial.getFlattening()
        );

        const physics::coordinate::Position position = physics::coordinate::Position::FromLLA(
            intermediateLLA,
            celestialSPtr
        ).inFrame(Frame::GCRF(), instant);

        positions.add(position);
    }

    const Array<Velocity> velocities = computeVelocities(positions, anInstantArray);

    Array<State> states = Array<State>::Empty();

    for (size_t i = 0; i < anInstantArray.getSize(); i++)
    {
        states.add(State(anInstantArray.at(i), positions.at(i), velocities.at(i)));
    }

    return Trajectory(states);
}

Trajectory Trajectory::GroundStripGeodeticNadir(
    const trajectory::Orbit& anOrbit, const Array<Instant>& anInstantArray, const Celestial& aCelestial
)
{
    if (!anOrbit.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Orbit");
    }

    if (anInstantArray.getSize() < 2)
    {
        throw ostk::core::error::RuntimeError("Atleast 2 instants must be provided.");
    }

    Array<physics::coordinate::Position> positions = Array<physics::coordinate::Position>::Empty();
    positions.reserve(anInstantArray.getSize());

    const Shared<Celestial> celestialSPtr = std::make_shared<Celestial>(aCelestial);

    for (const auto& instant : anInstantArray)
    {
        const State state = anOrbit.getStateAt(instant);

        const LLA lla = LLA::FromPosition(state.getPosition().inFrame(Frame::ITRF(), instant), celestialSPtr).onSurface();

        const physics::coordinate::Position position =
            physics::coordinate::Position::FromLLA(lla, celestialSPtr).inFrame(Frame::GCRF(), instant);

        positions.add(position);
    }

    Array<Velocity> velocities = computeVelocities(positions, anInstantArray);

    Array<State> states = Array<State>::Empty();

    for (size_t i = 0; i < anInstantArray.getSize(); i++)
    {
        states.add(State(anInstantArray.at(i), positions.at(i), velocities.at(i)));
    }

    return Trajectory(states);
}

Trajectory::Trajectory()
    : modelUPtr_(nullptr)
{
}

Array<Velocity> Trajectory::computeVelocities(
    const Array<physics::coordinate::Position>& aPositionArray, const Array<Instant>& anInstantArray
)
{
    Array<Velocity> velocities = Array<Velocity>::Empty();

    for (size_t i = 0; i < anInstantArray.getSize(); i++)
    {
        Vector3d velocityCoordinates = Vector3d::Zero();

        if (i == 0)
        {
            const physics::coordinate::Position currentPosition = aPositionArray.at(i);
            const physics::coordinate::Position nextPosition = aPositionArray.at(i + 1);
            const Instant currentInstant = anInstantArray.at(i);
            const Instant nextInstant = anInstantArray.at(i + 1);

            velocityCoordinates = (nextPosition.getCoordinates() - currentPosition.getCoordinates()) /
                                  (nextInstant - currentInstant).inSeconds();
        }

        else if (i == aPositionArray.getSize() - 1)
        {
            const physics::coordinate::Position currentPosition = aPositionArray.at(i);
            const physics::coordinate::Position previousPosition = aPositionArray.at(i - 1);
            const Instant currentInstant = anInstantArray.at(i);
            const Instant previousInstant = anInstantArray.at(i - 1);

            velocityCoordinates = (currentPosition.getCoordinates() - previousPosition.getCoordinates()) /
                                  (currentInstant - previousInstant).inSeconds();
        }

        else
        {
            const physics::coordinate::Position nextPosition = aPositionArray.at(i + 1);
            const physics::coordinate::Position previousPosition = aPositionArray.at(i - 1);
            const Instant nextInstant = anInstantArray.at(i + 1);
            const Instant previousInstant = anInstantArray.at(i - 1);

            velocityCoordinates = (nextPosition.getCoordinates() - previousPosition.getCoordinates()) /
                                  (nextInstant - previousInstant).inSeconds();
        }

        const Velocity velocity = Velocity::MetersPerSecond(velocityCoordinates, aPositionArray.at(i).accessFrame());

        velocities.add(velocity);
    }

    return velocities;
}

}  // namespace astrodynamics
}  // namespace ostk
