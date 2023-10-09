/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/OrbitalElements.hpp>

namespace ostk
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

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

static const Real Tolerance = 1e-30;
static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

bool OrbitalElements::operator==(const OrbitalElements& anOrbitalElementSet) const
{
    if ((!this->isDefined()) || (!anOrbitalElementSet.isDefined()))
    {
        return false;
    }

    return (semiMajorAxis_ == anOrbitalElementSet.semiMajorAxis_) &&
           (eccentricity_ == anOrbitalElementSet.eccentricity_) && (inclination_ == anOrbitalElementSet.inclination_) &&
           (raan_ == anOrbitalElementSet.raan_) && (aop_ == anOrbitalElementSet.aop_) &&
           (anomaly_ == anOrbitalElementSet.trueAnomaly_) && (anomalyType_ == AnomalyType::True);
}

bool OrbitalElements::operator!=(const OrbitalElements& anOrbitalElementSet) const
{
    return !((*this) == anOrbitalElementSet);
}

std::ostream& operator<<(std::ostream& anOutputStream, const OrbitalElements& anOrbitalElementSet)
{
    anOrbitalElementSet.print(anOutputStream);

    return anOutputStream;
}

bool OrbitalElements::isDefined() const
{
    return semiMajorAxis_.isDefined() && eccentricity_.isDefined() && inclination_.isDefined() && raan_.isDefined() &&
           aop_.isDefined() && anomaly_.isDefined();
}

Length OrbitalElements::getSemiMajorAxis() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return semiMajorAxis_;
}

Real OrbitalElements::getEccentricity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return eccentricity_;
}

Angle OrbitalElements::getInclination() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return inclination_;
}

Angle OrbitalElements::getRaan() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return raan_;
}

Angle OrbitalElements::getAop() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return aop_;
}

Angle OrbitalElements::getTrueAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return ConvertAnomaly(anomaly_, eccentricity_, anomalyType_, AnomalyType::True, 1e-12);
}

Angle OrbitalElements::getMeanAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return ConvertAnomaly(anomaly_, eccentricity_, anomalyType_, AnomalyType::Mean, 1e-12);
}

Angle OrbitalElements::getEccentricAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return return ConvertAnomaly(anomaly_, eccentricity_, anomalyType_, AnomalyType::Eccentric, 1e-12);
}

Length OrbitalElements::getPeriapsisRadius() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return this->semiMajorAxis_ * (1.0 - this->eccentricity_);
}

Length OrbitalElements::getApoapsisRadius() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return this->semiMajorAxis_ * this->eccentricity_;
}

Derived OrbitalElements::getMeanMotion(const Derived& aGravitationalParameter) const
{
    using ostk::physics::units::Time;

    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational parameter");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    const Real semiMajorAxis_m = semiMajorAxis_.inMeters();

    const Real gravitationalParameter_SI = aGravitationalParameter.in(GravitationalParameterSIUnit);

    return Derived(
        std::sqrt(gravitationalParameter_SI / (semiMajorAxis_m * semiMajorAxis_m * semiMajorAxis_m)),
        Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Second)
    );
}

Duration OrbitalElements::getOrbitalPeriod(const Derived& aGravitationalParameter) const
{
    using ostk::physics::units::Time;

    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational parameter");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    return Duration::Seconds(
        Real::TwoPi() / this->getMeanMotion(aGravitationalParameter)
                            .in(Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Second))
    );
}

Vector6d OrbitalElements::getVector(const OrbitalElements::AnomalyType& anAnomalyType) const
{
    return {
        semiMajorAxis_.inMeters(),
        eccentricity_,
        inclination_.inRadians(),
        raan_.inRadians(),
        aop_.inRadians(),
        OrbitalElements::ConvertAnomaly(anomaly_, eccentricity_, anomalyType_, anAnomalyType, 1e-12).inRadians(),
    };
}

OrbitalElements::CartesianState OrbitalElements::getCartesianState(
    const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr
) const
{
    using ostk::core::types::Shared;

    using ostk::math::geom::d3::trf::rot::RotationMatrix;
    using ostk::math::obj::Vector3d;

    using ostk::physics::units::Time;

    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational parameter");
    }

    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("OrbitalElements");
    }

    const Real a_m = semiMajorAxis_.inMeters();
    const Real inclination_rad = inclination_.inRadians();
    const Real raan_rad = raan_.inRadians();
    const Real aop_rad = aop_.inRadians();
    const Real nu_rad = trueAnomaly_.inRadians();
    const Real mu_SI = aGravitationalParameter.in(GravitationalParameterSIUnit);

    const Real p_m = a_m * (1.0 - eccentricity_ * eccentricity_);

    const Vector3d R_pqw = {
        p_m * std::cos(nu_rad) / (1.0 + eccentricity_ * std::cos(nu_rad)),
        p_m * std::sin(nu_rad) / (1.0 + eccentricity_ * std::cos(nu_rad)),
        0.0,
    };

    const Vector3d V_pqw = {
        -std::sqrt(mu_SI / p_m) * std::sin(nu_rad),
        +std::sqrt(mu_SI / p_m) * (eccentricity_ + std::cos(nu_rad)),
        0.0,
    };

    try
    {
        const Vector3d x_ECI = RotationMatrix::RZ(Angle::Radians(-raan_rad)) *
                               RotationMatrix::RX(Angle::Radians(-inclination_rad)) *
                               RotationMatrix::RZ(Angle::Radians(-aop_rad)) * R_pqw;
        const Vector3d v_ECI = RotationMatrix::RZ(Angle::Radians(-raan_rad)) *
                               RotationMatrix::RX(Angle::Radians(-inclination_rad)) *
                               RotationMatrix::RZ(Angle::Radians(-aop_rad)) * V_pqw;

        const Position position = Position::Meters(x_ECI, aFrameSPtr);
        const Velocity velocity = Velocity::MetersPerSecond(v_ECI, aFrameSPtr);

        return {
            position,
            velocity,
        };
    }
    catch (const ostk::core::error::Exception& anException)
    {
        std::cout << "raan_rad = " << raan_rad << std::endl;
        std::cout << "inclination_rad = " << inclination_rad << std::endl;
        std::cout << "aop_rad = " << aop_rad << std::endl;
        std::cout << "R_pqw = " << R_pqw << std::endl;
        std::cout << "V_pqw = " << V_pqw << std::endl;

        throw anException;
    }
}

void OrbitalElements::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::types::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Classical Orbital Elements") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Semi-major axis:"
        << (semiMajorAxis_.isDefined() ? String::Format("{} [m]", semiMajorAxis_.inMeters().toString()) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Eccentricity:" << (eccentricity_.isDefined() ? eccentricity_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Inclination:"
        << (inclination_.isDefined() ? String::Format("{} [deg]", inclination_.inDegrees(0.0, 360.0).toString())
                                     : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Right ascension of the ascending node:"
        << (raan_.isDefined() ? String::Format("{} [deg]", raan_.inDegrees(0.0, 360.0).toString()) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Argument of periapsis:"
        << (aop_.isDefined() ? String::Format("{} [deg]", aop_.inDegrees(0.0, 360.0).toString()) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "True anomaly:"
        << (anomaly_.isDefined() ? String::Format("{} [deg]", getTrueAnomaly().inDegrees(0.0, 360.0).toString())
                                 : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

OrbitalElements OrbitalElements::Undefined()
{
    return {
        Length::Undefined(),
        Real::Undefined(),
        Angle::Undefined(),
        Angle::Undefined(),
        Angle::Undefined(),
        Angle::Undefined(),
    };
}

OrbitalElements OrbitalElements::FromVector(const Vector6d& aCOEVector, const AnomalyType& anAnomalyType)
{
    return {
        Length::Meters(aCOEVector[0]),
        aCOEVector[1],
        Angle::Radians(aCOEVector[2]),
        Angle::Radians(aCOEVector[3]),
        Angle::Radians(aCOEVector[4]),
        OrbitalElements::ConvertAnomaly(
            Angle::Radians(aCOEVector[5]), aCOEVector[1], anAnomalyType, AnomalyType::True, 1e-12
        ),
    };
}

Angle OrbitalElements::EccentricAnomalyFromTrueAnomaly(const Angle& aTrueAnomaly, const Real& anEccentricity)
{
    if (!aTrueAnomaly.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("True anomaly");
    }

    if (!anEccentricity.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Eccentricity");
    }

    const Real trueAnomaly_rad = aTrueAnomaly.inRadians();

    Real eccentricAnomaly_rad = Real::Undefined();
    Real m = Real::Undefined();

    if (anEccentricity.abs() < Tolerance)  // Circular trajectory
    {
        m = trueAnomaly_rad;
        eccentricAnomaly_rad = trueAnomaly_rad;
    }
    else
    {
        if (anEccentricity < (1.0 - Tolerance))  // Elliptical trajectory
        {
            const Real sinE = (std::sqrt(1.0 - anEccentricity * anEccentricity) * std::sin(trueAnomaly_rad)) /
                              (1.0 + anEccentricity * std::cos(trueAnomaly_rad));
            const Real cosE =
                (anEccentricity + std::cos(trueAnomaly_rad)) / (1.0 + anEccentricity * std::cos(trueAnomaly_rad));

            eccentricAnomaly_rad = std::atan2(sinE, cosE);
            m = eccentricAnomaly_rad - anEccentricity * std::sin(eccentricAnomaly_rad);
        }
        else
        {
            if (anEccentricity > (1.0 + Tolerance))  // Hyperbolic trajectory
            {
                if ((anEccentricity > 1.0) &&
                    (std::fabs(trueAnomaly_rad) + 0.00001 < (M_PI - std::acos(1.0 / anEccentricity))))
                {
                    const Real sinE = (std::sqrt(anEccentricity * anEccentricity - 1.0) * std::sin(trueAnomaly_rad)) /
                                      (1.0 + anEccentricity * std::cos(trueAnomaly_rad));

                    eccentricAnomaly_rad = std::asinh(sinE);
                    m = anEccentricity * std::sinh(eccentricAnomaly_rad) - eccentricAnomaly_rad;
                }
                else
                {
                    throw ostk::core::error::RuntimeError("Algorithm error.");
                }
            }
            else
            {
                if (std::fabs(trueAnomaly_rad) < 168.0 * M_PI / 180.0)  // Parabolic trajectory
                {
                    eccentricAnomaly_rad = std::tan(trueAnomaly_rad * 0.5);
                    m = eccentricAnomaly_rad +
                        (eccentricAnomaly_rad * eccentricAnomaly_rad * eccentricAnomaly_rad) / 3.0;
                }
                else
                {
                    throw ostk::core::error::RuntimeError("Algorithm error.");
                }
            }
        }
    }

    if (anEccentricity < 1.0)
    {
        m = std::fmod(m, 2.0 * M_PI);

        if (m < 0.0)
        {
            m += 2.0 * M_PI;
        }

        eccentricAnomaly_rad = std::fmod(eccentricAnomaly_rad, 2.0 * M_PI);
    }

    return Angle::Radians(Angle::Radians(eccentricAnomaly_rad).inRadians(0.0, Real::TwoPi()));
}

Angle OrbitalElements::TrueAnomalyFromEccentricAnomaly(const Angle& anEccentricAnomaly, const Real& anEccentricity)
{
    if (!anEccentricAnomaly.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Eccentric anomaly");
    }

    if (!anEccentricity.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Eccentricity");
    }

    const Real eccentricAnomaly_rad = anEccentricAnomaly.inRadians();
    const Real trueAnomaly_rad = 2.0 * std::atan2(
                                           (std::sqrt(1.0 + anEccentricity) * std::sin(eccentricAnomaly_rad / 2.0)),
                                           (std::sqrt(1.0 - anEccentricity) * std::cos(eccentricAnomaly_rad / 2.0))
                                       );

    return Angle::Radians(Angle::Radians(trueAnomaly_rad).inRadians(0.0, Real::TwoPi()));
}

Angle OrbitalElements::MeanAnomalyFromEccentricAnomaly(const Angle& anEccentricAnomaly, const Real& anEccentricity)
{
    if (!anEccentricAnomaly.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Eccentric anomaly");
    }

    if (!anEccentricity.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Eccentricity");
    }

    const Real eccentricAnomaly_rad = anEccentricAnomaly.inRadians();
    const Real meanAnomaly_rad = eccentricAnomaly_rad - anEccentricity * std::sin(eccentricAnomaly_rad);

    return Angle::Radians(Angle::Radians(meanAnomaly_rad).inRadians(0.0, Real::TwoPi()));
}

Angle OrbitalElements::EccentricAnomalyFromMeanAnomaly(
    const Angle& aMeanAnomaly, const Real& anEccentricity, const Real& aTolerance
)
{
    using ostk::core::types::Real;
    using ostk::core::types::Size;

    if (!aMeanAnomaly.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Mean anomaly");
    }

    if (!anEccentricity.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Eccentricity");
    }

    if (!aTolerance.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tolerance");
    }

    // https://en.wikipedia.org/wiki/Kepler%27s_equation
    // http://alpheratz.net/dynamics/twobody/KeplerIterations_summary.pdf
    // https://gist.github.com/j-faria/1fd079e677325ce820971d9d5d286dad

    // Provides a starting value to solve Kepler's equation

    auto keplerstart3 = [](Real e, Real M) -> Real
    {
        const Real t34 = e * e;
        const Real t35 = e * t34;
        const Real t33 = std::cos(M);

        return M + (-0.5 * t35 + e + (t34 + 1.5 * t33 * t35) * t33) * std::sin(M);
    };

    // An iteration (correction) method to solve Kepler's equation

    auto eps3 = [](Real e, Real M, Real x) -> Real
    {
        const Real t1 = std::cos(x);
        const Real t2 = -1.0 + e * t1;
        const Real t3 = std::sin(x);
        const Real t4 = e * t3;
        const Real t5 = -x + t4 + M;
        const Real t6 = t5 / (0.5 * t5 * t4 / t2 + t2);

        return t5 / (((0.5 * t3) - ((1.0 / 6.0) * t1 * t6)) * e * t6 + t2);
    };

    const Real meanAnomaly_rad = aMeanAnomaly.inRadians();

    const Real M = meanAnomaly_rad;
    const Real Mnorm = std::fmod(M, 2.0 * M_PI);

    Real E = Real::Undefined();

    Real E0 = keplerstart3(anEccentricity, Mnorm);
    Real dE = aTolerance + 1.0;
    Size count = 0;

    while (dE > aTolerance)
    {
        E = E0 - eps3(anEccentricity, Mnorm, E0);
        dE = std::abs(E - E0);
        E0 = E;

        count++;

        if (count > 1000)  // Failed to converge, this only happens for nearly parabolic orbits
        {
            throw ostk::core::error::RuntimeError(
                "Cannot converge to solution ({}, {}, {}).",
                aMeanAnomaly.toString(32),
                anEccentricity.toString(32),
                aTolerance.toString(32)
            );
        }
    }

    return Angle::Radians(E);
}

Angle OrbitalElements::TrueAnomalyFromMeanAnomaly(
    const Angle& aMeanAnomly, const Real& anEccentricity, const Real& aTolerance
)
{
    return TrueAnomalyFromEccentricAnomaly(
        EccentricAnomalyFromMeanAnomaly(aMeanAnomly, anEccentricity, aTolerance), anEccentricity
    );
}

Angle OrbitalElements::ConvertAnomaly(
    const Angle& anAnomaly,
    const Real& anEccentricity,
    const AnomalyType& fromAnomalyType,
    const AnomalyType& toAnomalyType,
    const Real& aTolerance
)
{
    switch (fromAnomalyType)
    {
        case (AnomalyType::True):
        {
            switch (toAnomalyType)
            {
                case (AnomalyType::True):
                    return anAnomaly;
                case (AnomalyType::Eccentric):
                    return EccentricAnomalyFromTrueAnomaly(anAnomaly, anEccentricity);
                case (AnomalyType::Mean):
                    return MeanAnomalyFromEccentricAnomaly(
                        EccentricAnomalyFromTrueAnomaly(anAnomaly, anEccentricity), anEccentricity
                    );
                default:
                    throw ostk::core::error::runtime::Wrong("To Anomaly type");
            }
        }

        case (AnomalyType::Eccentric):
        {
            switch (toAnomalyType)
            {
                case (AnomalyType::True):
                    return TrueAnomalyFromEccentricAnomaly(anAnomaly, anEccentricity);
                case (AnomalyType::Eccentric):
                    return anAnomaly;
                case (AnomalyType::Mean):
                    return MeanAnomalyFromEccentricAnomaly(anAnomaly, anEccentricity);
                default:
                    throw ostk::core::error::runtime::Wrong("To Anomaly type");
            }
        }

        case (AnomalyType::Mean):
        {
            switch (toAnomalyType)
            {
                case (AnomalyType::True):
                    return TrueAnomalyFromMeanAnomaly(anAnomaly, anEccentricity, aTolerance);
                case (AnomalyType::Eccentric):
                    return EccentricAnomalyFromMeanAnomaly(anAnomaly, anEccentricity, aTolerance);
                case (AnomalyType::Mean):
                    return anAnomaly;
                default:
                    throw ostk::core::error::runtime::Wrong("To Anomaly type");
            }
        }

        default:
            throw ostk::core::error::runtime::Wrong("From Anomaly type");
    }
}

String OrbitalElements::StringFromElement(const OrbitalElements::Element& anElement)
{
    switch (anElement)
    {
        case OrbitalElements::Element::SemiMajorAxis:
            return "SemiMajorAxis";
        case OrbitalElements::Element::Eccentricity:
            return "Eccentricity";
        case OrbitalElements::Element::Inclination:
            return "Inclination";
        case OrbitalElements::Element::Aop:
            return "Aop";
        case OrbitalElements::Element::Raan:
            return "Raan";
        case OrbitalElements::Element::TrueAnomaly:
            return "TrueAnomaly";
        case OrbitalElements::Element::MeanAnomaly:
            return "MeanAnomaly";
        case OrbitalElements::Element::EccentricAnomaly:
            return "EccentricAnomaly";
    }

    throw ostk::core::error::runtime::Wrong("Element");
}

}  // namespace kepler
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
