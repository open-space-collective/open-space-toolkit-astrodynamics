/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

const Shared<const CartesianPosition> State::CARTESIAN_POSITION = CartesianPosition::ThreeDimensional();
const Shared<const CartesianVelocity> State::CARTESIAN_VELOCITY = CartesianVelocity::ThreeDimensional();
const Shared<const CoordinatesBroker> State::CARTESIAN_POSVEL_COORDINATES_BROKER =
    CoordinatesBroker::FromSubsets({CARTESIAN_POSITION, CARTESIAN_VELOCITY});

State::State(
    const Instant& anInstant,
    const VectorXd& aCoordinates,
    const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker> aCoordinatesBrokerSPtr
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

    this->coordinates_ = coordinates;
    this->frameSPtr_ = aPosition.accessFrame();
    this->coordinatesBrokerSPtr_ = State::CARTESIAN_POSVEL_COORDINATES_BROKER;
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

    if (this->coordinates_ != aState.coordinates_)
    {
        return false;
    }

    if (this->coordinatesBrokerSPtr_ == aState.coordinatesBrokerSPtr_)
    {
        return true;
    }

    if (*this->coordinatesBrokerSPtr_ != *aState.coordinatesBrokerSPtr_)
    {
        return false;
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

    if (this->coordinates_.size() != aState.coordinates_.size())
    {
        throw ostk::core::error::runtime::Wrong("Coordinates size");
    }

    return {this->instant_, this->coordinates_ + aState.coordinates_, this->frameSPtr_, this->coordinatesBrokerSPtr_};
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

    if (this->coordinates_.size() != aState.coordinates_.size())
    {
        throw ostk::core::error::runtime::Wrong("Coordinates size");
    }

    return {this->instant_, this->coordinates_ - aState.coordinates_, this->frameSPtr_, this->coordinatesBrokerSPtr_};
}

std::ostream& operator<<(std::ostream& anOutputStream, const State& aState)
{
    aState.print(anOutputStream);

    return anOutputStream;
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

    return Position::Meters(
        this->coordinates_.segment(this->coordinatesBrokerSPtr_->getSubsetIndex(State::CARTESIAN_POSITION), 3),
        this->frameSPtr_
    );
}

Velocity State::getVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return Velocity::MetersPerSecond(
        this->coordinates_.segment(this->coordinatesBrokerSPtr_->getSubsetIndex(State::CARTESIAN_VELOCITY), 3),
        this->frameSPtr_
    );
}

VectorXd State::getCoordinates() const
{
    return this->accessCoordinates();
}

bool State::isDefined() const
{
    return this->instant_.isDefined() && this->coordinates_.isDefined() && (this->frameSPtr_ != nullptr) &&
           this->frameSPtr_->isDefined() && (this->coordinatesBrokerSPtr_ != nullptr);
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

    VectorXd inFrame = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Index i = 0;

    for (const Shared<const CoordinatesSubset> subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        const VectorXd subsetInFrame = subset->inFrame(
            this->instant_, this->coordinates_, this->frameSPtr_, this->coordinatesBrokerSPtr_, aFrameSPtr
        );

        for (int j = 0; j < subsetInFrame.size(); j++)
        {
            inFrame(i) = subsetInFrame(j);
            i++;
        }
    }

    return {this->instant_, inFrame, aFrameSPtr, this->coordinatesBrokerSPtr_};
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

State State::fromStdVector(
    const Instant& anInstant,
    const std::vector<double>& aCoordinates,
    const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker> aCoordinatesBrokerSPtr
)
{
    return {
        anInstant,
        VectorXd::Map(aCoordinates.data(), static_cast<Eigen::Index>(aCoordinates.size())),
        aFrameSPtr,
        aCoordinatesBrokerSPtr};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
