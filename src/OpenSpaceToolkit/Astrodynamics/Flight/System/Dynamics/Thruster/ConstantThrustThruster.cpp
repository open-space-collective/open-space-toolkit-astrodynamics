/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrustThruster.hpp>

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
namespace thruster
{

using ostk::astro::flight::system::SatelliteSystem;

ConstantThrustThruster::ConstantThrustThruster(const SatelliteSystem& aSatelliteSystem, const Direction& aThrustDirection, const String& aName)
    : Thruster(aSatelliteSystem, aName),
      direction_(aThrustDirection)
{
}

ConstantThrustThruster::~ConstantThrustThruster() {}

ConstantThrustThruster* ConstantThrustThruster::clone() const
{
    return new ConstantThrustThruster(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const ConstantThrustThruster& aConstantThrustThruster)
{
    aConstantThrustThruster.print(anOutputStream);

    return anOutputStream;
}

bool ConstantThrustThruster::isDefined() const
{
    return direction_.isDefined();
}

Scalar ConstantThrustThruster::getThrust() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Constant Thrust Thruster");
    }

    return this->getSatelliteSystem().getPropulsionSystem().getThrust();
}

void ConstantThrustThruster::applyContribution(const NumericalSolver::StateVector& x, NumericalSolver::StateVector& dxdt, const Instant& anInstant) const
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

    const SatelliteSystem satelliteSystem = this->getSatelliteSystem();

    const Vector3d acceleration_VNC = satelliteSystem.getPropulsionSystem().getAcceleration(
                                          Mass::Kilograms(x[6] + satelliteSystem.getMass().inKilograms())
                                      ) *
                                      this->direction_.getValue();  // TBI: don't hardcode VNC, define local orbital frame

    const Vector3d acceleration_GCRF = R_GCRF_VNC * acceleration_VNC;

    dxdt[0] = x[3];
    dxdt[1] = x[4];
    dxdt[2] = x[5];
    dxdt[3] += acceleration_GCRF[0];
    dxdt[4] += acceleration_GCRF[1];
    dxdt[5] += acceleration_GCRF[2];
    dxdt[6] -= satelliteSystem.getPropulsionSystem().getMassFlowRate().getValue();  // TBI: Everything is constant anyway as it comes from PropulsionSystem
}

void ConstantThrustThruster::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "ConstantThrustThruster Dynamics") : void();

    // satelliteSystem_.print(anOutputStream, false);

    // TBI: Print direction

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace thruster
}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
