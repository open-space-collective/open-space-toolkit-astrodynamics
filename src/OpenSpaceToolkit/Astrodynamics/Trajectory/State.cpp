/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Index;

using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

State::State(
    const Instant& anInstant,
    const VectorXd& aCoordinates,
    const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
)
    : instant_(anInstant),
      coordinates_(aCoordinates),
      frameSPtr_(aFrameSPtr),
      coordinatesBrokerSPtr_(aCoordinatesBrokerSPtr)
{
}

State::State(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity)
    : instant_(anInstant)
{
    if (!aPosition.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Position");
    }

    if (!aVelocity.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Velocity");
    }

    if (aPosition.accessFrame() != aVelocity.accessFrame())
    {
        throw ostk::core::error::runtime::Wrong("Position-Velocity Frames");
    }

    VectorXd coordinates(6);
    coordinates.segment(0, 3) = aPosition.inUnit(Position::Unit::Meter).accessCoordinates();
    coordinates.segment(3, 3) = aVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

    static const Shared<CoordinatesBroker> coordinatesBrokerSPtr =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker({
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
        }));

    this->coordinates_ = coordinates;
    this->frameSPtr_ = aPosition.accessFrame();
    this->coordinatesBrokerSPtr_ = coordinatesBrokerSPtr;
}

bool State::operator==(const State& aState) const
{
    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        return false;
    }

    if (this->instant_ != aState.instant_)
    {
        return false;
    }

    if (this->frameSPtr_ != aState.frameSPtr_)
    {
        return false;
    }

    for (const Shared<const CoordinatesSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        if (!aState.coordinatesBrokerSPtr_->hasSubset(subset))
        {
            return false;
        }

        if (this->extractCoordinates(subset) != aState.extractCoordinates(subset))
        {
            return false;
        }
    }

    return true;
}

bool State::operator!=(const State& aState) const
{
    return !((*this) == aState);
}

State State::operator+(const State& aState) const
{
    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (this->instant_ != aState.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if (this->frameSPtr_ != aState.frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if (this->getSize() != aState.getSize())
    {
        throw ostk::core::error::runtime::Wrong("Size");
    }

    VectorXd addedCoordinates = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Index i = 0;
    for (const Shared<const CoordinatesSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        Size subsetSize = subset->getSize();
        addedCoordinates.segment(i, subsetSize) = subset->add(
            this->instant_,
            this->coordinates_,
            aState.accessCoordinates(),
            this->frameSPtr_,
            this->coordinatesBrokerSPtr_
        );
        i += subsetSize;
    }

    return {
        this->instant_,
        addedCoordinates,
        this->frameSPtr_,
        this->coordinatesBrokerSPtr_,
    };
}

State State::operator-(const State& aState) const
{
    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (this->instant_ != aState.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if (this->frameSPtr_ != aState.frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if (this->getSize() != aState.getSize())
    {
        throw ostk::core::error::runtime::Wrong("Size");
    }

    VectorXd subtractedCoordinates = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Index i = 0;
    for (const Shared<const CoordinatesSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        Size subsetSize = subset->getSize();
        subtractedCoordinates.segment(i, subsetSize) = subset->subtract(
            this->instant_,
            this->coordinates_,
            aState.accessCoordinates(),
            this->frameSPtr_,
            this->coordinatesBrokerSPtr_
        );
        i += subsetSize;
    }

    return {
        this->instant_,
        subtractedCoordinates,
        this->frameSPtr_,
        this->coordinatesBrokerSPtr_,
    };
}

std::ostream& operator<<(std::ostream& anOutputStream, const State& aState)
{
    aState.print(anOutputStream);

    return anOutputStream;
}

bool State::isDefined() const
{
    return this->instant_.isDefined() && this->coordinates_.isDefined() && (this->frameSPtr_ != nullptr) &&
           this->frameSPtr_->isDefined() && (this->coordinatesBrokerSPtr_ != nullptr);
}

const Instant& State::accessInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->instant_;
}

const Shared<const Frame> State::accessFrame() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->frameSPtr_;
}

const VectorXd& State::accessCoordinates() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinates_;
}

const Shared<const CoordinatesBroker>& State::accessCoordinatesBroker() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinatesBrokerSPtr_;
}

Size State::getSize() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinates_.size();
}

Instant State::getInstant() const
{
    return this->accessInstant();
}

Shared<const Frame> State::getFrame() const
{
    return this->accessFrame();
}

Position State::getPosition() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return Position::Meters(this->extractCoordinates(CartesianPosition::Default()), this->frameSPtr_);
}

Velocity State::getVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return Velocity::MetersPerSecond(this->extractCoordinates(CartesianVelocity::Default()), this->frameSPtr_);
}

VectorXd State::getCoordinates() const
{
    return this->accessCoordinates();
}

VectorXd State::extractCoordinates(const Shared<const CoordinatesSubset>& aSubetSPtr) const
{
    return this->coordinatesBrokerSPtr_->extractCoordinates(this->accessCoordinates(), aSubetSPtr);
}

State State::inFrame(const Shared<const Frame>& aFrameSPtr) const
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (aFrameSPtr == this->frameSPtr_)
    {
        return {this->instant_, this->coordinates_, this->frameSPtr_, this->coordinatesBrokerSPtr_};
    }

    VectorXd inFrameCoordinates = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Index i = 0;
    for (const Shared<const CoordinatesSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        const VectorXd subsetInFrame = subset->inFrame(
            this->instant_, this->coordinates_, this->frameSPtr_, aFrameSPtr, this->coordinatesBrokerSPtr_
        );

        inFrameCoordinates.segment(i, subsetInFrame.size()) = subsetInFrame;
        i += subsetInFrame.size();
    }

    return {
        this->instant_,
        inFrameCoordinates,
        aFrameSPtr,
        this->coordinatesBrokerSPtr_,
    };
}

void State::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::types::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory :: State") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->instant_.isDefined() ? this->instant_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Position:" << (this->isDefined() ? this->getPosition().toString(12) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Velocity:" << (this->isDefined() ? this->getVelocity().toString(12) : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

State State::Undefined()
{
    return {Instant::Undefined(), VectorXd(0), Frame::Undefined(), nullptr};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
