/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AttitudeQuaternion.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::Index;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatessubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatessubset::AttitudeQuaternion;
using ostk::astrodynamics::trajectory::state::coordinatessubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatessubset::CartesianVelocity;

State::State(
    const Instant& anInstant,
    const VectorXd& aCoordinates,
    const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
)
    : instant_(anInstant),
      coordinates_(aCoordinates),
      frameSPtr_(aFrameSPtr),
      coordinatesBrokerSPtr_(aCoordinateBrokerSPtr)
{
    if (coordinatesBrokerSPtr_ && (Size)coordinates_.size() != coordinatesBrokerSPtr_->getNumberOfCoordinates())
    {
        throw ostk::core::error::runtime::Wrong("Number of Coordinates");
    }
}

State::State(
    const Instant& anInstant,
    const VectorXd& aCoordinates,
    const Shared<const Frame>& aFrameSPtr,
    const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
)
    : instant_(anInstant),
      coordinates_(aCoordinates),
      frameSPtr_(aFrameSPtr),
      coordinatesBrokerSPtr_(std::make_shared<CoordinateBroker>(CoordinateBroker(aCoordinateSubsetsArray)))
{
}

State::State(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity)
    : instant_(anInstant)
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

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

    static const Shared<CoordinateBroker> coordinatesBrokerSPtr =
        std::make_shared<CoordinateBroker>(CoordinateBroker({
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
        }));

    this->coordinates_ = coordinates;
    this->frameSPtr_ = aPosition.accessFrame();
    this->coordinatesBrokerSPtr_ = coordinatesBrokerSPtr;
}

State::State(
    const Instant& anInstant,
    const Position& aPosition,
    const Velocity& aVelocity,
    const Quaternion& anAttitude,
    const Vector3d& anAngularVelocity,
    const Shared<const Frame>& anAttitudeReferenceFrame
)
    : instant_(anInstant)
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!aPosition.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Position");
    }

    if (!aVelocity.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Velocity");
    }

    if (!anAttitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Attitude");
    }

    if (!anAngularVelocity.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Angular Velocity");
    }

    if ((aPosition.accessFrame() != aVelocity.accessFrame()) || (aPosition.accessFrame() != anAttitudeReferenceFrame))
    {
        throw ostk::core::error::runtime::Wrong("Position-Velocity-Attitude Frames");
    }

    VectorXd coordinates(13);
    coordinates.segment(0, 3) = aPosition.inUnit(Position::Unit::Meter).accessCoordinates();
    coordinates.segment(3, 3) = aVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();
    coordinates.segment(6, 4) = AttitudeQuaternion::quaterionToCoordinates(anAttitude);
    coordinates.segment(10, 3) = anAngularVelocity;

    static const Shared<CoordinateBroker> coordinatesBrokerSPtr =
        std::make_shared<CoordinateBroker>(CoordinateBroker(
            {CartesianPosition::Default(),
             CartesianVelocity::Default(),
             AttitudeQuaternion::Default(),
             AngularVelocity::Default()}
        ));

    this->coordinates_ = coordinates;
    this->frameSPtr_ = aPosition.accessFrame();
    this->coordinatesBrokerSPtr_ = coordinatesBrokerSPtr;
}

State::State(const State& aState)
    : instant_(aState.instant_),
      coordinates_(aState.coordinates_),
      frameSPtr_(aState.frameSPtr_),
      coordinatesBrokerSPtr_(aState.coordinatesBrokerSPtr_)
{
}

State& State::operator=(const State& aState)
{
    if (this != &aState)
    {
        instant_ = aState.instant_;
        coordinates_ = aState.coordinates_;
        frameSPtr_ = aState.frameSPtr_;
        coordinatesBrokerSPtr_ = aState.coordinatesBrokerSPtr_;
    }
    return *this;
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

    if (this->getSize() != aState.getSize())
    {
        return false;
    }

    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        if (!aState.coordinatesBrokerSPtr_->hasSubset(subset))
        {
            return false;
        }

        if (this->extractCoordinate(subset) != aState.extractCoordinate(subset))
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
    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
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
    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
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

const Shared<const CoordinateBroker>& State::accessCoordinateBroker() const
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

    return Position::Meters(this->extractCoordinate(CartesianPosition::Default()), this->frameSPtr_);
}

Velocity State::getVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return Velocity::MetersPerSecond(this->extractCoordinate(CartesianVelocity::Default()), this->frameSPtr_);
}

Quaternion State::getAttitude() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return AttitudeQuaternion::coordinatesToQuaternion(this->extractCoordinate(AttitudeQuaternion::Default()));
}

Vector3d State::getAngularVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->extractCoordinate(AngularVelocity::Default());
}

VectorXd State::getCoordinates() const
{
    return this->accessCoordinates();
}

const Array<Shared<const CoordinateSubset>> State::getCoordinateSubsets() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinatesBrokerSPtr_->getSubsets();
}

bool State::hasSubset(const Shared<const CoordinateSubset>& aSubsetSPtr) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinatesBrokerSPtr_->hasSubset(aSubsetSPtr);
}

VectorXd State::extractCoordinate(const Shared<const CoordinateSubset>& aSubsetSPtr) const
{
    return this->coordinatesBrokerSPtr_->extractCoordinate(this->accessCoordinates(), aSubsetSPtr);
}

VectorXd State::extractCoordinates(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const
{
    return this->coordinatesBrokerSPtr_->extractCoordinates(this->accessCoordinates(), aCoordinateSubsetsArray);
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
    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
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
    using ostk::core::type::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory :: State") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->instant_.isDefined() ? this->instant_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Frame:" << (this->frameSPtr_->isDefined() ? this->frameSPtr_->getName() : "Undefined");

    if (!this->isDefined())
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Coordinates: Undefined";
    }
    else
    {
        const Array<Shared<const CoordinateSubset>> subsets = this->coordinatesBrokerSPtr_->getSubsets();

        for (const auto& subset : subsets)
        {
            ostk::core::utils::Print::Line(anOutputStream)
                << subset->getName() << this->extractCoordinate(subset).toString(4);
        }
    }
    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

State State::Undefined()
{
    return {Instant::Undefined(), VectorXd(0), Frame::Undefined(), nullptr};
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
