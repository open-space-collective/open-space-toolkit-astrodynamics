/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/BMLOE.hpp>

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

using ostk::core::types::Size;
using ostk::core::types::Integer;

using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

static const Real Tolerance = 1e-12;

BMLOE::BMLOE(
    const Length &aSemiMajorAxis,
    const Real &anEccentricity,
    const Angle &anInclination,
    const Angle &aRaan,
    const Angle &anAop,
    const Angle &aMeanAnomly
)
    : semiMajorAxis_(aSemiMajorAxis),
      eccentricity_(anEccentricity),
      inclination_(anInclination),
      raan_(aRaan),
      aop_(anAop),
      meanAnomaly_(aMeanAnomly)
{
}

Vector6d BMLOE::asVector() const
{
    return {
        semiMajorAxis_.inMeters(),
        eccentricity_,
        inclination_.inRadians(),
        raan_.inRadians(),
        aop_.inRadians(),
        meanAnomaly_.inRadians(),
    };
}

// BMLOE BMLOE::Cartesian(const COE::CartesianState &aCartesianState, const Derived &aGravitationalParameter)
// {
//     const COE coe = COE::Cartesian(aCartesianState, aGravitationalParameter);

//     Vector6d cartesian;
//     cartesian.segment(0, 3) = aCartesianState.first.accessCoordinates();
//     cartesian.segment(3, 3) = aCartesianState.second.accessCoordinates();

//     const Real perigee = coe.getPeriapsisRadius().inMeters();

//     if (perigee < 3000000.0)
//     {
//         throw ostk::core::error::runtime::Wrong("Perigee must be larger than 3000.0 km.");
//     }

//     if (coe.getInclination().inDegrees() > 180.0)
//     {
//         throw ostk::core::error::runtime::Wrong("Inclination must be lesser than 180.0 degrees.");
//     }

//     const auto getAeq = [](const Vector6d &aCOEVector) -> Vector6d
//     {
//         return {
//             aCOEVector[0],
//             aCOEVector[1] * std::sin(aCOEVector[3] + aCOEVector[4]),
//             aCOEVector[1] * std::cos(aCOEVector[3] + aCOEVector[4]),
//             std::sin(aCOEVector[2] / 2.0) * std::sin(aCOEVector[3]),
//             std::sin(aCOEVector[2] / 2.0) * std::cos(aCOEVector[3]),
//             aCOEVector[2] + aCOEVector[3] + aCOEVector[4],
//         };
//     };

//     const auto getBLMeanFromAeq = [](const Vector6d &anAeqVector) -> Vector6d
//     {
//         Real meanInc = Real::Undefined();

//         if ((anAeqVector[3] * anAeqVector[3] + anAeqVector[4] * anAeqVector[4]) <= 1.0)
//         {
//             meanInc = std::acos(1.0 - 2.0 * (anAeqVector[3] * anAeqVector[3] + anAeqVector[4] * anAeqVector[4]));
//         }
//         else
//         {
//             meanInc = std::acos(1.0 - 2.0 * 1.0);
//         }

//         Real meanRaan = std::atan2(anAeqVector[3], anAeqVector[4]);
//         if (meanRaan < 0.0)
//         {
//             meanRaan = meanRaan + Real::TwoPi();
//         }

//         return {
//             anAeqVector[0],
//             std::sqrt(anAeqVector[1] * anAeqVector[1] + anAeqVector[2] * anAeqVector[2]),
//             meanInc,
//             meanRaan,
//             std::atan2(anAeqVector[1], anAeqVector[2]) - meanRaan,
//             anAeqVector[5] - std::atan2(anAeqVector[1], anAeqVector[2]),
//         };
//     };

//     Vector6d coeVector = coe.asVector(COE::AnomalyType::Mean);
//     std::cout << coeVector << std::endl;

//     Vector6d aeq = getAeq(coeVector);

//     Vector6d coeVector2 = BMLOE::FromVector(coeVector).toCOE().asVector(COE::AnomalyType::Mean);
//     std::cout << coeVector2 << std::endl;

//     Vector6d aeq2 = getAeq(coeVector2);

//     Vector6d aeqMean = getAeq(coeVector);

//     Vector6d aeqMean2 = aeqMean + (aeq - aeq2);

//     Real emag = 0.9;
//     Real emag_old = 10.0;
//     Size iter = 0;
//     Vector6d tmp;
//     Vector6d cartesian2;
//     Vector6d blmean2;

//     const Real cartesianNorm = cartesian.norm();

//     while (emag > 1e-8)
//     {
//         blmean2 = getBLMeanFromAeq(aeqMean2);

//         coeVector2 = BMLOE::FromVector(blmean2).toCOE().asVector(COE::AnomalyType::Mean);
//         std::cout << "blmean2: " << blmean2 << std::endl;
//         std::cout << "coeVector2: " << coeVector2 << std::endl;
//         const COE::CartesianState cartesianState = COE::FromVector(coeVector2, COE::AnomalyType::Mean)
//                                                        .getCartesianState(aGravitationalParameter, Frame::GCRF());
//         cartesian2.segment(0, 3) = cartesianState.first.accessCoordinates();
//         cartesian2.segment(3, 3) = cartesianState.second.accessCoordinates();

//         tmp = cartesian - cartesian2;
//         emag = tmp.norm() / cartesianNorm;

//         std::cout << "cartesian: " << cartesian << std::endl;
//         std::cout << "cartesian2: " << cartesian2 << std::endl;
//         std::cout << "tmp: " << tmp << std::endl;
//         std::cout << "norms: " << tmp.norm() << " " << cartesian.norm() << std::endl;
//         std::cout << "emag: " << emag << " " << emag_old << std::endl;

//         if (emag_old > emag)
//         {
//             emag_old = emag;
//             // blmean	= blmean2;

//             // blmean2= blmean + (kep - kep2);
//             aeq2 = getAeq(coeVector2);
//             aeqMean = aeqMean2;
//             aeqMean2 = aeqMean + (aeq - aeq2);
//         }
//         else
//         {
//             throw ostk::core::error::RuntimeError("Brouwer mean long algorithm cannot converge due to emag.");
//         }
//         if (iter > 75)
//         {
//             throw ostk::core::error::RuntimeError(
//                 "Iteration limit reached. Brouwer mean long algorithm cannot converge."
//             );
//         }
//         ++iter;
//     }

//     blmean2 = getBLMeanFromAeq(aeqMean);

//     // if (pseudostate != 0)
//     // {
//     //     blmean2[2] = 180.0 - blmean2[2];
//     //     blmean2[3] = -blmean2[3];
//     // }

//     blmean2[3] = std::fmod(blmean2[3], Real::TwoPi());
//     blmean2[4] = std::fmod(blmean2[4], Real::TwoPi());
//     blmean2[5] = std::fmod(blmean2[5], Real::TwoPi());
//     if (blmean2[3] < 0.0)
//     {
//         blmean2[3] = blmean2[3] + Real::TwoPi();
//     }
//     if (blmean2[4] < 0.0)
//     {
//         blmean2[4] = blmean2[4] + Real::TwoPi();
//     }
//     if (blmean2[5] < 0.0)
//     {
//         blmean2[5] = blmean2[5] + Real::TwoPi();
//     }

//     return {
//         Length::Meters(blmean2[0]),
//         blmean2[1],
//         Angle::Radians(blmean2[2]),
//         Angle::Radians(blmean2[3]),
//         Angle::Radians(blmean2[4]),
//         Angle::Radians(blmean2[5]),
//     };
// }

BMLOE BMLOE::Cartesian(const COE::CartesianState &aCartesianState, const Derived &aGravitationalParameter)
{
    Real tol = 1.0E-8;
    Integer maxiter = 75;

    const COE coe = COE::Cartesian(aCartesianState, aGravitationalParameter);

    Vector6d cartesian;
    cartesian.segment(0, 3) = aCartesianState.first.accessCoordinates();
    cartesian.segment(3, 3) = aCartesianState.second.accessCoordinates();

    bool possibleInaccuracyLongWritten = false;
    bool inaccuracyCriticalAngleWritten = false;

    Vector6d kep = coe.asVector(COE::AnomalyType::Mean);
    std::cout.precision(16);
    std::cout << kep << std::endl;

    if ((kep[1] > 0.99) || (kep[1] < 0.0))
    {
        std::stringstream errmsg("");
        errmsg.precision(21);
        errmsg << " While converting from the Cartesian to";
        errmsg << " the BrouwerMeanLong, an error has been encountered.";
        errmsg << " BrouwerMeanLong is applicable";
        errmsg << " only if 0.0 < ECC < 0.99." << std::endl;
        throw ostk::core::error::RuntimeError(errmsg.str());
    }
    Real radper = (kep[0] * (1.0 - kep[1]));
    if (radper < 3000.0)
    {
        std::stringstream errmsg("");
        errmsg.precision(21);
        errmsg << " While converting from the Cartesian to";
        errmsg << " the BrouwerMeanLong, an error has been encountered.";
        errmsg << " BrouwerMeanLong is applicable";
        errmsg << " only if RadPer is larger than  3000km." << std::endl;
        throw ostk::core::error::RuntimeError(errmsg.str());
    }
    if (radper < 6378.0)
    {
        if (!possibleInaccuracyLongWritten)
        {
            std::cout << "Warning: For BrouwerMeanLong, when RadPer < 6378km, there is a "
                         "possible inaccuracy due to singularity "
                         "related with inside-of-earth orbit.\n";
            possibleInaccuracyLongWritten = true;
        }
    }
    if (kep[2] > Real::Pi())
    {
        std::stringstream errmsg("");
        errmsg.precision(21);
        errmsg << " While converting from the Cartesian to";
        errmsg << " the BrouwerMeanLong, an error has been encountered.";
        errmsg << " BrouwerMeanLong is applicable";
        errmsg << " only if inclination is smaller than 180 DEG." << std::endl;
        throw ostk::core::error::RuntimeError(errmsg.str());
    }

    if ((58.80 < kep[2] && kep[2] < 65.78) || (114.22 < kep[2] && kep[2] < 121.2))
    {
        if (!inaccuracyCriticalAngleWritten)
        {
            std::cout << "Warning: For BrouwserMeanLong, when 58.80 DEG < "
                         "INC < 65.78 DEG, or 114.22 DEG "
                         "< INC < 121.2DEG, there is a possible inaccuracy due to "
                         "singularity related with critical angle.\n";
            inaccuracyCriticalAngleWritten = true;
        }
    }

    Integer pseudostate = 0;
    // if (kep[2] > 175.0)
    // {
    // kep[2] = Real::Pi() - kep[2];  // INC = 180 - INC
    // kep[3] = -kep[3];         // RAAN = - RAAN
    // cartesian = KeplerianToCartesian(mu, kep, type);
    // pseudostate = 1;
    // }

    Vector6d blmean = kep;
    Vector6d kep2 = BMLOE::FromVector(kep).toCOE().asVector(COE::AnomalyType::Mean);
    std::cout << "kep2: " << kep2 << std::endl;

    Vector6d blmean2;

    // blmean2= blmean + (kep - kep2);
    Vector6d aeq;
    Vector6d aeq2;
    Vector6d aeqmean;
    Vector6d aeqmean2;

    aeq[0] = kep[0];
    aeq[1] = kep[1] * std::sin((kep[4] + kep[3]));
    aeq[2] = kep[1] * std::cos((kep[4] + kep[3]));
    aeq[3] = std::sin(kep[2] / 2.0) * std::sin(kep[3]);
    aeq[4] = std::sin(kep[2] / 2.0) * std::cos(kep[3]);
    aeq[5] = kep[3] + kep[4] + kep[5];

    aeq2[0] = kep2[0];
    aeq2[1] = kep2[1] * std::sin((kep2[4] + kep2[3]));
    aeq2[2] = kep2[1] * std::cos((kep2[4] + kep2[3]));
    aeq2[3] = std::sin(kep2[2] / 2.0) * std::sin(kep2[3]);
    aeq2[4] = std::sin(kep2[2] / 2.0) * std::cos(kep2[3]);
    aeq2[5] = kep2[3] + kep2[4] + kep2[5];

    aeqmean[0] = blmean[0];
    aeqmean[1] = blmean[1] * std::sin((blmean[4] + blmean[3]));
    aeqmean[2] = blmean[1] * std::cos((blmean[4] + blmean[3]));
    aeqmean[3] = std::sin(blmean[2] / 2.0) * std::sin(blmean[3]);
    aeqmean[4] = std::sin(blmean[2] / 2.0) * std::cos(blmean[3]);
    aeqmean[5] = blmean[3] + blmean[4] + blmean[5];

    aeqmean2 = aeqmean + (aeq - aeq2);
    Real emag = 0.9;
    Real emag_old = 1.0;
    Integer ii = 0;
    Vector6d tmp;
    Vector6d cart2;
    while (emag > tol)
    {
        blmean2[0] = aeqmean2[0];
        blmean2[1] = std::sqrt(aeqmean2[1] * aeqmean2[1] + aeqmean2[2] * aeqmean2[2]);
        if ((aeqmean2[3] * aeqmean2[3] + aeqmean2[4] * aeqmean2[4]) <= 1.0)
            blmean2[2] = std::acos(1.0 - 2.0 * (aeqmean2[3] * aeqmean2[3] + aeqmean2[4] * aeqmean2[4]));
        if ((aeqmean2[3] * aeqmean2[3] + aeqmean2[4] * aeqmean2[4]) > 1.0)
            blmean2[2] = std::acos(1.0 - 2.0 * 1.0);

        blmean2[3] = std::atan2(aeqmean2[3], aeqmean2[4]);
        if (blmean2[3] < 0.0)
        {
            blmean2[3] = blmean2[3] + Real::TwoPi();
        }
        blmean2[4] = std::atan2(aeqmean2[1], aeqmean2[2]) - blmean2[3];
        blmean2[5] = aeqmean2[5] - std::atan2(aeqmean2[1], aeqmean2[2]);

        kep2 = BMLOE::FromVector(blmean2).toCOE().asVector(COE::AnomalyType::Mean);
        std::cout << "kep2: " << kep2 << std::endl;

        const COE coe2 = COE::FromVector(kep2, COE::AnomalyType::Mean);
        Position position2 = Position::Undefined();
        Velocity velocity2 = Velocity::Undefined();
        std::tie(position2, velocity2) =
            coe2.getCartesianState(EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF());
        cart2.segment(0, 3) = position2.getCoordinates();
        cart2.segment(3, 3) = velocity2.getCoordinates();

        tmp = cartesian - cart2;

        emag = std::sqrt(
                   std::pow(tmp[0], 2.0) + std::pow(tmp[1], 2.0) + std::pow(tmp[2], 2.0) + std::pow(tmp[3], 2.0) +
                   std::pow(tmp[4], 2.0) + std::pow(tmp[5], 2.0)
               ) /
               std::sqrt(
                   std::pow(cartesian[0], 2.0) + std::pow(cartesian[1], 2.0) + std::pow(cartesian[2], 2.0) +
                   std::pow(cartesian[3], 2.0) + std::pow(cartesian[4], 2.0) + std::pow(cartesian[5], 2.0)
               );

        if (emag_old > emag)
        {
            emag_old = emag;
            // blmean	= blmean2;

            // blmean2= blmean + (kep - kep2);

            aeq2[0] = kep2[0];
            aeq2[1] = kep2[1] * std::sin((kep2[4] + kep2[3]));
            aeq2[2] = kep2[1] * std::cos((kep2[4] + kep2[3]));
            aeq2[3] = std::sin(kep2[2] / 2.0) * std::sin(kep2[3]);
            aeq2[4] = std::sin(kep2[2] / 2.0) * std::cos(kep2[3]);
            aeq2[5] = kep2[3] + kep2[4] + kep2[5];

            aeqmean = aeqmean2;
            aeqmean2 = aeqmean + (aeq - aeq2);
        }
        else
        {
            std::cout << String::Format(
                "Warning: the iterative algorithm converting from Cartesian "
                "to BrouwerMeanLong is not converging. So, it has been "
                "interrupted. The current relative error is {} . \n",
                emag_old
            );
            break;
        }
        if (ii > maxiter)
        {
            std::cout << "Warning: Maximum iteration number has been reached. There is a possible inaccuracy.\n";
            break;
        }
        ii = ii + 1;
    }

    blmean[0] = aeqmean[0];
    blmean[1] = std::sqrt(aeqmean[1] * aeqmean[1] + aeqmean[2] * aeqmean[2]);
    if ((aeqmean[3] * aeqmean[3] + aeqmean[4] * aeqmean[4]) <= 1.0)
        blmean[2] = std::acos(1.0 - 2.0 * (aeqmean[3] * aeqmean[3] + aeqmean[4] * aeqmean[4]));
    if ((aeqmean[3] * aeqmean[3] + aeqmean[4] * aeqmean[4]) > 1.0)
        blmean[2] = std::acos(1.0 - 2.0 * 1.0);

    blmean[3] = std::atan2(aeqmean[3], aeqmean[4]);
    if (blmean[3] < 0.0)
    {
        blmean[3] = blmean[3] + Real::TwoPi();
    }
    blmean[4] = std::atan2(aeqmean[1], aeqmean[2]) - blmean[3];
    blmean[5] = aeqmean[5] - std::atan2(aeqmean[1], aeqmean[2]);

    if (pseudostate != 0)
    {
        blmean[2] = Real::Pi() - blmean[2];
        blmean[3] = -blmean[3];
    }

    blmean[3] = std::fmod(blmean[3], Real::TwoPi());
    blmean[4] = std::fmod(blmean[4], Real::TwoPi());
    blmean[5] = std::fmod(blmean[5], Real::TwoPi());
    if (blmean[3] < 0.0)
    {
        blmean[3] = blmean[3] + Real::TwoPi();
    }
    if (blmean[4] < 0.0)
    {
        blmean[4] = blmean[4] + Real::TwoPi();
    }
    if (blmean[5] < 0.0)
    {
        blmean[5] = blmean[5] + Real::TwoPi();
    }

    return {
        Length::Meters(blmean[0]),
        blmean[1],
        Angle::Radians(blmean[2]),
        Angle::Radians(blmean[3]),
        Angle::Radians(blmean[4]),
        Angle::Radians(blmean[5]),
    };
}

COE BMLOE::toCOE() const
{
    // TBM: Make configurable?
    const Real equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();

    Integer pseudostate = 0;
    bool possibleInaccuracyLongWritten = false;
    bool criticalInclinationWritten = false;

    const Real j2 = 1.082626925638815E-03;
    const Real j3 = -0.2532307818191774E-5;
    const Real j4 = -0.1620429990000000E-5;
    const Real j5 = -0.2270711043920343E-6;
    const Real ae = 1.0;
    Real smadp = semiMajorAxis_.inMeters() / equatorialRadius;
    Real eccdp = eccentricity_;
    Real incdp = inclination_.inRadians();
    Real raandp = raan_.inRadians(0.0, Real::TwoPi());
    Real aopdp = aop_.inRadians(0.0, Real::TwoPi());
    Real meanAnom = meanAnomaly_.inRadians(0.0, Real::TwoPi());

    // TBI: Case for inclination > 175.0
    if (eccdp > 0.99)
    {
        std::stringstream errmsg("");
        errmsg.precision(21);
        errmsg << " while converting from the BrouwerMeanLong to";
        errmsg << " the Cartesian, an error has been encountered.";
        errmsg << " BrouwerMeanLong is applicable";
        errmsg << " only if mean ECC is smaller than 0.99." << std::endl;
        throw ostk::core::error::RuntimeError(errmsg.str());
    }

    Real radper = (semiMajorAxis_.inMeters() * (1.0 - eccdp));
    std::cout << semiMajorAxis_.inMeters() << " " << eccdp << " " << radper << std::endl;
    if (radper < 3000000.0)
    {
        std::stringstream errmsg("");
        errmsg.precision(21);
        errmsg << " While converting from the BrouwerMeanLong to";
        errmsg << " the Cartesian, an error has been encountered.";
        errmsg << " BrouwerMeanLong is applicable";
        errmsg << " only if RadPer is larger than 3000km." << std::endl;
        throw ostk::core::error::RuntimeError(errmsg.str());
    }
    if (radper < 6378000.0)
    {
        if (!possibleInaccuracyLongWritten)
        {
            std::cout << "Warning: For BrouwerMeanLong, when RadPer < 6378km, there "
                         "is a possible inaccuracy due to singularity related "
                         "with inside-of-earth orbit.\n";
            possibleInaccuracyLongWritten = true;
        }
    }

    if (incdp > Real::Pi())
    {
        std::stringstream errmsg("");
        errmsg.precision(21);
        errmsg << " While converting from the BrouwerMeanLong to";
        errmsg << " the Cartesian, an error has been encountered.";
        errmsg << " BrouwerMeanLong is applicable";
        errmsg << " only if inclination is smaller than 180 DEG." << std::endl;
        throw ostk::core::error::RuntimeError(errmsg.str());
    }

    raandp = std::fmod(raandp, Real::TwoPi());
    aopdp = std::fmod(aopdp, Real::TwoPi());
    meanAnom = std::fmod(meanAnom, Real::TwoPi());

    if (raandp < 0.0)
    {
        raandp = raandp + Real::TwoPi();
    }
    if (aopdp < 0.0)
    {
        aopdp = aopdp + Real::TwoPi();
    }
    if (meanAnom < 0.0)
    {
        meanAnom = meanAnom + Real::TwoPi();
    }

    const Real bk2 = (1.0 / 2.0) * (j2 * ae * ae);
    const Real bk3 = -j3 * std::pow(ae, 3.0);
    const Real bk4 = -(3.0 / 8.0) * j4 * std::pow(ae, 4.0);
    const Real bk5 = -j5 * std::pow(ae, 5.0);
    const Real eccdp2 = eccdp * eccdp;
    const Real cn2 = 1.0 - eccdp2;
    const Real cn = std::sqrt(cn2);
    const Real gm2 = bk2 / std::pow(smadp, 2.0);
    const Real gmp2 = gm2 / (cn2 * cn2);
    const Real gm4 = bk4 / std::pow(smadp, 4.0);
    const Real gmp4 = gm4 / std::pow(cn, 8.0);
    const Real theta = std::cos(incdp);
    const Real theta2 = theta * theta;
    const Real theta4 = theta2 * theta2;

    const Real gm3 = bk3 / std::pow(smadp, 3.0);
    const Real gmp3 = gm3 / (cn2 * cn2 * cn2);
    const Real gm5 = bk5 / std::pow(smadp, 5.0);
    const Real gmp5 = gm5 / std::pow(cn, 10.0);

    const Real g3dg2 = gmp3 / gmp2;
    const Real g4dg2 = gmp4 / gmp2;
    const Real g5dg2 = gmp5 / gmp2;

    const Real sinMADP = std::sin(meanAnom);
    const Real cosMADP = std::cos(meanAnom);
    const Real sinraandp = std::sin(raandp);
    const Real cosraandp = std::cos(raandp);

    // True anomaly (double primed)
    const Real tadp = COE::TrueAnomalyFromMeanAnomaly(meanAnomaly_, eccdp, 1e-12).inRadians();

    const Real rp = smadp * (1.0 - eccdp * eccdp) / (1.0 + eccdp * std::cos(tadp));
    const Real adr = smadp / rp;
    const Real sinta = std::sin(tadp);
    const Real costa = std::cos(tadp);
    const Real cs2gta = std::cos(2.0 * aopdp + 2.0 * tadp);
    const Real adr2 = adr * adr;
    const Real adr3 = adr2 * adr;
    const Real costa2 = costa * costa;

    const Real a1 = ((1.0 / 8.0) * gmp2 * cn2) * (1.0 - 11.0 * theta2 - ((40.0 * theta4) / (1.0 - 5.0 * theta2)));
    const Real a2 = ((5.0 / 12.0) * g4dg2 * cn2) * (1.0 - ((8.0 * theta4) / (1.0 - 5.0 * theta2)) - 3.0 * theta2);
    const Real a3 = g5dg2 * ((3.0 * eccdp2) + 4.0);
    const Real a4 = g5dg2 * (1.0 - (24.0 * theta4) / (1.0 - 5.0 * theta2) - 9.0 * theta2);
    const Real a5 = (g5dg2 * (3.0 * eccdp2 + 4.0)) * (1.0 - (24.0 * theta4) / (1.0 - 5.0 * theta2) - 9.0 * theta2);
    const Real a6 = g3dg2 * (1.0 / 4.0);
    const Real sinI = std::sin(incdp);
    const Real a10 = cn2 * sinI;
    const Real a7 = a6 * a10;
    const Real a8p = g5dg2 * eccdp * (1.0 - (16.0 * theta4) / (1.0 - 5.0 * theta2) - 5.0 * theta2);
    const Real a8 = a8p * eccdp;

    const Real b13 = eccdp * (a1 - a2);
    const Real b14 = a7 + (5.0 / 64.0) * a5 * a10;
    const Real b15 = a8 * a10 * (35.0 / 384.0);

    const Real a11 = 2.0 + eccdp2;
    const Real a12 = 3.0 * eccdp2 + 2.0;
    const Real a13 = theta2 * a12;
    const Real a14 = (5.0 * eccdp2 + 2.0) * (theta4 / (1.0 - 5.0 * theta2));
    const Real a17 = theta4 / ((1.0 - 5.0 * theta2) * (1.0 - 5.0 * theta2));
    const Real a15 = (eccdp2 * theta4 * theta2) / ((1.0 - 5.0 * theta2) * (1.0 - 5.0 * theta2));
    const Real a16 = theta2 / (1.0 - 5.0 * theta2);
    const Real a18 = eccdp * sinI;
    const Real a19 = a18 / (1.0 + cn);
    const Real a21 = eccdp * theta;
    const Real a22 = eccdp2 * theta;
    const Real sinI2 = std::sin(incdp / 2.0);
    const Real cosI2 = std::cos(incdp / 2.0);
    const Real tanI2 = std::tan(incdp / 2.0);
    const Real a26 = 16.0 * a16 + 40.0 * a17 + 3.0;
    const Real a27 = a22 * (1.0 / 8.0) * (11.0 + 200.0 * a17 + 80.0 * a16);

    const Real b1 = cn * (a1 - a2) -
                    ((a11 - 400.0 * a15 - 40.0 * a14 - 11.0 * a13) * (1.0 / 16.0) +
                     (11.0 + 200.0 * a17 + 80.0 * a16) * a22 * (1.0 / 8.0)) *
                        gmp2 +
                    ((-80.0 * a15 - 8.0 * a14 - 3.0 * a13 + a11) * (5.0 / 24.0) + (5.0 / 12.0) * a26 * a22) * g4dg2;
    const Real b2 = a6 * a19 * (2.0 + cn - eccdp2) + (5.0 / 64.0) * a5 * a19 * cn2 -
                    (15.0 / 32.0) * a4 * a18 * cn * cn2 + ((5.0 / 64.0) * a5 + a6) * a21 * tanI2 +
                    (9.0 * eccdp2 + 26.0) * (5.0 / 64.0) * a4 * a18 +
                    (15.0 / 32.0) * a3 * a21 * a26 * sinI * (1.0 - theta);
    const Real b3 = ((80.0 * a17 + 5.0 + 32.0 * a16) * a22 * sinI * (theta - 1.0) * (35.0 / 576.0) * g5dg2 * eccdp) -
                    ((a22 * tanI2 + (2.0 * eccdp2 + 3.0 * (1.0 - cn2 * cn)) * sinI) * (35.0 / 1152.0) * a8p);
    const Real b4 = cn * eccdp * (a1 - a2);
    const Real b5 = ((9.0 * eccdp2 + 4.0) * a10 * a4 * (5.0 / 64.0) + a7) * cn;
    const Real b6 = (35.0 / 384.0) * a8 * cn2 * cn * sinI;
    const Real b7 = ((cn2 * a18) / (1.0 - 5.0 * theta2)) *
                    ((1.0 / 8.0) * gmp2 * (1.0 - 15.0 * theta2) + (1.0 - 7.0 * theta2) * g4dg2 * (-(5.0 / 12.0)));
    const Real b8 =
        (5.0 / 64.0) * (a3 * cn2 * (1.0 - 9.0 * theta2 - (24.0 * theta4 / (1.0 - 5.0 * theta2)))) + a6 * cn2;
    const Real b9 = a8 * (35.0 / 384.0) * cn2;
    const Real b10 = sinI * (a22 * a26 * g4dg2 * (5.0 / 12.0) - a27 * gmp2);
    const Real b11 = a21 * (a5 * (5.0 / 64.0) + a6 + a3 * a26 * (15.0 / 32.0) * sinI * sinI);
    const Real b12 =
        -((80.0 * a17 + 32.0 * a16 + 5.0) * (a22 * eccdp * sinI * sinI * (35.0 / 576.0) * g5dg2) +
          (a8 * a21 * (35.0 / 1152.0)));

    // Semi-Major Axis
    const Real sma =
        smadp * (1.0 + gm2 * ((3.0 * theta2 - 1.0) * (eccdp2 / (cn2 * cn2 * cn2)) * (cn + (1.0 / (1.0 + cn))) +
                              ((3.0 * theta2 - 1.0) / (cn2 * cn2 * cn2)) * (eccdp * costa) *
                                  (3.0 + 3.0 * eccdp * costa + eccdp2 * costa2) +
                              3.0 * (1.0 - theta2) * adr3 * cs2gta));
    const Real sn2gta = std::sin(2.0 * aopdp + 2.0 * tadp);
    const Real snf2gd = std::sin(2.0 * aopdp + tadp);
    const Real csf2gd = std::cos(2.0 * aopdp + tadp);
    const Real sn2gd = std::sin(2.0 * aopdp);
    const Real cs2gd = std::cos(2.0 * aopdp);
    const Real sin3gd = std::sin(3.0 * aopdp);
    const Real cs3gd = std::cos(3.0 * aopdp);
    const Real sn3fgd = std::sin(3.0 * tadp + 2.0 * aopdp);
    const Real cs3fgd = std::cos(3.0 * tadp + 2.0 * aopdp);
    const Real sinGD = std::sin(aopdp);
    const Real cosGD = std::cos(aopdp);

    // L + G + H primed
    const Real bisubc = std::pow((1.0 - 5.0 * theta2), -2.0) * ((25.0 * theta4 * theta) * (gmp2 * eccdp2));
    Real blghp = 0.0;
    Real eccdpdl = 0.0;
    Real dltI = 0.0;
    Real sinDH = 0.0;
    Real dlt1e = 0.0;
    if (bisubc >= 0.001)
    {  // modifications for critical inclination
        if (!criticalInclinationWritten)
        {
            std::cout << "Warning: Mean inclination is close to critical "
                         "inclination 63 or 117 DEG. There is a possible inaccuracy.\n";
            criticalInclinationWritten = true;
        }
    }
    else
    {
        blghp = raandp + aopdp + meanAnom + b3 * cs3gd + b1 * sn2gd + b2 * cosGD;
        blghp = std::fmod(blghp, (Real::TwoPi()));
        if (blghp < 0.0)
        {
            blghp = blghp + (Real::TwoPi());
        }
        dlt1e = b14 * sinGD + b13 * cs2gd - b15 * sin3gd;
        eccdpdl = b4 * sn2gd - b5 * cosGD + b6 * cs3gd -
                  (1.0 / 4.0) * cn2 * cn * gmp2 *
                      (2.0 * (3.0 * theta2 - 1.0) * (adr2 * cn2 + adr + 1.0) * sinta +
                       3.0 * (1.0 - theta2) *
                           ((-adr2 * cn2 - adr + 1.0) * snf2gd + (adr2 * cn2 + adr + (1.0 / 3.0)) * sn3fgd));
        dltI = (1.0 / 2.0) * theta * gmp2 * sinI * (eccdp * cs3fgd + 3.0 * (eccdp * csf2gd + cs2gta)) -
               (a21 / cn2) * (b8 * sinGD + b7 * cs2gd - b9 * sin3gd);
        sinDH = (1.0 / cosI2) *
                ((1.0 / 2.0) *
                 (b12 * cs3gd + b11 * cosGD + b10 * sn2gd -
                  ((1.0 / 2.0) * gmp2 * theta * sinI *
                   (6.0 * (eccdp * sinta - meanAnom + tadp) - (3.0 * (sn2gta + eccdp * snf2gd) + eccdp * sn3fgd)))));
    }

    // L + G + H
    Real blgh =
        blghp +
        ((1.0 / (cn + 1.0)) * (1.0 / 4.0) * eccdp * gmp2 * cn2 *
         (3.0 * (1.0 - theta2) * (sn3fgd * ((1.0 / 3.0) + adr2 * cn2 + adr) + snf2gd * (1.0 - (adr2 * cn2 + adr))) +
          2.0 * sinta * (3.0 * theta2 - 1.0) * (adr2 * cn2 + adr + 1.0))) +
        gmp2 * (3.0 / 2.0) * ((-2.0 * theta - 1.0 + 5.0 * theta2) * (eccdp * sinta + tadp - meanAnom)) +
        (3.0 + 2.0 * theta - 5.0 * theta2) * (gmp2 * (1.0 / 4.0) * (eccdp * sn3fgd + 3.0 * (sn2gta + eccdp * snf2gd)));
    blgh = std::fmod(blgh, Real::TwoPi());
    if (blgh < 0.0)
    {
        blgh = blgh + Real::TwoPi();
    }

    const Real dlte =
        dlt1e +
        ((1.0 / 2.0) * cn2 *
         ((3.0 * (1.0 / (cn2 * cn2 * cn2)) * gm2 * (1.0 - theta2) * cs2gta *
           (3.0 * eccdp * costa2 + 3.0 * costa + eccdp2 * costa * costa2 + eccdp)) -
          (gmp2 * (1.0 - theta2) * (3.0 * csf2gd + cs3fgd)) +
          (3.0 * theta2 - 1.0) * gm2 * (1.0 / (cn2 * cn2 * cn2)) *
              (eccdp * cn + (eccdp / (1.0 + cn)) + 3.0 * eccdp * costa2 + 3.0 * costa + eccdp2 * costa * costa2)));
    const Real eccdpdl2 = eccdpdl * eccdpdl;
    const Real eccdpde2 = (eccdp + dlte) * (eccdp + dlte);

    // Eccentricity
    const Real ecc = std::sqrt(eccdpdl2 + eccdpde2);
    const Real sinDH2 = sinDH * sinDH;
    const Real squar = (dltI * cosI2 * (1.0 / 2.0) + sinI2) * (dltI * cosI2 * (1.0 / 2.0) + sinI2);
    const Real sqrI = std::sqrt(sinDH2 + squar);

    // Inclination
    Real inc = 2 * std::asin(sqrI);
    inc = std::fmod(inc, Real::TwoPi());

    // Mean anomaly (longitude of ascending node and Argument of perigee)
    Real ma = Real::Undefined();
    Real raan = Real::Undefined();
    Real aop = Real::Undefined();
    if (ecc <= 1.0E-11)
    {
        aop = 0.0;
        if (inc <= 1.0E-7)
        {
            raan = 0.0;
            ma = blgh;
        }
        else
        {
            const Real arg1 = sinDH * cosraandp + sinraandp * ((1.0 / 2.0) * dltI * cosI2 + sinI2);
            const Real arg2 = cosraandp * ((1.0 / 2.0) * dltI * cosI2 + sinI2) - (sinDH * sinraandp);
            raan = std::atan2(arg1, arg2);
            ma = blgh - aop - raan;
        }
    }
    else
    {
        const Real arg1 = eccdpdl * cosMADP + (eccdp + dlte) * sinMADP;
        const Real arg2 = (eccdp + dlte) * cosMADP - (eccdpdl * sinMADP);
        ma = std::atan2(arg1, arg2);
        ma = std::fmod(ma, (Real::TwoPi()));

        if (inc <= 1.0E-7)
        {
            raan = 0.0;
            aop = blgh - raan - ma;
        }
        else
        {
            const Real arg1 = sinDH * cosraandp + sinraandp * ((1.0 / 2.0) * dltI * cosI2 + sinI2);
            const Real arg2 = cosraandp * ((1.0 / 2.0) * dltI * cosI2 + sinI2) - (sinDH * sinraandp);
            raan = std::atan2(arg1, arg2);
            aop = blgh - ma - raan;
        }
    }
    if (ma < 0.0)
    {
        ma = ma + Real::TwoPi();
    }

    raan = std::fmod(raan, (Real::TwoPi()));
    if (raan < 0.0)
    {
        raan = raan + (Real::TwoPi());
    }

    aop = std::fmod(aop, (Real::TwoPi()));
    if (aop < 0.0)
    {
        aop = aop + Real::TwoPi();
    }

    if (pseudostate != 0)
    {
        inc = Real::Pi() - inc;
        raan = Real::TwoPi() - raan;
    }

    return COE::FromVector(
        {
            sma * equatorialRadius,
            ecc,
            inc,
            raan,
            aop,
            ma,
        },
        COE::AnomalyType::Mean
    );
}

BMLOE BMLOE::FromVector(const Vector6d &aCOEVector)
{
    return BMLOE(
        Length::Meters(aCOEVector[0]),
        aCOEVector[1],
        Angle::Radians(aCOEVector[2]),
        Angle::Radians(aCOEVector[3]),
        Angle::Radians(aCOEVector[4]),
        Angle::Radians(aCOEVector[5])
    );
}

}  // namespace kepler
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
