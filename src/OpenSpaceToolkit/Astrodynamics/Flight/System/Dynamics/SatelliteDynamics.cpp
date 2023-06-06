/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{

using ostk::core::types::Shared;
using ostk::core::types::Size;

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;
using ostk::physics::units::Mass;

using ostk::astro::trajectory::Force;
using ostk::astro::trajectory::force::GravityForce;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

SatelliteDynamics::SatelliteDynamics(const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem)
    : Dynamics(),
      environment_(anEnvironment),
      gcrfSPtr_(Frame::GCRF()),
      satelliteSystem_(aSatelliteSystem),
      forceArray_(Array<Shared<Force>>::Empty()),  // Initialize default
      instant_(Instant::Undefined())
{

    // Initialize forceArray_ for gravitational forces
    Size numberCelesialObjects = environment_.accessObjects().getSize();
    forceArray_.reserve(numberCelesialObjects);

    std::cout << "Constructor" << std::endl;

    for (const auto& objectName : environment_.getObjectNames())
    {
        std::cout << objectName << std::endl;
        GravityForce gravityForce = GravityForce(anEnvironment.accessCelestialObjectWithName(objectName));
        Shared<GravityForce> gravityForceSPtr = std::make_shared<GravityForce>(gravityForce);  // Flaky check what's best here, Child class is the way to go
        forceArray_.add(gravityForceSPtr);
    }

}

SatelliteDynamics::SatelliteDynamics(const SatelliteDynamics& aSatelliteDynamics)
    : Dynamics(aSatelliteDynamics),
      environment_(aSatelliteDynamics.environment_),
      gcrfSPtr_(aSatelliteDynamics.gcrfSPtr_),
      satelliteSystem_(aSatelliteDynamics.satelliteSystem_),
      instant_(Instant::Undefined())
{
}

SatelliteDynamics::~SatelliteDynamics() {}

SatelliteDynamics* SatelliteDynamics::clone() const
{
    return new SatelliteDynamics(*this);
}

bool SatelliteDynamics::operator==(const SatelliteDynamics& aSatelliteDynamics) const
{
    if ((!this->isDefined()) || (!aSatelliteDynamics.isDefined()))
    {
        return false;
    }

    return (environment_.getInstant() == aSatelliteDynamics.environment_.getInstant()) &&
           (environment_.getObjectNames() == aSatelliteDynamics.environment_.getObjectNames()) &&
           (satelliteSystem_ == aSatelliteDynamics.satelliteSystem_);
}

bool SatelliteDynamics::operator!=(const SatelliteDynamics& aSatelliteDynamics) const
{
    return !((*this) == aSatelliteDynamics);
}

std::ostream& operator<<(std::ostream& anOutputStream, const SatelliteDynamics& aSatelliteDynamics)
{
    aSatelliteDynamics.print(anOutputStream);

    return anOutputStream;
}

bool SatelliteDynamics::isDefined() const
{
    return environment_.isDefined() && satelliteSystem_.isDefined();
}

void SatelliteDynamics::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Satellite Dynamics") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Environment:" << environment_;

    ostk::core::utils::Print::Separator(anOutputStream, "Satellite System");
    satelliteSystem_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Instant SatelliteDynamics::getInstant() const
{
    return instant_;
}

void SatelliteDynamics::setInstant(const Instant& anInstant)
{
    this->instant_ = anInstant;
}

Array<Shared<const Force>> SatelliteDynamics::accessForces () const
{

    Array<Shared<const Force>> forces = Array<Shared<const Force>>::Empty() ;

    forces.reserve(forceArray_.getSize()) ;

    for (const auto& forceSPtr : forceArray_)
    {
        forces.add(forceSPtr) ;
    }

    return forces ;

}

Dynamics::DynamicalEquationWrapper SatelliteDynamics::getDynamicalEquations()
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteDynamics");
    }

    if (!this->instant_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    return std::bind(
        &SatelliteDynamics::DynamicalEquations,
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    );
}

void SatelliteDynamics::DynamicalEquations(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const double t)
{

    // Extract State Components
    const Position currentPosition = Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_);
    const Velocity currentVelocity = Velocity::MetersPerSecond({x[3], x[4], x[5]}, gcrfSPtr_);  // Not Great: TBI

    // Extract System Charateristics
    const Real mass = satelliteSystem_.getMass().inKilograms();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();

    // Check for radii below 70km altitude
    if (currentPosition.getCoordinates().norm() < Earth::Models::EGM2008::EquatorialRadius.inMeters() + 70000.0)
    {
        throw ostk::core::error::RuntimeError("Satellite altitude too low, has re-entered.");
    }

    // Update environment time
    const Instant currentInstant = instant_ + Duration::Seconds(t);
    environment_.setInstant(currentInstant);

    // Initialize total acceleration value
    Vector3d totalForce_SI = {0.0, 0.0, 0.0};

    // Add force models acceleration contributions
    for (const auto& forceModelSPtr : this->forceArray_)
    {
        std::cout << "HERE WE ADD CONTRIBUTIONS" << std::endl;

        totalForce_SI += forceModelSPtr->getContribution(
            currentPosition,
            currentInstant,
            currentVelocity,
            surfaceArea,
            dragCoefficient,
            mass
        );
    }

    Vector3d totalAcceleration_SI = totalForce_SI / mass;

    // Integrate position and velocity states
    dxdt[0] = x[3];
    dxdt[1] = x[4];
    dxdt[2] = x[5];
    dxdt[3] = totalAcceleration_SI[0];
    dxdt[4] = totalAcceleration_SI[1];
    dxdt[5] = totalAcceleration_SI[2];
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
