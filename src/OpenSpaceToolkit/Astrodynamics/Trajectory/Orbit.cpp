/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationVector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Utility.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::Index;
using ostk::core::type::Real;
using ostk::core::type::Uint8;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationVector;
using ostk::mathematics::object::Vector3d;

using orbit::model::Kepler;
using orbit::model::kepler::COE;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::Duration;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::RootSolver;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, ostk::physics::unit::Time::Unit::Second);

static const Derived::Unit AngularVelocitySIUnit =
    Derived::Unit::AngularVelocity(ostk::physics::unit::Angle::Unit::Radian, ostk::physics::unit::Time::Unit::Second);

static const Real epsilon = 1e-6;

static const RootSolver rootSolver = RootSolver::Default();

Orbit::Orbit(const orbit::Model& aModel, const Shared<const Celestial>& aCelestialObjectSPtr)
    : Trajectory(aModel),
      modelPtr_(dynamic_cast<const orbit::Model*>(&this->accessModel())),
      celestialObjectSPtr_(aCelestialObjectSPtr)
{
}

Orbit::Orbit(
    const Array<State>& aStateArray,
    const Integer& anInitialRevolutionNumber,
    const Shared<const Celestial>& aCelestialObjectSPtr
)
    : Trajectory(orbit::model::Tabulated(aStateArray, anInitialRevolutionNumber)),
      modelPtr_(dynamic_cast<const orbit::Model*>(&this->accessModel())),
      celestialObjectSPtr_(aCelestialObjectSPtr)
{
}

Orbit::Orbit(const Orbit& anOrbit)
    : Trajectory(anOrbit),
      modelPtr_(dynamic_cast<const orbit::Model*>(&this->accessModel())),
      celestialObjectSPtr_(anOrbit.celestialObjectSPtr_)
{
}

Orbit::~Orbit()
{
    using FrameManager = ostk::physics::coordinate::frame::Manager;

    static const Array<Orbit::FrameType> frameTypes = {
        Orbit::FrameType::NED,
        Orbit::FrameType::LVLH,
        Orbit::FrameType::VVLH,
        Orbit::FrameType::LVLHGD,
        Orbit::FrameType::LVLHGDGT,
        Orbit::FrameType::QSW,
        Orbit::FrameType::TNW,
        Orbit::FrameType::VNC
    };

    for (const auto& frameType : frameTypes)
    {
        const String frameName = this->generateFrameName(frameType);

        if (FrameManager::Get().hasFrameWithName(frameName))
        {
            FrameManager::Get().removeFrameWithName(frameName);
        }
    }
}

Orbit& Orbit::operator=(const Orbit& anOrbit)
{
    if (this != &anOrbit)
    {
        Trajectory::operator=(anOrbit);

        this->modelPtr_ = dynamic_cast<const orbit::Model*>(&this->accessModel());
        this->celestialObjectSPtr_ = anOrbit.celestialObjectSPtr_;

        this->passMap_.clear();
    }

    return *this;
}

bool Orbit::operator==(const Orbit& anOrbit) const
{
    if ((!this->isDefined()) || (!anOrbit.isDefined()))
    {
        return false;
    }

    return Trajectory::operator==(anOrbit);
}

bool Orbit::operator!=(const Orbit& anOrbit) const
{
    return !((*this) == anOrbit);
}

bool Orbit::isDefined() const
{
    return Trajectory::isDefined() && (this->celestialObjectSPtr_ != nullptr) &&
           this->celestialObjectSPtr_->isDefined();
}

Integer Orbit::getRevolutionNumberAt(const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Orbit");
    }

    return this->modelPtr_->calculateRevolutionNumberAt(anInstant);
}

Pass Orbit::getPassAt(const Instant& anInstant) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Orbit");
    }

    return this->getPassWithRevolutionNumber(this->getRevolutionNumberAt(anInstant));
}

Pass Orbit::getPassWithRevolutionNumber(const Integer& aRevolutionNumber, const Duration& aStepDuration) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Orbit");
    }

    const std::lock_guard<std::mutex> lock {this->mutex_};

    const auto passMapIt = this->passMap_.find(aRevolutionNumber);

    if (passMapIt != this->passMap_.end())
    {
        return passMapIt->second;
    }
    else
    {
        // If any, get pass with closest revolution number

        Pass const* closestPasSPtr = nullptr;

        const auto lowerBoundMapIt = this->passMap_.lower_bound(aRevolutionNumber);

        if (lowerBoundMapIt != this->passMap_.end())
        {
            if (lowerBoundMapIt == this->passMap_.begin())
            {
                closestPasSPtr = &(lowerBoundMapIt->second);
            }
            else
            {
                const auto closestPassMapIt = std::prev(lowerBoundMapIt);

                if ((aRevolutionNumber - closestPassMapIt->first) < (lowerBoundMapIt->first - aRevolutionNumber))
                {
                    closestPasSPtr = &(closestPassMapIt->second);
                }
                else
                {
                    closestPasSPtr = &(lowerBoundMapIt->second);
                }
            }
        }
        else if (this->passMap_.size() > 0)
        {
            closestPasSPtr = &(this->passMap_.begin()->second);
        }

        Pass currentPass = Pass::Undefined();

        if (closestPasSPtr != nullptr)  // Pass with closest revolution number found
        {
            currentPass = *closestPasSPtr;
        }

        const Instant epoch = this->modelPtr_->getEpoch();

        const auto getZ = [this, &epoch](const double& aDurationInSeconds) -> Real
        {
            return this->modelPtr_->calculateStateAt(epoch + Duration::Seconds(aDurationInSeconds))
                .getPosition()
                .accessCoordinates()
                .z();
        };

        const auto getZDot = [this, &epoch](const double& aDurationInSeconds) -> Real
        {
            return this->modelPtr_->calculateStateAt(epoch + Duration::Seconds(aDurationInSeconds))
                .getVelocity()
                .accessCoordinates()
                .z();
        };

        while ((!currentPass.isDefined()) || (currentPass.getRevolutionNumber() != aRevolutionNumber))
        {
            Integer currentRevolutionNumber = currentPass.isDefined() ? currentPass.getRevolutionNumber()
                                                                      : this->modelPtr_->getRevolutionNumberAtEpoch();
            Instant previousInstant = Instant::Undefined();

            Duration stepDuration = aStepDuration;
            if (currentPass.isDefined() && currentPass.isComplete())
            {
                Array<Duration> durations = {
                    (currentPass.accessInstantAtNorthPoint() - currentPass.accessInstantAtAscendingNode()),
                    (currentPass.accessInstantAtDescendingNode() - currentPass.accessInstantAtNorthPoint()),
                    (currentPass.accessInstantAtSouthPoint() - currentPass.accessInstantAtDescendingNode()),
                    (currentPass.accessInstantAtPassBreak() - currentPass.accessInstantAtSouthPoint()),
                };
                stepDuration = *std::min_element(durations.begin(), durations.end()) / 2.0;
            }

            if (currentRevolutionNumber <= aRevolutionNumber)  // Forward propagation
            {
                previousInstant = currentPass.isDefined() ? currentPass.getEndInstant() + Duration::Microseconds(1.0)
                                                          : this->modelPtr_->getEpoch();

                const State previousState = this->modelPtr_->calculateStateAt(previousInstant);
                Real previousStateCoordinates_ECI_z = previousState.getPosition().accessCoordinates().z();
                Real previousStateCoordinates_ECI_zdot = previousState.getVelocity().accessCoordinates().z();

                Instant northPointCrossing = Instant::Undefined();
                Instant southPointCrossing = Instant::Undefined();
                Instant descendingNodeCrossing = Instant::Undefined();

                while (true)
                {
                    const Instant currentInstant = previousInstant + stepDuration;

                    const State currentState = this->modelPtr_->calculateStateAt(currentInstant);
                    const Real currentStateCoordinates_ECI_z = currentState.getPosition().accessCoordinates().z();
                    const Real currentStateCoordinates_ECI_zdot = currentState.getVelocity().accessCoordinates().z();

                    if ((previousStateCoordinates_ECI_z == 0.0) && (currentStateCoordinates_ECI_z == 0.0))
                    {
                        throw ostk::core::error::runtime::ToBeImplemented("Equatorial orbit support.");
                    }

                    if (((previousStateCoordinates_ECI_zdot > 0.0) && (currentStateCoordinates_ECI_zdot <= 0.0)) ||
                        ((previousStateCoordinates_ECI_zdot < 0.0) && (currentStateCoordinates_ECI_zdot >= 0.0)))
                    {
                        if (currentStateCoordinates_ECI_z > 0.0)
                        {
                            northPointCrossing =
                                Orbit::GetCrossingInstant(epoch, previousInstant, currentInstant, getZDot);
                        }
                        else
                        {
                            southPointCrossing =
                                Orbit::GetCrossingInstant(epoch, previousInstant, currentInstant, getZDot);
                        }
                    }

                    if ((previousStateCoordinates_ECI_z > 0.0) && (currentStateCoordinates_ECI_z <= 0.0))
                    {
                        descendingNodeCrossing =
                            Orbit::GetCrossingInstant(epoch, previousInstant, currentInstant, getZ);
                    }

                    if ((previousStateCoordinates_ECI_z < 0.0) && (currentStateCoordinates_ECI_z >= 0.0))
                    {
                        previousInstant = Orbit::GetCrossingInstant(epoch, previousInstant, currentInstant, getZ);
                        break;
                    }

                    previousStateCoordinates_ECI_z = currentStateCoordinates_ECI_z;
                    previousStateCoordinates_ECI_zdot = currentStateCoordinates_ECI_zdot;
                    previousInstant = currentInstant;
                }

                if (currentPass.isDefined())
                {
                    currentPass = {
                        currentRevolutionNumber + 1,
                        currentPass.accessInstantAtPassBreak(),
                        northPointCrossing.isDefined() ? northPointCrossing : currentPass.accessInstantAtNorthPoint(),
                        descendingNodeCrossing.isDefined() ? descendingNodeCrossing
                                                           : currentPass.accessInstantAtDescendingNode(),
                        southPointCrossing.isDefined() ? southPointCrossing : currentPass.accessInstantAtSouthPoint(),
                        previousInstant,
                    };
                }
                else
                {
                    const Instant ascendingNodeCrossing =
                        Real(this->modelPtr_->calculateStateAt(this->modelPtr_->getEpoch())
                                 .getPosition()
                                 .accessCoordinates()
                                 .z())
                                .isNear(0.0, epsilon)
                            ? this->modelPtr_->getEpoch()
                            : Instant::Undefined();

                    currentPass = {
                        currentRevolutionNumber,
                        ascendingNodeCrossing,
                        northPointCrossing,
                        descendingNodeCrossing,
                        southPointCrossing,
                        previousInstant,
                    };
                }
            }
            else  // Reverse propagation
            {
                previousInstant = currentPass.isDefined() ? currentPass.getStartInstant() - Duration::Microseconds(1.0)
                                                          : this->modelPtr_->getEpoch();

                const State previousState = this->modelPtr_->calculateStateAt(previousInstant);
                Real previousStateCoordinates_ECI_z = previousState.getPosition().accessCoordinates().z();
                Real previousStateCoordinates_ECI_zdot = previousState.getVelocity().accessCoordinates().z();

                Instant northPointCrossing = Instant::Undefined();
                Instant southPointCrossing = Instant::Undefined();
                Instant descendingNodeCrossing = Instant::Undefined();

                while (true)
                {
                    const Instant currentInstant = previousInstant - stepDuration;

                    const State currentState = this->modelPtr_->calculateStateAt(currentInstant);
                    const Real currentStateCoordinates_ECI_z = currentState.getPosition().accessCoordinates().z();
                    const Real currentStateCoordinates_ECI_zdot = currentState.getVelocity().accessCoordinates().z();

                    if ((previousStateCoordinates_ECI_z == 0.0) && (currentStateCoordinates_ECI_z == 0.0))
                    {
                        throw ostk::core::error::runtime::ToBeImplemented("Equatorial orbit support.");
                    }

                    if (((previousStateCoordinates_ECI_zdot > 0.0) && (currentStateCoordinates_ECI_zdot <= 0.0)) ||
                        ((previousStateCoordinates_ECI_zdot < 0.0) && (currentStateCoordinates_ECI_zdot >= 0.0)))
                    {
                        if (currentStateCoordinates_ECI_z > 0.0)
                        {
                            northPointCrossing =
                                Orbit::GetCrossingInstant(epoch, previousInstant, currentInstant, getZDot);
                        }
                        else
                        {
                            southPointCrossing =
                                Orbit::GetCrossingInstant(epoch, previousInstant, currentInstant, getZDot);
                        }
                    }

                    if ((previousStateCoordinates_ECI_z > 0.0) && (currentStateCoordinates_ECI_z <= 0.0))
                    {
                        previousInstant = Orbit::GetCrossingInstant(epoch, previousInstant, currentInstant, getZ);
                        break;
                    }

                    if ((previousStateCoordinates_ECI_z < 0.0) && (currentStateCoordinates_ECI_z >= 0.0))
                    {
                        descendingNodeCrossing =
                            Orbit::GetCrossingInstant(epoch, previousInstant, currentInstant, getZ);
                    }

                    previousStateCoordinates_ECI_z = currentStateCoordinates_ECI_z;
                    previousStateCoordinates_ECI_zdot = currentStateCoordinates_ECI_zdot;
                    previousInstant = currentInstant;
                }

                if (currentPass.isDefined())
                {
                    currentPass = {
                        currentRevolutionNumber - 1,
                        previousInstant,
                        northPointCrossing.isDefined() ? northPointCrossing : currentPass.accessInstantAtNorthPoint(),
                        descendingNodeCrossing.isDefined() ? descendingNodeCrossing
                                                           : currentPass.accessInstantAtDescendingNode(),
                        southPointCrossing.isDefined() ? southPointCrossing : currentPass.accessInstantAtSouthPoint(),
                        currentPass.accessInstantAtAscendingNode(),
                    };
                }
                else
                {
                    const Instant passBreakCrossing =
                        Real(this->modelPtr_->calculateStateAt(this->modelPtr_->getEpoch())
                                 .getPosition()
                                 .accessCoordinates()
                                 .z())
                                .isNear(0.0, epsilon)
                            ? this->modelPtr_->getEpoch()
                            : Instant::Undefined();

                    currentPass = {
                        currentRevolutionNumber,
                        previousInstant,
                        northPointCrossing,
                        descendingNodeCrossing,
                        southPointCrossing,
                        passBreakCrossing,
                    };
                }
            }

            if (currentPass.isDefined())
            {
                this->passMap_.insert({currentPass.getRevolutionNumber(), currentPass});
            }
        }

        if (currentPass.getRevolutionNumber() == aRevolutionNumber)
        {
            return currentPass;
        }
        else
        {
            throw ostk::core::error::RuntimeError("Cannot get pass with revolution # [{}].", aRevolutionNumber);
        }
    }

    return Pass::Undefined();
}

Shared<const Frame> Orbit::getOrbitalFrame(const Orbit::FrameType& aFrameType) const
{
    using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
    using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
    using ostk::mathematics::geometry::d3::transformation::rotation::RotationVector;
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::coordinate::spherical::LLA;
    using ostk::physics::time::Duration;
    using FrameManager = ostk::physics::coordinate::frame::Manager;
    using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;
    using ostk::physics::coordinate::Transform;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Orbit");
    }

    const String frameName = this->generateFrameName(aFrameType);

    if (FrameManager::Get().hasFrameWithName(frameName))
    {
        return FrameManager::Get().accessFrameWithName(frameName);
    }

    const auto generateDynamicProvider =
        [this](const auto& anAttitudeGenerator, const Shared<const Frame>& aReferenceFrame) -> auto
    {
        const Shared<const DynamicProvider> dynamicProviderSPtr = std::make_shared<const DynamicProvider>(
            // TBM: Using `this` here will trigger a segfault if the Orbit is de-allocated while the Frame is used.
            [this, anAttitudeGenerator, aReferenceFrame](const Instant& anInstant) -> Transform
            {
                const State state = this->getStateAt(anInstant).inFrame(aReferenceFrame);

                const Vector3d x_GCRF = state.getPosition().accessCoordinates();
                const Vector3d v_GCRF = state.getVelocity().accessCoordinates();

                const Quaternion q_LOF_GCRF = anAttitudeGenerator(state);  // LOF -> Local Orbital Frame

                const Duration delta = Duration::Seconds(0.1);  // TBM This should be a parameter

                const Quaternion q_LOF_GCRF_next =
                    anAttitudeGenerator(this->getStateAt(anInstant + delta).inFrame(Frame::GCRF()));

                const Quaternion q_LOF_next_LOF = (q_LOF_GCRF_next * q_LOF_GCRF.toConjugate()).toNormalized();
                const RotationVector rv_LOF_next_LOF = RotationVector::Quaternion(q_LOF_next_LOF);

                const Vector3d w_LOF_GCRF_in_LOF = rv_LOF_next_LOF.getAxis() * (rv_LOF_next_LOF.getAngle().inRadians() /
                                                                                delta.inSeconds());  // [rad/s]

                return {
                    anInstant,
                    -x_GCRF,
                    -v_GCRF,
                    q_LOF_GCRF,
                    w_LOF_GCRF_in_LOF,
                    Transform::Type::Passive,
                };
            }
        );

        return dynamicProviderSPtr;
    };

    Shared<const Frame> orbitalFrameSPtr = nullptr;

    switch (aFrameType)
    {
        case Orbit::FrameType::NED:
        {
            const auto calculateAttitude = [this](const State& aState) -> Quaternion
            {
                // Get state in central body centered, central body fixed frame

                const State state =
                    this->getStateAt(aState.getInstant()).inFrame(this->celestialObjectSPtr_->accessFrame());

                // Express the state position in geodetic coordinates

                const LLA lla = LLA::Cartesian(
                    state.getPosition().accessCoordinates(),
                    this->celestialObjectSPtr_->getEquatorialRadius(),
                    this->celestialObjectSPtr_->getFlattening()
                );

                // Compute the NED frame to central body centered, central body fixed frame transform at position

                const Transform transform = ostk::physics::coordinate::frame::utilities::NorthEastDownTransformAt(
                    lla,
                    this->celestialObjectSPtr_->getEquatorialRadius(),
                    celestialObjectSPtr_->getFlattening()
                );  // [TBM] This should be optimized: LLA <> ECEF calculation done twice

                const Quaternion q_NED_GCRF = transform.getOrientation().toNormalized();

                return q_NED_GCRF;
            };

            orbitalFrameSPtr = Frame::Construct(
                frameName,
                false,
                this->celestialObjectSPtr_->accessFrame(),
                generateDynamicProvider(calculateAttitude, this->celestialObjectSPtr_->accessFrame())
            );

            break;
        }

        case Orbit::FrameType::LVLH:
        case Orbit::FrameType::QSW:
        {
            // X axis along position vector
            // Z axis along orbital momentum
            // Y axis toward velocity vector

            const auto calculateAttitude = [](const State& aState) -> Quaternion
            {
                const Vector3d x_GCRF = aState.getPosition().accessCoordinates();
                const Vector3d v_GCRF = aState.getVelocity().accessCoordinates();

                const Vector3d xAxis = x_GCRF.normalized();
                const Vector3d zAxis = x_GCRF.cross(v_GCRF).normalized();
                const Vector3d yAxis = zAxis.cross(xAxis);

                const Quaternion q_LVLH_GCRF =
                    Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

                return q_LVLH_GCRF;
            };

            orbitalFrameSPtr = Frame::Construct(
                frameName, false, Frame::GCRF(), generateDynamicProvider(calculateAttitude, Frame::GCRF())
            );

            break;
        }

        case Orbit::FrameType::VVLH:
        {
            // Z axis along negative position vector
            // Y axis along negative orbital momentum
            // X axis toward velocity vector

            const auto calculateAttitude = [](const State& aState) -> Quaternion
            {
                const Vector3d x_GCRF = aState.getPosition().accessCoordinates();
                const Vector3d v_GCRF = aState.getVelocity().accessCoordinates();

                const Vector3d zAxis = -x_GCRF.normalized();
                const Vector3d yAxis = -x_GCRF.cross(v_GCRF).normalized();
                const Vector3d xAxis = yAxis.cross(zAxis);

                const Quaternion q_VVLH_GCRF =
                    Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

                return q_VVLH_GCRF;
            };

            orbitalFrameSPtr = Frame::Construct(
                frameName, false, Frame::GCRF(), generateDynamicProvider(calculateAttitude, Frame::GCRF())
            );

            break;
        }

        case Orbit::FrameType::TNW:
        {
            // X axis along velocity vector
            // Z axis along orbital momentum

            const auto calculateAttitude = [](const State& aState) -> Quaternion
            {
                const Vector3d x_GCRF = aState.getPosition().accessCoordinates();
                const Vector3d v_GCRF = aState.getVelocity().accessCoordinates();

                const Vector3d xAxis = v_GCRF.normalized();
                const Vector3d zAxis = x_GCRF.cross(v_GCRF).normalized();
                const Vector3d yAxis = zAxis.cross(xAxis);

                const Quaternion q_TNW_GCRF =
                    Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

                return q_TNW_GCRF;
            };

            orbitalFrameSPtr = Frame::Construct(
                frameName, false, Frame::GCRF(), generateDynamicProvider(calculateAttitude, Frame::GCRF())
            );

            break;
        }

        case Orbit::FrameType::VNC:
        {
            // X axis along velocity vector
            // Y axis along orbital momentum

            const auto calculateAttitude = [](const State& aState) -> Quaternion
            {
                const Vector3d x_GCRF = aState.getPosition().accessCoordinates();
                const Vector3d v_GCRF = aState.getVelocity().accessCoordinates();

                const Vector3d xAxis = v_GCRF.normalized();
                const Vector3d yAxis = x_GCRF.cross(v_GCRF).normalized();
                const Vector3d zAxis = xAxis.cross(yAxis);

                const Quaternion q_VNC_GCRF =
                    Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

                return q_VNC_GCRF;
            };

            orbitalFrameSPtr = Frame::Construct(
                frameName, false, Frame::GCRF(), generateDynamicProvider(calculateAttitude, Frame::GCRF())
            );

            break;
        }

        case Orbit::FrameType::LVLHGD:
        {
            // X axis along geodetic position vector
            // Z axis toward orbital momentum
            // Y axis toward velocity vector

            const auto calculateAttitude = [this](const State& aState) -> Quaternion
            {
                // Express the state position in geodetic coordinates

                const LLA lla = LLA::Cartesian(
                    aState.inFrame(this->celestialObjectSPtr_->accessFrame()).getPosition().accessCoordinates(),
                    this->celestialObjectSPtr_->getEquatorialRadius(),
                    this->celestialObjectSPtr_->getFlattening()
                );

                const Vector3d x_GCRF = aState.getPosition().accessCoordinates();
                const Vector3d v_GCRF = aState.getVelocity().accessCoordinates();

                const Real latitude_radians = lla.getLatitude().inRadians();
                const Real longitude_radians = lla.getLongitude().inRadians();

                const Vector3d nadir_ITRF = {
                    std::cos(latitude_radians) * std::cos(longitude_radians),
                    std::cos(latitude_radians) * std::sin(longitude_radians),
                    std::sin(latitude_radians),
                };

                const Vector3d xAxis = -(Frame::ITRF()->getTransformTo(Frame::GCRF(), aState.accessInstant()))
                                            .applyToVector(nadir_ITRF)
                                            .normalized();

                const Vector3d zAxis = (xAxis.cross(x_GCRF.cross(v_GCRF).normalized())).cross(xAxis).normalized();
                const Vector3d yAxis = zAxis.cross(xAxis).normalized();

                const Quaternion q_LVLHGD_GCRF =
                    Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

                return q_LVLHGD_GCRF;
            };

            orbitalFrameSPtr = Frame::Construct(
                frameName, false, Frame::GCRF(), generateDynamicProvider(calculateAttitude, Frame::GCRF())
            );

            break;
        }

        case Orbit::FrameType::LVLHGDGT:
        {
            // X axis along geodetic position vector
            // Z axis along orbital momentum
            // Y axis toward velocity vector

            const auto calculateAttitude = [this](const State& aState) -> Quaternion
            {
                // TBM: We can calculate the geodetic vector in a simpler fashion, refer to FDTk profile.py

                // Express the state position in geodetic coordinates
                const LLA lla = LLA::Cartesian(
                    aState.inFrame(this->celestialObjectSPtr_->accessFrame()).getPosition().accessCoordinates(),
                    this->celestialObjectSPtr_->getEquatorialRadius(),
                    this->celestialObjectSPtr_->getFlattening()
                );

                const Vector3d x_GCRF = aState.getPosition().accessCoordinates();
                const Vector3d v_GCRF = aState.getVelocity().accessCoordinates();

                const Real latitude_radians = lla.getLatitude().inRadians();
                const Real longitude_radians = lla.getLongitude().inRadians();

                const Vector3d nadir_ITRF = {
                    std::cos(latitude_radians) * std::cos(longitude_radians),
                    std::cos(latitude_radians) * std::sin(longitude_radians),
                    std::sin(latitude_radians),
                };

                const Vector3d xAxis = -(Frame::ITRF()->getTransformTo(Frame::GCRF(), aState.accessInstant()))
                                            .applyToVector(nadir_ITRF)
                                            .normalized();

                const Vector3d zAxis = (xAxis.cross(x_GCRF.cross(v_GCRF).normalized())).cross(xAxis).normalized();
                const Vector3d yAxis = zAxis.cross(xAxis).normalized();

                const Quaternion q_LVLHGD_GCRF =
                    Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

                const COE coe = COE::Cartesian(
                    {aState.getPosition(), aState.getVelocity()},
                    this->celestialObjectSPtr_->getGravitationalParameter()
                );
                const Angle inclination = coe.getInclination();
                const Derived meanMotion = coe.getMeanMotion(this->celestialObjectSPtr_->getGravitationalParameter());
                const Derived nodalPrecessionRate = coe.getNodalPrecessionRate(
                    this->celestialObjectSPtr_->getGravitationalParameter(),
                    this->celestialObjectSPtr_->getEquatorialRadius(),
                    this->celestialObjectSPtr_->getJ2()
                );

                const bool isPassDescending = v_GCRF.z() < 0.0;

                const Real w_ITRF_GCRF_in_ITRF_z =
                    (Frame::GCRF()->getTransformTo(Frame::ITRF(), aState.accessInstant())).getAngularVelocity().z();

                const Real k = meanMotion.in(AngularVelocitySIUnit) /
                               (w_ITRF_GCRF_in_ITRF_z - nodalPrecessionRate.in(AngularVelocitySIUnit));

                Real temp1 = std::pow(std::cos(lla.getLatitude().inRadians()), 2.0) -
                             std::pow(std::cos(inclination.inRadians()), 2.0);
                temp1 = temp1 > 0.0 ? std::sqrt(temp1) : 0.0;
                const Real temp2 = k - std::cos(inclination.inRadians());
                const Real delta = (isPassDescending ? 1.0 : -1.0) * std::atan2(temp1, temp2);

                const Quaternion q_LVLHGDGT_LVLHGD =
                    Quaternion::RotationVector(RotationVector::X(Angle::Radians(delta)));

                return (q_LVLHGDGT_LVLHGD * q_LVLHGD_GCRF).toNormalized();
            };

            orbitalFrameSPtr = Frame::Construct(
                frameName, false, Frame::GCRF(), generateDynamicProvider(calculateAttitude, Frame::GCRF())
            );

            break;
        }

        default:
            throw ostk::core::error::runtime::Wrong("Frame type");
            break;
    }

    FrameManager::Get().addFrame(orbitalFrameSPtr);

    return orbitalFrameSPtr;
}

void Orbit::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Orbit") : void();

    Trajectory::print(anOutputStream, false);

    const std::lock_guard<std::mutex> lock {this->mutex_};

    for (const auto& passIt : this->passMap_)
    {
        const Pass& pass = passIt.second;

        ostk::core::utils::Print::Separator(anOutputStream, String::Format("Pass #{}", pass.getRevolutionNumber()));

        pass.print(anOutputStream, false);
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Orbit Orbit::Undefined()
{
    return {Array<State>::Empty(), Integer::Undefined(), nullptr};
}

Orbit Orbit::Circular(
    const Instant& anEpoch,
    const Length& anAltitude,
    const Angle& anInclination,
    const Shared<const Celestial>& aCelestialObjectSPtr
)
{
    using orbit::model::Kepler;
    using orbit::model::kepler::COE;

    if (!anEpoch.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Epoch");
    }

    if (!anAltitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Altitude");
    }

    if (!anInclination.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Inclination");
    }

    if ((aCelestialObjectSPtr == nullptr) || (!aCelestialObjectSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Celestial object");
    }

    const Length semiMajorAxis = aCelestialObjectSPtr->getEquatorialRadius() + anAltitude;
    const Real eccentricity = 0.0;
    const Angle inclination = anInclination;
    const Angle raan = Angle::Zero();
    const Angle aop = Angle::Zero();
    const Angle trueAnomaly = Angle::Zero();

    const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

    const Kepler orbitalModel = {coe, anEpoch, (*aCelestialObjectSPtr), Kepler::PerturbationType::None, false};

    return {orbitalModel, aCelestialObjectSPtr};
}

Orbit Orbit::Equatorial(
    const Instant& anEpoch,
    const Length& anApoapsisAltitude,
    const Length& aPeriapsisAltitude,
    const Shared<const Celestial>& aCelestialObjectSPtr
)
{
    using orbit::model::Kepler;
    using orbit::model::kepler::COE;

    if (!anEpoch.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Epoch");
    }

    if (!anApoapsisAltitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Apoapsis altitude");
    }

    if (!aPeriapsisAltitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Periapsis altitude");
    }

    if ((aCelestialObjectSPtr == nullptr) || (!aCelestialObjectSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Celestial object");
    }

    if (anApoapsisAltitude < aPeriapsisAltitude)
    {
        throw ostk::core::error::RuntimeError(
            "Apoapsis altitude [{}] lower than periapsis altitude [{}].",
            anApoapsisAltitude.toString(),
            aPeriapsisAltitude.toString()
        );
    }

    const Real r_a = aCelestialObjectSPtr->getEquatorialRadius().inMeters() + anApoapsisAltitude.inMeters();
    const Real r_p = aCelestialObjectSPtr->getEquatorialRadius().inMeters() + aPeriapsisAltitude.inMeters();

    const Real eccentricity = (r_a - r_p) / (r_a + r_p);
    const Length semiMajorAxis = Length::Meters(r_a / (1.0 + eccentricity));
    const Angle inclination = Angle::Zero();
    const Angle raan = Angle::Zero();
    const Angle aop = Angle::Zero();
    const Angle trueAnomaly = Angle::Zero();

    const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

    const Kepler orbitalModel = {coe, anEpoch, (*aCelestialObjectSPtr), Kepler::PerturbationType::None, false};

    return {orbitalModel, aCelestialObjectSPtr};
}

Orbit Orbit::CircularEquatorial(
    const Instant& anEpoch, const Length& anAltitude, const Shared<const Celestial>& aCelestialObjectSPtr
)
{
    return Orbit::Circular(anEpoch, anAltitude, Angle::Zero(), aCelestialObjectSPtr);
}

Orbit Orbit::GeoSynchronous(
    const Instant& anEpoch,
    const Angle& anInclination,
    const Angle& aLongitude,
    const Shared<const Celestial>& aCelestialObjectSPtr
)
{
    using ostk::mathematics::object::Vector3d;

    using ostk::physics::coordinate::Position;
    using ostk::physics::coordinate::spherical::LLA;

    using orbit::model::Kepler;
    using orbit::model::kepler::COE;

    if (!anEpoch.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Epoch");
    }

    if (!anInclination.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Inclination");
    }

    if (!aLongitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Longitude");
    }

    if ((aCelestialObjectSPtr == nullptr) || (!aCelestialObjectSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Celestial object");
    }

    if (aCelestialObjectSPtr->getType() != Celestial::Type::Earth)
    {
        throw ostk::core::error::runtime::ToBeImplemented(
            "Geosynchronous orbits currently not suppported for celestial bodies other than Earth"
        );
    }

    // [TBI] Add a way to calculate the sidereal period of each planet to generalize this
    const Length geosynchronousAltitude = Length::Meters(35786000.0);

    // Convert the given longitude (earth referenced) into a usable raan (inertial referenced)
    const LLA lla = {Angle::Zero(), aLongitude, geosynchronousAltitude};
    const Vector3d ascendingNodeVectorITRF =
        lla.toCartesian(aCelestialObjectSPtr->getEquatorialRadius(), aCelestialObjectSPtr->getFlattening());
    const Position ascendingNodePositionITRF = Position::Meters(ascendingNodeVectorITRF, Frame::ITRF());
    const Vector3d ascendingNodeVectorGCRF = ascendingNodePositionITRF.inFrame(Frame::GCRF(), anEpoch).getCoordinates();

    // Define COEs that make up this orbit
    const Length semiMajorAxis = aCelestialObjectSPtr->getEquatorialRadius() + geosynchronousAltitude;
    const Real eccentricity = Real::Epsilon();
    const Angle inclination = anInclination;
    const Angle raan = Angle::Radians(std::atan2(ascendingNodeVectorGCRF[1], ascendingNodeVectorGCRF[0]));
    const Angle aop = Angle::Zero();
    const Angle trueAnomaly = Angle::Zero();

    const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

    const Kepler orbitalModel = {coe, anEpoch, (*aCelestialObjectSPtr), Kepler::PerturbationType::J2, false};

    return {orbitalModel, aCelestialObjectSPtr};
}

Orbit Orbit::SunSynchronous(
    const Instant& anEpoch,
    const Length& anAltitude,
    const Time& aLocalTimeAtDescendingNode,
    const Shared<const Celestial>& aCelestialObjectSPtr,
    const Angle& anArgumentOfLatitude
)
{
    if (!anEpoch.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Epoch");
    }

    if (!anAltitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Altitude");
    }

    if (!aLocalTimeAtDescendingNode.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("LTDN");
    }

    if ((aCelestialObjectSPtr == nullptr) || (!aCelestialObjectSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Celestial object");
    }

    if (!anArgumentOfLatitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Argument of latitude");
    }

    const auto calculateSunSynchronousInclination = [&aCelestialObjectSPtr](const Length& aSemiMajorAxis) -> Angle
    {
        /// @ref Capderou M., Handbook of Satellite Orbits: From Kepler to GPS, p.292

        const Real a = aSemiMajorAxis.inMeters();
        const Real R = aCelestialObjectSPtr->getEquatorialRadius().inMeters();
        const Real mu = aCelestialObjectSPtr->getGravitationalParameter().in(GravitationalParameterSIUnit);
        const Real j2 = aCelestialObjectSPtr->getJ2();

        const Real T_sid = 31558149.504;                                                 // [s] Sidereal year
        const Real k_h = 3.0 / (4.0 * M_PI) * j2 * std::sqrt(mu / (R * R * R)) * T_sid;  // Sun-synchronicity constant

        return Angle::Radians(std::acos(-1.0 / k_h * std::pow((a / R), (7.0 / 2.0))));
    };

    const auto calculateEquationOfTime = [](const Instant& anInstant) -> Angle
    {
        const Real julianDate = anInstant.getJulianDate(Scale::UTC);

        // Julian Date of J2000.0

        static const Real julianDate_J2000 = 2451545.0;

        // Number of Julian centuries from J2000.0

        const Real T_UT1 = (julianDate - julianDate_J2000) / 36525.0;

        // Mean longitude of the Sun

        const Real sunMeanLongitude_deg = std::fmod(280.460 + 36000.771 * T_UT1, 360.0);

        // Mean anomaly of the Sun

        const Real sunMeanAnomaly_rad = Angle::Degrees(std::fmod(357.5291092 + 35999.05034 * T_UT1, 360.0)).inRadians();

        // Ecliptic latitude of the Sun

        const Real sunEclipticLatitude_rad =
            Angle::Degrees(std::fmod(
                               sunMeanLongitude_deg + 1.914666471 * std::sin(sunMeanAnomaly_rad) +
                                   0.019994643 * std::sin(2.0 * sunMeanAnomaly_rad),
                               360.0
                           ))
                .inRadians();

        // Compute the equation of time

        const Real equationOfTime_deg =
            -1.914666471 * std::sin(sunMeanAnomaly_rad) - 0.019994643 * std::sin(2.0 * sunMeanAnomaly_rad) +
            2.466 * std::sin(2.0 * sunEclipticLatitude_rad) - 0.0053 * std::sin(4.0 * sunEclipticLatitude_rad);

        return Angle::Degrees(equationOfTime_deg);
    };

    const auto calculateRaan = [calculateEquationOfTime, &anEpoch](const Time& aLocalTimeAtAscendingNode) -> Angle
    {
        const Real localTime = (aLocalTimeAtAscendingNode.getHour() / 1.0) +
                               (aLocalTimeAtAscendingNode.getMinute() / 60.0) +
                               (aLocalTimeAtAscendingNode.getSecond() / 3600.0) +
                               (aLocalTimeAtAscendingNode.getMillisecond() / (3600.0 * 1e3)) +
                               (aLocalTimeAtAscendingNode.getMicrosecond() / (3600.0 * 1e6)) +
                               (aLocalTimeAtAscendingNode.getNanosecond() / (3600.0 * 1e9));

        Sun sun = Sun::Default();  // [TBM] This is a temporary solution

        // Sun direction in GCRF

        const Vector3d sunDirection_GCRF = sun.getPositionIn(Frame::GCRF(), anEpoch).getCoordinates().normalized();

        // Desired angle between the Sun and the ascending node

        const Angle alpha = Angle::Degrees((localTime - 12.0) / 12.0 * 180.0);

        // Sun Apparent Local Time (right ascension of the Sun in GCRF)
        // https://en.wikipedia.org/wiki/Solar_time#Apparent_solar_time

        const Angle apparentSolarTime = Angle::Radians(std::atan2(sunDirection_GCRF.y(), sunDirection_GCRF.x()));

        // Equation of Time
        // https://en.wikipedia.org/wiki/Equation_of_time

        const Angle equationOfTime = calculateEquationOfTime(anEpoch);

        // Sun Mean Local Time
        // https://en.wikipedia.org/wiki/Solar_time#Mean_solar_time

        const Angle meanSolarTime = apparentSolarTime + equationOfTime;

        // Right Ascension of the Ascending Node

        const Angle raan = Angle::Radians(std::fmod(meanSolarTime.inRadians() + alpha.inRadians(), Real::TwoPi()));

        return raan;
    };

    const auto calculateLocalTimeAtAscendingNode = [&aLocalTimeAtDescendingNode]() -> Time
    {
        return {
            Uint8((aLocalTimeAtDescendingNode.getHour() + 12) % 24),
            aLocalTimeAtDescendingNode.getMinute(),
            aLocalTimeAtDescendingNode.getSecond(),
            aLocalTimeAtDescendingNode.getMillisecond(),
            aLocalTimeAtDescendingNode.getMicrosecond(),
            aLocalTimeAtDescendingNode.getNanosecond()
        };
    };

    const Length semiMajorAxis = aCelestialObjectSPtr->getEquatorialRadius() + anAltitude;
    const Real eccentricity = 0.0;
    const Angle inclination = calculateSunSynchronousInclination(semiMajorAxis);
    const Angle raan = calculateRaan(calculateLocalTimeAtAscendingNode());
    const Angle aop = Angle::Zero();
    const Angle trueAnomaly = anArgumentOfLatitude - aop;

    const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

    const Kepler orbitalModel = {coe, anEpoch, (*aCelestialObjectSPtr), Kepler::PerturbationType::J2, false};

    // [TBM] STK propagates SSOs in the TrueOfOrbitEpoch (true equator and true equinox of the Earth at the orbit epoch)
    // frame, most likely to preserve the J2 symmetry around the true Z-axis (true equator normal).

    return {orbitalModel, aCelestialObjectSPtr};
}

String Orbit::StringFromFrameType(const Orbit::FrameType& aFrameType)
{
    switch (aFrameType)
    {
        case Orbit::FrameType::Undefined:
            return "Undefined";

        case Orbit::FrameType::NED:
            return "NED";

        case Orbit::FrameType::LVLH:
            return "LVLH";

        case Orbit::FrameType::VVLH:
            return "VVLH";

        case Orbit::FrameType::LVLHGD:
            return "LVLHGD";

        case Orbit::FrameType::LVLHGDGT:
            return "LVLHGDGT";

        case Orbit::FrameType::QSW:
            return "QSW";

        case Orbit::FrameType::TNW:
            return "TNW";

        case Orbit::FrameType::VNC:
            return "VNC";

        default:
            throw ostk::core::error::runtime::Wrong("Frame type");
            break;
    }

    return String::Empty();
}

String Orbit::generateFrameName(const Orbit::FrameType& aFrameType) const
{
    return String::Format("{} @ Orbit [{}]", Orbit::StringFromFrameType(aFrameType), fmt::ptr(this));
}

Array<Pair<Index, Pass>> Orbit::ComputePasses(const Array<State>& aStateArray, const Integer& anInitialRevolutionNumber)
{
    if (!anInitialRevolutionNumber.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Initial revolution number");
    }

    // [TBI] Deal w/ z_ECI always equal to 0.0 case (equatorial orbit)

    Array<Pair<Index, Pass>> passMap;

    if (aStateArray.getSize() < 2)
    {
        throw ostk::core::error::RuntimeError(
            "Greater than 2 states required to compute passes: {}", aStateArray.getSize()
        );
    }

    for (Index i = 1; i < aStateArray.getSize(); ++i)
    {
        if (aStateArray[i - 1].accessInstant() > aStateArray[i].accessInstant())
        {
            throw ostk::core::error::RuntimeError("States are not in chronological order.");
        }
    }

    const model::Tabulated tabulated = model::Tabulated(aStateArray, Interpolator::Type::BarycentricRational);

    const Instant& epoch = aStateArray.accessFirst().accessInstant();

    const auto getZ = [&tabulated, &epoch](const double& aDurationInSeconds) -> double
    {
        return tabulated.calculateStateAt(epoch + Duration::Seconds(aDurationInSeconds))
            .getPosition()
            .accessCoordinates()
            .z();
    };

    const auto getZDot = [&tabulated, &epoch](const double& aDurationInSeconds) -> double
    {
        return tabulated.calculateStateAt(epoch + Duration::Seconds(aDurationInSeconds))
            .getVelocity()
            .accessCoordinates()
            .z();
    };

    Index currentIndex = 0;
    State const* previousStatePtr = nullptr;

    Integer revolutionNumber = anInitialRevolutionNumber;

    Instant previousPassEndInstant =
        (Real(aStateArray.accessFirst().getPosition().accessCoordinates().z()).isNear(0.0, epsilon))
            ? aStateArray.accessFirst().accessInstant()
            : Instant::Undefined();
    Instant northPointCrossing = Instant::Undefined();
    Instant descendingNodeCrossing = Instant::Undefined();
    Instant southPointCrossing = Instant::Undefined();
    Instant passBreak = Instant::Undefined();

    for (const auto& state : aStateArray)
    {
        if (previousStatePtr != nullptr)
        {
            const Vector3d previousPositionCoordinates_ECI = previousStatePtr->getPosition().accessCoordinates();
            const Vector3d previousVelocityCoordinates_ECI = previousStatePtr->getVelocity().accessCoordinates();
            const Vector3d currentPositionCoordinates_ECI = state.getPosition().accessCoordinates();
            const Vector3d currentVelocityCoordinates_ECI = state.getVelocity().accessCoordinates();

            // North & South point crossings
            if (((previousVelocityCoordinates_ECI.z() > 0.0) && (currentVelocityCoordinates_ECI.z() <= 0.0)) ||
                ((previousVelocityCoordinates_ECI.z() < 0.0) && (currentVelocityCoordinates_ECI.z() >= 0.0)))
            {
                if (currentPositionCoordinates_ECI.z() > 0.0)
                {
                    northPointCrossing = Orbit::GetCrossingInstant(
                        epoch, previousStatePtr->accessInstant(), state.accessInstant(), getZDot
                    );
                }
                else
                {
                    southPointCrossing = Orbit::GetCrossingInstant(
                        epoch, previousStatePtr->accessInstant(), state.accessInstant(), getZDot
                    );
                }
            }

            // Descending node
            if ((previousPositionCoordinates_ECI.z() > 0.0) && (currentPositionCoordinates_ECI.z() <= 0.0))
            {
                descendingNodeCrossing =
                    Orbit::GetCrossingInstant(epoch, previousStatePtr->accessInstant(), state.accessInstant(), getZ);
            }

            // Pass break
            if ((previousPositionCoordinates_ECI.z() < 0.0) && (currentPositionCoordinates_ECI.z() >= 0.0))
            {
                passBreak =
                    Orbit::GetCrossingInstant(epoch, previousStatePtr->accessInstant(), state.accessInstant(), getZ);
            }

            if (passBreak.isDefined())
            {
                const Pass pass = {
                    revolutionNumber,
                    previousPassEndInstant,
                    northPointCrossing,
                    descendingNodeCrossing,
                    southPointCrossing,
                    passBreak,
                };

                passMap.add({currentIndex, pass});

                revolutionNumber++;
                previousPassEndInstant = passBreak;

                southPointCrossing = Instant::Undefined();
                northPointCrossing = Instant::Undefined();
                descendingNodeCrossing = Instant::Undefined();
                passBreak = Instant::Undefined();
            }
        }

        previousStatePtr = &state;
        currentIndex++;
    }

    // Add last partial pass
    const Pass pass = {
        revolutionNumber,
        previousPassEndInstant,
        northPointCrossing,
        descendingNodeCrossing,
        southPointCrossing,
        passBreak,
    };

    passMap.add({currentIndex, pass});

    return passMap;
}

Instant Orbit::GetCrossingInstant(
    const Instant& anEpoch,
    const Instant& previousInstant,
    const Instant& currentInstant,
    const std::function<double(double)>& getValue
)
{
    const RootSolver::Solution solution =
        rootSolver.bisection(getValue, (previousInstant - anEpoch).inSeconds(), (currentInstant - anEpoch).inSeconds());

    if (!solution.hasConverged)
    {
        throw ostk::core::error::RuntimeError("Root solver did not converge.");
    }

    return anEpoch + Duration::Seconds(solution.root);
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
