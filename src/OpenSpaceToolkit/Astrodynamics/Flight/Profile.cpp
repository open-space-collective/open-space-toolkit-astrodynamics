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
    const Target& anAlignmentTarget, const Target& aClockingTarget, const Angle& anAngularOffset
)
{
    if ((anAlignmentTarget.type == TargetType::VelocityECEF) || (aClockingTarget.type == TargetType::VelocityECEF))
    {
        throw ostk::core::error::runtime::ToBeImplemented("Velocity ECEF");
    }
    if ((anAlignmentTarget.type == aClockingTarget.type) && (anAlignmentTarget.type != TargetType::Trajectory))
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

    const auto axisVectorGenerator = [](const Target& aTarget) -> std::function<Vector3d(const State&)>
    {
        switch (aTarget.type)
        {
            case TargetType::GeocentricNadir:
                return Profile::ComputeGeocentricNadirDirectionVector;
            case TargetType::GeodeticNadir:
                return Profile::ComputeGeodeticNadirDirectionVector;
            case TargetType::Trajectory:
                return [aTarget](const State& aState)
                {
                    return Profile::ComputeTargetDirectionVector(aState, aTarget.trajectory);
                };
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
            default:
                throw ostk::core::error::RuntimeError("Invalid alignment target type.");
        }
    };

    const auto alignmentAxisVectorFunction = axisVectorGenerator(anAlignmentTarget);
    const auto clockingAxisVectorFunction = axisVectorGenerator(aClockingTarget);

    const Integer alignmentSign = anAlignmentTarget.antiDirection ? -1 : 1;
    const Integer clockingSign = aClockingTarget.antiDirection ? -1 : 1;

    return [anAngularOffset,
            alignmentAxisVectorFunction,
            clockingAxisVectorFunction,
            anAlignmentTarget,
            aClockingTarget,
            alignmentSign,
            clockingSign](const State& aState) -> Quaternion
    {
        const Vector3d alignemntAxisVector = alignmentAxisVectorFunction(aState) * alignmentSign;
        const Vector3d clockingVector = clockingAxisVectorFunction(aState) * clockingSign;

        const Vector3d clockingAxisVector = Profile::ComputeClockingAxisVector(alignemntAxisVector, clockingVector);

        const Real thetaOffsetRad = anAngularOffset.inRadians();

        const Vector3d rotatedClockingAxisVector =
            (clockingAxisVector * std::cos(thetaOffsetRad) +
             (alignemntAxisVector.cross(clockingAxisVector)) * std::sin(thetaOffsetRad) +
             alignemntAxisVector * (alignemntAxisVector.dot(clockingAxisVector)) * (1.0 - std::cos(thetaOffsetRad)));

        return Profile::ComputeBodyToECIQuaternion(
            anAlignmentTarget.axis, aClockingTarget.axis, alignemntAxisVector, rotatedClockingAxisVector
        );
    };
}

Profile Profile::GenerateTrackingProfile(
    const trajectory::Orbit& anOrbit, const std::function<Quaternion(const State&)>& anOrientationGenerator
)
{
    auto dynamicProviderGenerator = [&anOrbit, &anOrientationGenerator](const Instant& anInstant) -> Transform
    {
        const State state = anOrbit.getStateAt(anInstant);

        const Position position_GCRF = state.getPosition();
        const Velocity velocity_GCRF = state.getVelocity();

        return Transform::Active(
            anInstant,
            -position_GCRF.accessCoordinates(),
            -velocity_GCRF.accessCoordinates(),
            anOrientationGenerator(state),
            Vector3d(0.0, 0.0, 0.0)  // TBM: Artificially set to 0 for now.
        );
    };

    return Profile(TransformModel(DynamicProvider(dynamicProviderGenerator), Frame::GCRF()));
}

Profile::Profile()
    : modelUPtr_(nullptr)
{
}

Vector3d Profile::ComputeGeocentricNadirDirectionVector(const State& aState)
{
    return -aState.getPosition().accessCoordinates().normalized();
}

Vector3d Profile::ComputeGeodeticNadirDirectionVector(const State& aState)
{
    const Transform ITRF_GCRF_transform = Frame::ITRF()->getTransformTo(Frame::GCRF(), aState.accessInstant());

    const LLA lla = LLA::Cartesian(
        ITRF_GCRF_transform.getInverse().applyToPosition(aState.getPosition().accessCoordinates()),
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

Vector3d Profile::ComputeCelestialDirectionVector(const State& aState, const Celestial& aCelestial)
{
    const Vector3d celestialPositionCoordinates =
        aCelestial.getPositionIn(Frame::GCRF(), aState.getInstant()).accessCoordinates();
    const Vector3d satellitePositionCoordinates = aState.getPosition().accessCoordinates();

    return (celestialPositionCoordinates - satellitePositionCoordinates).normalized();
}

Vector3d Profile::ComputeVelocityDirectionVector_ECI(const State& aState)
{
    return aState.getVelocity().accessCoordinates().normalized();
}

Vector3d Profile::ComputeVelocityDirectionVector_ECEF(const State& aState)
{
    return aState.inFrame(Frame::ITRF()).getVelocity().accessCoordinates().normalized();
}

Vector3d Profile::ComputeTargetDirectionVector(const State& aState, const Trajectory& aTrajectory)
{
    if (!aTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory");
    }

    const Vector3d targetPositionCoordinates =
        aTrajectory.getStateAt(aState.accessInstant()).getPosition().accessCoordinates();
    const Vector3d satellitePositionCoordinates = aState.getPosition().accessCoordinates();

    return (targetPositionCoordinates - satellitePositionCoordinates).normalized();
}

Vector3d Profile::ComputeClockingAxisVector(const Vector3d& anAlignmentAxisVector, const Vector3d& aClockingVector)
{
    return (anAlignmentAxisVector.cross(aClockingVector)).cross(anAlignmentAxisVector).normalized();
}

Quaternion Profile::ComputeBodyToECIQuaternion(
    const Axis& anAlignmentAxis,
    const Axis& aClockingAxis,
    const Vector3d& anAlignmentAxisVector,
    const Vector3d& aClockingAxisVector
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

    const std::set<Axis> usedAxes = {anAlignmentAxis, aClockingAxis};

    std::vector<Axis> difference;
    std::set_difference(
        allAxes.begin(), allAxes.end(), usedAxes.begin(), usedAxes.end(), std::inserter(difference, difference.begin())
    );
    const Axis triadAxis = difference.front();

    Integer firstAxisVectorIndex = Integer::Undefined();
    Integer secondAxisVectorIndex = Integer::Undefined();
    std::tie(firstAxisVectorIndex, secondAxisVectorIndex) =
        triadAxisIndexMap.at(Tuple<Axis, Axis> {anAlignmentAxis, aClockingAxis});

    Matrix3d rotationMatrix = Matrix3d::Zero();

    rotationMatrix.row(axisIndexMap.at(anAlignmentAxis)) = anAlignmentAxisVector;
    rotationMatrix.row(axisIndexMap.at(aClockingAxis)) = aClockingAxisVector;
    rotationMatrix.row(axisIndexMap.at(triadAxis)) =
        rotationMatrix.row(firstAxisVectorIndex).cross(rotationMatrix.row(secondAxisVectorIndex));

    return Quaternion::RotationMatrix(rotationMatrix);
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
