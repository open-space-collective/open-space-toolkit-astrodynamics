/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster.hpp>

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

Thruster::Thruster(const Direction& aThrusterDirection, const SatelliteSystem& aSatelliteSystem)
    : Dynamics(),
      direction_(aThrusterDirection),
      satelliteSystem_(aSatelliteSystem)
{
}

Thruster::Thruster(const Thruster& aThruster)
    : Dynamics(aThruster),
      direction_(aThruster.direction_),
      satelliteSystem_(aThruster.satelliteSystem_)
{
}

Thruster::Thruster(const Direction& aThrusterDirection, const SatelliteSystem& aSatelliteSystem, const String& aName)
    : Dynamics(aName),
      direction_(aThruster.direction_),
      satelliteSystem_(aThruster.satelliteSystem_)
{
}

Thruster::~Thruster() {}

Thruster* Thruster::clone() const
{
    return new Thruster(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Thruster& aThruster)
{
    aThruster.print(anOutputStream);

    return anOutputStream;
}

bool Thruster::isDefined() const
{
    return direction_.isDefined() && satelliteSystem_.isDefined();
}

void Thruster::applyContribution(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant) const
{
    (void)anInstant;

    const Vector3d x_GCRF = {x[0], x[1], x[2]};
    const Vector3d v_GCRF = {x[3], x[4], x[5]};

    const Vector3d xAxis = v_GCRF.normalized();
    const Vector3d yAxis = x_GCRF.cross(v_GCRF).normalized();
    const Vector3d zAxis = xAxis.cross(yAxis);
    const RotationMatrix R_GCRF_VNC = RotationMatrix::Rows(xAxis, yAxis, zAxis).transpose();

    if (x[6] <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Out of fuel.");
    }

    const Vector3d acceleration_VNC = satelliteSystem_.getPropulsionSystem().getAcceleration(
                                          Mass::Kilograms(x[6] + satelliteSystem_.getMass().inKilograms())
                                      ) *
                                      direction_.getValue();  // TBI: don't hardcode VNC, define local orbital frame

    const Vector3d acceleration_GCRF = R_GCRF_VNC * acceleration_VNC;

    dxdt[0] = x[3];
    dxdt[1] = x[4];
    dxdt[2] = x[5];
    dxdt[3] += acceleration_GCRF[0];
    dxdt[4] += acceleration_GCRF[1];
    dxdt[5] += acceleration_GCRF[2];
    dxdt[6] -= satelliteSystem_.getPropulsionSystem().getMassFlowRate().getValue();
}

void Thruster::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Thrusterer Dynamics") : void();

    satelliteSystem_.print(anOutputStream, false);

    // TBI: Print direction

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
