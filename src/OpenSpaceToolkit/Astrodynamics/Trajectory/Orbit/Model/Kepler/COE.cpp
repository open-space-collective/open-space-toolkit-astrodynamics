/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

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

using ostk::core::type::Array;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::time::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::ElectricCurrent;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;
using TimeUnit = ostk::physics::unit::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

static const Real Tolerance = 1e-30;
static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::MeterCubedPerSecondSquared();
static const Derived::Unit angularVelocitySIUnit = Derived::Unit::RadianPerSecond();
static const Derived::Unit AngularMomentumSIUnit = {
    Length::Unit::Meter,
    {2},
    Mass::Unit::Kilogram,
    {1},
    TimeUnit::Unit::Second,
    {-2},
    ElectricCurrent::Unit::Undefined,
    {0},
    Angle::Unit::Undefined,
    {0},
};

COE::COE(
    const Length& aSemiMajorAxis,
    const Real& anEccentricity,
    const Angle& anInclination,
    const Angle& aRaan,
    const Angle& anAop,
    const Angle& aTrueAnomaly
)
    : semiMajorAxis_(aSemiMajorAxis),
      eccentricity_(anEccentricity),
      inclination_(anInclination),
      raan_(aRaan),
      aop_(anAop),
      anomaly_(aTrueAnomaly),
      anomalyType_(COE::AnomalyType::True)
{
}

bool COE::operator==(const COE& aCOE) const
{
    if ((!this->isDefined()) || (!aCOE.isDefined()))
    {
        return false;
    }

    return (semiMajorAxis_ == aCOE.semiMajorAxis_) && (eccentricity_ == aCOE.eccentricity_) &&
           (inclination_ == aCOE.inclination_) && (raan_ == aCOE.raan_) && (aop_ == aCOE.aop_) &&
           (anomaly_ == aCOE.anomaly_) && (anomalyType_ == aCOE.anomalyType_);
}

bool COE::operator!=(const COE& aCOE) const
{
    return !((*this) == aCOE);
}

std::ostream& operator<<(std::ostream& anOutputStream, const COE& aCOE)
{
    aCOE.print(anOutputStream);

    return anOutputStream;
}

bool COE::isDefined() const
{
    return semiMajorAxis_.isDefined() && eccentricity_.isDefined() && inclination_.isDefined() && raan_.isDefined() &&
           aop_.isDefined() && anomaly_.isDefined();
}

Length COE::getSemiMajorAxis() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return semiMajorAxis_;
}

Real COE::getEccentricity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return eccentricity_;
}

Angle COE::getInclination() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return inclination_;
}

Angle COE::getRaan() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return raan_;
}

Angle COE::getAop() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return aop_;
}

Angle COE::getTrueAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return anomaly_;
}

Angle COE::getArgumentOfLatitude() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return aop_ + anomaly_;
}

Angle COE::getMeanAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return COE::MeanAnomalyFromEccentricAnomaly(this->getEccentricAnomaly(), eccentricity_);
}

Angle COE::getEccentricAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return COE::EccentricAnomalyFromTrueAnomaly(anomaly_, eccentricity_);
}

Length COE::getPeriapsisRadius() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return this->semiMajorAxis_ * (1.0 - this->eccentricity_);
}

Length COE::getApoapsisRadius() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return this->semiMajorAxis_ * (1.0 + this->eccentricity_);
}

Length COE::getSemiLatusRectum() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return Length::Meters(COE::ComputeSemiLatusRectum(semiMajorAxis_.inMeters(), eccentricity_));
}

Length COE::getRadialDistance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return Length::Meters(COE::ComputeRadialDistance(
        semiMajorAxis_.inMeters(),
        eccentricity_,
        COE::ConvertAnomaly(anomaly_, eccentricity_, anomalyType_, AnomalyType::True, 1e-12).inRadians()
    ));
}

Derived COE::getAngularMomentum(const Derived& aGravitationalParameter) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return Derived(
        COE::ComputeAngularMomentum(
            COE::ComputeSemiLatusRectum(semiMajorAxis_.inMeters(), eccentricity_), aGravitationalParameter
        ),
        AngularMomentumSIUnit
    );
}

Derived COE::getMeanMotion(const Derived& aGravitationalParameter) const
{
    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational parameter");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    const Real semiMajorAxis_m = semiMajorAxis_.inMeters();

    const Real gravitationalParameter_SI = aGravitationalParameter.in(GravitationalParameterSIUnit);

    return Derived(
        std::sqrt(gravitationalParameter_SI / (semiMajorAxis_m * semiMajorAxis_m * semiMajorAxis_m)),
        angularVelocitySIUnit
    );
}

Derived COE::getNodalPrecessionRate(
    const Derived& aGravitationalParameter, const Length& anEquatorialRadius, const Real& aJ2Parameter
) const
{
    const Real omega = this->getMeanMotion(aGravitationalParameter).in(angularVelocitySIUnit);

    const Real omega_p =
        -(3.0 / 2.0) * std::pow(anEquatorialRadius.inMeters(), 2.0) * aJ2Parameter * omega *
        std::cos(this->inclination_.inRadians()) /
        std::pow(this->semiMajorAxis_.inMeters() * (1.0 - (this->eccentricity_ * this->eccentricity_)), 2.0);

    return Derived(omega_p, angularVelocitySIUnit);
}

Duration COE::getOrbitalPeriod(const Derived& aGravitationalParameter) const
{
    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational parameter");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    return Duration::Seconds(Real::TwoPi() / this->getMeanMotion(aGravitationalParameter).in(angularVelocitySIUnit));
}

COE::CartesianState COE::getCartesianState(
    const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr
) const
{
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
        throw ostk::core::error::runtime::Undefined("COE");
    }

    const Real a_m = semiMajorAxis_.inMeters();
    const Real inclination_rad = inclination_.inRadians();
    const Real raan_rad = raan_.inRadians();
    const Real aop_rad = aop_.inRadians();
    const Real nu_rad = anomaly_.inRadians();
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

Vector6d COE::getSIVector(const COE::AnomalyType& anAnomalyType) const
{
    return {
        semiMajorAxis_.inMeters(),
        eccentricity_,
        inclination_.inRadians(),
        raan_.inRadians(),
        aop_.inRadians(),
        COE::ConvertAnomaly(anomaly_, eccentricity_, AnomalyType::True, anAnomalyType, 1e-12).inRadians(),
    };
}

void COE::print(std::ostream& anOutputStream, bool displayDecorator) const
{
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
    String anomalyType = String::Empty();
    switch (anomalyType_)
    {
        case COE::AnomalyType::True:
            anomalyType = "True";
            break;
        case COE::AnomalyType::Mean:
            anomalyType = "Mean";
            break;
        case COE::AnomalyType::Eccentric:
            anomalyType = "Eccentric";
            break;
        default:
            throw ostk::core::error::runtime::Wrong("Anomaly type");
    }
    ostk::core::utils::Print::Line(anOutputStream)
        << String::Format("{} anomaly:", anomalyType)
        << (anomaly_.isDefined() ? String::Format("{} [deg]", anomaly_.inDegrees(0.0, 360.0).toString()) : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

COE COE::Undefined()
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

COE COE::Cartesian(const COE::CartesianState& aCartesianState, const Derived& aGravitationalParameter)
{
    if ((!aCartesianState.first.isDefined()) || (!aCartesianState.second.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Cartesian state");
    }

    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational parameter");
    }

    static const Real tolerance = 1e-11;

    const Real mu = aGravitationalParameter.in(GravitationalParameterSIUnit);

    if (std::abs(mu) < Real::Epsilon())
    {
        throw ostk::core::error::runtime::Wrong("Gravitational parameter", String::Format("{}", mu));
    }

    const Vector3d& positionVector = aCartesianState.first.accessCoordinates();
    const Vector3d& velocityVector = aCartesianState.second.accessCoordinates();

    const Real position = positionVector.norm();
    const Real velocity = velocityVector.norm();

    if (std::abs(position) < Real::Epsilon())
    {
        throw ostk::core::error::runtime::Wrong("Position vector", String::Format("{}", position));
    }

    // Angular momentum

    const Vector3d angularMomentumVector = positionVector.cross(velocityVector);

    const Real angularMomentum = angularMomentumVector.norm();

    if (std::abs(angularMomentum) < Real::Epsilon())
    {
        throw ostk::core::error::runtime::Wrong("Angular momentum", String::Format("{}", angularMomentum));
    }

    // Node

    const Vector3d nodeVector = Vector3d::Z().cross(angularMomentumVector);

    const Real node = nodeVector.norm();

    // Eccentricity

    const Vector3d eccentricityVector = (1.0 / mu) * ((((velocity * velocity) - (mu / position)) * positionVector) -
                                                      ((positionVector.dot(velocityVector)) * (velocityVector)));

    const Real e = eccentricityVector.norm();

    if ((std::abs(1.0 - e)) <= Real::Epsilon())
    {
        throw ostk::core::error::runtime::ToBeImplemented("Support for parabolic orbits.");
    }

    // Semi-major axis

    const Real E = (0.5 * velocity * velocity) - (mu / position);

    if (std::abs(E) < Real::Epsilon())
    {
        throw ostk::core::error::runtime::Wrong("Specific orbital energy", String::Format("{}", E));
    }

    const Real a_m = -mu / (2.0 * E);

    if (std::abs(a_m * (1.0 - e)) < Real::Epsilon())
    {
        throw ostk::core::error::RuntimeError("Conic section is singular.");
    }

    // Inclination

    const Real i_rad = std::acos(angularMomentumVector(2) / angularMomentum);

    // Other angles

    Real raan_rad = 0.0;  // [rad] Right Ascension of the Ascending Node (RAAN)
    Real aop_rad = 0.0;   // [rad] Argument Of Periapsis (AOP)
    Real nu_rad = 0.0;    // [rad] True anomaly (Nu)

    if ((e >= tolerance) && ((i_rad >= tolerance) && (i_rad <= (Real::Pi() - tolerance))))  // Non-circular, inclined
    {
        if (std::abs(node) < Real::Epsilon())
        {
            throw ostk::core::error::runtime::Undefined("Node");
        }

        raan_rad = std::acos(nodeVector(0) / node);

        if (nodeVector(1) < 0.0)
        {
            raan_rad = Real::TwoPi() - raan_rad;
        }

        aop_rad = std::acos(std::clamp((double)(nodeVector.dot(eccentricityVector) / (node * e)), -1.0, 1.0));

        if (eccentricityVector(2) < 0.0)
        {
            aop_rad = Real::TwoPi() - aop_rad;
        }

        nu_rad = std::acos(std::clamp((double)(eccentricityVector.dot(positionVector) / (e * position)), -1.0, 1.0));

        if (positionVector.dot(velocityVector) < 0.0)
        {
            nu_rad = Real::TwoPi() - nu_rad;
        }
    }
    else if ((e >= tolerance) && ((i_rad < tolerance) || (i_rad > (Real::Pi() - tolerance))))  // Non-circular,
                                                                                               // equatorial
    {
        raan_rad = 0.0;
        aop_rad = std::acos(eccentricityVector(0) / e);

        if (eccentricityVector(1) < 0.0)
        {
            aop_rad = Real::TwoPi() - aop_rad;
        }

        if (i_rad > (Real::Pi() - tolerance))
        {
            aop_rad = aop_rad * -1.0;
        }

        if (aop_rad < 0.0)
        {
            aop_rad = aop_rad + Real::TwoPi();
        }

        nu_rad = std::acos(std::clamp((double)(eccentricityVector.dot(positionVector) / (e * position)), -1.0, 1.0));

        if (positionVector.dot(velocityVector) < 0.0)
        {
            nu_rad = Real::TwoPi() - nu_rad;
        }
    }
    else if ((e < tolerance) && ((i_rad >= tolerance) && (i_rad <= (Real::Pi() - tolerance))))  // Circular, inclined
    {
        if (std::abs(node) < Real::Epsilon())
        {
            throw ostk::core::error::runtime::Undefined("Node");
        }

        raan_rad = std::acos(nodeVector(0) / node);

        if (nodeVector(1) < 0.0)
        {
            raan_rad = Real::TwoPi() - raan_rad;
        }

        aop_rad = 0.0;

        nu_rad = std::acos(std::clamp((double)(nodeVector.dot(positionVector) / (node * position)), -1.0, 1.0));

        if (positionVector(2) < 0.0)
        {
            nu_rad = Real::TwoPi() - nu_rad;
        }
    }
    else if ((e < tolerance) && ((i_rad < tolerance) || (i_rad > (Real::Pi() - tolerance))))  // Circular, equatorial
    {
        raan_rad = 0.0;
        aop_rad = 0.0;

        nu_rad = std::acos(positionVector(0) / position);

        if (positionVector(1) < 0.0)
        {
            nu_rad = Real::TwoPi() - nu_rad;
        }

        if (i_rad > (Real::Pi() - tolerance))
        {
            nu_rad = nu_rad * -1.0;
        }

        if (nu_rad < 0.0)
        {
            nu_rad = nu_rad + Real::TwoPi();
        }
    }

    return {
        Length::Meters(a_m),
        e,
        Angle::Radians(i_rad),
        Angle::Radians(raan_rad),
        Angle::Radians(aop_rad),
        Angle::Radians(nu_rad),
    };
}

COE COE::FromSIVector(const Vector6d& aCOEVector, const AnomalyType& anAnomalyType)
{
    return {
        Length::Meters(aCOEVector[0]),
        aCOEVector[1],
        Angle::Radians(aCOEVector[2]),
        Angle::Radians(aCOEVector[3]),
        Angle::Radians(aCOEVector[4]),
        COE::ConvertAnomaly(Angle::Radians(aCOEVector[5]), aCOEVector[1], anAnomalyType, AnomalyType::True, 1e-15),
    };
}

COE COE::FrozenOrbit(
    const Length& aSemiMajorAxis,
    const Shared<const Celestial>& aCelestialObjectSPtr,
    const Real& anEccentricity,
    const Angle& anInclination,
    const Angle& aRaan,
    const Angle& anAop,
    const Angle& aTrueAnomaly
)
{
    if ((aCelestialObjectSPtr == nullptr) || (!aCelestialObjectSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Celestial object");
    }

    const Length equatorialRadius = aCelestialObjectSPtr->getEquatorialRadius();
    const Real j2 = aCelestialObjectSPtr->accessGravitationalModel()->getParameters().J2_;
    const Real j3 = aCelestialObjectSPtr->accessGravitationalModel()->getParameters().J3_;

    return COE::FrozenOrbit(
        aSemiMajorAxis, equatorialRadius, j2, j3, anEccentricity, anInclination, aRaan, anAop, aTrueAnomaly
    );
}

COE COE::FrozenOrbit(
    const Length& aSemiMajorAxis,
    const Length& anEquatorialRadius,
    const Real& aJ2,
    const Real& aJ3,
    const Real& anEccentricity,
    const Angle& anInclination,
    const Angle& aRaan,
    const Angle& anAop,
    const Angle& aTrueAnomaly
)
{
    if (!aSemiMajorAxis.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Semi-major Axis");
    }
    if (!anEquatorialRadius.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Equatorial Radius");
    }
    if (!aJ2.isDefined() || std::abs(aJ2) < Real::Epsilon())
    {
        throw ostk::core::error::runtime::Undefined("J2");
    }
    if (!aJ3.isDefined() || std::abs(aJ3) < Real::Epsilon())
    {
        throw ostk::core::error::runtime::Undefined("J3");
    }
    if (!aRaan.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Raan");
    }
    if (!aTrueAnomaly.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("True anomaly");
    }
    if (anEccentricity.isDefined() && anInclination.isDefined())
    {
        throw ostk::core::error::RuntimeError("Cannot define both eccentricity and inclination");
    }

    const Array<Angle> criticalInclinations = {Angle::Degrees(63.4349), Angle::Degrees(116.5651)};
    const Array<Angle> criticalAops = {Angle::Degrees(90.0), Angle::Degrees(270.0)};

    const Real equatorialRadius_meters = anEquatorialRadius.inMeters();
    const Real j2 = aJ2;
    const Real j3 = aJ3;

    // ecc =~ eccCoefficient * sin(incl)
    const Real eccCoefficient = -j3 * equatorialRadius_meters / 2.0 / j2 / aSemiMajorAxis.inMeters();

    const auto eccentricityFromInclination = [&eccCoefficient](const Angle& inclination) -> Real
    {
        return eccCoefficient * std::sin(inclination.inRadians());
    };

    const auto inclinationFromEccentricity = [&eccCoefficient](const Real& eccentricity) -> Angle
    {
        return Angle::Radians(asin(eccentricity / eccCoefficient));
    };

    // TBI: this only works because neither set of critical angles are 0/360 degrees
    const auto isCritical = [](const Angle& angle, const Array<Angle>& criticalAngles) -> bool
    {
        const Real epsilon = Angle::Arcseconds(1.0).inRadians();  // TBI: make configurable?

        return (angle.inRadians().isNear(criticalAngles[0].inRadians(), epsilon)) ||
               (angle.inRadians().isNear(criticalAngles[1].inRadians(), epsilon));
    };

    // Use the provided AoP, or default to a critical value
    const Angle aop = anAop.isDefined() ? anAop : criticalAops[0];

    // If AoP matches a critical value
    if (isCritical(aop, criticalAops))
    {
        Angle inclination = Angle::Undefined();
        Real eccentricity = Real::Undefined();

        // If ecc not defined, use the given inclination or default to critical inclination, and calculate ecc
        if (!anEccentricity.isDefined())
        {
            inclination = anInclination.isDefined() ? anInclination : criticalInclinations[0];
            eccentricity = eccentricityFromInclination(inclination);
        }
        // If ecc defined, use it to calculate inc
        else
        {
            // If the eccentricity is larger than this value, the approximation isn't valid
            if (anEccentricity > eccCoefficient)
            {
                throw ostk::core::error::RuntimeError(
                    "Provided eccentricity [" + anEccentricity.toString() + "] cannot be greater than " +
                    eccCoefficient.toString()
                );
            }
            inclination = inclinationFromEccentricity(anEccentricity);
            eccentricity = anEccentricity;
        }

        return {
            aSemiMajorAxis,
            eccentricity,
            inclination,
            aRaan,
            aop,
            aTrueAnomaly,
        };
    }

    // If AoP is (given) and not a critical angle, then the inclination must be critical
    const Angle inclination = anInclination.isDefined() ? anInclination : criticalInclinations[0];
    if (!isCritical(inclination, criticalInclinations))
    {
        throw ostk::core::error::RuntimeError("Provided inclination must be a critical value");
    }

    const Real eccentricity = eccentricityFromInclination(inclination);

    return {
        aSemiMajorAxis,
        eccentricity,
        inclination,
        aRaan,
        aop,
        aTrueAnomaly,
    };
}

Angle COE::EccentricAnomalyFromTrueAnomaly(const Angle& aTrueAnomaly, const Real& anEccentricity)
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

Angle COE::TrueAnomalyFromEccentricAnomaly(const Angle& anEccentricAnomaly, const Real& anEccentricity)
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

Angle COE::MeanAnomalyFromEccentricAnomaly(const Angle& anEccentricAnomaly, const Real& anEccentricity)
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

Angle COE::EccentricAnomalyFromMeanAnomaly(
    const Angle& aMeanAnomaly, const Real& anEccentricity, const Real& aTolerance
)
{
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

Angle COE::TrueAnomalyFromMeanAnomaly(const Angle& aMeanAnomly, const Real& anEccentricity, const Real& aTolerance)
{
    return TrueAnomalyFromEccentricAnomaly(
        EccentricAnomalyFromMeanAnomaly(aMeanAnomly, anEccentricity, aTolerance), anEccentricity
    );
}

Real COE::ComputeSemiLatusRectum(const Real& aSemiMajorAxis, const Real& anEccentricity)
{
    return aSemiMajorAxis * (1.0 - (anEccentricity * anEccentricity));
}

Real COE::ComputeRadialDistance(const Real& aSemiMajorAxis, const Real& anEccentricity, const Real& aTrueAnomaly)
{
    return ComputeSemiLatusRectum(aSemiMajorAxis, anEccentricity) / (1.0 + anEccentricity * std::cos(aTrueAnomaly));
}

Real COE::ComputeAngularMomentum(
    const Real& aSemiMajorAxis, const Real& anEccentricity, const Derived& aGravitationalParameter
)
{
    const Real mu_SI = aGravitationalParameter.in(GravitationalParameterSIUnit);

    return std::sqrt(mu_SI * ComputeSemiLatusRectum(aSemiMajorAxis, anEccentricity));
}

Real COE::ComputeAngularMomentum(const Real& aSemiLatusRectum, const Derived& aGravitationalParameter)
{
    const Real mu_SI = aGravitationalParameter.in(GravitationalParameterSIUnit);

    return std::sqrt(mu_SI * aSemiLatusRectum);
}

Time COE::ComputeMeanLTAN(const Angle& raan, const Instant& anInstant, const Sun& aSun)
{
    // Calculate sun position
    const Vector3d sunDirectionGCRF = aSun.getPositionIn(Frame::GCRF(), anInstant).getCoordinates();

    // Calculate sun apparent local time
    const Angle apparentSolarTime = Angle::Radians(std::atan2(sunDirectionGCRF.y(), sunDirectionGCRF.x()));

    // Get equation of time
    const Angle equationOfTime = COE::ComputeEquationOfTime(anInstant);

    // Compute sun mean local time
    const Angle smlt = apparentSolarTime + equationOfTime;

    // Get angle between sun and ascending node
    const Real alpha = std::fmod((raan - smlt).inRadians(), 2.0 * M_PI);

    // Get Mean LTAN
    const Real meanLTAN = std::fmod((alpha * 12.0 / M_PI) + 12.0, 24.0);

    return Time::Hours(meanLTAN);
}

Time COE::ComputeMeanLTDN(const Angle& raan, const Instant& anInstant, const Sun& aSun)
{
    const Real meanLTAN = COE::ComputeMeanLTAN(raan, anInstant, aSun).getTotalFloatingHours();

    const Real meanLTDN = std::fmod(meanLTAN + 12.0, 24.0);

    return Time::Hours(meanLTDN);
}

Time COE::ComputeLTAN(const Angle& raan, const Instant& anInstant, const Sun& aSun)
{
    // Calculate sun position
    const Vector3d sunDirectionGCRF = aSun.getPositionIn(Frame::GCRF(), anInstant).getCoordinates();

    // Calculate sun apparent local time
    const Angle apparentSolarTime = Angle::Radians(std::atan2(sunDirectionGCRF.y(), sunDirectionGCRF.x()));

    // Get angle between sun and ascending node
    const Real alpha = std::fmod((raan - apparentSolarTime).inRadians(), 2.0 * M_PI);

    // Get LTAN
    const Real LTAN = std::fmod((alpha * 12.0 / M_PI) + 12.0, 24.0);

    return Time::Hours(LTAN);
}

Time COE::ComputeLTDN(const Angle& raan, const Instant& anInstant, const Sun& aSun)
{
    const Real LTAN = COE::ComputeLTAN(raan, anInstant, aSun).getTotalFloatingHours();

    const Real LTDN = std::fmod(LTAN + 12.0, 24.0);

    return Time::Hours(LTDN);
}

Angle COE::ComputeSunSynchronousInclination(
    const Length& aSemiMajorAxis, const Shared<const Celestial>& aCelestialObjectSPtr
)
{
    if (!aSemiMajorAxis.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Semi-major axis");
    }

    if ((aCelestialObjectSPtr == nullptr) || (!aCelestialObjectSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Celestial object");
    }

    /// @ref Capderou M., Handbook of Satellite Orbits: From Kepler to GPS, p.292

    const Real a = aSemiMajorAxis.inMeters();
    const Real R = aCelestialObjectSPtr->getEquatorialRadius().inMeters();
    const Real mu = aCelestialObjectSPtr->getGravitationalParameter().in(GravitationalParameterSIUnit);
    const Real j2 = aCelestialObjectSPtr->getJ2();

    const Real T_sid = 31558149.504;                                                 // [s] Sidereal year
    const Real k_h = 3.0 / (4.0 * M_PI) * j2 * std::sqrt(mu / (R * R * R)) * T_sid;  // Sun-synchronicity constant

    return Angle::Radians(std::acos(-1.0 / k_h * std::pow((a / R), (7.0 / 2.0))));
}

Angle COE::ComputeRaanFromLTAN(const Time& aLocalTimeAtAscendingNode, const Instant& anEpoch, const Sun& aSun)
{
    if (!aLocalTimeAtAscendingNode.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Local time at ascending node");
    }

    if (!anEpoch.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Epoch");
    }

    const Real localTime = (aLocalTimeAtAscendingNode.getHour() / 1.0) +
                           (aLocalTimeAtAscendingNode.getMinute() / 60.0) +
                           (aLocalTimeAtAscendingNode.getSecond() / 3600.0) +
                           (aLocalTimeAtAscendingNode.getMillisecond() / (3600.0 * 1e3)) +
                           (aLocalTimeAtAscendingNode.getMicrosecond() / (3600.0 * 1e6)) +
                           (aLocalTimeAtAscendingNode.getNanosecond() / (3600.0 * 1e9));

    // Sun direction in GCRF
    const Vector3d sunDirection_GCRF = aSun.getPositionIn(Frame::GCRF(), anEpoch).getCoordinates().normalized();

    // Desired angle between the Sun and the ascending node
    const Angle alpha = Angle::Degrees((localTime - 12.0) / 12.0 * 180.0);

    // Sun Apparent Local Time (right ascension of the Sun in GCRF)
    // https://en.wikipedia.org/wiki/Solar_time#Apparent_solar_time
    const Angle apparentSolarTime = Angle::Radians(std::atan2(sunDirection_GCRF.y(), sunDirection_GCRF.x()));

    // Equation of Time
    // https://en.wikipedia.org/wiki/Equation_of_time
    const Angle equationOfTime = COE::ComputeEquationOfTime(anEpoch);

    // Sun Mean Local Time
    // https://en.wikipedia.org/wiki/Solar_time#Mean_solar_time
    const Angle meanSolarTime = apparentSolarTime + equationOfTime;

    // Right Ascension of the Ascending Node
    const Angle raan = Angle::Radians(std::fmod(meanSolarTime.inRadians() + alpha.inRadians(), Real::TwoPi()));

    return raan;
}

COE COE::SunSynchronous(
    const Length& aSemiMajorAxis,
    const Time& aLocalTimeAtAscendingNode,
    const Instant& anEpoch,
    const Shared<const Celestial>& aCelestialObjectSPtr,
    const Angle& anArgumentOfLatitude
)
{
    if (!aSemiMajorAxis.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Semi-major axis");
    }

    if (!aLocalTimeAtAscendingNode.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Local time at ascending node");
    }

    if (!anEpoch.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Epoch");
    }

    if ((aCelestialObjectSPtr == nullptr) || (!aCelestialObjectSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Celestial object");
    }

    if (!anArgumentOfLatitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Argument of latitude");
    }

    const Real eccentricity = 0.0;
    const Angle inclination = COE::ComputeSunSynchronousInclination(aSemiMajorAxis, aCelestialObjectSPtr);
    const Angle raan = COE::ComputeRaanFromLTAN(aLocalTimeAtAscendingNode, anEpoch);
    const Angle aop = Angle::Zero();
    const Angle trueAnomaly = anArgumentOfLatitude - aop;

    return {
        aSemiMajorAxis,
        eccentricity,
        inclination,
        raan,
        aop,
        trueAnomaly,
    };
}

COE COE::GeoSynchronous(
    const Instant& anEpoch,
    const Angle& anInclination,
    const Angle& aLongitude,
    const Shared<const Celestial>& aCelestialObjectSPtr
)
{
    if (!anEpoch.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Epoch");
    }

    if (!anInclination.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Inclination");
    }

    if (!aLongitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Longitude");
    }

    if ((aCelestialObjectSPtr == nullptr) || (!aCelestialObjectSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Celestial object");
    }

    if (aCelestialObjectSPtr->getType() != Celestial::Type::Earth)
    {
        throw ostk::core::error::runtime::ToBeImplemented(
            "Geosynchronous orbits currently not supported for celestial bodies other than Earth"
        );
    }

    // [TBI] Add a way to calculate the sidereal period of each planet to generalize this
    const Length geosynchronousAltitude = Length::Meters(35786000.0);

    // Convert the given longitude (earth referenced) into a usable raan (inertial referenced)
    const LLA lla = {Angle::Zero(), aLongitude, geosynchronousAltitude};
    const Vector3d ascendingNodeVectorITRF =
        lla.toCartesian(aCelestialObjectSPtr->getEquatorialRadius(), aCelestialObjectSPtr->getFlattening());
    const Position ascendingNodePositionITRF = Position::Meters(ascendingNodeVectorITRF, Frame::ITRF());
    const Vector3d ascendingNodeVectorGCRF = ascendingNodePositionITRF.inFrame(Frame::GCRF(), anEpoch).getCoordinates();

    // Define COEs that make up this orbit
    const Length semiMajorAxis = aCelestialObjectSPtr->getEquatorialRadius() + geosynchronousAltitude;
    const Real eccentricity = Real::Epsilon();
    const Angle inclination = anInclination;
    const Angle raan = Angle::Radians(std::atan2(ascendingNodeVectorGCRF[1], ascendingNodeVectorGCRF[0]));
    const Angle aop = Angle::Zero();
    const Angle trueAnomaly = Angle::Zero();

    return {
        semiMajorAxis,
        eccentricity,
        inclination,
        raan,
        aop,
        trueAnomaly,
    };
}

COE COE::Circular(const Length& aSemiMajorAxis, const Angle& anInclination, const Angle& anArgumentOfLatitude)
{
    if (!aSemiMajorAxis.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Semi-major axis");
    }

    if (!anInclination.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Inclination");
    }

    if (!anArgumentOfLatitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Argument of latitude");
    }

    const Real eccentricity = 0.0;
    const Angle raan = Angle::Zero();
    const Angle aop = Angle::Zero();
    const Angle trueAnomaly = anArgumentOfLatitude;

    return {
        aSemiMajorAxis,
        eccentricity,
        anInclination,
        raan,
        aop,
        trueAnomaly,
    };
}

COE COE::Equatorial(const Length& aSemiMajorAxis, const Real& anEccentricity, const Angle& aTrueAnomaly)
{
    if (!aSemiMajorAxis.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Semi-major axis");
    }

    if (!anEccentricity.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Eccentricity");
    }

    if (!aTrueAnomaly.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("True anomaly");
    }

    if (anEccentricity < 0.0 || anEccentricity >= 1.0)
    {
        throw ostk::core::error::RuntimeError("Eccentricity must be in range [0, 1) for elliptical orbits.");
    }

    const Angle inclination = Angle::Zero();
    const Angle raan = Angle::Zero();
    const Angle aop = Angle::Zero();

    return {
        aSemiMajorAxis,
        anEccentricity,
        inclination,
        raan,
        aop,
        aTrueAnomaly,
    };
}

COE COE::CircularEquatorial(const Length& aSemiMajorAxis, const Angle& aTrueAnomaly)
{
    return COE::Circular(aSemiMajorAxis, Angle::Zero(), aTrueAnomaly);
}

String COE::StringFromElement(const COE::Element& anElement)
{
    switch (anElement)
    {
        case COE::Element::SemiMajorAxis:
            return "SemiMajorAxis";
        case COE::Element::Eccentricity:
            return "Eccentricity";
        case COE::Element::Inclination:
            return "Inclination";
        case COE::Element::Aop:
            return "Aop";
        case COE::Element::Raan:
            return "Raan";
        case COE::Element::TrueAnomaly:
            return "TrueAnomaly";
        case COE::Element::MeanAnomaly:
            return "MeanAnomaly";
        case COE::Element::EccentricAnomaly:
            return "EccentricAnomaly";
        case COE::Element::ArgumentOfLatitude:
            return "ArgumentOfLatitude";
    }

    throw ostk::core::error::runtime::Wrong("Element");
}

COE::COE(
    const Length& aSemiMajorAxis,
    const Real& anEccentricity,
    const Angle& anInclination,
    const Angle& aRaan,
    const Angle& anAop,
    const Angle& anAnomaly,
    const AnomalyType& anAnomalyType
)
    : semiMajorAxis_(aSemiMajorAxis),
      eccentricity_(anEccentricity),
      inclination_(anInclination),
      raan_(aRaan),
      aop_(anAop),
      anomaly_(anAnomaly),
      anomalyType_(anAnomalyType)
{
}

Angle COE::ConvertAnomaly(
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

Angle COE::ComputeEquationOfTime(const Instant& anInstant)
{
    // Assume instant is a datetime object
    const Real julianDate = anInstant.getJulianDate(Scale::UTC);

    // Julian Date of J2000.0
    const Real julianDate_J2000 = 2451545.0;

    // Number of Julian centuries from J2000.0
    const Real T_UT1 = (julianDate - julianDate_J2000) / 36525.0;

    // Mean longitude of the Sun
    const Real sunMeanLongitude_deg = std::fmod(280.460 + 36000.771 * T_UT1, 360.0);

    // Mean anomaly of the Sun
    const Real sunMeanAnomaly_rad = Angle::Degrees(std::fmod(357.5291092 + 35999.05034 * T_UT1, 360.0)).inRadians();

    // Ecliptic latitude of the Sun
    const Real sunEclipticLatitude_rad =
        Angle::Degrees(std::fmod(
                           sunMeanLongitude_deg + 1.914666471 * std::sin(sunMeanAnomaly_rad) +
                               0.019994643 * std::sin(2.0 * sunMeanAnomaly_rad),
                           360.0
                       ))
            .inRadians();

    // Compute the equation of time
    const Real equationOfTime_deg =
        -1.914666471 * std::sin(sunMeanAnomaly_rad) - 0.019994643 * std::sin(2.0 * sunMeanAnomaly_rad) +
        2.466 * std::sin(2.0 * sunEclipticLatitude_rad) - 0.0053 * std::sin(4.0 * sunEclipticLatitude_rad);

    return Angle::Degrees(equationOfTime_deg);
}

}  // namespace kepler
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
