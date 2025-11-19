/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Transform.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{

using ostk::core::type::Real;

using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::coordinate::frame::Manager;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Velocity;
using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;

using TransformModel = ostk::astrodynamics::flight::profile::model::Transform;

static const Shared<const Frame> DEFAULT_PROFILE_FRAME = Frame::GCRF();

Profile::Target::Target(const TargetType& aType, const Vector3d& aDirection)
    : type(aType),
      direction(aDirection)
{
}

Profile::Target::Target(const TargetType& aType, const Axis& anAxis, const bool& isAntiDirection)
    : type(aType),
      direction(Profile::AxisToDirection(anAxis, isAntiDirection))
{
}

Profile::TrajectoryTarget::TrajectoryTarget(
    const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirection
)
    : Target(TargetType::Trajectory, aDirection),
      trajectory(aTrajectory)
{
    if (!trajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }
}

Profile::TrajectoryTarget Profile::TrajectoryTarget::TargetPosition(
    const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirection
)
{
    return TrajectoryTarget(TargetType::TargetPosition, aTrajectory, aDirection);
}

Profile::TrajectoryTarget Profile::TrajectoryTarget::TargetVelocity(
    const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirection
)
{
    return TrajectoryTarget(TargetType::TargetVelocity, aTrajectory, aDirection);
}

Profile::TrajectoryTarget Profile::TrajectoryTarget::TargetSlidingGroundVelocity(
    const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirection
)
{
    return TrajectoryTarget(TargetType::TargetSlidingGroundVelocity, aTrajectory, aDirection);
}

Profile::TrajectoryTarget Profile::TrajectoryTarget::TargetPosition(
    const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection
)
{
    return TrajectoryTarget(TargetType::TargetPosition, aTrajectory, Profile::AxisToDirection(anAxis, isAntiDirection));
}

Profile::TrajectoryTarget Profile::TrajectoryTarget::TargetVelocity(
    const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection
)
{
    return TrajectoryTarget(TargetType::TargetVelocity, aTrajectory, Profile::AxisToDirection(anAxis, isAntiDirection));
}

Profile::TrajectoryTarget Profile::TrajectoryTarget::TargetSlidingGroundVelocity(
    const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection
)
{
    return TrajectoryTarget(
        TargetType::TargetSlidingGroundVelocity, aTrajectory, Profile::AxisToDirection(anAxis, isAntiDirection)
    );
}

Profile::TrajectoryTarget::TrajectoryTarget(
    const TargetType& aType, const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirection
)
    : Target(aType, aDirection),
      trajectory(aTrajectory)
{
    if (!trajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }

    if (aType == TargetType::Trajectory)
    {
        throw ostk::core::error::RuntimeError(
            "TargetType::Trajectory is deprecated. Use TargetType::TargetPosition instead."
        );
    }

    if (aType != TargetType::TargetPosition && aType != TargetType::TargetVelocity &&
        aType != TargetType::TargetSlidingGroundVelocity)
    {
        throw ostk::core::error::runtime::Wrong("Target type");
    }
}

Profile::OrientationProfileTarget::OrientationProfileTarget(
    const Array<Pair<Instant, Vector3d>>& anOrientationProfile, const Vector3d& aDirection
)
    : Target(TargetType::OrientationProfile, aDirection),
      orientationProfile(anOrientationProfile)
{
    if (orientationProfile.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Orientation Profile");
    }

    VectorXd timestamps(anOrientationProfile.getSize());
    MatrixXd coordinates(anOrientationProfile.getSize(), 3);

    for (Index i = 0; i < anOrientationProfile.getSize(); ++i)
    {
        timestamps(i) = (anOrientationProfile[i].first - anOrientationProfile.accessFirst().first).inSeconds();

        coordinates.row(i) = anOrientationProfile[i].second;
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(
            Interpolator::GenerateInterpolator(Interpolator::Type::BarycentricRational, timestamps, coordinates.col(i))
        );
    }
}

Profile::OrientationProfileTarget::OrientationProfileTarget(
    const Array<Pair<Instant, Vector3d>>& anOrientationProfile, const Axis& anAxis, const bool& isAntiDirection
)
    : Target(TargetType::OrientationProfile, anAxis, isAntiDirection),
      orientationProfile(anOrientationProfile)
{
    if (orientationProfile.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Orientation Profile");
    }

    VectorXd timestamps(anOrientationProfile.getSize());
    MatrixXd coordinates(anOrientationProfile.getSize(), 3);

    for (Index i = 0; i < anOrientationProfile.getSize(); ++i)
    {
        timestamps(i) = (anOrientationProfile[i].first - anOrientationProfile.accessFirst().first).inSeconds();

        coordinates.row(i) = anOrientationProfile[i].second;
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(
            Interpolator::GenerateInterpolator(Interpolator::Type::BarycentricRational, timestamps, coordinates.col(i))
        );
    }
}

Vector3d Profile::OrientationProfileTarget::getAlignmentVectorAt(const Instant& anInstant) const
{
    Vector3d interpolatedCoordinates = Vector3d::Zero();

    const Real duration = (anInstant - orientationProfile.accessFirst().first).inSeconds();

    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        interpolatedCoordinates(i) = interpolators_[i]->evaluate(duration);
    }

    return interpolatedCoordinates;
}

Profile::CustomTarget::CustomTarget(
    std::function<Vector3d(const State&)> anOrientationGenerator, const Vector3d& aDirection
)
    : Target(TargetType::Custom, aDirection),
      orientationGenerator(anOrientationGenerator)
{
}

Profile::CustomTarget::CustomTarget(
    std::function<Vector3d(const State&)> anOrientationGenerator, const Axis& anAxis, const bool& isAntiDirection
)
    : Target(TargetType::Custom, anAxis, isAntiDirection),
      orientationGenerator(anOrientationGenerator)
{
}

Profile::Profile(const Model& aModel)
    : modelUPtr_(aModel.clone())
{
}

Profile::Profile(const Profile& aProfile)
    : modelUPtr_(aProfile.modelUPtr_ ? Unique<Model>(aProfile.modelUPtr_->clone()) : nullptr)
{
}

Profile& Profile::operator=(const Profile& aProfile)
{
    if (this != &aProfile)
    {
        this->modelUPtr_ = aProfile.modelUPtr_ ? Unique<Model>(aProfile.modelUPtr_->clone()) : nullptr;
    }

    return *this;
}

std::ostream& operator<<(std::ostream& anOutputStream, const Profile& aProfile)
{
    aProfile.print(anOutputStream, true);

    return anOutputStream;
}

bool Profile::isDefined() const
{
    return (this->modelUPtr_ != nullptr) && this->modelUPtr_->isDefined();
}

const Model& Profile::accessModel() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile");
    }

    return *modelUPtr_;
}

State Profile::getStateAt(const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile");
    }

    return this->modelUPtr_->calculateStateAt(anInstant);
}

Array<State> Profile::getStatesAt(const Array<Instant>& anInstantArray) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile");
    }

    return this->modelUPtr_->calculateStatesAt(anInstantArray);
}

Axes Profile::getAxesAt(const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile");
    }

    return modelUPtr_->getAxesAt(anInstant);
}

Shared<const Frame> Profile::getBodyFrame(const String& aFrameName) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile");
    }

    if (Manager::Get().hasFrameWithName(aFrameName))
    {
        throw ostk::core::error::RuntimeError("A frame with the same name already exists.");
    }

    return modelUPtr_->getBodyFrame(aFrameName);
}

Shared<const Frame> Profile::constructBodyFrame(const String& aFrameName, const bool& overwrite) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile");
    }

    if (Manager::Get().hasFrameWithName(aFrameName))
    {
        if (overwrite)
        {
            Manager::Get().removeFrameWithName(aFrameName);
        }
        else
        {
            throw ostk::core::error::RuntimeError("A frame with the same name already exists.");
        }
    }

    return modelUPtr_->getBodyFrame(aFrameName);
}

void Profile::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Flight Profile") : void();

    if (this->isDefined())
    {
        this->modelUPtr_->print(anOutputStream, false);
    }
    else
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Model:" << "Undefined";
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Profile Profile::Undefined()
{
    return {};
}

Profile Profile::InertialPointing(const ostk::astrodynamics::Trajectory& aTrajectory, const Quaternion& aQuaternion)
{
    return {TransformModel::InertialPointing(aTrajectory, aQuaternion)};
}

Profile Profile::LocalOrbitalFramePointing(
    const trajectory::Orbit& anOrbit, const trajectory::Orbit::FrameType& anOrbitalFrameType
)
{
    return {TransformModel::LocalOrbitalFramePointing(anOrbit, anOrbitalFrameType)};
}

Profile Profile::CustomPointing(
    const trajectory::Orbit& anOrbit,
    const Shared<const Target>& anAlignmentTargetSPtr,
    const Shared<const Target>& aClockingTargetSPtr
)
{
    const auto orientationGenerator = Profile::AlignAndConstrain(anAlignmentTargetSPtr, aClockingTargetSPtr);

    return Profile::CustomPointing(anOrbit, orientationGenerator);
}

Profile Profile::CustomPointing(
    const trajectory::Orbit& anOrbit, const std::function<Quaternion(const State&)>& anOrientationGenerator
)
{
    // Copy the orbit and orientation generator to avoid dangling references.
    auto dynamicProviderGenerator = [anOrbit, anOrientationGenerator](const Instant& anInstant) -> Transform
    {
        const State state = anOrbit.getStateAt(anInstant).inFrame(DEFAULT_PROFILE_FRAME);

        const Position position = state.getPosition();
        const Velocity velocity = state.getVelocity();

        return Transform::Active(
            anInstant,
            -position.accessCoordinates(),
            -velocity.accessCoordinates(),
            anOrientationGenerator(state),
            Vector3d(0.0, 0.0, 0.0)  // TBM: Artificially set to 0 for now.
        );
    };

    return Profile(TransformModel(DynamicProvider(dynamicProviderGenerator), DEFAULT_PROFILE_FRAME));
}

std::function<Quaternion(const State&)> Profile::AlignAndConstrain(
    const Shared<const Target>& anAlignmentTargetSPtr, const Shared<const Target>& aClockingTargetSPtr
)
{
    if ((anAlignmentTargetSPtr->type == TargetType::VelocityECEF) ||
        (aClockingTargetSPtr->type == TargetType::VelocityECEF))
    {
        throw ostk::core::error::runtime::ToBeImplemented("Velocity ECEF");
    }
    if ((anAlignmentTargetSPtr->type == aClockingTargetSPtr->type) &&
        (anAlignmentTargetSPtr->type != TargetType::Trajectory))
    {
        throw ostk::core::error::RuntimeError("Alignment and clocking target cannot be the same.");
    }

    {
        {
            const Vector3d& ad = anAlignmentTargetSPtr->direction;
            const Vector3d& cd = aClockingTargetSPtr->direction;
            const Real cosTheta = ad.normalized().dot(cd.normalized());
            if (std::abs(cosTheta) >= 1.0 - Real::Epsilon())
            {
                throw ostk::core::error::RuntimeError("Alignment and clocking direction cannot be colinear.");
            }
        }
    }

    if (std::set<TargetType>({anAlignmentTargetSPtr->type, aClockingTargetSPtr->type}) ==
        std::set<TargetType>({
            TargetType::GeocentricNadir,
            TargetType::GeodeticNadir,
        }))
    {
        throw ostk::core::error::RuntimeError(
            "Alignment and clocking target cannot be both geocentric nadir and geodetic nadir."
        );
    }

    const auto targetVectorGenerator = [](const Shared<const Target>& aTargetSPtr
                                       ) -> std::function<Vector3d(const State&)>
    {
        switch (aTargetSPtr->type)
        {
            case TargetType::GeocentricNadir:
                return Profile::ComputeGeocentricNadirDirectionVector;
            case TargetType::GeodeticNadir:
                return Profile::ComputeGeodeticNadirDirectionVector;
            case TargetType::Trajectory:
            case TargetType::TargetPosition:
            {
                const Shared<const TrajectoryTarget> targetPositionSPtr =
                    std::static_pointer_cast<const TrajectoryTarget>(aTargetSPtr);
                return [targetPositionSPtr](const State& aState) -> Vector3d
                {
                    return Profile::ComputeTargetDirectionVector(aState, targetPositionSPtr->trajectory);
                };
            }
            case TargetType::TargetVelocity:
            {
                const Shared<const TrajectoryTarget> targetVelocitySPtr =
                    std::static_pointer_cast<const TrajectoryTarget>(aTargetSPtr);
                return [targetVelocitySPtr](const State& aState) -> Vector3d
                {
                    return Profile::ComputeTargetVelocityVector(aState, targetVelocitySPtr->trajectory);
                };
            }
            case TargetType::TargetSlidingGroundVelocity:
            {
                const Shared<const TrajectoryTarget> targetVelocitySPtr =
                    std::static_pointer_cast<const TrajectoryTarget>(aTargetSPtr);
                return [targetVelocitySPtr](const State& aState) -> Vector3d
                {
                    return Profile::ComputeTargetSlidingGroundVelocityVector(aState, targetVelocitySPtr->trajectory);
                };
            }
            case TargetType::Sun:
                return [](const State& aState)
                {
                    return Profile::ComputeCelestialDirectionVector(aState, Sun::Default());
                };
            case TargetType::Moon:
                return [](const State& aState)
                {
                    return Profile::ComputeCelestialDirectionVector(aState, Moon::Default());
                };
            case TargetType::VelocityECI:
                return Profile::ComputeVelocityDirectionVector_ECI;
            case TargetType::VelocityECEF:
                return Profile::ComputeVelocityDirectionVector_ECEF;
            case TargetType::OrbitalMomentum:
                return Profile::ComputeOrbitalMomentumDirectionVector;
            case TargetType::OrientationProfile:
            {
                const Shared<const OrientationProfileTarget> orientationProfileTargetSPtr =
                    std::static_pointer_cast<const OrientationProfileTarget>(aTargetSPtr);
                return [orientationProfileTargetSPtr](const State& aState) -> Vector3d
                {
                    return orientationProfileTargetSPtr->getAlignmentVectorAt(aState.accessInstant());
                };
            }
            case TargetType::Custom:
            {
                const Shared<const CustomTarget> customTargetSPtr =
                    std::static_pointer_cast<const CustomTarget>(aTargetSPtr);
                return [customTargetSPtr](const State& aState) -> Vector3d
                {
                    return customTargetSPtr->orientationGenerator(aState);
                };
            }
            default:
                throw ostk::core::error::RuntimeError("Invalid alignment target type.");
        }
    };

    const Vector3d alignmentDirection = anAlignmentTargetSPtr->direction;
    const Vector3d clockingDirection =
        Profile::ComputeClockingVector(alignmentDirection, aClockingTargetSPtr->direction);
    const auto alignmentTargetVectorFunction = targetVectorGenerator(anAlignmentTargetSPtr);
    const auto clockingTargetVectorFunction = targetVectorGenerator(aClockingTargetSPtr);

    return [alignmentTargetVectorFunction, clockingTargetVectorFunction, alignmentDirection, clockingDirection](
               const State& aState
           ) -> Quaternion
    {
        const Vector3d alignmentTarget = alignmentTargetVectorFunction(aState);
        const Vector3d clockingTarget =
            Profile::ComputeClockingVector(alignmentTarget, clockingTargetVectorFunction(aState));

        return Profile::ComputeBodyToECIQuaternion(
            alignmentDirection, clockingDirection, alignmentTarget, clockingTarget
        );
    };
}

Profile::Profile()
    : modelUPtr_(nullptr)
{
}

Vector3d Profile::ComputeGeocentricNadirDirectionVector(const State& aState)
{
    return -aState.inFrame(DEFAULT_PROFILE_FRAME).getPosition().accessCoordinates().normalized();
}

Vector3d Profile::ComputeGeodeticNadirDirectionVector(const State& aState)
{
    const Transform ITRF_GCRF_transform = Frame::ITRF()->getTransformTo(DEFAULT_PROFILE_FRAME, aState.accessInstant());

    const LLA lla = LLA::Cartesian(
        ITRF_GCRF_transform.getInverse().applyToPosition(
            aState.inFrame(DEFAULT_PROFILE_FRAME).getPosition().accessCoordinates()
        ),
        EarthGravitationalModel::EGM2008.equatorialRadius_,
        EarthGravitationalModel::EGM2008.flattening_
    );

    const Vector3d nadir = {
        -std::cos(lla.getLatitude().inRadians()) * std::cos(lla.getLongitude().inRadians()),
        -std::cos(lla.getLatitude().inRadians()) * std::sin(lla.getLongitude().inRadians()),
        -std::sin(lla.getLatitude().inRadians()),
    };

    return ITRF_GCRF_transform.applyToVector(nadir).normalized();
}

Vector3d Profile::ComputeTargetDirectionVector(const State& aState, const ostk::astrodynamics::Trajectory& aTrajectory)
{
    const Vector3d targetPositionCoordinates =
        aTrajectory.getStateAt(aState.accessInstant()).inFrame(DEFAULT_PROFILE_FRAME).getPosition().accessCoordinates();
    const Vector3d satellitePositionCoordinates =
        aState.inFrame(DEFAULT_PROFILE_FRAME).getPosition().accessCoordinates();

    return (targetPositionCoordinates - satellitePositionCoordinates).normalized();
}

Vector3d Profile::ComputeTargetVelocityVector(const State& aState, const ostk::astrodynamics::Trajectory& aTrajectory)
{
    const Vector3d targetVelocityCoordinates =
        aTrajectory.getStateAt(aState.accessInstant()).inFrame(DEFAULT_PROFILE_FRAME).getVelocity().accessCoordinates();

    if (targetVelocityCoordinates.isZero())
    {
        throw ostk::core::error::RuntimeError(
            "Cannot compute a Target Velocity Vector if the target's velocity is zero."
        );
    }
    return targetVelocityCoordinates.normalized();
}

Vector3d Profile::ComputeTargetSlidingGroundVelocityVector(
    const State& aState, const ostk::astrodynamics::Trajectory& aTrajectory
)
{
    const Instant& instant = aState.accessInstant();

    const Vector3d targetSlidingGroundVelocityCoordinates =
        aTrajectory.getStateAt(instant).inFrame(Frame::ITRF()).getVelocity().accessCoordinates();

    if (targetSlidingGroundVelocityCoordinates.isZero())
    {
        throw ostk::core::error::RuntimeError(
            "Cannot compute a Target Sliding Ground Velocity Vector if the target's sliding velocity with respect to "
            "the ground is zero."
        );
    }

    const Transform ITRF_GCRF_transform = Frame::ITRF()->getTransformTo(DEFAULT_PROFILE_FRAME, instant);

    const Vector3d slidingTargetGroundVelocityCoordinatesRotated =
        ITRF_GCRF_transform.applyToVector(targetSlidingGroundVelocityCoordinates);

    return slidingTargetGroundVelocityCoordinatesRotated.normalized();
}

Vector3d Profile::ComputeCelestialDirectionVector(const State& aState, const Celestial& aCelestial)
{
    const Vector3d celestialPositionCoordinates =
        aCelestial.getPositionIn(DEFAULT_PROFILE_FRAME, aState.getInstant()).accessCoordinates();
    const Vector3d satellitePositionCoordinates =
        aState.inFrame(DEFAULT_PROFILE_FRAME).getPosition().accessCoordinates();

    return (celestialPositionCoordinates - satellitePositionCoordinates).normalized();
}

Vector3d Profile::ComputeVelocityDirectionVector_ECI(const State& aState)
{
    return aState.inFrame(DEFAULT_PROFILE_FRAME).getVelocity().accessCoordinates().normalized();
}

Vector3d Profile::ComputeVelocityDirectionVector_ECEF(const State& aState)
{
    return aState.inFrame(Frame::ITRF()).getVelocity().accessCoordinates().normalized();
}

Vector3d Profile::ComputeOrbitalMomentumDirectionVector(const State& aState)
{
    const State state = aState.inFrame(DEFAULT_PROFILE_FRAME);
    const Vector3d positionDirection = state.getPosition().getCoordinates().normalized();
    const Vector3d velocityDirection = state.getVelocity().getCoordinates().normalized();

    return positionDirection.cross(velocityDirection).normalized();
}

Vector3d Profile::ComputeClockingVector(const Vector3d& anAlignmentVector, const Vector3d& aDesiredClockingVector)
{
    return (anAlignmentVector.cross(aDesiredClockingVector)).cross(anAlignmentVector).normalized();
}

Quaternion Profile::ComputeBodyToECIQuaternion(
    const Vector3d& anAlignmentDirection,
    const Vector3d& aClockingDirection,
    const Vector3d& anAlignmentTarget,
    const Vector3d& aClockingTarget
)
{
    // Given two matrices U and V, the rotation from U to V is compuated as:
    // U * R = V
    // R = U^-1 * V

    Matrix3d bodyFrameMatrix = Matrix3d::Zero();
    bodyFrameMatrix.row(0) = anAlignmentDirection.normalized();
    bodyFrameMatrix.row(1) = aClockingDirection.normalized();
    bodyFrameMatrix.row(2) = anAlignmentDirection.cross(aClockingDirection).normalized();

    Matrix3d inertialFrameMatrix = Matrix3d::Zero();
    inertialFrameMatrix.row(0) = anAlignmentTarget.normalized();
    inertialFrameMatrix.row(1) = aClockingTarget.normalized();
    inertialFrameMatrix.row(2) = anAlignmentTarget.cross(aClockingTarget).normalized();

    Matrix3d rotationMatrix = bodyFrameMatrix.transpose() * inertialFrameMatrix;

    Quaternion quaternion = Quaternion::RotationMatrix(rotationMatrix).toNormalized();
    quaternion.rectify();

    return quaternion;
}

Vector3d Profile::AxisToDirection(const Axis& anAxis, const bool& isAntiDirection)
{
    const Real sign = isAntiDirection ? -1.0 : 1.0;

    switch (anAxis)
    {
        case Axis::X:
            return Vector3d(sign, 0.0, 0.0);
        case Axis::Y:
            return Vector3d(0.0, sign, 0.0);
        case Axis::Z:
            return Vector3d(0.0, 0.0, sign);
        default:
            throw ostk::core::error::runtime::Wrong("Axis");
    }
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
