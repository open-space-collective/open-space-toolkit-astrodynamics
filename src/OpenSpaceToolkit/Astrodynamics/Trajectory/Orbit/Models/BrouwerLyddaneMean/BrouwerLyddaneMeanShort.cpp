/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>

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
namespace blm
{

using ostk::core::type::Size;
using ostk::core::type::Integer;

using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

BrouwerLyddaneMeanShort::BrouwerLyddaneMeanShort(
    const Length &aSemiMajorAxis,
    const Real &anEccentricity,
    const Angle &anInclination,
    const Angle &aRaan,
    const Angle &anAop,
    const Angle &aMeanAnomaly
)
    : BrouwerLyddaneMean(aSemiMajorAxis, anEccentricity, anInclination, aRaan, anAop, aMeanAnomaly)
{
}

COE BrouwerLyddaneMeanShort::toCOE() const
{
    // ref:
    // https://github.com/ChristopherRabotin/GMAT/blob/37201a6290e7f7b941bc98ee973a527a5857104b/src/base/util/StateConversionUtil.cpp#L3679

    const auto mod = [](const Real &value, const Real &bound) -> Real
    {
        return value - bound * std::floor(value / bound);
    };

    // TBM: Make configurable?
    const Real equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();

    Integer pseudoState = 0;
    bool possibleInaccuracyLongWritten = false;
    bool brouwerSmallEccentricityWritten = false;

    const Real re = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();
    const Real j2 = 1.082626925638815E-03;
    const Real ae = 1.0;
    Real smap = semiMajorAxis_.inMeters() / re;
    Real eccp = eccentricity_;
    Real incp = inclination_.inRadians();
    Real raanp = raan_.inRadians(0.0, Real::TwoPi());
    Real aopp = aop_.inRadians(0.0, Real::TwoPi());
    Real meanAnomalyp = anomaly_.inRadians(0.0, Real::TwoPi());

    if ((incp < 0.0) || (incp > Real::Pi()))
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMeanShort is applicable only if 0.0 < inclination: [{}] < 180.0 degrees.",
            Angle::Radians(incp).inDegrees()
        );
    }

    if (incp > 3.0543261909900763)
    {
        incp = Real::Pi() - incp;
        raanp = -raanp;
        pseudoState = 1;
    }

    // negative eccentricity aviodance lines
    if (eccp < 0.0)
    {
        eccp *= -1.0;
        meanAnomalyp -= Real::TwoPi() / 2.0;
        aopp += +Real::TwoPi() / 2.0;
        if (!brouwerSmallEccentricityWritten)
        {
            std::cout << "Warning: Because eccentricity is smaller than 0.0, "
                         "the current apoapsis will be taken to be new periapsis."
                      << std::endl;
            brouwerSmallEccentricityWritten = true;
        }
    }

    if (eccp > 0.99)
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMeanShort is applicable only if 0.0 < Ecc: [{}] < 0.99.", eccp
        );
    }

    const Real perigee = this->getPeriapsisRadius().inMeters();
    if (perigee < 3000000.0)
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMeanShort is applicable only if Perigee radius: [{}] > 3000000.0 m.", perigee
        );
    }
    if (perigee < 6378000.0)
    {
        if (!possibleInaccuracyLongWritten)
        {
            std::cout << "Warning: For BrouwerLyddaneMeanShort, when RadPer < 6378km, there "
                         "is a possible inaccuracy due to singularity related "
                         "with inside-of-earth orbit.\n";
            possibleInaccuracyLongWritten = true;
        }
    }

    const Real eta = std::sqrt(1.0 - std::pow(eccp, 2.0));
    const Real theta = std::cos(incp);
    const Real p = smap * std::pow(eta, 2.0);
    const Real k2 = 1.0 / 2.0 * j2 * ae * ae;
    const Real gm2 = k2 / std::pow(smap, 2.0);
    const Real gm2p = gm2 / std::pow(eta, 4.0);

    //-------------------------------------I
    // COMPUTE TRUE ANOMALY(DOUBLE PRIMED) I
    //-------------------------------------I
    const Real tap = COE::TrueAnomalyFromMeanAnomaly(anomaly_, eccentricity_, 1e-15).inRadians(0.0, Real::TwoPi());

    const Real rp = p / (1.0 + eccp * std::cos(tap));
    const Real adr = smap / rp;

    Real sma = smap + smap * gm2 *
                          ((std::pow(adr, 3.0) - 1.0 / std::pow(eta, 3.0)) * (-1.0 + 3.0 * std::pow(theta, 2.0)) +
                           3.0 * (1.0 - std::pow(theta, 2.0)) * std::pow(adr, 3.0) * std::cos(2.0 * aopp + 2.0 * tap));

    const Real decc =
        std::pow(eta, 2.0) / 2.0 *
        ((3.0 * (1.0 / std::pow(eta, 6.0)) * gm2 * (1.0 - std::pow(theta, 2.0)) * std::cos(2.0 * aopp + 2.0 * tap) *
          (3.0 * eccp * std::pow(std::cos(tap), 2.0) + 3.0 * std::cos(tap) +
           std::pow(eccp, 2.0) * std::pow(std::cos(tap), 3.0) + eccp)) -
         gm2p * (1.0 - std::pow(theta, 2.0)) * (3.0 * std::cos(2.0 * aopp + tap) + std::cos(3.0 * tap + 2.0 * aopp)) +
         (3.0 * std::pow(theta, 2.0) - 1.0) * gm2 / std::pow(eta, 6.0) *
             (eccp * eta + eccp / (1.0 + eta) + 3.0 * eccp * std::pow(std::cos(tap), 2.0) + 3.0 * std::cos(tap) +
              std::pow(eccp, 2.0) * std::pow(std::cos(tap), 3.0)));

    const Real dinc = gm2p / 2.0 * theta * std::sin(incp) *
                      (3.0 * std::cos(2.0 * aopp + 2.0 * tap) + 3.0 * eccp * std::cos(2.0 * aopp + tap) +
                       eccp * std::cos(2.0 * aopp + 3.0 * tap));

    const Real draan = -gm2p / 2.0 * theta *
                       (6.0 * (tap - meanAnomalyp + eccp * std::sin(tap)) - 3.0 * std::sin(2.0 * aopp + 2.0 * tap) -
                        3.0 * eccp * std::sin(2.0 * aopp + tap) - eccp * std::sin(2.0 * aopp + 3.0 * tap));

    Real aop =
        aopp + 3.0 * j2 / 2.0 / std::pow(p, 2.0) *
                   ((2.0 - 5.0 / 2.0 * std::pow(std::sin(incp), 2.0)) * (tap - meanAnomalyp + eccp * std::sin(tap)) +
                    (1.0 - 3.0 / 2.0 * std::pow(std::sin(incp), 2.0)) *
                        (1.0 / eccp * (1.0 - 1.0 / 4.0 * std::pow(eccp, 2.0)) * std::sin(tap) +
                         1.0 / 2.0 * std::sin(2.0 * tap) + eccp / 12.0 * std::sin(3.0 * tap)) -
                    1.0 / eccp *
                        (1.0 / 4.0 * std::pow(std::sin(incp), 2.0) +
                         (1.0 / 2.0 - 15.0 / 16.0 * std::pow(std::sin(incp), 2.0)) * std::pow(eccp, 2.0)) *
                        std::sin(tap + 2.0 * aopp) +
                    eccp / 16.0 * std::pow(std::sin(incp), 2.0) * std::sin(tap - 2.0 * aopp) -
                    1.0 / 2.0 * (1.0 - 5.0 / 2.0 * std::pow(std::sin(incp), 2.0)) * std::sin(2.0 * tap + 2.0 * aopp) +
                    1.0 / eccp *
                        (7.0 / 12.0 * std::pow(std::sin(incp), 2.0) -
                         1.0 / 6.0 * (1.0 - 19.0 / 8.0 * std::pow(std::sin(incp), 2.0)) * std::pow(eccp, 2.0)) *
                        std::sin(3.0 * tap + 2.0 * aopp) +
                    3.0 / 8.0 * std::pow(std::sin(incp), 2.0) * std::sin(4.0 * tap + 2.0 * aopp) +
                    eccp / 16.0 * std::pow(std::sin(incp), 2.0) * std::sin(5.0 * tap + 2.0 * aopp));

    Real meanAnomaly =
        meanAnomalyp + 3.0 * j2 * eta / 2.0 / eccp / std::pow(p, 2) *
                           (-(1.0 - 3.0 / 2.0 * std::pow(std::sin(incp), 2.0)) *
                                ((1.0 - std::pow(eccp, 2.0) / 4.0) * std::sin(tap) + eccp / 2.0 * std::sin(2.0 * tap) +
                                 std::pow(eccp, 2.0) / 12.0 * std::sin(3.0 * tap)) +
                            std::pow(std::sin(incp), 2.0) *
                                (1.0 / 4.0 * (1.0 + 5.0 / 4.0 * std::pow(eccp, 2.0)) * std::sin(tap + 2.0 * aopp) -
                                 std::pow(eccp, 2.0) / 16.0 * std::sin(tap - 2.0 * aopp) -
                                 7.0 / 12.0 * (1.0 - std::pow(eccp, 2.0) / 28.0) * std::sin(3.0 * tap + 2.0 * aopp) -
                                 3.0 * eccp / 8.0 * std::sin(4.0 * tap + 2.0 * aopp) -
                                 std::pow(eccp, 2.0) / 16.0 * std::sin(5.0 * tap + 2.0 * aopp)));

    const Real lgh =
        raanp + aopp + meanAnomalyp +
        gm2p / 4.0 *
            (6.0 * (-1.0 - 2.0 * theta + 5.0 * std::pow(theta, 2.0)) * (tap - meanAnomalyp + eccp * std::sin(tap)) +
             (3.0 + 2.0 * theta - 5.0 * std::pow(theta, 2.0)) *
                 (3.0 * std::sin(2.0 * aopp + 2.0 * tap) + 3.0 * eccp * std::sin(2.0 * aopp + tap) +
                  eccp * std::sin(2.0 * aopp + 3.0 * tap))) +
        gm2p / 4.0 * std::pow(eta, 2.0) / (eta + 1.0) * eccp *
            (3.0 * (1.0 - std::pow(theta, 2.0)) *
                 (std::sin(3.0 * tap + 2.0 * aopp) * (1.0 / 3.0 + std::pow(adr, 2.0) * std::pow(eta, 2.0) + adr) +
                  std::sin(2.0 * aopp + tap) * (1.0 - std::pow(adr, 2.0) * std::pow(eta, 2.0) - adr)) +
             2.0 * std::sin(tap) * (3.0 * std::pow(theta, 2.0) - 1.0) *
                 (1.0 + std::pow(adr, 2.0) * std::pow(eta, 2.0) + adr));

    const Real eccpdl =
        -std::pow(eta, 3.0) / 4.0 * gm2p *
        (2.0 * (-1.0 + 3.0 * std::pow(theta, 2.0)) * (std::pow(adr, 2.0) * std::pow(eta, 2.0) + adr + 1.0) *
             std::sin(tap) +
         3.0 * (1.0 - std::pow(theta, 2.0)) *
             ((-std::pow(adr, 2.0) * std::pow(eta, 2.0) - adr + 1.0) * std::sin(2.0 * aopp + tap) +
              (std::pow(adr, 2.0) * std::pow(eta, 2.0) + adr + 1.0 / 3.0) * std::sin(2.0 * aopp + 3.0 * tap)));
    const Real ecosl = (eccp + decc) * std::cos(meanAnomalyp) - eccpdl * std::sin(meanAnomalyp);
    const Real esinl = (eccp + decc) * std::sin(meanAnomalyp) + eccpdl * std::cos(meanAnomalyp);
    const Real ecc1 = std::sqrt(std::pow(ecosl, 2.0) + std::pow(esinl, 2.0));

    if (ecc1 < 1.0E-11)
    {
        meanAnomaly = 0.0;
    }
    else
    {
        meanAnomaly = std::atan2(esinl, ecosl);
        if (meanAnomaly < 0.0)
        {
            meanAnomaly += Real::TwoPi();
        }
    }

    const Real sinhalfisinh = (std::sin(0.5 * incp) + std::cos(0.5 * incp) * 0.5 * dinc) * std::sin(raanp) +
                              1.0 / 2.0 * std::sin(incp) / std::cos(incp / 2.0) * draan * std::cos(raanp);
    const Real sinhalficosh = (std::sin(0.5 * incp) + std::cos(0.5 * incp) * 0.5 * dinc) * std::cos(raanp) -
                              1.0 / 2.0 * std::sin(incp) / std::cos(incp / 2.0) * draan * std::sin(raanp);

    Real inc = 2.0 * std::asin(std::sqrt(std::pow(sinhalfisinh, 2.0) + std::pow(sinhalficosh, 2.0)));
    if (std::sqrt(std::pow(sinhalfisinh, 2.0) + std::pow(sinhalficosh, 2.0)) > 1.0)
    {
        inc = 2.0 * std::asin(1);
    }
    else if (std::sqrt(std::pow(sinhalfisinh, 2.0) + std::pow(sinhalficosh, 2.0)) < -1.0)
    {
        inc = 2.0 * std::asin(-1.0);
    }

    Real raan = 0.0;
    if ((inc == 0.0) || (inc == Real::Pi()))
    {
        aop = lgh - meanAnomaly - raan;
    }
    else
    {
        raan = std::atan2(sinhalfisinh, sinhalficosh);
        if (raan < 0.0)
        {
            raan = raan + Real::TwoPi();
        }
        aop = lgh - meanAnomaly - raan;
    }
    if (raan < 0.0)
    {
        raan += Real::TwoPi();
    }

    aop = mod(aop, Real::TwoPi());
    if (aop < 0.0)
    {
        aop += Real::TwoPi();
    }

    return COE::FromSIVector(
        {
            sma * equatorialRadius,
            ecc1,
            inc,
            raan,
            aop,
            meanAnomaly,
        },
        COE::AnomalyType::Mean
    );
}

BrouwerLyddaneMeanShort BrouwerLyddaneMeanShort::COE(const classicalOE &aCOE)
{
    if (!aCOE.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    const classicalOE::CartesianState cartesianState =
        aCOE.getCartesianState(EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF());

    return BrouwerLyddaneMeanShort::Cartesian(cartesianState, EarthGravitationalModel::EGM2008.gravitationalParameter_);
}

BrouwerLyddaneMeanShort BrouwerLyddaneMeanShort::Cartesian(
    const COE::CartesianState &aCartesianState, const Derived &aGravitationalParameter
)
{
    const auto toCOEVector = [](const Vector6d &aVector) -> Vector6d
    {
        return BrouwerLyddaneMeanShort::FromSIVector(aVector).toCOE().getSIVector(COE::AnomalyType::Mean);
    };

    return BrouwerLyddaneMeanShort::FromSIVector(
        BrouwerLyddaneMean::Cartesian(aCartesianState, aGravitationalParameter, toCOEVector)
    );
}

BrouwerLyddaneMeanShort BrouwerLyddaneMeanShort::Undefined()
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

BrouwerLyddaneMeanShort BrouwerLyddaneMeanShort::FromSIVector(const Vector6d &aVector)
{
    return {
        Length::Meters(aVector[0]),
        aVector[1],
        Angle::Radians(aVector[2]),
        Angle::Radians(aVector[3]),
        Angle::Radians(aVector[4]),
        Angle::Radians(aVector[5]),
    };
}

}  // namespace blm
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
