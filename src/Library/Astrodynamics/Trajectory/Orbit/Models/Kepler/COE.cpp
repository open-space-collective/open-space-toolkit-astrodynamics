////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

#include <Library/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>

#include <Library/Core/Types/Size.hpp>
#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{
namespace kepler
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::physics::units::Length ;
using library::physics::units::Time ;
using library::physics::units::Derived ;

static const Real Tolerance = 1e-30 ;
static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                COE::COE                                    (   const   Length&                     aSemiMajorAxis,
                                                                                const   Real&                       anEccentricity,
                                                                                const   Angle&                      anInclination,
                                                                                const   Angle&                      aRaan,
                                                                                const   Angle&                      anAop,
                                                                                const   Angle&                      aTrueAnomaly                                )
                                :   semiMajorAxis_(aSemiMajorAxis),
                                    eccentricity_(anEccentricity),
                                    inclination_(anInclination),
                                    raan_(aRaan),
                                    aop_(anAop),
                                    trueAnomaly_(aTrueAnomaly)
{

}

bool                            COE::operator ==                            (   const   COE&                        aCOE                                        ) const
{

    if ((!this->isDefined()) || (!aCOE.isDefined()))
    {
        return false ;
    }

    return (semiMajorAxis_ == aCOE.semiMajorAxis_)
        && (eccentricity_ == aCOE.eccentricity_)
        && (inclination_ == aCOE.inclination_)
        && (raan_ == aCOE.raan_)
        && (aop_ == aCOE.aop_)
        && (trueAnomaly_ == aCOE.trueAnomaly_) ;

}

bool                            COE::operator !=                            (   const   COE&                        aCOE                                        ) const
{
    return !((*this) == aCOE) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   COE&                        aCOE                                        )
{

    aCOE.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            COE::isDefined                              ( ) const
{
    return semiMajorAxis_.isDefined() && eccentricity_.isDefined() && inclination_.isDefined() && raan_.isDefined() && aop_.isDefined() && trueAnomaly_.isDefined() ;
}

Length                          COE::getSemiMajorAxis                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return semiMajorAxis_ ;

}

Real                            COE::getEccentricity                        ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return eccentricity_ ;

}

Angle                           COE::getInclination                         ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return inclination_ ;

}

Angle                           COE::getRaan                                ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return raan_ ;

}

Angle                           COE::getAop                                 ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return aop_ ;

}

Angle                           COE::getTrueAnomaly                         ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return trueAnomaly_ ;

}

Angle                           COE::getMeanAnomaly                         ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return COE::MeanAnomalyFromEccentricAnomaly(this->getEccentricAnomaly(), eccentricity_) ;

}

Angle                           COE::getEccentricAnomaly                    ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return COE::EccentricAnomalyFromTrueAnomaly(trueAnomaly_, eccentricity_) ;

}

Derived                         COE::getMeanMotion                          (   const   Derived&                    aGravitationalParameter                     ) const
{

    using library::physics::units::Mass ;
    using library::physics::units::Time ;

    if (!aGravitationalParameter.isDefined())
    {
        throw library::core::error::runtime::Undefined("Gravitational parameter") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    const Real semiMajorAxis_m = semiMajorAxis_.inMeters() ;

	const Real gravitationalParameter_SI = aGravitationalParameter.in(GravitationalParameterSIUnit) ;

	return Derived(std::sqrt(gravitationalParameter_SI / (semiMajorAxis_m * semiMajorAxis_m * semiMajorAxis_m)), Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Second)) ;

}

Duration                        COE::getOrbitalPeriod                       (   const   Derived&                    aGravitationalParameter                     ) const
{

    using library::physics::units::Time ;

    if (!aGravitationalParameter.isDefined())
    {
        throw library::core::error::runtime::Undefined("Gravitational parameter") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    return Duration::Seconds(Real::TwoPi() / this->getMeanMotion(aGravitationalParameter).in(Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Second))) ;

}

COE::CartesianState             COE::getCartesianState                      (   const   Derived&                    aGravitationalParameter,
                                                                                const   Shared<const Frame>&        aFrameSPtr                                  ) const
{

    using library::core::types::Shared ;

    using library::math::obj::Vector3d ;
    using library::math::geom::d3::trf::rot::RotationMatrix ;

    using library::physics::units::Mass ;
    using library::physics::units::Time ;

    if (!aGravitationalParameter.isDefined())
    {
        throw library::core::error::runtime::Undefined("Gravitational parameter") ;
    }

    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw library::core::error::runtime::Undefined("Frame") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("COE") ;
    }

    const Real a_m = semiMajorAxis_.inMeters() ;
	const Real inclination_rad = inclination_.inRadians() ;
	const Real raan_rad = raan_.inRadians() ;
	const Real aop_rad = aop_.inRadians() ;
	const Real nu_rad = trueAnomaly_.inRadians() ;
	const Real mu_SI = aGravitationalParameter.in(GravitationalParameterSIUnit) ;

	const Real p_m = a_m * (1.0 - eccentricity_ * eccentricity_) ;

	const Vector3d R_pqw =
    {
        p_m * std::cos(nu_rad) / (1.0 + eccentricity_ * std::cos(nu_rad)),
        p_m * std::sin(nu_rad) / (1.0 + eccentricity_ * std::cos(nu_rad)),
        0.0
    } ;

	const Vector3d V_pqw =
    {
        -std::sqrt(mu_SI / p_m) * std::sin(nu_rad),
        +std::sqrt(mu_SI / p_m) * (eccentricity_ + std::cos(nu_rad)),
        0.0
    } ;

    try
    {

        const Vector3d x_ECI = RotationMatrix::RZ(Angle::Radians(-raan_rad)) * RotationMatrix::RX(Angle::Radians(-inclination_rad)) * RotationMatrix::RZ(Angle::Radians(-aop_rad)) * R_pqw ;
        const Vector3d v_ECI = RotationMatrix::RZ(Angle::Radians(-raan_rad)) * RotationMatrix::RX(Angle::Radians(-inclination_rad)) * RotationMatrix::RZ(Angle::Radians(-aop_rad)) * V_pqw ;

        const Position position = { x_ECI, Position::Unit::Meter, aFrameSPtr } ;
        const Velocity velocity = { v_ECI, Velocity::Unit::MeterPerSecond, aFrameSPtr } ;

        return { position, velocity } ;

    }
    catch (const library::core::error::Exception& anException)
    {

        std::cout << "raan_rad = " << raan_rad << std::endl ;
        std::cout << "inclination_rad = " << inclination_rad << std::endl ;
        std::cout << "aop_rad = " << aop_rad << std::endl ;
        std::cout << "R_pqw = " << R_pqw << std::endl ;
        std::cout << "V_pqw = " << V_pqw << std::endl ;

        throw anException ;

    }

}

void                            COE::print                                  (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    using library::core::types::String ;

    displayDecorator ? library::core::utils::Print::Header(anOutputStream, "Classical Orbital Elements") : void () ;

    library::core::utils::Print::Line(anOutputStream) << "Semi-major axis:"                         << (semiMajorAxis_.isDefined() ? String::Format("{} [m]", semiMajorAxis_.inMeters().toString()) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Eccentricity:"                            << (eccentricity_.isDefined() ? eccentricity_.toString() : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Inclination:"                             << (inclination_.isDefined() ? String::Format("{} [deg]", inclination_.inDegrees(0.0, 360.0).toString()) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Right ascension of the ascending node:"   << (raan_.isDefined() ? String::Format("{} [deg]", raan_.inDegrees(0.0, 360.0).toString()) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Argument of periapsis:"                   << (aop_.isDefined() ? String::Format("{} [deg]", aop_.inDegrees(0.0, 360.0).toString()) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "True anomaly:"                            << (trueAnomaly_.isDefined() ? String::Format("{} [deg]", trueAnomaly_.inDegrees(0.0, 360.0).toString()) : "Undefined") ;

    displayDecorator ? library::core::utils::Print::Footer(anOutputStream) : void () ;

}

COE                             COE::Undefined                              ( )
{
    return { Length::Undefined(), Real::Undefined(), Angle::Undefined(), Angle::Undefined(), Angle::Undefined(), Angle::Undefined() } ;
}

COE                             COE::Cartesian                              (   const   COE::CartesianState&        aCartesianState,
                                                                                const   Derived&                    aGravitationalParameter                     )
{

    using library::math::obj::Vector3d ;

    using library::physics::units::Mass ;
    using library::physics::units::Time ;

    if ((!aCartesianState.first.isDefined()) || (!aCartesianState.second.isDefined()))
    {
        throw library::core::error::runtime::Undefined("Cartesian state") ;
    }

    if (!aGravitationalParameter.isDefined())
    {
        throw library::core::error::runtime::Undefined("Gravitational parameter") ;
    }

    static const Real tolerance = 1e-11 ;

    const Real mu = aGravitationalParameter.in(GravitationalParameterSIUnit) ;

    if (mu == 0.0)
    {
        throw library::core::error::runtime::Wrong("Gravitational parameter") ;
    }

    const Vector3d& positionVector = aCartesianState.first.accessCoordinates() ;
    const Vector3d& velocityVector = aCartesianState.second.accessCoordinates() ;

    const Real position = positionVector.norm() ;
    const Real velocity = velocityVector.norm() ;

    if (position == 0.0)
    {
        throw library::core::error::runtime::Wrong("Position vector") ;
    }

    // Angular momentum

    const Vector3d angularMomentumVector = positionVector.cross(velocityVector) ;

    const Real angularMomentum = angularMomentumVector.norm() ;

    if (angularMomentum == 0.0)
    {
        throw library::core::error::runtime::Wrong("Angular momentum") ;
    }

    // Node

    const Vector3d nodeVector = Vector3d::Z().cross(angularMomentumVector) ;

    const Real node = nodeVector.norm() ;

    // Eccentricity

    const Vector3d eccentricityVector = (1.0 / mu) * ((((velocity * velocity) - (mu / position)) * positionVector) - ((positionVector.dot(velocityVector)) * (velocityVector))) ;

    const Real e = eccentricityVector.norm() ;

    if ((std::abs(1.0 - e)) <= Real::Epsilon())
    {
        throw library::core::error::runtime::ToBeImplemented("Support for parabolic orbits.") ;
    }

    // Semi-major axis

    const Real E = (0.5 * velocity * velocity) - (mu / position) ;

    if (E == 0.0)
    {
        throw library::core::error::runtime::Wrong("Specific orbital energy") ;
    }

    const Real a_m = - mu / (2.0 * E) ;

    if (std::abs(a_m * (1.0 - e)) < Real::Epsilon())
    {
        throw library::core::error::RuntimeError("Conic section is singular.") ;
    }

    // Inclination

    const Real i_rad = std::acos(angularMomentumVector(2)/ angularMomentum) ;

    // Other angles

    Real raan_rad = 0.0 ; // [rad] Right Ascension of the Ascending Node (RAAN)
    Real aop_rad = 0.0 ; // [rad] Argument Of Periapsis (AOP)
    Real nu_rad = 0.0 ; // [rad] True anomaly

    if ((e >= tolerance) && ((i_rad >= tolerance) && (i_rad <= (Real::Pi() - tolerance)))) // Non-circular, inclined
    {

        if (node == 0.0)
        {
            throw library::core::error::runtime::Undefined("Node") ;
        }

        raan_rad = std::acos(nodeVector(0) / node) ;

        if (nodeVector(1) < 0.0)
        {
            raan_rad = Real::TwoPi() - raan_rad ;
        }

        aop_rad = std::acos((nodeVector.dot(eccentricityVector)) / (node * e)) ;

        if (eccentricityVector(2) < 0.0)
        {
            aop_rad = Real::TwoPi() - aop_rad ;
        }

        nu_rad = std::acos((eccentricityVector.dot(positionVector)) / (e * position)) ;

        if (positionVector.dot(velocityVector) < 0.0)
        {
            nu_rad = Real::TwoPi() - nu_rad ;
        }

    }
    else if ((e >= tolerance) && ((i_rad < tolerance) || (i_rad > (Real::Pi() - tolerance)))) // Non-circular, equatorial
    {

        raan_rad = 0.0 ;
        aop_rad = std::acos(eccentricityVector(0) / e) ;

        if (eccentricityVector(1) < 0.0)
        {
            aop_rad = Real::TwoPi() - aop_rad ;
        }

        if (i_rad > (Real::Pi() - tolerance))
        {
            aop_rad= aop_rad * -1.0 ;
        }

        if (aop_rad < 0.0)
        {
            aop_rad = aop_rad + Real::TwoPi() ;
        }

        nu_rad = std::acos((eccentricityVector.dot(positionVector)) / (e * position)) ;

        if (positionVector.dot(velocityVector) < 0.0)
        {
            nu_rad = Real::TwoPi() - nu_rad ;
        }

    }
    else if ((e < tolerance) && ((i_rad >= tolerance) && (i_rad <= (Real::Pi() - tolerance)))) // Circular, inclined
    {

        if (node == 0.0)
        {
            throw library::core::error::runtime::Undefined("Node") ;
        }

        raan_rad = std::acos(nodeVector(0) / node) ;

        if (nodeVector(1) < 0.0)
        {
            raan_rad = Real::TwoPi() - raan_rad ;
        }

        aop_rad = 0.0 ;

        nu_rad = std::acos((nodeVector.dot(positionVector)) / (node * position)) ;

        if (positionVector(2) < 0.0)
        {
            nu_rad = Real::TwoPi() - nu_rad ;
        }

    }
    else if ((e < tolerance) && ((i_rad < tolerance) || (i_rad > (Real::Pi() - tolerance)))) // Circular, equatorial
    {

        raan_rad = 0.0 ;
        aop_rad = 0.0 ;

        nu_rad = std::acos(positionVector(0) / position) ;

        if (positionVector(1) < 0.0)
        {
            nu_rad = Real::TwoPi() - nu_rad ;
        }

        if (i_rad > (Real::Pi() - tolerance))
        {
            nu_rad = nu_rad * -1.0 ;
        }

        if (nu_rad < 0.0)
        {
            nu_rad = nu_rad + Real::TwoPi() ;
        }

    }

    return { Length::Meters(a_m), e, Angle::Radians(i_rad), Angle::Radians(raan_rad), Angle::Radians(aop_rad), Angle::Radians(nu_rad) } ;

}

Angle                           COE::EccentricAnomalyFromTrueAnomaly        (   const   Angle&                      aTrueAnomaly,
                                                                                const   Real&                       anEccentricity                              )
{

    if (!aTrueAnomaly.isDefined())
    {
        throw library::core::error::runtime::Undefined("True anomaly") ;
    }

    if (!anEccentricity.isDefined())
    {
        throw library::core::error::runtime::Undefined("Eccentricity") ;
    }

    const Real trueAnomaly_rad = aTrueAnomaly.inRadians() ;

    Real eccentricAnomaly_rad = Real::Undefined() ;
    Real m = Real::Undefined() ;

    if (anEccentricity.abs() < Tolerance) // Circular trajectory
    {

        m = trueAnomaly_rad ;
        eccentricAnomaly_rad = trueAnomaly_rad ;

    }
    else
    {

        if (anEccentricity < (1.0 - Tolerance)) // Elliptical trajectory
        {

            const Real sinE = (std::sqrt(1.0 - anEccentricity * anEccentricity) * std::sin(trueAnomaly_rad)) / (1.0 + anEccentricity * std::cos(trueAnomaly_rad)) ;
            const Real cosE = (anEccentricity + std::cos(trueAnomaly_rad)) / (1.0  + anEccentricity * std::cos(trueAnomaly_rad)) ;

            eccentricAnomaly_rad = std::atan2(sinE, cosE) ;
            m = eccentricAnomaly_rad - anEccentricity * std::sin(eccentricAnomaly_rad) ;

        }
        else
        {

            if (anEccentricity > (1.0 + Tolerance)) // Hyperbolic trajectory
            {

                if ((anEccentricity > 1.0 ) && (std::fabs(trueAnomaly_rad) + 0.00001 < (M_PI - std::acos(1.0 / anEccentricity))))
                {

                    const Real sinE = (std::sqrt(anEccentricity * anEccentricity - 1.0) * std::sin(trueAnomaly_rad)) / (1.0  + anEccentricity * std::cos(trueAnomaly_rad)) ;

                    eccentricAnomaly_rad = std::asinh(sinE) ;
                    m = anEccentricity * std::sinh(eccentricAnomaly_rad) - eccentricAnomaly_rad ;

                }
                else
                {
                    throw library::core::error::RuntimeError("Algorithm error.") ;
                }

            }
            else
            {

                if (std::fabs(trueAnomaly_rad) < 168.0 * M_PI / 180.0) // Parabolic trajectory
                {

                    eccentricAnomaly_rad = std::tan(trueAnomaly_rad * 0.5) ;
                    m = eccentricAnomaly_rad + (eccentricAnomaly_rad * eccentricAnomaly_rad * eccentricAnomaly_rad) / 3.0 ;

                }
                else
                {
                    throw library::core::error::RuntimeError("Algorithm error.") ;
                }

            }

        }

    }

    if (anEccentricity < 1.0)
    {

        m = std::fmod(m, 2.0 * M_PI) ;

        if (m < 0.0)
        {
            m += 2.0 * M_PI ;
        }

        eccentricAnomaly_rad = std::fmod(eccentricAnomaly_rad, 2.0 * M_PI) ;

    }

    return Angle::Radians(Angle::Radians(eccentricAnomaly_rad).inRadians(0.0, Real::TwoPi())) ;

}

Angle                           COE::TrueAnomalyFromEccentricAnomaly        (   const   Angle&                      anEccentricAnomaly,
                                                                                const   Real&                       anEccentricity                              )
{

    if (!anEccentricAnomaly.isDefined())
    {
        throw library::core::error::runtime::Undefined("Eccentric anomaly") ;
    }

    if (!anEccentricity.isDefined())
    {
        throw library::core::error::runtime::Undefined("Eccentricity") ;
    }

    const Real eccentricAnomaly_rad = anEccentricAnomaly.inRadians() ;
    const Real trueAnomaly_rad = 2.0 * std::atan2((std::sqrt(1.0 + anEccentricity) * std::sin(eccentricAnomaly_rad / 2.0)), (std::sqrt(1.0 - anEccentricity) * std::cos(eccentricAnomaly_rad / 2.0))) ;

    return Angle::Radians(Angle::Radians(trueAnomaly_rad).inRadians(0.0, Real::TwoPi())) ;

}

Angle                           COE::MeanAnomalyFromEccentricAnomaly        (   const   Angle&                      anEccentricAnomaly,
                                                                                const   Real&                       anEccentricity                              )
{

    if (!anEccentricAnomaly.isDefined())
    {
        throw library::core::error::runtime::Undefined("Eccentric anomaly") ;
    }

    if (!anEccentricity.isDefined())
    {
        throw library::core::error::runtime::Undefined("Eccentricity") ;
    }

    const Real eccentricAnomaly_rad = anEccentricAnomaly.inRadians() ;
    const Real meanAnomaly_rad = eccentricAnomaly_rad - anEccentricity * std::sin(eccentricAnomaly_rad) ;

    return Angle::Radians(Angle::Radians(meanAnomaly_rad).inRadians(0.0, Real::TwoPi())) ;

}

Angle                           COE::EccentricAnomalyFromMeanAnomaly        (   const   Angle&                      aMeanAnomaly,
                                                                                const   Real&                       anEccentricity,
                                                                                const   Real&                       aTolerance                                  )
{

    using library::core::types::Size ;
    using library::core::types::Real ;

    if (!aMeanAnomaly.isDefined())
    {
        throw library::core::error::runtime::Undefined("Mean anomaly") ;
    }

    if (!anEccentricity.isDefined())
    {
        throw library::core::error::runtime::Undefined("Eccentricity") ;
    }

    if (!aTolerance.isDefined())
    {
        throw library::core::error::runtime::Undefined("Tolerance") ;
    }

    // https://en.wikipedia.org/wiki/Kepler%27s_equation
    // http://alpheratz.net/dynamics/twobody/KeplerIterations_summary.pdf
    // https://gist.github.com/j-faria/1fd079e677325ce820971d9d5d286dad

    // Provides a starting value to solve Kepler's equation

    auto keplerstart3 = [] (Real e, Real M) -> Real
    {

        const Real t34 = e * e ;
        const Real t35 = e * t34 ;
        const Real t33 = std::cos(M) ;

        return M + (-0.5 * t35 + e + (t34 + 1.5 * t33 * t35) * t33) * std::sin(M) ;

    } ;

    // An iteration (correction) method to solve Kepler's equation

    auto eps3 = [] (Real e, Real M, Real x) -> Real
    {

        const Real t1 = std::cos(x) ;
        const Real t2 = -1.0 + e * t1 ;
        const Real t3 = std::sin(x) ;
        const Real t4 = e * t3 ;
        const Real t5 = -x + t4 + M ;
        const Real t6 = t5 / (0.5 * t5 * t4 / t2 + t2) ;

        return t5 / (((0.5 * t3) - ((1.0 / 6.0) * t1 * t6)) * e * t6 + t2) ;

    } ;

    const Real meanAnomaly_rad = aMeanAnomaly.inRadians() ;

    const Real M = meanAnomaly_rad ;
    const Real Mnorm = std::fmod(M, 2.0 * M_PI) ;

    Real E = Real::Undefined() ;

    Real E0 = keplerstart3(anEccentricity, Mnorm) ;
    Real dE = aTolerance + 1.0 ;
    Size count = 0 ;

    while (dE > aTolerance)
    {

        E = E0 - eps3(anEccentricity, Mnorm, E0) ;
        dE = std::abs(E - E0) ;
        E0 = E ;

        count++ ;

        if (count > 1000) // Failed to converge, this only happens for nearly parabolic orbits
        {
            throw library::core::error::RuntimeError("Cannot converge to solution ({}, {}, {}).", aMeanAnomaly.toString(32), anEccentricity.toString(32), aTolerance.toString(32)) ;
        }

    }

    return Angle::Radians(E) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
