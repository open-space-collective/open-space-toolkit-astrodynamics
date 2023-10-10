/// Apache License 2.0

// #include <Eigen/Core>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>
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

StateBuilder::StateBuilder(
    const Shared<const Frame>& aFrameSPtr,
    const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray
)
    : 
      frameSPtr_(aFrameSPtr),
      coordinatesBrokerSPtr_(Shared<CoordinatesBroker>(aCoordinatesSubsetsArray))
{
}

bool StateBuilder::operator==(const StateBuilder& aStateBuilder) const
{
    if ((!this->isDefined()) || (!aStateBuilder.isDefined()))
    {
        return false;
    }

    if (this->frameSPtr_ != aState.frameSPtr_)
    {
        return false;
    }

    for (const Shared<const CoordinatesSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        if (!aStateBuilder.coordinatesBrokerSPtr_->hasSubset(subset))
        {
            return false;
        }

        if (this->extractCoordinates(subset) != aStateBuilder.extractCoordinates(subset))
        {
            return false;
        }
    }

    return true;
}

bool StateBuilder::operator!=(const StateBuilder& aStateBuilder) const
{
    return !((*this) == aStateBuilder);
}

std::ostream& operator<<(std::ostream& anOutputStream, const StateBuilder& aState)
{
    aState.print(anOutputStream);

    return anOutputStream;
}

bool StateBuilder::isDefined() const
{
    return this->instant_.isDefined() && this->coordinates_.isDefined() && (this->frameSPtr_ != nullptr) &&
           this->frameSPtr_->isDefined() && (this->coordinatesBrokerSPtr_ != nullptr);
}

const Instant& StateBuilder::accessInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->instant_;
}

const Shared<const Frame> StateBuilder::accessFrame() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->frameSPtr_;
}

const VectorXd& StateBuilder::accessCoordinates() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinates_;
}

const Shared<const CoordinatesBroker>& StateBuilder::accessCoordinatesBroker() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinatesBrokerSPtr_;
}

Size StateBuilder::getSize() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinates_.size();
}

Instant StateBuilder::getInstant() const
{
    return this->accessInstant();
}

Shared<const Frame> StateBuilder::getFrame() const
{
    return this->accessFrame();
}

Position StateBuilder::getPosition() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return Position::Meters(this->extractCoordinates(CartesianPosition::Default()), this->frameSPtr_);
}

Velocity StateBuilder::getVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return Velocity::MetersPerSecond(this->extractCoordinates(CartesianVelocity::Default()), this->frameSPtr_);
}

VectorXd StateBuilder::getCoordinates() const
{
    return this->accessCoordinates();
}

const Array<Shared<const CoordinatesSubset>> StateBuilder::getCoordinatesSubsets() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    return this->coordinatesBrokerSPtr_->getSubsets();
}

VectorXd StateBuilder::extractCoordinates(const Shared<const CoordinatesSubset>& aSubsetSPtr) const
{
    return this->coordinatesBrokerSPtr_->extractCoordinates(this->accessCoordinates(), aSubsetSPtr);
}

VectorXd StateBuilder::extractCoordinates(const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray) const
{
    return this->coordinatesBrokerSPtr_->extractCoordinates(this->accessCoordinates(), aCoordinatesSubsetsArray);
}

State StateBuilder::inFrame(const Shared<const Frame>& aFrameSPtr) const
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

void StateBuilder::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::types::String;

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
        const Array<Shared<const CoordinatesSubset>> subsets = this->coordinatesBrokerSPtr_->getSubsets();

        for (const auto& subset : subsets)
        {
            ostk::core::utils::Print::Line(anOutputStream)
                << subset->getName() << this->extractCoordinates(subset).toString(4);
        }
    }
    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

State StateBuilder::Undefined()
{
    return {Instant::Undefined(), VectorXd(0), Frame::Undefined(), nullptr};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
