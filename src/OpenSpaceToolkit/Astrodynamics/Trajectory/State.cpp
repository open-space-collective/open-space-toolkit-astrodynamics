/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

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

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

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
    if (frameSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (coordinatesBrokerSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Coordinate broker");
    }

    if ((Size)coordinates_.size() != coordinatesBrokerSPtr_->getNumberOfCoordinates())
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
    if (frameSPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }
}

State::State(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity)
    : instant_(anInstant)
{
    if ((*aPosition.getFrame()) != (*aVelocity.getFrame()))
    {
        throw ostk::core::error::runtime::Wrong("Position-Velocity Frames");
    }

    VectorXd coordinates(6);
    coordinates.segment(0, 3) = aPosition.inUnit(Position::Unit::Meter).getCoordinates();
    coordinates.segment(3, 3) = aVelocity.inUnit(Velocity::Unit::MeterPerSecond).getCoordinates();

    static const Shared<CoordinateBroker> coordinatesBrokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
    }));

    this->coordinates_ = coordinates;
    this->frameSPtr_ = aPosition.getFrame();
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
    if (((*aPosition.getFrame()) != (*aVelocity.getFrame())) ||
        ((*aPosition.getFrame()) != (*anAttitudeReferenceFrame)))
    {
        throw ostk::core::error::runtime::Wrong("Position-Velocity-Attitude Frames");
    }

    VectorXd coordinates(13);
    coordinates.segment(0, 3) = aPosition.inUnit(Position::Unit::Meter).getCoordinates();
    coordinates.segment(3, 3) = aVelocity.inUnit(Velocity::Unit::MeterPerSecond).getCoordinates();
    coordinates.segment(6, 4) = anAttitude.toVector(Quaternion::Format::XYZS);
    coordinates.segment(10, 3) = anAngularVelocity;

    static const Shared<CoordinateBroker> coordinatesBrokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker(
        {CartesianPosition::Default(),
         CartesianVelocity::Default(),
         AttitudeQuaternion::Default(),
         AngularVelocity::Default()}
    ));

    this->coordinates_ = coordinates;
    this->frameSPtr_ = aPosition.getFrame();
    this->coordinatesBrokerSPtr_ = coordinatesBrokerSPtr;
}

State::State(const State& aState)
    : instant_(aState.instant_),
      coordinates_(aState.coordinates_),
      frameSPtr_(aState.frameSPtr_),
      coordinatesBrokerSPtr_(aState.coordinatesBrokerSPtr_),
      covariance_(aState.covariance_)
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
        covariance_ = aState.covariance_;
    }
    return *this;
}

bool State::operator==(const State& aState) const
{
    if (this->instant_ != aState.instant_)
    {
        return false;
    }

    if ((*this->frameSPtr_) != (*aState.frameSPtr_))
    {
        return false;
    }

    if (this->getSize() != aState.getSize())
    {
        return false;
    }

    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->getSubsets())
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
    if (this->instant_ != aState.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if ((*this->frameSPtr_) != (*aState.frameSPtr_))
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if (*this->coordinatesBrokerSPtr_ != *aState.coordinatesBrokerSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Coordinate Subsets");
    }

    VectorXd addedCoordinates = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Index i = 0;
    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        Size subsetSize = subset->getSize();
        addedCoordinates.segment(i, subsetSize) = subset->add(
            this->instant_, this->coordinates_, aState.getCoordinates(), this->frameSPtr_, this->coordinatesBrokerSPtr_
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
    if (this->instant_ != aState.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if ((*this->frameSPtr_) != (*aState.frameSPtr_))
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if (*this->coordinatesBrokerSPtr_ != *aState.coordinatesBrokerSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Coordinate Subsets");
    }

    VectorXd subtractedCoordinates = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Index i = 0;
    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        Size subsetSize = subset->getSize();
        subtractedCoordinates.segment(i, subsetSize) = subset->subtract(
            this->instant_, this->coordinates_, aState.getCoordinates(), this->frameSPtr_, this->coordinatesBrokerSPtr_
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

const Instant& State::getInstant() const
{
    return this->instant_;
}

const Shared<const Frame>& State::getFrame() const
{
    return this->frameSPtr_;
}

const VectorXd& State::getCoordinates() const&
{
    return this->coordinates_;
}

VectorXd State::getCoordinates() &&
{
    return this->getCoordinates();
}

const Shared<const CoordinateBroker>& State::getCoordinateBroker() const
{
    return this->coordinatesBrokerSPtr_;
}

Size State::getSize() const
{
    return this->coordinates_.size();
}

const std::optional<Covariance>& State::getCovariance() const
{
    return this->covariance_;
}

Position State::getPosition() const
{
    return Position::Meters(this->extractCoordinate(CartesianPosition::Default()), this->frameSPtr_);
}

Velocity State::getVelocity() const
{
    return Velocity::MetersPerSecond(this->extractCoordinate(CartesianVelocity::Default()), this->frameSPtr_);
}

Quaternion State::getAttitude() const
{
    const VectorXd attitudeQuaternionCoordinates = this->extractCoordinate(AttitudeQuaternion::Default());

    return Quaternion::XYZS(
        attitudeQuaternionCoordinates[0],
        attitudeQuaternionCoordinates[1],
        attitudeQuaternionCoordinates[2],
        attitudeQuaternionCoordinates[3]
    );
}

Vector3d State::getAngularVelocity() const
{
    return this->extractCoordinate(AngularVelocity::Default());
}

const Array<Shared<const CoordinateSubset>>& State::getCoordinateSubsets() const&
{
    return this->coordinatesBrokerSPtr_->getSubsets();
}

Array<Shared<const CoordinateSubset>> State::getCoordinateSubsets() &&
{
    return this->getCoordinateSubsets();
}

bool State::hasSubset(const Shared<const CoordinateSubset>& aSubsetSPtr) const
{
    return this->coordinatesBrokerSPtr_->hasSubset(aSubsetSPtr);
}

bool State::hasCovariance() const
{
    return this->covariance_.has_value();
}

VectorXd State::extractCoordinate(const Shared<const CoordinateSubset>& aSubsetSPtr) const
{
    return this->coordinatesBrokerSPtr_->extractCoordinate(this->getCoordinates(), aSubsetSPtr);
}

VectorXd State::extractCoordinates(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const
{
    return this->coordinatesBrokerSPtr_->extractCoordinates(this->getCoordinates(), aCoordinateSubsetsArray);
}

State State::inFrame(const Shared<const Frame>& aFrameSPtr) const
{
    if (aFrameSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (aFrameSPtr == this->frameSPtr_)
    {
        return *this;
    }

    VectorXd inFrameCoordinates = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Index i = 0;
    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->getSubsets())
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

State State::withCovariance(const std::optional<Covariance>& aCovariance) const
{
    if (aCovariance.has_value() && aCovariance->getInstant() != this->instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    State state = *this;
    state.covariance_ = aCovariance;

    return state;
}

Covariance::MahalanobisDistanceAnalysis State::computeMahalanobisDistanceAnalysis(
    const State& anotherState, const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
) const
{
    if (!this->covariance_.has_value())
    {
        throw ostk::core::error::runtime::Wrong("Covariance");
    }

    if (aCoordinateSubsetsArray.isEmpty())
    {
        throw ostk::core::error::runtime::Wrong("Coordinate Subsets");
    }

    const Covariance& covariance = this->covariance_.value();
    const Shared<const Frame>& covarianceFrameSPtr = covariance.getFrame();

    // States are reduced after the frame transformation (which may require other subsets), so that only the
    // analyzed subsets need to be shared by both States
    const auto reduceInCovarianceFrame = [&](const State& aState) -> State
    {
        const State stateInCovarianceFrame = aState.inFrame(covarianceFrameSPtr);

        return {
            stateInCovarianceFrame.getInstant(),
            stateInCovarianceFrame.extractCoordinates(aCoordinateSubsetsArray),
            covarianceFrameSPtr,
            aCoordinateSubsetsArray,
        };
    };

    const State residualState = reduceInCovarianceFrame(anotherState) - reduceInCovarianceFrame(*this);

    return covariance.reduce(aCoordinateSubsetsArray)
        .computeMahalanobisDistanceAnalysis(residualState.getCoordinates());
}

void State::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::type::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory :: State") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Instant:" << this->instant_.toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Frame:" << this->frameSPtr_->getName();

    const Array<Shared<const CoordinateSubset>> subsets = this->coordinatesBrokerSPtr_->getSubsets();

    for (const auto& subset : subsets)
    {
        ostk::core::utils::Print::Line(anOutputStream)
            << subset->getName() << this->extractCoordinate(subset).toString(4);
    }
    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
