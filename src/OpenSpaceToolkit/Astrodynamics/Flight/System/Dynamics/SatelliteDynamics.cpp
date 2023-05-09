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

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

SatelliteDynamics::SatelliteDynamics(const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem)
    : Dynamics(),
      environment_(anEnvironment),
      gcrfSPtr_(Frame::GCRF()),
      satelliteSystem_(aSatelliteSystem),
      instant_(Instant::Undefined())
{
<<<<<<< HEAD
}

SatelliteDynamics::SatelliteDynamics(const SatelliteDynamics& aSatelliteDynamics)
    : Dynamics(aSatelliteDynamics),
      environment_(aSatelliteDynamics.environment_),
      gcrfSPtr_(aSatelliteDynamics.gcrfSPtr_),
      satelliteSystem_(aSatelliteDynamics.satelliteSystem_),
      instant_(Instant::Undefined())
=======

}

                                SatelliteDynamics::SatelliteDynamics        (   const   SatelliteDynamics&          aSatelliteDynamics                          )
                                :   Dynamics(aSatelliteDynamics),
                                    environment_(aSatelliteDynamics.environment_),
                                    gcrfSPtr_(aSatelliteDynamics.gcrfSPtr_),
                                    satelliteSystem_(aSatelliteDynamics.satelliteSystem_),
                                    instant_(Instant::Undefined()),
                                    dragWellDefined_(aSatelliteDynamics.dragWellDefined_)
>>>>>>> feat: add drag-defined flag
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
    const Position currentPosition = Position::Meters({x[0], x[1], x[2]}, gcrfSPtr_);

    // Check for radii below 70km altitude
    if (currentPosition.getCoordinates().norm() < Earth::Models::EGM2008::EquatorialRadius.inMeters() + 70000.0)
    {
        throw ostk::core::error::RuntimeError("Satellite altitude too low, has re-entered.");
    }

    // Update environment time
    const Instant currentInstant = instant_ + Duration::Seconds(t);
    environment_.setInstant(currentInstant);

    dxdt[3] = 0 ;
    dxdt[4] = 0 ;
    dxdt[5] = 0 ;

    // GRAVITY
    // Initialize gravitational acceleration vector
    Vector3d totalGravitationalAcceleration_SI = {0.0, 0.0, 0.0};

    // Access all objects in the environment and loop through them
    for (const auto& objectName : environment_.getObjectNames())
    {
        if (objectName != "Earth")
        {
            // Obtain 3rd body effect on center of Earth (origin in GCRF) aka 3rd body correction
            const Vector gravitationalAcceleration3rdBodyCorrection =
                environment_.accessCelestialObjectWithName(objectName)
                    ->getGravitationalFieldAt(Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_));

            // Subtract 3rd body correct from total gravitational acceleration
            totalGravitationalAcceleration_SI -=
                gravitationalAcceleration3rdBodyCorrection.inFrame(gcrfSPtr_, currentInstant).getValue();
        }

        // Obtain gravitational acceleration from current object
        const Vector gravitationalAcceleration =
            environment_.accessCelestialObjectWithName(objectName)->getGravitationalFieldAt(currentPosition);

        // Add object's gravity to total gravitational acceleration
        totalGravitationalAcceleration_SI += gravitationalAcceleration.inFrame(gcrfSPtr_, currentInstant).getValue();
    }

    dxdt[3] += totalGravitationalAcceleration_SI[0] ;
    dxdt[4] += totalGravitationalAcceleration_SI[1] ;
    dxdt[5] += totalGravitationalAcceleration_SI[2] ;

    // DRAG
    const Real mass = satelliteSystem_.getMass().inKilograms() ;
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient() ;
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea() ;

    for (const auto& objectName : environment_.getObjectNames())
    {
        // TBI: currently only defined for Earth
        if (environment_.accessCelestialObjectWithName(objectName)->accessAtmosphericModel() != nullptr)
        {
            
            const Real rho = environment_.accessCelestialObjectWithName(objectName)->getAtmosphericDensityAt(currentPosition).getValue() ;

            // [TBI]: Define in Physics celestial body
            const Vector3d earthAngularVelocity = { 0, 0, 7.2921159e-5 } ; // rad/s

            const Vector3d relativeVelocity = Vector3d( x[3], x[4], x[5] ) - earthAngularVelocity.cross(Vector3d( x[0], x[1], x[2] )) ;

            // Calculate drag acceleration
            const Vector3d dragAcceleration = -( 0.5 / mass ) * dragCoefficient * surfaceArea * rho * relativeVelocity.norm() * relativeVelocity ;

            dxdt[3] += dragAcceleration[0] ;
            dxdt[4] += dragAcceleration[1] ;
            dxdt[5] += dragAcceleration[2] ;

        }

    }
    // Propagate velocity
    dxdt[0] = x[3] ;
    dxdt[1] = x[4] ;
    dxdt[2] = x[5] ;

}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
