/// Apache License 2.0

#include <iostream>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationVector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Providers/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Utilities.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>
#include <OpenSpaceToolkit/Physics/Units/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::physics::units::Derived;
using ostk::physics::units::Length;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, ostk::physics::units::Time::Unit::Second);

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
    : Trajectory(orbit::models::Tabulated(aStateArray, anInitialRevolutionNumber)),
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
    using FrameManager = ostk::physics::coord::frame::Manager;

    static const Array<Orbit::FrameType> frameTypes = {
        Orbit::FrameType::NED,
        Orbit::FrameType::LVLH,
        Orbit::FrameType::VVLH,
        Orbit::FrameType::LVLHGD,
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

Pass Orbit::getPassWithRevolutionNumber(const Integer& aRevolutionNumber) const
{
    using ostk::core::types::Index;
    using ostk::core::types::Real;

    using ostk::math::object::Vector3d;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Interval;

    // [TBI] Dead with equatorial case

    // std::cout << "aRevolutionNumber = " << aRevolutionNumber.toString() << std::endl ;
    // std::cout << "this->passMap_.size() = " << this->passMap_.size() << std::endl ;

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

        Pass const* closestPassPtr = nullptr;

        const auto lowerBoundMapIt = this->passMap_.lower_bound(aRevolutionNumber);

        if (lowerBoundMapIt != this->passMap_.end())
        {
            // std::cout << "lowerBoundMap = " << lowerBoundMapIt->second << std::endl ;

            if (lowerBoundMapIt == this->passMap_.begin())
            {
                closestPassPtr = &(lowerBoundMapIt->second);
            }
            else
            {
                const auto closestPassMapIt = std::prev(lowerBoundMapIt);

                if ((aRevolutionNumber - closestPassMapIt->first) < (lowerBoundMapIt->first - aRevolutionNumber))
                {
                    closestPassPtr = &(closestPassMapIt->second);
                }
                else
                {
                    closestPassPtr = &(lowerBoundMapIt->second);
                }
            }
        }
        else if (this->passMap_.size() > 0)
        {
            // std::cout << "this->passMap_ IS NOT EMPTY" << std::endl ;
            closestPassPtr = &(this->passMap_.begin()->second);
        }

        Pass currentPass = Pass::Undefined();

        if (closestPassPtr != nullptr)  // Pass with closest revolution number found
        {
            currentPass = *closestPassPtr;
        }

        while ((!currentPass.isDefined()) || (currentPass.getRevolutionNumber() != aRevolutionNumber))
        {
            // std::cout << "currentPass = " << currentPass << std::endl ;
            static const Real tolerance = 1e-3;  // [TBM] Param

            Integer currentRevolutionNumber = currentPass.isDefined() ? currentPass.getRevolutionNumber()
                                                                      : this->modelPtr_->getRevolutionNumberAtEpoch();
            // std::cout << "currentRevolutionNumber = " << currentRevolutionNumber.toString() << std::endl ;

            Instant currentInstant =
                currentPass.isDefined() ? currentPass.getInterval().accessEnd() : this->modelPtr_->getEpoch();
            // std::cout << "currentInstant = " << currentInstant.toString() << std::endl ;
            Duration stepDuration = currentPass.isDefined() ? currentPass.getInterval().getDuration() / 5.0
                                                            : Duration::Minutes(10.0);  // [TBM] param
            // std::cout << "stepDuration = " << stepDuration.toString() << std::endl ;

            if (currentRevolutionNumber <= aRevolutionNumber)  // Forward propagation
            {
                Real previousStateCoordinates_ECI_z =
                    this->modelPtr_->calculateStateAt(currentInstant).getPosition().accessCoordinates().z();
                // std::cout << "previousStateCoordinates_ECI_z = " << previousStateCoordinates_ECI_z << std::endl ;

                Real residual = Real::Undefined();
                // std::cout << "residual = " << residual << std::endl ;
                Index iterationCount = 0;
                static const Index maxIterationCount = 1000;  // [TBM] Param
                // std::cout << "iterationCount = " << iterationCount << std::endl ;

                bool equatorCrossingDetected = false;
                // std::cout << "equatorCrossingDetected = " << equatorCrossingDetected << std::endl ;

                while (((!residual.isDefined()) || (residual < 0.0) || (residual.abs() > tolerance)) &&
                       (iterationCount < maxIterationCount))
                {
                    currentInstant += stepDuration;

                    const Real currentStateCoordinates_ECI_z =
                        this->modelPtr_->calculateStateAt(currentInstant).getPosition().accessCoordinates().z();
                    // std::cout << "currentStateCoordinates_ECI_z = " << currentStateCoordinates_ECI_z << std::endl ;

                    if ((previousStateCoordinates_ECI_z == 0.0) && (currentStateCoordinates_ECI_z == 0.0))
                    {
                        throw ostk::core::error::runtime::ToBeImplemented("Equatorial orbit support.");
                    }

                    if ((previousStateCoordinates_ECI_z < 0.0) && (currentStateCoordinates_ECI_z >= 0.0))
                    {
                        equatorCrossingDetected = true;
                        // std::cout << "equatorCrossingDetected = " << equatorCrossingDetected << std::endl ;
                    }

                    if (equatorCrossingDetected)
                    {
                        // const Real& z1_m = previousStateCoordinates_ECI_z ;
                        // const Real& z2_m = currentStateCoordinates_ECI_z ;
                        // const Real dz_m = currentStateCoordinates_ECI_z - previousStateCoordinates_ECI_z ;
                        // std::cout << "z1_m = " << z1_m.toString() << std::endl ;
                        // std::cout << "z2_m = " << z2_m.toString() << std::endl ;
                        // std::cout << "dz_m = " << dz_m.toString() << std::endl ;

                        // const Real dt_s = stepDuration.inSeconds() ;
                        // std::cout << "dt_s = " << dt_s.toString() << std::endl ;
                        // std::cout << "-z2_m * dt_s / dz_m = " << (-z2_m * dt_s / dz_m).toString(15) << std::endl ;

                        if ((currentStateCoordinates_ECI_z - previousStateCoordinates_ECI_z) != 0.0)
                        {
                            stepDuration = Duration::Seconds(
                                -currentStateCoordinates_ECI_z * stepDuration.inSeconds() /
                                (currentStateCoordinates_ECI_z - previousStateCoordinates_ECI_z)
                            );
                        }

                        if (stepDuration.isZero())
                        {
                            stepDuration = (currentStateCoordinates_ECI_z < 0.0) ? Duration::Nanoseconds(+1.0)
                                                                                 : Duration::Nanoseconds(-1.0);
                        }

                        // std::cout << "stepDuration = " << stepDuration.toString() << std::endl ;

                        residual = currentStateCoordinates_ECI_z;
                        // std::cout << "residual= " << residual.toString() << std::endl ;
                    }

                    previousStateCoordinates_ECI_z = currentStateCoordinates_ECI_z;

                    iterationCount++;
                }

                if (iterationCount > maxIterationCount)
                {
                    throw ostk::core::error::RuntimeError("Maximum iteration count reached.");
                }

                if (currentPass.isDefined())
                {
                    currentPass = {
                        Pass::Type::Complete,
                        currentRevolutionNumber + 1,
                        Interval::Closed(currentPass.getInterval().accessEnd(), currentInstant)
                    };
                }
                else
                {
                    if (this->modelPtr_->calculateStateAt(this->modelPtr_->getEpoch())
                            .getPosition()
                            .accessCoordinates()
                            .z() == 0.0)
                    {
                        currentPass = {
                            Pass::Type::Complete,
                            currentRevolutionNumber,
                            Interval::Closed(this->modelPtr_->getEpoch(), currentInstant)
                        };
                    }
                    else
                    {
                        currentPass = {
                            Pass::Type::Partial,
                            currentRevolutionNumber,
                            Interval::Closed(this->modelPtr_->getEpoch(), currentInstant)
                        };
                    }
                }

                // std::cout << "currentPass = " << currentPass << std::endl ;
                // previousStateCoordinates_ECI_z = tolerance * 2.0 ;

                currentRevolutionNumber++;
            }
            else  // Reverse propagation
            {
                throw ostk::core::error::runtime::ToBeImplemented(
                    "Orbit::getPassWithRevolutionNumber > Reverse propagation"
                );
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
    using ostk::math::geometry::d3::transformations::rotations::Quaternion;
    using ostk::math::geometry::d3::transformations::rotations::RotationMatrix;
    using ostk::math::geometry::d3::transformations::rotations::RotationVector;
    using ostk::math::object::Vector3d;

    using ostk::physics::coord::spherical::LLA;
    using ostk::physics::time::Duration;
    using FrameManager = ostk::physics::coord::frame::Manager;
    using DynamicProvider = ostk::physics::coord::frame::provider::Dynamic;
    using ostk::physics::coord::Transform;

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
                const Vector3d v_GCRF_in_GCRF = state.getVelocity().accessCoordinates();

                const Vector3d x_VVLH_GCRF_in_GCRF = -x_GCRF;          // [m]
                const Vector3d v_VVLH_GCRF_in_GCRF = -v_GCRF_in_GCRF;  // [m/s]

                const Quaternion q_VVLH_GCRF = anAttitudeGenerator(state);

                const Duration delta = Duration::Seconds(1.0);  // TBM This should be a parameter

                const Quaternion q_VVLH_next_GCRF =
                    anAttitudeGenerator(this->getStateAt(anInstant + delta).inFrame(Frame::GCRF()));

                const Quaternion q_VVLH_next_VVLH = (q_VVLH_next_GCRF * q_VVLH_GCRF.toConjugate()).toNormalized();
                const RotationVector rv_VVLH_next_VVLH = RotationVector::Quaternion(q_VVLH_next_VVLH);

                const Vector3d w_VVLH_GCRF_in_VVLH =
                    rv_VVLH_next_VVLH.getAxis() *
                    (rv_VVLH_next_VVLH.getAngle().inRadians() / delta.inSeconds());  // [rad/s]

                return {
                    anInstant,
                    x_VVLH_GCRF_in_GCRF,
                    v_VVLH_GCRF_in_GCRF,
                    q_VVLH_GCRF,
                    w_VVLH_GCRF_in_VVLH,
                    Transform::Type::Passive
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

                const Transform transform = ostk::physics::coord::frame::utilities::NorthEastDownTransformAt(
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

        case Orbit::FrameType::QSW:
        {
            // X axis along position vector
            // Z axis along orbital momentum

            const auto calculateAttitude = [](const State& aState) -> Quaternion
            {
                const Vector3d x_GCRF = aState.getPosition().accessCoordinates();
                const Vector3d v_GCRF = aState.getVelocity().accessCoordinates();

                const Vector3d xAxis = x_GCRF.normalized();
                const Vector3d zAxis = x_GCRF.cross(v_GCRF).normalized();
                const Vector3d yAxis = zAxis.cross(xAxis);

                const Quaternion q_QSW_GCRF =
                    Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

                return q_QSW_GCRF;
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
            throw ostk::core::error::runtime::ToBeImplemented(Orbit::StringFromFrameType(aFrameType));
            break;

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

        ostk::core::utils::Print::Line(anOutputStream) << "Type:" << Pass::StringFromType(pass.getType());
        ostk::core::utils::Print::Line(anOutputStream) << "Revolution #:" << pass.getRevolutionNumber().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "Start time:" << pass.getInterval().accessStart().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "End time:" << pass.getInterval().accessEnd().toString();
        ostk::core::utils::Print::Line(anOutputStream) << "Duration:" << pass.getInterval().getDuration().toString();
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
    using orbit::models::Kepler;
    using orbit::models::kepler::COE;

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
    using orbit::models::Kepler;
    using orbit::models::kepler::COE;

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
    using ostk::math::object::Vector3d;

    using ostk::physics::coord::spherical::LLA;
    using ostk::physics::coord::Position;

    using orbit::models::Kepler;
    using orbit::models::kepler::COE;

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
    const Shared<const Celestial>& aCelestialObjectSPtr
)
{
    using ostk::core::types::Uint8;

    using ostk::math::object::Vector3d;

    using ostk::physics::time::Scale;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Mass;
    using ostk::physics::environment::object::celestial::Sun;

    using orbit::models::Kepler;
    using orbit::models::kepler::COE;

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
    const Angle trueAnomaly = Angle::Zero();

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

// Map<Index, Pass>                Orbit::GeneratePassMap                      (   const   Array<State>& aStateArray,
//                                                                                 const   Integer&
//                                                                                 anInitialRevolutionNumber )
// {

//     using ostk::core::types::Real ;

//     using ostk::math::object::Vector3d ;

//     using ostk::physics::time::Duration ;

//     if (!anInitialRevolutionNumber.isDefined())
//     {
//         throw ostk::core::error::runtime::Undefined("Initial revolution number") ;
//     }

//     // [TBI] Deal w/ z_ECI always equal to 0.0 case (equatorial orbit)

//     Map<Index, Pass> passMap ;

//     Index index = 0 ;
//     Integer revolutionNumber = anInitialRevolutionNumber ;
//     Instant previousPassEndInstant = Instant::Undefined() ;

//     Index currentIndex = 0 ;
//     State const* previousStatePtr = nullptr ;

//     for (const auto& state : aStateArray)
//     {

//         bool passHasBeenAdded = false ;

//         if (previousStatePtr != nullptr)
//         {

//             const Vector3d& previousStateCoordinates_ECI = previousStatePtr->accessPosition().accessCoordinates() ;
//             const Vector3d& currentStateCoordinates_ECI = state.accessPosition().accessCoordinates() ;

//             if ((previousStateCoordinates_ECI.z() < 0.0) && (currentStateCoordinates_ECI.z() >= 0.0))
//             {

//                 const Pass::Type passType = ((!previousPassEndInstant.isDefined()) &&
//                 aStateArray.accessFirst().accessPosition().accessCoordinates().z() != 0.0) ? Pass::Type::Partial :
//                 Pass::Type::Complete ;

//                 const Instant passStartInstant = previousPassEndInstant.isDefined() ? previousPassEndInstant :
//                 aStateArray.accessFirst().accessInstant() ; const Instant passEndInstant =
//                 previousStatePtr->accessInstant() - Duration::Between(previousStatePtr->accessInstant(),
//                 state.accessInstant()) * (previousStateCoordinates_ECI.z() / (currentStateCoordinates_ECI.z() -
//                 previousStateCoordinates_ECI.z())) ;

//                 const Interval passInterval = Interval::Closed(passStartInstant, passEndInstant) ;

//                 const Pass pass = { passType, revolutionNumber, passInterval } ;

//                 passMap.insert({ index, pass }) ;

//                 index = currentIndex ;
//                 revolutionNumber++ ;

//                 previousPassEndInstant = passEndInstant ;
//                 passHasBeenAdded = true ;

//             }

//         }

//         if ((!passHasBeenAdded) && (&state == &aStateArray.accessLast())) // Last state
//         {

//             const Pass::Type passType = ((previousStatePtr != nullptr) &&
//             (previousStatePtr->accessPosition().accessCoordinates().z() < 0.0) &&
//             (state.accessPosition().accessCoordinates().z() == 0.0)) ? Pass::Type::Complete : Pass::Type::Partial ;

//             const Instant passStartInstant = previousPassEndInstant.isDefined() ? previousPassEndInstant :
//             aStateArray.accessFirst().accessInstant() ; const Instant passEndInstant = state.accessInstant() ;

//             const Interval passInterval = Interval::Closed(passStartInstant, passEndInstant) ;

//             const Pass pass = { passType, revolutionNumber, passInterval } ;

//             passMap.insert({ index, pass }) ;

//         }

//         previousStatePtr = &state ;
//         currentIndex++ ;

//     }

//     return passMap ;

// }

// Array<State>                    Orbit::GenerateStates                       (   const   Model& aModel,
//                                                                                 const   Array<Instant>& anInstantGrid
//                                                                                 )
// {

//     Array<State> states = Array<State>::Empty() ;

//     states.reserve(anInstantGrid.getSize()) ;

//     for (const auto& instant : anInstantGrid)
//     {
//         states.add(aModel.calculateStateAt(instant)) ;
//     }

//     return states ;

// }

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
