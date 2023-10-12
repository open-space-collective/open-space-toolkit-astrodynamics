/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/BrouwerLyddaneMeanLong.hpp>

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

BrouwerLyddaneMeanLong::BrouwerLyddaneMeanLong(
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

COE BrouwerLyddaneMeanLong::toCOE() const
{
    const auto mod = [](const Real &value, const Real &bound) -> Real
    {
        return value - bound * std::floor(value / bound);
    };

    // TBM: Make configurable?
    const Real equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();

    Integer pseudostate = 0;
    bool possibleInaccuracyLongWritten = false;
    bool criticalInclinationWritten = false;

    const Real re = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();
    const Real j2 = 1.082626925638815E-03;
    const Real j3 = -0.2532307818191774E-5;
    const Real j4 = -0.1620429990000000E-5;
    const Real j5 = -0.2270711043920343E-6;
    const Real ae = 1.0;
    Real smadp = semiMajorAxis_.inMeters() / re;
    Real eccdp = eccentricity_;
    Real incdp = inclination_.inRadians();
    Real raandp = raan_.inRadians(0.0, Real::TwoPi());
    Real aopdp = aop_.inRadians(0.0, Real::TwoPi());
    Real meanAnom = anomaly_.inRadians(0.0, Real::TwoPi());

    if (incdp > 3.0543261909900763)
    {
        incdp = Real::Pi() - incdp;
        raandp = -raandp;
        pseudostate = 1;
    }

    // negative eccentricity aviodance lines
    if (eccdp > 0.99)
    {
        throw ostk::core::error::RuntimeError(
            String::Format("BrouwerLyddaneMeanLong is applicable only if 0.0 < Ecc: [{}] < 0.99.", eccdp)
        );
    }

    const Real perigee = this->getPeriapsisRadius().inMeters();
    if (perigee < 3000000.0)
    {
        throw ostk::core::error::RuntimeError(
            String::Format("BrouwerLyddaneMeanLong is applicable only if Perigee radius: [{}] > 3000000.0 m.", perigee)
        );
    }
    if (perigee < 6378000.0)
    {
        if (!possibleInaccuracyLongWritten)
        {
            std::cout << "Warning: For BrouwerLyddaneMeanLong, when RadPer < 6378km, there "
                         "is a possible inaccuracy due to singularity related "
                         "with inside-of-earth orbit.\n";
            possibleInaccuracyLongWritten = true;
        }
    }

    if (incdp > Real::Pi())
    {
        throw ostk::core::error::RuntimeError(String::Format(
            "BrouwerLyddaneMeanLong is applicable only if inclination: [{}] < 180.0 degrees.",
            Angle::Radians(incdp).inDegrees()
        ));
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

    //-------------------------------------I
    // COMPUTE TRUE ANOMALY(DOUBLE PRIMED) I
    //-------------------------------------I
    const Real tadp = COE::TrueAnomalyFromMeanAnomaly(anomaly_, eccentricity_, 1e-15).inRadians();

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

    //----------------------------I
    // COMPUTE (SEMI-MAJOR AXIS)  I
    //----------------------------I
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

    //------------------------I
    // COMPUTE (L+G+H) PRIMED I
    //------------------------I
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
        blghp = mod(blghp, Real::TwoPi());
        if (blghp < 0.0)
        {
            blghp += Real::TwoPi();
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

    //-----------------I
    // COMPUTE (L+G+H) I
    //-----------------I
    Real blgh =
        blghp +
        ((1.0 / (cn + 1.0)) * (1.0 / 4.0) * eccdp * gmp2 * cn2 *
         (3.0 * (1.0 - theta2) * (sn3fgd * ((1.0 / 3.0) + adr2 * cn2 + adr) + snf2gd * (1.0 - (adr2 * cn2 + adr))) +
          2.0 * sinta * (3.0 * theta2 - 1.0) * (adr2 * cn2 + adr + 1.0))) +
        gmp2 * (3.0 / 2.0) * ((-2.0 * theta - 1.0 + 5.0 * theta2) * (eccdp * sinta + tadp - meanAnom)) +
        (3.0 + 2.0 * theta - 5.0 * theta2) * (gmp2 * (1.0 / 4.0) * (eccdp * sn3fgd + 3.0 * (sn2gta + eccdp * snf2gd)));
    blgh = mod(blgh, Real::TwoPi());
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

    //-------------------------I
    // COMPUTE ECC             I
    //-------------------------I
    const Real ecc = sqrt(eccdpdl2 + eccdpde2);
    const Real sinDH2 = sinDH * sinDH;
    const Real squar = (dltI * cosI2 * (1.0 / 2.0) + sinI2) * (dltI * cosI2 * (1.0 / 2.0) + sinI2);
    const Real sqrI = sqrt(sinDH2 + squar);

    //--------------------------I
    // COMPUTE (INCLINATION) I
    //--------------------------I
    Real inc = 2 * std::asin(sqrI);
    inc = mod(inc, Real::TwoPi());

    //-------------------------I
    // COMPUTE (MEAN ANOMALY), (LONGITUDE OF ASCENDING NODE) and (ARGUMENT  OF PERIGEE) I
    //-------------------------I
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
        Real arg1 = eccdpdl * cosMADP + (eccdp + dlte) * sinMADP;
        Real arg2 = (eccdp + dlte) * cosMADP - (eccdpdl * sinMADP);
        ma = std::atan2(arg1, arg2);
        ma = mod(ma, Real::TwoPi());

        if (inc <= 1.0E-7)
        {
            raan = 0.0;
            aop = blgh - raan - ma;
        }
        else
        {
            arg1 = sinDH * cosraandp + sinraandp * ((1.0 / 2.0) * dltI * cosI2 + sinI2);
            arg2 = cosraandp * ((1.0 / 2.0) * dltI * cosI2 + sinI2) - (sinDH * sinraandp);
            raan = std::atan2(arg1, arg2);
            aop = blgh - ma - raan;
        }
    }
    if (ma < 0.0)
    {
        ma += Real::TwoPi();
    }

    raan = mod(raan, Real::TwoPi());
    if (raan < 0.0)
    {
        raan += Real::TwoPi();
    }

    aop = mod(aop, Real::TwoPi());
    if (aop < 0.0)
    {
        aop += Real::TwoPi();
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

BrouwerLyddaneMeanLong BrouwerLyddaneMeanLong::Cartesian(
    const COE::CartesianState &aCartesianState, const Derived &aGravitationalParameter
)
{
    const auto toCOEVector = [](const Vector6d &aVector) -> Vector6d
    {
        return BrouwerLyddaneMeanLong::FromVector(aVector).toCOE().getVector(COE::AnomalyType::Mean);
    };

    return BrouwerLyddaneMeanLong::FromVector(
        BrouwerLyddaneMean::Cartesian(aCartesianState, aGravitationalParameter, toCOEVector)
    );
}

BrouwerLyddaneMeanLong BrouwerLyddaneMeanLong::Undefined()
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

BrouwerLyddaneMeanLong BrouwerLyddaneMeanLong::FromVector(const Vector6d &aVector)
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

}  // namespace kepler
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
