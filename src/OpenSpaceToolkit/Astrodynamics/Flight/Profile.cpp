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
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::frame::Manager;
using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;

using TransformModel = ostk::astrodynamics::flight::profile::model::Transform;

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

Profile Profile::InertialPointing(const Trajectory& aTrajectory, const Quaternion& aQuaternion)
{
    using ostk::astrodynamics::flight::profile::model::Transform;

    return {Transform::InertialPointing(aTrajectory, aQuaternion)};
}

Profile Profile::NadirPointing(const trajectory::Orbit& anOrbit, const trajectory::Orbit::FrameType& anOrbitalFrameType)
{
    using ostk::astrodynamics::flight::profile::model::Transform;

    return {Transform::NadirPointing(anOrbit, anOrbitalFrameType)};
}

std::function<Quaternion(const State&)> Profile::GenerateCustomOrientation(
    const Target& anAlignmentTarget,
    const Target& aClockingTarget,
    const Angle& anAngularOffset,
    const Trajectory& aTrajectory
)
{
    if (anAlignmentTarget.type == aClockingTarget.type)
    {
        throw ostk::core::error::RuntimeError("Alignment and clocking target cannot be the same.");
    }

    if (anAlignmentTarget.axis == aClockingTarget.axis)
    {
        throw ostk::core::error::RuntimeError("Alignment and clocking axis cannot be the same.");
    }

    if (std::set<TargetType>({anAlignmentTarget.type, aClockingTarget.type}) == std::set<TargetType>({
                                                                                    TargetType::GeocentricNadir,
                                                                                    TargetType::GeodeticNadir,
                                                                                }))
    {
        throw ostk::core::error::RuntimeError(
            "Alignment and clocking target cannot be both geocentric nadir and geodetic nadir."
        );
    }

    if ((std::set<TargetType>({anAlignmentTarget.type, aClockingTarget.type}).count(TargetType::Trajectory)) &&
        !aTrajectory.isDefined())
    {
        throw ostk::core::error::RuntimeError("A target trajectory must be provided for space or ground targets.");
    }

    const auto axisVectorGenerator = [&aTrajectory](const TargetType& aTargetType
                                     ) -> std::function<Vector3d(const State&)>
    {
        if (aTargetType == TargetType::GeocentricNadir)
        {
            return Profile::GetGeocentricNadirDirectionVector;
        }
        else if (aTargetType == TargetType::GeodeticNadir)
        {
            return Profile::GetGeodeticNadirDirectionVector;
        }
        else if (aTargetType == TargetType::Trajectory)
        {
            return [&aTrajectory](const State& aState)
            {
                return Profile::GetTargetDirectionVector(aState, aTrajectory);
            };
        }
        else if (aTargetType == TargetType::Sun)
        {
            return [](const State& aState)
            {
                return Profile::GetCelestialDirectionVector(aState, Sun::Default());
            };
        }
        else if (aTargetType == TargetType::Moon)
        {
            return [](const State& aState)
            {
                return Profile::GetCelestialDirectionVector(aState, Moon::Default());
            };
        }
        else if (aTargetType == TargetType::Velocity)
        {
            return Profile::GetVelocityDirectionVector;
        }
        else
        {
            throw ostk::core::error::RuntimeError("Invalid alignment target type.");
        }
    };

    const auto alignmentAxisVectorFunction = axisVectorGenerator(anAlignmentTarget.type);
    const auto clockingAxisVectorFunction = axisVectorGenerator(aClockingTarget.type);

    const Integer alignmentSign = anAlignmentTarget.getSign();
    const Integer clockingSign = aClockingTarget.getSign();

    return [&anAngularOffset,
            &alignmentAxisVectorFunction,
            &clockingAxisVectorFunction,
            &anAlignmentTarget,
            &aClockingTarget,
            &alignmentSign,
            &clockingSign](const State& aState) -> Quaternion
    {
        const Vector3d alignmentAxisVector = alignmentAxisVectorFunction(aState) * alignmentSign;
        const Vector3d clockingVector = clockingAxisVectorFunction(aState) * clockingSign;

        const Vector3d clockingAxisVector = Profile::GetClockingAxisVector(alignmentAxisVector, clockingVector);

        const Real thetaOffsetRad = anAngularOffset.inRadians();

        const Vector3d rotatedClockingAxisVector =
            (clockingAxisVector * std::cos(thetaOffsetRad) +
             (alignmentAxisVector.cross(clockingAxisVector)) * std::sin(thetaOffsetRad) +
             alignmentAxisVector * (alignmentAxisVector.dot(clockingAxisVector)) * (1.0 - std::cos(thetaOffsetRad)));

        const RotationMatrix rotationMatrix = Profile::GetRotationMatrix(
            anAlignmentTarget.axis, aClockingTarget.axis, alignmentAxisVector, rotatedClockingAxisVector
        );

        return Quaternion::RotationMatrix(rotationMatrix);
    };
}

Profile Profile::GenerateTrackingProfile(
    const trajectory::Orbit& orbit, const std::function<Quaternion(const State&)>& calculateOrientation
)
{
    auto dynamicProviderGenerator = [&orbit, &calculateOrientation](const Instant& instant) -> Transform
    {
        const State state = orbit.getStateAt(instant);

        const Position position_GCRF = state.getPosition();
        const Velocity velocity_GCRF = state.getVelocity();

        return Transform::Active(
            instant,
            -position_GCRF.accessCoordinates(),
            -velocity_GCRF.accessCoordinates(),
            calculateOrientation(state),
            Vector3d(0.0, 0.0, 0.0)  // TBM: Artificially set to 0 for now.
        );
    };

    return Profile(TransformModel(DynamicProvider(dynamicProviderGenerator), Frame::GCRF()));
}

Profile::Profile()
    : modelUPtr_(nullptr)
{
}

Vector3d Profile::GetGeocentricNadirDirectionVector(const State& state)
{
    return -state.getPosition().accessCoordinates().normalized();
}

Vector3d Profile::GetGeodeticNadirDirectionVector(const State& state)
{
    const LLA lla = LLA::Cartesian(
        state.getPosition().accessCoordinates(),
        EarthGravitationalModel::EGM2008.equatorialRadius_,
        EarthGravitationalModel::EGM2008.flattening_
    );
    const Vector3d nadir = {
        -std::cos(lla.getLatitude().inRadians()) * std::cos(lla.getLongitude().inRadians()),
        -std::cos(lla.getLatitude().inRadians()) * std::sin(lla.getLongitude().inRadians()),
        -std::sin(lla.getLatitude().inRadians()),
    };

    const Transform itrfGcrfTransform = Frame::ITRF()->getTransformTo(Frame::GCRF(), state.getInstant());

    return itrfGcrfTransform.applyToVector(nadir).normalized();
}

Vector3d Profile::GetCelestialDirectionVector(const State& state, const Celestial& aCelestial)
{
    const Vector3d celestialPositionCoordinates =
        aCelestial.getPositionIn(Frame::GCRF(), state.getInstant()).accessCoordinates();
    const Vector3d satellitePositionCoordinates = state.getPosition().accessCoordinates();

    return (celestialPositionCoordinates - satellitePositionCoordinates).normalized();
}

Vector3d Profile::GetVelocityDirectionVector(const State& state)
{
    return state.getVelocity().accessCoordinates().normalized();
}

Vector3d Profile::GetTargetDirectionVector(const State& state, const Trajectory& targetTrajectory)
{
    if (!targetTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }

    const Vector3d targetPositionCoordinates =
        targetTrajectory.getStateAt(state.accessInstant()).getPosition().accessCoordinates();
    const Vector3d satellitePositionCoordinates = state.getPosition().accessCoordinates();

    return (targetPositionCoordinates - satellitePositionCoordinates).normalized();
}

Vector3d Profile::GetClockingAxisVector(const Vector3d& alignmentAxisVector, const Vector3d& clockingVector)
{
    return (alignmentAxisVector.cross(clockingVector)).cross(alignmentAxisVector).normalized();
}

RotationMatrix Profile::GetRotationMatrix(
    const Axis& alignmentAxis,
    const Axis& clockingAxis,
    const Vector3d& alignmentAxisVector,
    const Vector3d& clockingAxisVector
)
{
    static const Map<Axis, Integer> axisIndexMap = {
        {Axis::X, 0},
        {Axis::Y, 1},
        {Axis::Z, 2},
    };

    static const Map<Tuple<Axis, Axis>, Tuple<Integer, Integer>> triadAxisIndexMap = {
        {{Axis::X, Axis::Y}, {0, 1}},
        {{Axis::X, Axis::Z}, {2, 0}},
        {{Axis::Y, Axis::X}, {0, 1}},
        {{Axis::Y, Axis::Z}, {1, 2}},
        {{Axis::Z, Axis::X}, {2, 0}},
        {{Axis::Z, Axis::Y}, {1, 2}},
    };
    static const std::set<Axis> allAxes = {Axis::X, Axis::Y, Axis::Z};
    const std::set<Axis> usedAxes = {alignmentAxis, clockingAxis};

    std::vector<Axis> difference;
    std::set_difference(
        allAxes.begin(), allAxes.end(), usedAxes.begin(), usedAxes.end(), std::inserter(difference, difference.begin())
    );
    const Axis triadAxis = difference.front();

    Integer firstAxisVectorIndex = Integer::Undefined();
    Integer secondAxisVectorIndex = Integer::Undefined();
    std::tie(firstAxisVectorIndex, secondAxisVectorIndex) =
        triadAxisIndexMap.at(Tuple<Axis, Axis> {alignmentAxis, clockingAxis});

    Matrix3d rotationMatrix = Matrix3d::Zero();

    rotationMatrix.row(axisIndexMap.at(alignmentAxis)) = alignmentAxisVector;
    rotationMatrix.row(axisIndexMap.at(clockingAxis)) = clockingAxisVector;
    rotationMatrix.row(axisIndexMap.at(triadAxis)) =
        rotationMatrix.row(firstAxisVectorIndex).cross(rotationMatrix.row(secondAxisVectorIndex));

    return RotationMatrix(rotationMatrix);
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
