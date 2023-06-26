/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Dynamics/Thrust.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace dynamics
{

Thrust::Thrust(const Direction& aThrustDirection, const SatelliteSystem& aSatelliteSystem)
    : Dynamics(),
      direction_(aThrustDirection),
      satelliteSystem_(aSatelliteSystem)
{
}

Thrust::Thrust(const Thrust& aThrust)
    : Dynamics(aThrust),
      direction_(aThrust.direction_),
      satelliteSystem_(aThrust.satelliteSystem_)
{
}

Thrust::~Thrust() {}

Thrust* Thrust::clone() const
{
    return new Thrust(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Thrust& aThrust)
{
    aThrust.print(anOutputStream);

    return anOutputStream;
}

bool Thrust::isDefined() const
{
    return direction_.isDefined() && satelliteSystem_.isDefined();
}

void Thrust::applyContribution(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant) const
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
                                      direction_.getValue();  // TBI: don't hardcode VNC

    const Vector3d acceleration_GCRF = R_GCRF_VNC * acceleration_VNC;

    dxdt[0] = x[3];
    dxdt[1] = x[4];
    dxdt[2] = x[5];
    dxdt[3] += acceleration_GCRF[0];
    dxdt[4] += acceleration_GCRF[1];
    dxdt[5] += acceleration_GCRF[2];
    dxdt[6] -= satelliteSystem_.getPropulsionSystem().getMassFlowRate().getValue();
}

void Thrust::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Thruster Dynamics") : void();

    satelliteSystem_.print(anOutputStream, false);

    // TBI: Print direction

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
