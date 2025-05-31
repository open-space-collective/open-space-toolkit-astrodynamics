#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/MEOE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Constant.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/Angle.hpp> // For Angle::DegreesFromValue

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <cmath> // For std::sqrt, std::cos, std::sin, std::tan, std::fmod

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

using ostk::mathematics::constant::Pi;
using ostk::mathematics::geometry::Angle; // Required for Angle::Radians

MEOE::MEOE(const Length& p, const Real& f, const Real& g, const Real& h, const Real& k, const Angle& L)
    : p_(p),
      f_(f),
      g_(g),
      h_(h),
      k_(k),
      L_(L)
{
}

bool MEOE::operator==(const MEOE& aMEOE) const
{
    if ((!this->isDefined()) || (!aMEOE.isDefined()))
    {
        return false;
    }

    // Comparison with tolerance for Real and Angle types
    const Real tolerance = Real::Epsilon() * 100.0; // Define an appropriate tolerance

    return (std::abs(p_.inMeters() - aMEOE.p_.inMeters()) <= tolerance) &&
           (std::abs(f_ - aMEOE.f_) <= tolerance) &&
           (std::abs(g_ - aMEOE.g_) <= tolerance) &&
           (std::abs(h_ - aMEOE.h_) <= tolerance) &&
           (std::abs(k_ - aMEOE.k_) <= tolerance) &&
           (std::abs(L_.inRadians() - aMEOE.L_.inRadians()) <= Angle::Radians(tolerance).inRadians());
}

bool MEOE::operator!=(const MEOE& aMEOE) const
{
    return !((*this) == aMEOE);
}

std::ostream& operator<<(std::ostream& anOutputStream, const MEOE& aMEOE)
{
    aMEOE.print(anOutputStream);
    return anOutputStream;
}

bool MEOE::isDefined() const
{
    return p_.isDefined() && f_.isDefined() && g_.isDefined() && h_.isDefined() && k_.isDefined() && L_.isDefined();
}

Length MEOE::getSemiLatusRectum() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MEOE");
    }
    return p_;
}

Real MEOE::getF() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MEOE");
    }
    return f_;
}

Real MEOE::getG() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MEOE");
    }
    return g_;
}

Real MEOE::getH() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MEOE");
    }
    return h_;
}

Real MEOE::getK() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MEOE");
    }
    return k_;
}

Angle MEOE::getTrueLongitude() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MEOE");
    }
    return L_;
}

Vector6d MEOE::getSIVector() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MEOE");
    }
    return Vector6d(p_.inMeters(), f_, g_, h_, k_, L_.inRadians());
}

Pair<Position, Velocity> MEOE::getCartesianState(
    const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("MEOE");
    }

    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational Parameter");
    }

    if (aFrameSPtr == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    const Real p_m = p_.inMeters();
    const Real f_val = f_;
    const Real g_val = g_;
    const Real h_val = h_;
    const Real k_val = k_;
    const Real L_rad = L_.inRadians();

    if (p_m <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Semi-latus rectum must be positive.");
    }

    const Real s_sq = 1.0 + h_val * h_val + k_val * k_val;
    if (s_sq == 0.0) // Should not happen if h and k are Real
    {
        throw ostk::core::error::RuntimeError("s_sq is zero.");
    }

    const Real cosL = std::cos(L_rad);
    const Real sinL = std::sin(L_rad);

    const Real q = 1.0 + f_val * cosL + g_val * sinL;
    if (q == 0.0)
    {
        throw ostk::core::error::RuntimeError("q is zero (singularity).");
    }

    const Real r_m = p_m / q;

    // Position components (JPL formulas, page 7)
    // x_m = (r_m / s_sq) * (cosL * (1.0 + h_val*h_val - k_val*k_val) + 2.0*h_val*k_val*sinL) -> This seems to be a typo in the provided formula, the JPL document has (cosL + alpha^2 * cosL - 2 * alpha * beta * sinL) which matches with ( (1+h^2-k^2)*cosL + 2hk*sinL ) if alpha = h, beta = k. Let's assume the provided one is correct.
    // y_m = (r_m / s_sq) * (sinL * (1.0 - h_val*h_val + k_val*k_val) + 2.0*h_val*k_val*cosL) -> JPL: (sinL - alpha^2 * sinL + 2 * alpha * beta * cosL) which matches ( (1-h^2+k^2)*sinL + 2hk*cosL )
    // z_m = (r_m / s_sq) * (2.0 * (h_val*sinL - k_val*cosL)) -> JPL: (2 * (alpha * sinL - beta * cosL)) which matches

    const Real x_m = (r_m / s_sq) * (cosL * (1.0 + h_val * h_val - k_val * k_val) + 2.0 * h_val * k_val * sinL);
    const Real y_m = (r_m / s_sq) * (sinL * (1.0 - h_val * h_val + k_val * k_val) + 2.0 * h_val * k_val * cosL);
    const Real z_m = (r_m / s_sq) * (2.0 * (h_val * sinL - k_val * cosL));

    const Position position = Position::Coordinates({x_m, y_m, z_m}, Position::Unit::Meter, aFrameSPtr);

    const Real mu_m3s2 = aGravitationalParameter.inSIUnits(); // [m^3/s^2]
    if (mu_m3s2 <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Gravitational parameter must be positive.");
    }

    const Real sqrt_mu_p = std::sqrt(mu_m3s2 / p_m);

    const Real term_sinL_plus_g = sinL + g_val;
    const Real term_cosL_plus_f = cosL + f_val;

    // Velocity components (JPL formulas, page 7, with corrected vz sign as requested)
    // Original JPL for vx: (-1/s_sq) * sqrt(mu/p) * [ (sinL + alpha^2*sinL - 2*alpha*beta*cosL) + (g + alpha^2*g - 2*alpha*beta*f) ]
    // Simplified vx: (-sqrt_mu_p/s_sq) * ( (cosL+f)*(2hk) - (sinL+g)*(1+h^2-k^2) ) -> This seems to be from another source or a different formulation.
    // The provided vx: vx_ms = (-sqrt_mu_p/s_sq) * (term_sinL_plus_g * (1.0 + h_val*h_val - k_val*k_val) - term_cosL_plus_f * 2.0*h_val*k_val_)
    // Let's use the provided formula.

    const Real vx_ms =
        (-sqrt_mu_p / s_sq) * (term_sinL_plus_g * (1.0 + h_val * h_val - k_val * k_val) - term_cosL_plus_f * 2.0 * h_val * k_val);

    // Original JPL for vy: (-1/s_sq) * sqrt(mu/p) * [ (-cosL + alpha^2*cosL + 2*alpha*beta*sinL) - (f - alpha^2*f + 2*alpha*beta*g) ]
    // Simplified vy: (-sqrt_mu_p/s_sq) * ( (sinL+g)*(-2hk) - (cosL+f)*(1-h^2+k^2) ) -> This seems to be from another source or a different formulation.
    // The provided vy: vy_ms = (-sqrt_mu_p/s_sq) * (term_sinL_plus_g * 2.0*h_val*k_val_ - term_cosL_plus_f * (-(1.0 - h_val*h_val + k_val*k_val)))
    // Note: -(1.0 - h*h + k*k) is (h*h - k*k - 1.0).
    // Let's use the provided formula.

    const Real vy_ms =
        (-sqrt_mu_p / s_sq) * (term_sinL_plus_g * 2.0 * h_val * k_val - term_cosL_plus_f * (-(1.0 - h_val * h_val + k_val * k_val)));

    // Original JPL for vz: (1/s_sq) * sqrt(mu/p) * [ 2*(k* (cosL+f) + h*(sinL+g)) ]
    // Corrected vz as requested: vz_ms = (sqrt_mu_p/s_sq) * (2.0*k_val*term_sinL_plus_g + 2.0*h_val*term_cosL_plus_f)
    // This matches the JPL formula structure if we consider term_sinL_plus_g and term_cosL_plus_f.
    const Real vz_ms = (sqrt_mu_p / s_sq) * (2.0 * k_val * term_sinL_plus_g + 2.0 * h_val * term_cosL_plus_f);


    const Velocity velocity = Velocity::Coordinates({vx_ms, vy_ms, vz_ms}, Velocity::Unit::MeterPerSecond, aFrameSPtr);

    return {position, velocity};
}

void MEOE::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        anOutputStream << "MEOE (";
    }

    anOutputStream << "p: " << (p_.isDefined() ? p_.toString() : "Undef.");
    anOutputStream << ", f: " << (f_.isDefined() ? f_.toString() : "Undef.");
    anOutputStream << ", g: " << (g_.isDefined() ? g_.toString() : "Undef.");
    anOutputStream << ", h: " << (h_.isDefined() ? h_.toString() : "Undef.");
    anOutputStream << ", k: " << (k_.isDefined() ? k_.toString() : "Undef.");
    anOutputStream << ", L: " << (L_.isDefined() ? L_.toString() : "Undef.");

    if (displayDecorator)
    {
        anOutputStream << ")";
    }
}

MEOE MEOE::Undefined()
{
    return {Length::Undefined(), Real::Undefined(), Real::Undefined(), Real::Undefined(), Real::Undefined(), Angle::Undefined()};
}

MEOE MEOE::Cartesian(const Pair<Position, Velocity>& aCartesianState, const Derived& aGravitationalParameter)
{
    if (!aCartesianState.first.isDefined() || !aCartesianState.second.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Cartesian State");
    }
    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational Parameter");
    }

    const COE coe = COE::Cartesian(aCartesianState, aGravitationalParameter);

    if (!coe.isDefined())
    {
        // This can happen for e.g. equatorial circular orbits if COE has issues there,
        // or if Cartesian state is invalid (e.g. zero position or velocity for some COE conversions)
        return MEOE::Undefined();
    }

    const Real sma_m = coe.getSemiMajorAxis().inMeters();
    const Real ecc_val = coe.getEccentricity();
    const Angle inc = coe.getInclination();
    const Angle raan = coe.getRaan();
    const Angle aop = coe.getAop();
    const Angle ta = coe.getTrueAnomaly();

    const Real inc_rad = inc.inRadians();
    const Real raan_rad = raan.inRadians();
    const Real aop_rad = aop.inRadians();
    const Real ta_rad = ta.inRadians();

    // Check for p_m calculation: if ecc_val is 1.0 (parabolic), p = 2 * periapsis distance.
    // If sma_m is infinite (parabolic) and ecc_val is 1, this formula is indeterminate.
    // COE::Cartesian should handle this and provide appropriate sma/ecc for parabolic/hyperbolic cases.
    // For hyperbolic, sma < 0.
    Real p_m;
    if (ecc_val == 1.0) // Parabolic
    {
        // For parabolic orbit, p = 2 * q (periapsis distance)
        // q = h^2 / mu, where h is specific angular momentum
        // h_vec = r x v
        // h = norm(h_vec)
        // p = norm(h_vec)^2 / mu
        // This is already what COE::Cartesian should implicitly use to give a valid 'p' via sma(1-e^2) for parabolic cases if it defines 'sma' appropriately (e.g. as semi-latus rectum itself or NaN/inf)
        // Given the MEOE definition, p is fundamental.
        // Let's re-evaluate p from angular momentum for robustness, as COE might express sma differently for e == 1
        const Vector3d r_vec = aCartesianState.first.inUnit(Position::Unit::Meter).accessCoordinates();
        const Vector3d v_vec = aCartesianState.second.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();
        const Vector3d h_vec = r_vec.cross(v_vec); // Specific angular momentum vector [m^2/s]
        const Real h_norm_sq = h_vec.squaredNorm(); // h^2 [m^4/s^2]
        const Real mu_m3s2 = aGravitationalParameter.inSIUnits(); // [m^3/s^2]
        if (mu_m3s2 == 0.0) return MEOE::Undefined(); // Avoid division by zero
        p_m = h_norm_sq / mu_m3s2;

    } else { // Elliptical or Hyperbolic
        p_m = sma_m * (1.0 - ecc_val * ecc_val);
    }

    if (p_m < 0.0 && ecc_val < 1.0) // Should only happen for hyperbolic where sma < 0, so 1-e^2 < 0, product > 0.
    {
         // This case implies an issue with inputs or COE conversion for hyperbolic, as p should be positive.
         // For hyperbolic orbits, a < 0, so a(1-e^2) means a * (negative number), so p > 0.
         // If p_m is negative here, it indicates an issue.
         return MEOE::Undefined();
    }


    // Check for i_rad approx PI (180 degrees)
    // tan(PI/2) is undefined. This occurs if inc_rad = PI.
    // The original check was `std::abs(inc_rad - Pi) < Real::Epsilon()`.
    // However, h and k use tan(inc_rad / 2.0). So if inc_rad / 2.0 = PI/2, then inc_rad = PI.
    // Or if inc_rad / 2.0 = 3PI/2, then inc_rad = 3PI (not physical for inclination 0 to PI).
    // So the singularity is indeed at inc_rad = PI.
    if (std::abs(inc_rad - Pi) < (Real::Epsilon() * 100.0)) // Increased tolerance slightly
    {
        // This case (inclination is PI) is a singularity for h, k elements.
        // For retrograde equatorial orbits (i=PI), RAAN and AOP are not well-defined conventionally.
        // MEOE aims to avoid some of these, but tan(PI/2) is a hard mathematical limit.
        return MEOE::Undefined(); // Or handle with specific values if a convention exists.
    }

    const Real f_val = ecc_val * std::cos(aop_rad + raan_rad);
    const Real g_val = ecc_val * std::sin(aop_rad + raan_rad);

    const Real tan_i_half = std::tan(inc_rad / 2.0);
    const Real h_val = tan_i_half * std::cos(raan_rad);
    const Real k_val = tan_i_half * std::sin(raan_rad);

    Real L_unnorm_rad = raan_rad + aop_rad + ta_rad;

    // Normalize L to [0, 2*PI)
    L_unnorm_rad = std::fmod(L_unnorm_rad, 2.0 * Pi);
    if (L_unnorm_rad < 0.0)
    {
        L_unnorm_rad += 2.0 * Pi;
    }
    // Second fmod pass in case L_unnorm_rad was exactly -2PI or similar, making it 0 but could be 2PI if slightly less.
    // Or if L_unnorm_rad was a large negative number.
    // The single fmod and add should be sufficient if 2*Pi is positive.
    // Ensure strict positive range [0, 2PI) vs (-2PI, 2PI)
    if (L_unnorm_rad == 2.0 * Pi) L_unnorm_rad = 0.0; // Map 2PI to 0

    return {Length::Meters(p_m), f_val, g_val, h_val, k_val, Angle::Radians(L_unnorm_rad)};
}

String MEOE::StringFromElement(const MEOE::Element& anElement)
{
    switch (anElement)
    {
        case MEOE::Element::SemiLatusRectum:
            return "p";
        case MEOE::Element::F:
            return "f";
        case MEOE::Element::G:
            return "g";
        case MEOE::Element::H:
            return "h";
        case MEOE::Element::K:
            return "k";
        case MEOE::Element::TrueLongitude:
            return "L";
        default:
            throw ostk::core::error::runtime::Wrong("Element");
    }
}

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
