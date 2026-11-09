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
    : modelUPtr_(aTrajectory.modelUPtr_->clone())
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

const Model& Trajectory::getModel() const
{
    return *modelUPtr_;
}

State Trajectory::getStateAt(const Instant& anInstant) const
{
    return modelUPtr_->calculateStateAt(anInstant);
}

Array<State> Trajectory::getStatesAt(const Array<Instant>& anInstantArray) const
{
    return modelUPtr_->calculateStatesAt(anInstantArray);
}

void Trajectory::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory") : void();

    ostk::core::utils::Print::Separator(anOutputStream, "Model");

    modelUPtr_->print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Trajectory Trajectory::Position(const physics::coordinate::Position& aPosition)
{
    return Trajectory(Static(aPosition));
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

        const Velocity currentVelocity = Velocity::MetersPerSecond(velocityCoordinates, currentPosition.getFrame());

        states.add(State(currentInstant, currentPosition, currentVelocity).inFrame(Frame::GCRF()));
    }

    return states;
}

}  // namespace astrodynamics
}  // namespace ostk
