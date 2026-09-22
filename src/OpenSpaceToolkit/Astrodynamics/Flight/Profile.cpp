/// Apache License 2.0

#include <optional>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
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

using ostk::physics::environment::object::celestial::Earth;
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

/// Time step used to compute the angular velocity of custom pointing profiles by finite difference of the orientation.
static const Duration ANGULAR_VELOCITY_FINITE_DIFFERENCE_STEP = Duration::Seconds(0.1);

Profile::Target::Target(const TargetType& aType, const Vector3d& aDirection)
    : type(aType),
      direction(aDirection)
{
    if (std::abs(aDirection.norm() - 1.0) > Real::Epsilon())
    {
        throw ostk::core::error::RuntimeError("Direction is not a unit vector.");
    }
}

Profile::Target::Target(const TargetType& aType, const Axis& anAxis, const bool& isAntiDirection)
    : type(aType),
      direction(Profile::AxisToDirection(anAxis, isAntiDirection))
{
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

    if (aType != TargetType::TargetPosition && aType != TargetType::TargetVelocity &&
        aType != TargetType::TargetSlidingGroundVelocity)
    {
        throw ostk::core::error::runtime::Wrong("Target type");
    }
}

Profile::OrientationProfileTarget::OrientationProfileTarget(
    const Array<Pair<Instant, Vector3d>>& anOrientationProfile,
    const Vector3d& aDirection,
    const Interpolator::Type& anInterpolatorType
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
        interpolators_.add(Interpolator::GenerateInterpolator(anInterpolatorType, timestamps, coordinates.col(i)));
    }
}

Profile::OrientationProfileTarget::OrientationProfileTarget(
    const Array<Pair<Instant, Vector3d>>& anOrientationProfile,
    const Axis& anAxis,
    const bool& isAntiDirection,
    const Interpolator::Type& anInterpolatorType
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
        interpolators_.add(Interpolator::GenerateInterpolator(anInterpolatorType, timestamps, coordinates.col(i)));
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

    return modelUPtr_->constructBodyFrame(aFrameName);
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
    const Shared<const Target>& aClockingTargetSPtr,
    const Angle& anAngularOffset
)
{
    const auto orientationGenerator = Profile::AlignAndConstrain(
        anAlignmentTargetSPtr, aClockingTargetSPtr, anOrbit.accessCelestialObject(), anAngularOffset
    );

    return Profile::CustomPointing(anOrbit, orientationGenerator);
}

Profile Profile::CustomPointing(
    const trajectory::Orbit& anOrbit, const std::function<Quaternion(const State&)>& anOrientationGenerator
)
{
    // Copy the orbit and orientation generator to avoid dangling references.
    auto dynamicProviderGenerator = [anOrbit, anOrientationGenerator](const Instant& anInstant) -> Transform
    {
        const auto orientationAt = [&anOrbit, &anOrientationGenerator](const Instant& anEvaluationInstant) -> Quaternion
        {
            return anOrientationGenerator(anOrbit.getStateAt(anEvaluationInstant).inFrame(DEFAULT_PROFILE_FRAME));
        };

        // The orbit or the orientation generator may only be defined over a bounded time range (e.g. tabulated orbit
        // or tabulated target trajectory): a failing evaluation marks a boundary of that range.
        const auto tryOrientationAt = [&orientationAt](const Instant& anEvaluationInstant) -> std::optional<Quaternion>
        {
            try
            {
                return orientationAt(anEvaluationInstant);
            }
            catch (const std::exception&)
            {
                return std::nullopt;
            }
        };

        const Duration& step = ANGULAR_VELOCITY_FINITE_DIFFERENCE_STEP;

        const std::optional<Quaternion> q_B_GCRF_previous = tryOrientationAt(anInstant - step);

        const State state = anOrbit.getStateAt(anInstant).inFrame(DEFAULT_PROFILE_FRAME);
        const Quaternion q_B_GCRF = anOrientationGenerator(state);

        const std::optional<Quaternion> q_B_GCRF_next = tryOrientationAt(anInstant + step);

        const Vector3d w_B_GCRF_in_B = [&]() -> Vector3d
        {
            if (q_B_GCRF_previous.has_value() && q_B_GCRF_next.has_value())
            {
                // Central difference
                return Profile::ComputeAngularVelocity(*q_B_GCRF_previous, *q_B_GCRF_next, step * 2.0);
            }

            if (q_B_GCRF_next.has_value())
            {
                // Forward difference (start of the time range)
                return Profile::ComputeAngularVelocity(q_B_GCRF, *q_B_GCRF_next, step);
            }

            if (q_B_GCRF_previous.has_value())
            {
                // Backward difference (end of the time range)
                return Profile::ComputeAngularVelocity(*q_B_GCRF_previous, q_B_GCRF, step);
            }

            throw ostk::core::error::RuntimeError(
                "Cannot compute the angular velocity at [{}]: the orientation is undefined at both [{}] and [{}].",
                anInstant.toString(),
                (anInstant - step).toString(),
                (anInstant + step).toString()
            );
        }();

        const Position position = state.getPosition();
        const Velocity velocity = state.getVelocity();

        return Transform::Active(
            anInstant, -position.accessCoordinates(), -velocity.accessCoordinates(), q_B_GCRF, w_B_GCRF_in_B
        );
    };

    return Profile(TransformModel(DynamicProvider(dynamicProviderGenerator), DEFAULT_PROFILE_FRAME));
}

std::function<Quaternion(const State&)> Profile::AlignAndConstrain(
    const Shared<const Target>& anAlignmentTargetSPtr,
    const Shared<const Target>& aClockingTargetSPtr,
    const Angle& anAngularOffset
)
{
    const Shared<const Celestial> celestialSPtr = std::make_shared<Earth>(Earth::Default());
    return Profile::AlignAndConstrain(anAlignmentTargetSPtr, aClockingTargetSPtr, celestialSPtr, anAngularOffset);
}

std::function<Quaternion(const State&)> Profile::AlignAndConstrain(
    const Shared<const Target>& anAlignmentTargetSPtr,
    const Shared<const Target>& aClockingTargetSPtr,
    const Shared<const Celestial>& aCelestialSPtr,
    const Angle& anAngularOffset
)
{
    if ((anAlignmentTargetSPtr->type == aClockingTargetSPtr->type) &&
        (anAlignmentTargetSPtr->type != TargetType::TargetPosition) &&
        (anAlignmentTargetSPtr->type != TargetType::TargetVelocity))
    {
        throw ostk::core::error::RuntimeError("Alignment and clocking target cannot be the same.");
    }

    {
        const Vector3d& bodyFrameAlignment = anAlignmentTargetSPtr->direction;
        const Vector3d& bodyFrameClocking = aClockingTargetSPtr->direction;
        const Real cosTheta = bodyFrameAlignment.normalized().dot(bodyFrameClocking.normalized());
        if (std::abs(cosTheta) >= 1.0 - Real::Epsilon())
        {
            throw ostk::core::error::RuntimeError("Alignment and clocking direction cannot be colinear.");
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

    const Shared<const Frame> celestialFrameSPtr = aCelestialSPtr->accessFrame();

    const auto targetVectorGenerator = [aCelestialSPtr, celestialFrameSPtr](const Shared<const Target>& aTargetSPtr
                                       ) -> std::function<Vector3d(const State&)>
    {
        switch (aTargetSPtr->type)
        {
            case TargetType::GeocentricNadir:
                return Profile::ComputeGeocentricNadirDirectionVector;
            case TargetType::GeodeticNadir:
                return [aCelestialSPtr](const State& aState) -> Vector3d
                {
                    return Profile::ComputeGeodeticNadirDirectionVector(aState, aCelestialSPtr);
                };
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
                return [targetVelocitySPtr, celestialFrameSPtr](const State& aState) -> Vector3d
                {
                    return Profile::ComputeTargetSlidingGroundVelocityVector(
                        aState, targetVelocitySPtr->trajectory, celestialFrameSPtr
                    );
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

    const Vector3d bodyFrameAlignment = anAlignmentTargetSPtr->direction;
    const Vector3d bodyFrameClocking =
        Profile::ComputeClockingVector(bodyFrameAlignment, aClockingTargetSPtr->direction);
    const auto inertialFrameAlignmentGenerator = targetVectorGenerator(anAlignmentTargetSPtr);
    const auto inertialFrameClockingGenerator = targetVectorGenerator(aClockingTargetSPtr);

    return [anAngularOffset,
            inertialFrameAlignmentGenerator,
            inertialFrameClockingGenerator,
            bodyFrameAlignment,
            bodyFrameClocking](const State& aState) -> Quaternion
    {
        const Vector3d inertialFrameAlignment = inertialFrameAlignmentGenerator(aState);
        const Vector3d inertialFrameClocking =
            Profile::ComputeClockingVector(inertialFrameAlignment, inertialFrameClockingGenerator(aState));

        // Apply angular offset rotation around the alignment axis to the clocking target using Rodrigues' formula
        const Real thetaOffsetRad = anAngularOffset.inRadians();
        const Vector3d inertialFrameClockingRotated =
            (inertialFrameClocking * std::cos(thetaOffsetRad) +
             (inertialFrameAlignment.cross(inertialFrameClocking)) * std::sin(thetaOffsetRad) +
             inertialFrameAlignment * (inertialFrameAlignment.dot(inertialFrameClocking)) *
                 (1.0 - std::cos(thetaOffsetRad)));

        // Quaternion from inertial frame to body frame
        const RotationMatrix rotationMatrix = RotationMatrix::VectorBasis(
            {inertialFrameAlignment, inertialFrameClockingRotated}, {bodyFrameAlignment, bodyFrameClocking}
        );
        return Quaternion::RotationMatrix(rotationMatrix).toNormalized().toRectify();
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

Vector3d Profile::ComputeGeodeticNadirDirectionVector(
    const State& aState, const Shared<const Celestial>& aCelestialSPtr
)
{
    const Shared<const Frame> celestialFrameSPtr = aCelestialSPtr->accessFrame();
    const Transform celestialFrame_GCRF_transform =
        celestialFrameSPtr->getTransformTo(DEFAULT_PROFILE_FRAME, aState.accessInstant());

    const LLA lla = LLA::Cartesian(
        celestialFrame_GCRF_transform.getInverse().applyToPosition(
            aState.inFrame(DEFAULT_PROFILE_FRAME).getPosition().accessCoordinates()
        ),
        aCelestialSPtr->getEquatorialRadius(),
        aCelestialSPtr->getFlattening()
    );

    const Vector3d nadir = {
        -std::cos(lla.getLatitude().inRadians()) * std::cos(lla.getLongitude().inRadians()),
        -std::cos(lla.getLatitude().inRadians()) * std::sin(lla.getLongitude().inRadians()),
        -std::sin(lla.getLatitude().inRadians()),
    };

    return celestialFrame_GCRF_transform.applyToVector(nadir).normalized();
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
    const State& aState,
    const ostk::astrodynamics::Trajectory& aTrajectory,
    const Shared<const Frame>& aCelestialFrameSPtr
)
{
    const Instant& instant = aState.accessInstant();

    const Vector3d targetSlidingGroundVelocityCoordinates =
        aTrajectory.getStateAt(instant).inFrame(aCelestialFrameSPtr).getVelocity().accessCoordinates();

    if (targetSlidingGroundVelocityCoordinates.isZero())
    {
        throw ostk::core::error::RuntimeError(
            "Cannot compute a Target Sliding Ground Velocity Vector if the target's sliding velocity with respect to "
            "the ground is zero."
        );
    }

    const Transform celestialFrame_GCRF_transform = aCelestialFrameSPtr->getTransformTo(DEFAULT_PROFILE_FRAME, instant);

    const Vector3d slidingTargetGroundVelocityCoordinatesRotated =
        celestialFrame_GCRF_transform.applyToVector(targetSlidingGroundVelocityCoordinates);

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

Vector3d Profile::ComputeAngularVelocity(
    const Quaternion& aStartOrientation, const Quaternion& anEndOrientation, const Duration& aTimeStep
)
{
    if (!aTimeStep.isDefined() || aTimeStep.isZero())
    {
        throw ostk::core::error::runtime::Wrong("Time step");
    }

    // Rotation from the body frame at the start of the time step (B1) to the body frame at its end (B2), i.e.
    // q_B2_B1 = q_B2_REF * q_REF_B1. Rectified so that the rotation angle lies in [0, π].
    const Quaternion q_B2_B1 = (anEndOrientation * aStartOrientation.toConjugate()).toNormalized().toRectify();

    const Vector3d vectorPart = q_B2_B1.getVectorPart();
    const Real vectorPartNorm = vectorPart.norm();

    if (vectorPartNorm == 0.0)
    {
        return Vector3d::Zero();
    }

    // Rotation angle from atan2 rather than acos, as the latter is ill-conditioned for the small angles at play here.
    const Real rotationAngle_rad = 2.0 * std::atan2(vectorPartNorm, q_B2_B1.getScalarPart());

    // The rotation vector is expressed in the body frame, hence so is the angular velocity.
    return vectorPart * (rotationAngle_rad / vectorPartNorm / aTimeStep.inSeconds());
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
