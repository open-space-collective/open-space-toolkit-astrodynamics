/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/ModifiedEquinoctial.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{
namespace kepler
{

using ostk::mathematics::object::Vector3d;

using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using TimeUnit = ostk::physics::unit::Time;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, TimeUnit::Unit::Second);

ModifiedEquinoctial::ModifiedEquinoctial(
    const Length& semiLatusRectum,
    const Real& eccentricityX,
    const Real& eccentricityY,
    const Real& nodeX,
    const Real& nodeY,
    const Angle& trueLongitude
)
    : semiLatusRectum_(semiLatusRectum),
      eccentricityX_(eccentricityX),
      eccentricityY_(eccentricityY),
      nodeX_(nodeX),
      nodeY_(nodeY),
      trueLongitude_(trueLongitude)
{
}

bool ModifiedEquinoctial::operator==(const ModifiedEquinoctial& aModifiedEquinoctial) const
{
    if ((!this->isDefined()) || (!aModifiedEquinoctial.isDefined()))
    {
        return false;
    }

    const Real tolerance = Real::Epsilon() * 1e3;

    return (std::abs(semiLatusRectum_.inMeters() - aModifiedEquinoctial.semiLatusRectum_.inMeters()) <= tolerance) &&
           (std::abs(eccentricityX_ - aModifiedEquinoctial.eccentricityX_) <= tolerance) &&
           (std::abs(eccentricityY_ - aModifiedEquinoctial.eccentricityY_) <= tolerance) &&
           (std::abs(nodeX_ - aModifiedEquinoctial.nodeX_) <= tolerance) &&
           (std::abs(nodeY_ - aModifiedEquinoctial.nodeY_) <= tolerance) &&
           (std::abs(
                trueLongitude_.inRadians(0.0, Real::TwoPi()) -
                aModifiedEquinoctial.trueLongitude_.inRadians(0.0, Real::TwoPi())
            ) <= Angle::Radians(tolerance).inRadians());
}

bool ModifiedEquinoctial::operator!=(const ModifiedEquinoctial& aModifiedEquinoctial) const
{
    return !((*this) == aModifiedEquinoctial);
}

std::ostream& operator<<(std::ostream& anOutputStream, const ModifiedEquinoctial& aModifiedEquinoctial)
{
    aModifiedEquinoctial.print(anOutputStream);
    return anOutputStream;
}

bool ModifiedEquinoctial::isDefined() const
{
    return semiLatusRectum_.isDefined() && eccentricityX_.isDefined() && eccentricityY_.isDefined() &&
           nodeX_.isDefined() && nodeY_.isDefined() && trueLongitude_.isDefined();
}

Length ModifiedEquinoctial::getSemiLatusRectum() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("ModifiedEquinoctial");
    }
    return semiLatusRectum_;
}

Real ModifiedEquinoctial::getEccentricityX() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("ModifiedEquinoctial");
    }
    return eccentricityX_;
}

Real ModifiedEquinoctial::getEccentricityY() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("ModifiedEquinoctial");
    }
    return eccentricityY_;
}

Real ModifiedEquinoctial::getNodeX() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("ModifiedEquinoctial");
    }
    return nodeX_;
}

Real ModifiedEquinoctial::getNodeY() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("ModifiedEquinoctial");
    }
    return nodeY_;
}

Angle ModifiedEquinoctial::getTrueLongitude() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("ModifiedEquinoctial");
    }
    return trueLongitude_;
}

Vector6d ModifiedEquinoctial::getSIVector() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("ModifiedEquinoctial");
    }
    return Vector6d(
        semiLatusRectum_.inMeters(), eccentricityX_, eccentricityY_, nodeX_, nodeY_, trueLongitude_.inRadians()
    );
}

Pair<Position, Velocity> ModifiedEquinoctial::getCartesianState(
    const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("ModifiedEquinoctial");
    }

    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational Parameter");
    }

    if (aFrameSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!aFrameSPtr->isQuasiInertial())
    {
        throw ostk::core::error::RuntimeError("Frame must be Quasi Inertial");
    }

    const Real p_m = semiLatusRectum_.inMeters();
    const Real f_val = eccentricityX_;
    const Real g_val = eccentricityY_;
    const Real h_val = nodeX_;
    const Real k_val = nodeY_;
    const Real L_rad = trueLongitude_.inRadians();

    const Real sSquaredInverse = 1.0 / (1.0 + h_val * h_val + k_val * k_val);

    const Real cosL = std::cos(L_rad);
    const Real sinL = std::sin(L_rad);

    const Real w = 1.0 + f_val * cosL + g_val * sinL;
    if (w == 0.0)
    {
        throw ostk::core::error::RuntimeError("w is zero (singularity).");
    }

    const Real r_m = p_m / w;

    const Real alphaSquared = h_val * h_val - k_val * k_val;

    const Real mu = aGravitationalParameter.in(GravitationalParameterSIUnit);

    const Real muOverPSqrt = std::sqrt(mu / p_m);

    // Perifocal frame components

    const Real x_pf = r_m * cosL;
    const Real y_pf = r_m * sinL;
    const Real vx_pf = -muOverPSqrt * (sinL + g_val);
    const Real vy_pf = muOverPSqrt * (cosL + f_val);

    // Inertial frame components

    const Real x = sSquaredInverse * (x_pf * (1.0 + alphaSquared) + 2.0 * h_val * k_val * y_pf);
    const Real y = sSquaredInverse * (y_pf * (1.0 - alphaSquared) + x_pf * (2.0 * h_val * k_val));
    const Real z = 2.0 * sSquaredInverse * (h_val * y_pf - k_val * x_pf);

    const Position position = Position::Meters({x, y, z}, aFrameSPtr);

    const Real vx = sSquaredInverse * (vx_pf * (1.0 + alphaSquared) + 2.0 * h_val * k_val * vy_pf);
    const Real vy = sSquaredInverse * (vy_pf * (1.0 - alphaSquared) + vx_pf * (2.0 * h_val * k_val));
    const Real vz = 2.0 * sSquaredInverse * (h_val * vy_pf - k_val * vx_pf);

    const Velocity velocity = Velocity::MetersPerSecond({vx, vy, vz}, aFrameSPtr);

    return {position, velocity};
}

void ModifiedEquinoctial::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Modified Equinoctial") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Semi-latus rectum:" << (semiLatusRectum_.isDefined() ? semiLatusRectum_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Eccentricity X:" << (eccentricityX_.isDefined() ? eccentricityX_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Eccentricity Y:" << (eccentricityY_.isDefined() ? eccentricityY_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Node X:" << (nodeX_.isDefined() ? nodeX_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Node Y:" << (nodeY_.isDefined() ? nodeY_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "True Longitude:" << (trueLongitude_.isDefined() ? trueLongitude_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

ModifiedEquinoctial ModifiedEquinoctial::Undefined()
{
    return {
        Length::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Angle::Undefined()
    };
}

ModifiedEquinoctial ModifiedEquinoctial::Cartesian(
    const Pair<Position, Velocity>& aCartesianState, const Derived& aGravitationalParameter
)
{
    if (!aCartesianState.first.isDefined() || !aCartesianState.second.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Cartesian State");
    }

    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational Parameter");
    }

    if (!aCartesianState.first.accessFrame()->isQuasiInertial() ||
        !aCartesianState.second.accessFrame()->isQuasiInertial())
    {
        throw ostk::core::error::RuntimeError("Frame must be Quasi Inertial");
    }

    const Real mu = aGravitationalParameter.in(GravitationalParameterSIUnit);

    const Vector3d positionCoordinates = aCartesianState.first.accessCoordinates();
    const Vector3d velocityCoordinates = aCartesianState.second.accessCoordinates();

    const Vector3d positionDirection = positionCoordinates.normalized();

    const Vector3d angularMomentumCoordinates = positionCoordinates.cross(velocityCoordinates);

    const Vector3d angularMomentumDirection = angularMomentumCoordinates.normalized();

    const Real angularMomentumMagnitude = angularMomentumCoordinates.norm();

    const Vector3d velocityDirectionInOrbitalPlane =
        (positionCoordinates.norm() * velocityCoordinates -
         positionCoordinates.dot(velocityCoordinates) * positionDirection) /
        angularMomentumMagnitude;

    // Modified equinoctial elements nodeY and nodeX
    const Real nodeX = angularMomentumDirection.x() / (1.0 + angularMomentumDirection.z());
    const Real nodeY = -angularMomentumDirection.y() / (1.0 + angularMomentumDirection.z());

    // Check for singularity (angularMomentumDirection.z() == -1, i.e., inclination = 180 degrees)
    if (std::abs(1.0 + angularMomentumDirection.z()) < Real::Epsilon())
    {
        return ModifiedEquinoctial::Undefined();
    }

    const Real nodeXSquared = nodeX * nodeX;
    const Real nodeYSquared = nodeY * nodeY;
    const Real sSquared = 1.0 + nodeYSquared + nodeXSquared;
    const Real tkh = 2.0 * nodeX * nodeY;

    // Eccentricity vector
    const Vector3d eccentricity = velocityCoordinates.cross(angularMomentumCoordinates) / mu - positionDirection;

    // Transformation vectors
    Vector3d fhat = {1.0 - nodeXSquared + nodeYSquared, tkh, -2.0 * nodeX};
    Vector3d ghat = {tkh, 1.0 + nodeXSquared - nodeYSquared, 2.0 * nodeY};

    fhat = fhat / sSquared;
    ghat = ghat / sSquared;

    // Modified equinoctial elements eccentricityX and eccentricityY
    const Real eccentricityX = eccentricity.dot(fhat);
    const Real eccentricityY = eccentricity.dot(ghat);

    // True longitude
    const double L_radians = std::atan2(
        positionDirection.y() - velocityDirectionInOrbitalPlane.x(),
        positionDirection.x() + velocityDirectionInOrbitalPlane.y()
    );
    const Angle L = Angle::Radians(Angle::Radians(L_radians).inRadians(0.0, Real::TwoPi()));

    const Length semiLatusRectum = Length::Meters(angularMomentumMagnitude * angularMomentumMagnitude / mu);

    return {semiLatusRectum, eccentricityX, eccentricityY, nodeY, nodeX, L};
}

ModifiedEquinoctial ModifiedEquinoctial::COE(const KeplerianCOE& aKeplerianElements)
{
    if (!aKeplerianElements.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Classical Orbital Elements");
    }

    const double sma_m = aKeplerianElements.getSemiMajorAxis().inMeters();
    const double ecc_val = aKeplerianElements.getEccentricity();
    const double inc_rad = aKeplerianElements.getInclination().inRadians();
    const double raan_rad = aKeplerianElements.getRaan().inRadians();
    const double aop_rad = aKeplerianElements.getAop().inRadians();
    const double ta_rad = aKeplerianElements.getTrueAnomaly().inRadians();

    // Check for parabolic orbit (eccentricity = 1)
    Length semiLatusRectum = Length::Undefined();
    if (ecc_val == 1.0)  // Parabolic
    {
        // For parabolic orbit, semiLatusRectum = 2 * q (periapsis distance)
        // q = nodeY^2 / mu, where nodeY is specific angular momentum
        // Since we don't have gravitational parameter here, we need to handle this case
        // For parabolic orbits, the semi-major axis is infinite, so we can't use the standard formula
        // We'll return undefined for now, as we need gravitational parameter for this conversion
        return ModifiedEquinoctial::Undefined();
    }
    else if (ecc_val > 1.0)
    {  // Hyperbolic
        return ModifiedEquinoctial::Undefined();
    }
    else
    {  // Elliptical
        semiLatusRectum = Length::Meters(sma_m * (1.0 - ecc_val * ecc_val));
    }

    // Check for inclination singularity (i = 180 degrees)
    if (std::abs(inc_rad - Real::Pi()) < Real::Epsilon())
    {
        // This case (inclination is PI) is a singularity for nodeY, nodeX elements.
        return ModifiedEquinoctial::Undefined();
    }

    // Convert to Modified Equinoctial elements
    const Real eccentricityX = ecc_val * std::cos(aop_rad + raan_rad);
    const Real eccentricityY = ecc_val * std::sin(aop_rad + raan_rad);

    const Real tan_i_half = std::tan(inc_rad / 2.0);
    const Real nodeX = tan_i_half * std::cos(raan_rad);
    const Real nodeY = tan_i_half * std::sin(raan_rad);

    const Angle L = Angle::Radians(Angle::Radians(raan_rad + aop_rad + ta_rad).inRadians(0.0, Real::TwoPi()));

    return {semiLatusRectum, eccentricityX, eccentricityY, nodeX, nodeY, L};
}

String ModifiedEquinoctial::StringFromElement(const ModifiedEquinoctial::Element& anElement)
{
    switch (anElement)
    {
        case ModifiedEquinoctial::Element::SemiLatusRectum:
            return "SemiLatusRectum";
        case ModifiedEquinoctial::Element::EccentricityX:
            return "EccentricityX";
        case ModifiedEquinoctial::Element::EccentricityY:
            return "EccentricityY";
        case ModifiedEquinoctial::Element::NodeX:
            return "NodeX";
        case ModifiedEquinoctial::Element::NodeY:
            return "NodeY";
        case ModifiedEquinoctial::Element::TrueLongitude:
            return "TrueLongitude";
        default:
            throw ostk::core::error::runtime::Wrong("Element");
    }
}

}  // namespace kepler
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
