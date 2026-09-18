/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Utility/Print.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameTransformProvider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

using ostk::core::container::Array;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Velocity;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

CloseApproach::CloseApproach(const State& anObject1State, const State& anObject2State)
    : CloseApproach(anObject1State, anObject2State, CovarianceMatrix::Undefined(), CovarianceMatrix::Undefined())
{
}

CloseApproach::CloseApproach(
    const State& anObject1State,
    const State& anObject2State,
    const CovarianceMatrix& anObject1CovarianceMatrix,
    const CovarianceMatrix& anObject2CovarianceMatrix
)
    : object1State_(anObject1State),
      object2State_(anObject2State),
      object1CovarianceMatrix_(anObject1CovarianceMatrix),
      object2CovarianceMatrix_(anObject2CovarianceMatrix),
      stateBuilder_(StateBuilder::Undefined())
{
    if (anObject1State.isDefined() && anObject2State.isDefined())
    {
        if (anObject1State.getInstant() != anObject2State.getInstant())
        {
            throw ostk::core::error::RuntimeError("Inconsistent state instants.");
        }

        CloseApproach::ThrowIfInconsistentCovarianceMatrixInstants(
            anObject1State.getInstant(), anObject1CovarianceMatrix, anObject2CovarianceMatrix
        );

        // Build state builder with the frame of Object 1 and position/velocity coordinate subsets
        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
        };

        stateBuilder_ = StateBuilder(anObject1State.accessFrame(), coordinateSubsets);
    }
}

bool CloseApproach::operator==(const CloseApproach& aCloseApproach) const
{
    if ((!this->isDefined()) || (!aCloseApproach.isDefined()))
    {
        return false;
    }

    return (object1State_ == aCloseApproach.object1State_) && (object2State_ == aCloseApproach.object2State_);
}

bool CloseApproach::operator!=(const CloseApproach& aCloseApproach) const
{
    return !((*this) == aCloseApproach);
}

std::ostream& operator<<(std::ostream& anOutputStream, const CloseApproach& aCloseApproach)
{
    aCloseApproach.print(anOutputStream);

    return anOutputStream;
}

bool CloseApproach::isDefined() const
{
    return object1State_.isDefined() && object2State_.isDefined() && stateBuilder_.isDefined();
}

State CloseApproach::getObject1State() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    return object1State_;
}

State CloseApproach::getObject2State() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    return object2State_;
}

CovarianceMatrix CloseApproach::getObject1CovarianceMatrix() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    return object1CovarianceMatrix_;
}

CovarianceMatrix CloseApproach::getObject2CovarianceMatrix() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    return object2CovarianceMatrix_;
}

void CloseApproach::setCovarianceMatrices(
    const CovarianceMatrix& anObject1CovarianceMatrix, const CovarianceMatrix& anObject2CovarianceMatrix
)
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    CloseApproach::ThrowIfInconsistentCovarianceMatrixInstants(
        object1State_.getInstant(), anObject1CovarianceMatrix, anObject2CovarianceMatrix
    );

    object1CovarianceMatrix_ = anObject1CovarianceMatrix;
    object2CovarianceMatrix_ = anObject2CovarianceMatrix;
}

CloseApproach CloseApproach::scale(const Real& aScaleFactor1, const Real& aScaleFactor2) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    const CovarianceMatrix scaledObject1CovarianceMatrix =
        aScaleFactor1.isDefined() ? object1CovarianceMatrix_.scale(aScaleFactor1) : object1CovarianceMatrix_;
    const CovarianceMatrix scaledObject2CovarianceMatrix =
        aScaleFactor2.isDefined() ? object2CovarianceMatrix_.scale(aScaleFactor2) : object2CovarianceMatrix_;

    return {object1State_, object2State_, scaledObject1CovarianceMatrix, scaledObject2CovarianceMatrix};
}

Instant CloseApproach::getInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    return object1State_.getInstant();
}

Length CloseApproach::getMissDistance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    const State relativeState = this->getRelativeState();
    const Position relativePosition = relativeState.getPosition();
    const Vector3d relativePositionCoordinates = relativePosition.inMeters().getCoordinates();

    return Length::Meters(relativePositionCoordinates.norm());
}

State CloseApproach::getRelativeState() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    const State reducedObject1State = stateBuilder_.reduce(object1State_);
    const State reducedObject2State = stateBuilder_.reduce(object2State_.inFrame(object1State_.accessFrame()));

    return reducedObject2State - reducedObject1State;
}

Derived CloseApproach::getRelativeVelocity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    const State relativeState = this->getRelativeState();
    const Velocity relativeVelocity = relativeState.getVelocity();
    const Vector3d relativeVelocityCoordinates =
        relativeVelocity.inUnit(Velocity::Unit::MeterPerSecond).getCoordinates();

    return Derived(relativeVelocityCoordinates.norm(), Derived::Unit::MeterPerSecond());
}

Shared<const Frame> CloseApproach::getEncounterFrame(const Shared<const Frame>& aFrameSPtr) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!aFrameSPtr->isQuasiInertial())
    {
        throw ostk::core::error::runtime::Wrong("Frame", aFrameSPtr->getName());
    }

    const StateBuilder stateBuilder = {
        aFrameSPtr,
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };

    const State object1StateInFrame = stateBuilder.reduce(object1State_);
    const State object2StateInFrame = stateBuilder.reduce(object2State_);

    const State relativeStateInFrame = object2StateInFrame - object1StateInFrame;

    const Vector3d positionCoordinates = object1StateInFrame.extractCoordinate(CartesianPosition::Default());
    const Vector3d velocityCoordinates = object1StateInFrame.extractCoordinate(CartesianVelocity::Default());
    const Vector3d relativePositionCoordinates = relativeStateInFrame.extractCoordinate(CartesianPosition::Default());
    const Vector3d relativeVelocityCoordinates = relativeStateInFrame.extractCoordinate(CartesianVelocity::Default());

    if (relativeVelocityCoordinates.norm() == 0.0)
    {
        throw ostk::core::error::RuntimeError("Relative velocity is zero.");
    }

    const Vector3d zAxis = relativeVelocityCoordinates.normalized();
    const Vector3d yAxisUnnormalized = zAxis.cross(relativePositionCoordinates);

    if (yAxisUnnormalized.norm() == 0.0)
    {
        throw ostk::core::error::RuntimeError("Relative position is aligned with relative velocity.");
    }

    const Vector3d yAxis = yAxisUnnormalized.normalized();
    const Vector3d xAxis = yAxis.cross(zAxis);

    const Quaternion transformOrientation =
        Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

    const Transform transform = {
        object1StateInFrame.accessInstant(),
        -positionCoordinates,
        -velocityCoordinates,
        transformOrientation,
        Vector3d {0.0, 0.0, 0.0},  // Angular velocity (TBD)
        Transform::Type::Passive,
    };

    const String frameName = String::Format(
        "ENCOUNTER@{}{}{}{}{}{}",
        object1StateInFrame.accessInstant().toString(),
        positionCoordinates.toString(),
        velocityCoordinates.toString(),
        relativePositionCoordinates.toString(),
        relativeVelocityCoordinates.toString(),
        aFrameSPtr->getName()
    );

    if (Frame::Exists(frameName))
    {
        return Frame::WithName(frameName);
    }

    const Shared<const LocalOrbitalFrameTransformProvider> providerSPtr =
        std::make_shared<const LocalOrbitalFrameTransformProvider>(transform);

    return Frame::Construct(frameName, false, aFrameSPtr, providerSPtr);
}

Tuple<Length, Length, Length> CloseApproach::computeMissDistanceComponentsInFrame(const Shared<const Frame>& aFrame
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    if ((aFrame == nullptr) || (!aFrame->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    const State relativeStateInFrame = this->getRelativeState().inFrame(aFrame);
    const Position relativePosition = relativeStateInFrame.getPosition();
    const Vector3d coordinates = relativePosition.inMeters().getCoordinates();

    return {Length::Meters(coordinates[0]), Length::Meters(coordinates[1]), Length::Meters(coordinates[2])};
}

Tuple<Length, Length, Length> CloseApproach::computeMissDistanceComponentsInLocalOrbitalFrame(
    const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CloseApproach");
    }

    if ((aLocalOrbitalFrameFactorySPtr == nullptr) || (!aLocalOrbitalFrameFactorySPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("LocalOrbitalFrameFactory");
    }

    const Shared<const Frame> localOrbitalFrame = aLocalOrbitalFrameFactorySPtr->generateFrame(object1State_);
    const State relativeStateInFrame = object2State_.inFrame(localOrbitalFrame);
    const Position relativePosition = relativeStateInFrame.getPosition();
    const Vector3d coordinates = relativePosition.inMeters().getCoordinates();

    return {Length::Meters(coordinates[0]), Length::Meters(coordinates[1]), Length::Meters(coordinates[2])};
}

void CloseApproach::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Close Approach") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->isDefined() ? this->getInstant().toString() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Miss Distance:" << (this->isDefined() ? this->getMissDistance().toString() : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Object 1 State:" << (object1State_.isDefined() ? "Defined" : "Undefined");

    ostk::core::utils::Print::Line(anOutputStream)
        << "Object 2 State:" << (object2State_.isDefined() ? "Defined" : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

CloseApproach CloseApproach::Undefined()
{
    return CloseApproach(State::Undefined(), State::Undefined());
}

void CloseApproach::ThrowIfInconsistentCovarianceMatrixInstants(
    const Instant& aStateInstant,
    const CovarianceMatrix& anObject1CovarianceMatrix,
    const CovarianceMatrix& anObject2CovarianceMatrix
)
{
    if (anObject1CovarianceMatrix.isDefined() && anObject2CovarianceMatrix.isDefined())
    {
        if ((anObject1CovarianceMatrix.getInstant() != aStateInstant) ||
            (anObject2CovarianceMatrix.getInstant() != aStateInstant))
        {
            throw ostk::core::error::RuntimeError("Inconsistent covariance matrix instants.");
        }
    }
}

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
