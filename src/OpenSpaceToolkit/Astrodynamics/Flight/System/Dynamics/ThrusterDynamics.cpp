/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThrusterDynamics.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>

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

ThrusterDynamics::ThrusterDynamics(const Propulsion& aPropulsion, const Direction& aThrustDirection)
    : Dynamics(),
      propulsion_(aPropulsion),
      direction_(aThrustDirection)
{
}

ThrusterDynamics::ThrusterDynamics(const ThrusterDynamics& aThrusterDynamics)
    : Dynamics(aThrusterDynamics),
      propulsion_(aThrusterDynamics.propulsion_),
      direction_(aThrusterDynamics.direction_)
{
}

ThrusterDynamics::~ThrusterDynamics() {}

ThrusterDynamics* ThrusterDynamics::clone() const
{
    return new ThrusterDynamics(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const ThrusterDynamics& aThrusterDynamics)
{
    aThrusterDynamics.print(anOutputStream);

    return anOutputStream;
}

bool ThrusterDynamics::isDefined() const
{
    return propulsion_.isDefined();
}

void ThrusterDynamics::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Thruster Dynamics") : void();

    propulsion_.print(anOutputStream, false);

    // TBI: Print vector

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

void ThrusterDynamics::update(
    const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant
)
{
    (void)anInstant;
    using ostk::math::geom::d3::trf::rot::RotationMatrix;

    const Vector3d x_GCRF = {x[0], x[1], x[2]};
    const Vector3d v_GCRF = {x[3], x[4], x[5]};

    const Vector3d xAxis = v_GCRF.normalized();
    const Vector3d yAxis = x_GCRF.cross(v_GCRF).normalized();
    const Vector3d zAxis = xAxis.cross(yAxis);
    const RotationMatrix R_GCRF_VNC = RotationMatrix::Rows(xAxis, yAxis, zAxis).transpose();

    const Vector3d acceleration_VNC = propulsion_.thrust_ / x[6] * direction_.getValue();  // TBI: don't hardcode VNC

    const Vector3d acceleration_GCRF = R_GCRF_VNC * acceleration_VNC;

    dxdt[0] = x[3];
    dxdt[1] = x[4];
    dxdt[2] = x[5];
    dxdt[3] += acceleration_GCRF[0];
    dxdt[4] += acceleration_GCRF[1];
    dxdt[5] += acceleration_GCRF[2];
    dxdt[6] -= propulsion_.massFlowRate_;
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
