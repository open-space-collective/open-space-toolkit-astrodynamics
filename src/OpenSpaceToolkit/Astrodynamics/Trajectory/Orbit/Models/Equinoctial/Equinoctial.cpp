/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Equinoctial/Equinoctial.hpp>

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
namespace equinoctial
{

using ostk::core::types::Size;
using ostk::core::types::Integer;
using ostk::core::types::Real;

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;
using ostk::physics::units::Angle;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::math::object::Vector3d;

static const Real Tolerance = 1e-30;
static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

Equinoctial::Equinoctial(
    const Length& aSemiMajorAxis,
    const Real& aH,
    const Real& aK,
    const Real& aP,
    const Real& aQ,
    const Angle& aMeanLongitude
)
    : semiMajorAxis_(aSemiMajorAxis),
      h_(aH),
      k_(aK),
      p_(aP),
      q_(aQ),
      longitude_(aMeanLongitude),
      longitudeType_(Equinoctial::LongitudeType::Mean)
{
}

bool Equinoctial::operator==(const Equinoctial& anEquinoctial) const
{
    if ((!this->isDefined()) || (!anEquinoctial.isDefined()))
    {
        return false;
    }

    return (semiMajorAxis_ == anEquinoctial.semiMajorAxis_) && (h_ == anEquinoctial.h_) && (k_ == anEquinoctial.k_) &&
           (p_ == anEquinoctial.q_) && (p_ == anEquinoctial.q_) && (longitude_ == anEquinoctial.longitude_) &&
           (longitudeType_ == anEquinoctial.longitudeType_);
}

bool Equinoctial::operator!=(const Equinoctial& anEquinoctial) const
{
    return !((*this) == anEquinoctial);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Equinoctial& anEquinoctial)
{
    anEquinoctial.print(anOutputStream);

    return anOutputStream;
}

bool Equinoctial::isDefined() const
{
    return semiMajorAxis_.isDefined() && h_.isDefined() && k_.isDefined() && p_.isDefined() && q_.isDefined() &&
           longitude_.isDefined();
}

Length Equinoctial::getSemiMajorAxis() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Equinoctial");
    }

    return semiMajorAxis_;
}

Real Equinoctial::getH() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Equinoctial");
    }

    return h_;
}

Real Equinoctial::getK() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Equinoctial");
    }

    return k_;
}

Real Equinoctial::getP() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Equinoctial");
    }

    return p_;
}

Real Equinoctial::getQ() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Equinoctial");
    }

    return q_;
}

// Angle Equinoctial::getTrueLongitude() const
// {
//     if (!this->isDefined())
//     {
//         throw ostk::core::error::runtime::Undefined("Equinoctial");
//     }

//     return longitude_; // TBI: To be converted
// }

Angle Equinoctial::getMeanLongitude() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Equinoctial");
    }

    return longitude_;
}

Angle Equinoctial::getEccentricLongitude() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Equinoctial");
    }

    return Equinoctial::EccentricLongitudeFromMeanLongitude(longitude_, h_, k_, 1e-12);
}

Vector6d Equinoctial::getSIVector(const Equinoctial::LongitudeType& aLongitudeType) const
{
    return {
        semiMajorAxis_.inMeters(),
        h_,
        k_,
        p_,
        q_,
        Equinoctial::ConvertLongitude(longitude_, h_, k_, LongitudeType::Mean, aLongitudeType, 1e-12).inRadians()
    };
}

void Equinoctial::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::types::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Equinoctial Orbital Elements") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Semi-major axis:"
        << (semiMajorAxis_.isDefined() ? String::Format("{} [m]", semiMajorAxis_.inMeters().toString()) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "h:" << (h_.isDefined() ? h_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "k:" << (k_.isDefined() ? k_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "p:" << (p_.isDefined() ? p_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "q:" << (q_.isDefined() ? q_.toString() : "Undefined");
    String longitudeType = String::Empty();
    switch (longitudeType_)
    {
        case Equinoctial::LongitudeType::Mean:
            longitudeType = "Mean";
            break;
        case Equinoctial::LongitudeType::Eccentric:
            longitudeType = "Eccentric";
            break;
        default:
            throw ostk::core::error::runtime::Wrong("Longitude type");
    }
    ostk::core::utils::Print::Line(anOutputStream)
        << String::Format("{} longitude:", longitudeType)
        << (longitude_.isDefined() ? String::Format("{} [deg]", longitude_.inDegrees(0.0, 360.0).toString())
                                   : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Equinoctial Equinoctial::Undefined()
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

Equinoctial Equinoctial::Cartesian(
    const classicalOE::CartesianState& aCartesianState, const Derived& aGravitationalParameter
)
{
    if ((!aCartesianState.first.isDefined()) || (!aCartesianState.second.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Cartesian state");
    }

    if (!aGravitationalParameter.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Gravitational parameter");
    }

    const Real mu = aGravitationalParameter.in(GravitationalParameterSIUnit);

    if (mu == 0.0)
    {
        throw ostk::core::error::runtime::Wrong("Gravitational parameter");
    }

    const Vector3d positionVector = aCartesianState.first.accessCoordinates();
    const Vector3d velocityVector = aCartesianState.second.accessCoordinates();
    const Vector3d angularMomentumVector = positionVector.cross(velocityVector);

    const Real position = positionVector.norm();
    const Real velocity = velocityVector.norm();
    const Real angularMomentum = angularMomentumVector.norm();

    const Real sma = 1.0 / ((2.0 / position) - (velocity * velocity / mu));
    const Vector3d eccentricityVector =
        -(positionVector / position) - ((angularMomentumVector).cross(velocityVector) / mu);
    const Vector3d w = angularMomentumVector / angularMomentum;

    const Real p = w[0] / (1.0 + w[2]);
    const Real q = w[1] / (1.0 + w[2]);

    const Real denom = 1.0 + p * p + q * q;
    const Vector3d g = {2.0 * p * q / denom, (1.0 + p * p - q * q) / denom, (-2.0 * p) / denom};
    const Vector3d f = g.cross(w);

    const Real h = eccentricityVector.dot(g);
    const Real k = eccentricityVector.dot(f);

    const Real alpha = sqrt(1.0 - h * h - k * k);
    const Real beta = 1.0 / (1.0 + alpha);
    const Real X1 = positionVector.dot(f);
    const Real Y1 = positionVector.dot(g);
    const Real cosF = k + ((1.0 - k * k * beta) * X1 - h * k * beta * Y1) / (sma * alpha);
    const Real sinF = h + ((1.0 - h * h * beta) * Y1 - h * k * beta * X1) / (sma * alpha);
    const Real eccentricLongitude_rad = atan2(sinF, cosF);
    const Real meanLongitude_rad = eccentricLongitude_rad - k * sinF + h * cosF;

    return Equinoctial(Length::Meters(sma), h, k, p, q, Angle::Radians(meanLongitude_rad));
}

classicalOE Equinoctial::toCOE() const
{
    // Compute eccentricity
    const Real ecc = sqrt(this->h_ * this->h_ + this->k_ * this->k_);

    // Compute inclination
    const Real inc = 2.0 * atan(sqrt(this->p_ * this->p_ + this->q_ * this->q_));

    // Compute right ascension of the ascending node
    const Real raan = atan2(this->p_, this->q_);

    // Compute argument of periapsis
    const Real aop = atan2(this->h_, this->k_) - atan2(this->p_, this->q_);

    // Compute mean anomaly
    const Real ma = this->longitude_.inRadians() - aop - raan;

    // Convert to eccentric anomaly
    const Angle trueAnomaly = classicalOE::TrueAnomalyFromMeanAnomaly(Angle::Radians(ma), ecc, 1e-12);

    return classicalOE(
        this->semiMajorAxis_, ecc, Angle::Radians(inc), Angle::Radians(raan), Angle::Radians(aop), trueAnomaly
    );
}

Equinoctial Equinoctial::COE(const classicalOE& aCOE)
{
    if (!aCOE.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    const classicalOE::CartesianState cartesianState =
        aCOE.getCartesianState(EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF());

    return Equinoctial::Cartesian(cartesianState, EarthGravitationalModel::EGM2008.gravitationalParameter_);
}

Equinoctial Equinoctial::FromSIVector(const Vector6d& aVector)
{
    return {
        Length::Meters(aVector[0]),
        aVector[1],
        aVector[2],
        aVector[3],
        aVector[4],
        Angle::Radians(aVector[5]),
    };
}

Angle Equinoctial::MeanLongitudeFromEccentricLongitude(
    const Angle& anEccentricLongitude, const Real& aH, const Real& aK
)
{
    if (!anEccentricLongitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Eccentric longitude");
    }

    if (!aH.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("h");
    }

    if (!aK.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("k");
    }

    const Real eccentricLongitude_rad = anEccentricLongitude.inRadians();
    const Real meanLongitude_rad =
        eccentricLongitude_rad - aK * std::sin(eccentricLongitude_rad) + aH * std::cos(eccentricLongitude_rad);

    return Angle::Radians(Angle::Radians(meanLongitude_rad).inRadians(0.0, Real::TwoPi()));
}

Angle Equinoctial::EccentricLongitudeFromMeanLongitude(
    const Angle& aMeanLongitude, const Real& aH, const Real& aK, const Real& aTolerance
)
{
    if (!aMeanLongitude.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Mean anomaly");
    }

    if (!aH.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("h");
    }

    if (!aK.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("k");
    }

    if (!aTolerance.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tolerance");
    }

    // Define the function and its derivative
    auto g = [](double F, double k, double h, double lambda) -> double
    {
        return F - k * sin(F) + h * cos(F) - lambda;
    };

    auto gDerivative = [](double F, double k, double h) -> double
    {
        return 1.0 - k * cos(F) - h * sin(F);
    };

    // Newton's method function to find F
    auto newtonMethod = [g, gDerivative](
                            double k, double h, double lambda, double initialGuess, double tolerance, int maxIterations
                        ) -> double
    {
        double F = initialGuess;

        for (int i = 0; i < maxIterations; ++i)
        {
            double value = g(F, k, h, lambda);
            double slope = gDerivative(F, k, h);

            F = F - value / slope;

            if (std::abs(value) < tolerance)
            {
                std::cout << "Converged after " << i + 1 << " iterations." << std::endl;
                return F;
            }
        }

        std::cerr << "Newton's method did not converge within the specified number of iterations." << std::endl;
        return std::numeric_limits<double>::quiet_NaN();  // Return NaN for failure
    };

    const Real meanLongitude_rad = aMeanLongitude.inRadians();

    // Use initial guess
    const Real initialGuess = meanLongitude_rad - aK * std::sin(meanLongitude_rad) + aH * std::cos(meanLongitude_rad);

    // Call Newton's method function
    int maxIterations = 1000;
    double F = newtonMethod(aK, aH, meanLongitude_rad, initialGuess, aTolerance, maxIterations);

    return Angle::Radians(F);
}

Angle Equinoctial::ConvertLongitude(
    const Angle& aLongitude,
    const Real& aH,
    const Real& aK,
    const LongitudeType& fromLongitudeType,
    const LongitudeType& toLongitudeType,
    const Real& aTolerance
)
{
    switch (fromLongitudeType)
    {
        case (LongitudeType::Eccentric):
        {
            switch (toLongitudeType)
            {
                case (LongitudeType::Eccentric):
                    return aLongitude;
                case (LongitudeType::Mean):
                    return MeanLongitudeFromEccentricLongitude(aLongitude, aH, aK);
                default:
                    throw ostk::core::error::runtime::Wrong("To Longitude type");
            }
        }

        case (LongitudeType::Mean):
        {
            switch (toLongitudeType)
            {
                case (LongitudeType::Eccentric):
                    return EccentricLongitudeFromMeanLongitude(aLongitude, aH, aK, aTolerance);
                case (LongitudeType::Mean):
                    return aLongitude;
                default:
                    throw ostk::core::error::runtime::Wrong("To Longitude type");
            }
        }

        default:
            throw ostk::core::error::runtime::Wrong("From Longitude type");
    }
}

}  // namespace equinoctial
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
