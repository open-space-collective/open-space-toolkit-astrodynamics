/// Apache License 2.0

#include <limits>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>

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
namespace blm
{

using ostk::core::type::Integer;
using ostk::core::type::Size;

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
    // ref:
    // https://github.com/ChristopherRabotin/GMAT/blob/37201a6290e7f7b941bc98ee973a527a5857104b/src/base/util/StateConversionUtil.cpp#L4206

    const auto mod = [](const double &value, const double &bound) -> double
    {
        return value - bound * std::floor(value / bound);
    };

    // TBM: Make configurable?
    const double equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();

    int pseudoState = 0;
    bool possibleInaccuracyLongWritten = false;
    bool criticalInclinationWritten = false;

    const double re = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();
    const double j2 = 1.082626925638815E-03;
    const double j3 = -0.2532307818191774E-5;
    const double j4 = -0.1620429990000000E-5;
    const double j5 = -0.2270711043920343E-6;
    const double ae = 1.0;
    double smadp = semiMajorAxis_.inMeters() / re;
    double eccdp = eccentricity_;
    double incdp = inclination_.inRadians();
    double raandp = raan_.inRadians(0.0, 2.0 * M_PI);
    double aopdp = aop_.inRadians(0.0, 2.0 * M_PI);
    double meanAnom = anomaly_.inRadians(0.0, 2.0 * M_PI);

    if (incdp > 3.0543261909900763)
    {
        incdp = M_PI - incdp;
        raandp = -raandp;
        pseudoState = 1;
    }

    // negative eccentricity aviodance lines
    if (eccdp > 0.99)
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMeanLong is applicable only if 0.0 < Ecc: [{}] < 0.99.", eccdp
        );
    }

    const double perigee = this->getPeriapsisRadius().inMeters();
    if (perigee < 3000000.0)
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMeanLong is applicable only if Perigee radius: [{}] > 3000000.0 m.", perigee
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

    if (incdp > M_PI)
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMeanLong is applicable only if inclination: [{}] < 180.0 degrees.",
            Angle::Radians(incdp).inDegrees()
        );
    }

    const double bk2 = (1.0 / 2.0) * (j2 * ae * ae);
    const double bk3 = -j3 * std::pow(ae, 3.0);
    const double bk4 = -(3.0 / 8.0) * j4 * std::pow(ae, 4.0);
    const double bk5 = -j5 * std::pow(ae, 5.0);
    const double eccdp2 = eccdp * eccdp;
    const double cn2 = 1.0 - eccdp2;
    const double cn = std::sqrt(cn2);
    const double gm2 = bk2 / std::pow(smadp, 2.0);
    const double gmp2 = gm2 / (cn2 * cn2);
    const double gm4 = bk4 / std::pow(smadp, 4.0);
    const double gmp4 = gm4 / std::pow(cn, 8.0);
    const double theta = std::cos(incdp);
    const double theta2 = theta * theta;
    const double theta4 = theta2 * theta2;

    const double gm3 = bk3 / std::pow(smadp, 3.0);
    const double gmp3 = gm3 / (cn2 * cn2 * cn2);
    const double gm5 = bk5 / std::pow(smadp, 5.0);
    const double gmp5 = gm5 / std::pow(cn, 10.0);

    const double g3dg2 = gmp3 / gmp2;
    const double g4dg2 = gmp4 / gmp2;
    const double g5dg2 = gmp5 / gmp2;

    const double sinMADP = std::sin(meanAnom);
    const double cosMADP = std::cos(meanAnom);
    const double sinraandp = std::sin(raandp);
    const double cosraandp = std::cos(raandp);

    //-------------------------------------I
    // COMPUTE TRUE ANOMALY(DOUBLE PRIMED) I
    //-------------------------------------I
    const double tadp = COE::TrueAnomalyFromMeanAnomaly(anomaly_, eccentricity_, 1e-15).inRadians();

    const double rp = smadp * (1.0 - eccdp * eccdp) / (1.0 + eccdp * std::cos(tadp));
    const double adr = smadp / rp;
    const double sinta = std::sin(tadp);
    const double costa = std::cos(tadp);
    const double cs2gta = std::cos(2.0 * aopdp + 2.0 * tadp);
    const double adr2 = adr * adr;
    const double adr3 = adr2 * adr;
    const double costa2 = costa * costa;

    const double a1 = ((1.0 / 8.0) * gmp2 * cn2) * (1.0 - 11.0 * theta2 - ((40.0 * theta4) / (1.0 - 5.0 * theta2)));
    const double a2 = ((5.0 / 12.0) * g4dg2 * cn2) * (1.0 - ((8.0 * theta4) / (1.0 - 5.0 * theta2)) - 3.0 * theta2);
    const double a3 = g5dg2 * ((3.0 * eccdp2) + 4.0);
    const double a4 = g5dg2 * (1.0 - (24.0 * theta4) / (1.0 - 5.0 * theta2) - 9.0 * theta2);
    const double a5 = (g5dg2 * (3.0 * eccdp2 + 4.0)) * (1.0 - (24.0 * theta4) / (1.0 - 5.0 * theta2) - 9.0 * theta2);
    const double a6 = g3dg2 * (1.0 / 4.0);
    const double sinI = std::sin(incdp);
    const double a10 = cn2 * sinI;
    const double a7 = a6 * a10;
    const double a8p = g5dg2 * eccdp * (1.0 - (16.0 * theta4) / (1.0 - 5.0 * theta2) - 5.0 * theta2);
    const double a8 = a8p * eccdp;

    const double b13 = eccdp * (a1 - a2);
    const double b14 = a7 + (5.0 / 64.0) * a5 * a10;
    const double b15 = a8 * a10 * (35.0 / 384.0);

    const double a11 = 2.0 + eccdp2;
    const double a12 = 3.0 * eccdp2 + 2.0;
    const double a13 = theta2 * a12;
    const double a14 = (5.0 * eccdp2 + 2.0) * (theta4 / (1.0 - 5.0 * theta2));
    const double a17 = theta4 / ((1.0 - 5.0 * theta2) * (1.0 - 5.0 * theta2));
    const double a15 = (eccdp2 * theta4 * theta2) / ((1.0 - 5.0 * theta2) * (1.0 - 5.0 * theta2));
    const double a16 = theta2 / (1.0 - 5.0 * theta2);
    const double a18 = eccdp * sinI;
    const double a19 = a18 / (1.0 + cn);
    const double a21 = eccdp * theta;
    const double a22 = eccdp2 * theta;
    const double sinI2 = std::sin(incdp / 2.0);
    const double cosI2 = std::cos(incdp / 2.0);
    const double tanI2 = std::tan(incdp / 2.0);
    const double a26 = 16.0 * a16 + 40.0 * a17 + 3.0;
    const double a27 = a22 * (1.0 / 8.0) * (11.0 + 200.0 * a17 + 80.0 * a16);

    const double b1 = cn * (a1 - a2) -
                      ((a11 - 400.0 * a15 - 40.0 * a14 - 11.0 * a13) * (1.0 / 16.0) +
                       (11.0 + 200.0 * a17 + 80.0 * a16) * a22 * (1.0 / 8.0)) *
                          gmp2 +
                      ((-80.0 * a15 - 8.0 * a14 - 3.0 * a13 + a11) * (5.0 / 24.0) + (5.0 / 12.0) * a26 * a22) * g4dg2;
    const double b2 = a6 * a19 * (2.0 + cn - eccdp2) + (5.0 / 64.0) * a5 * a19 * cn2 -
                      (15.0 / 32.0) * a4 * a18 * cn * cn2 + ((5.0 / 64.0) * a5 + a6) * a21 * tanI2 +
                      (9.0 * eccdp2 + 26.0) * (5.0 / 64.0) * a4 * a18 +
                      (15.0 / 32.0) * a3 * a21 * a26 * sinI * (1.0 - theta);
    const double b3 = ((80.0 * a17 + 5.0 + 32.0 * a16) * a22 * sinI * (theta - 1.0) * (35.0 / 576.0) * g5dg2 * eccdp) -
                      ((a22 * tanI2 + (2.0 * eccdp2 + 3.0 * (1.0 - cn2 * cn)) * sinI) * (35.0 / 1152.0) * a8p);
    const double b4 = cn * eccdp * (a1 - a2);
    const double b5 = ((9.0 * eccdp2 + 4.0) * a10 * a4 * (5.0 / 64.0) + a7) * cn;
    const double b6 = (35.0 / 384.0) * a8 * cn2 * cn * sinI;
    const double b7 = ((cn2 * a18) / (1.0 - 5.0 * theta2)) *
                      ((1.0 / 8.0) * gmp2 * (1.0 - 15.0 * theta2) + (1.0 - 7.0 * theta2) * g4dg2 * (-(5.0 / 12.0)));
    const double b8 =
        (5.0 / 64.0) * (a3 * cn2 * (1.0 - 9.0 * theta2 - (24.0 * theta4 / (1.0 - 5.0 * theta2)))) + a6 * cn2;
    const double b9 = a8 * (35.0 / 384.0) * cn2;
    const double b10 = sinI * (a22 * a26 * g4dg2 * (5.0 / 12.0) - a27 * gmp2);
    const double b11 = a21 * (a5 * (5.0 / 64.0) + a6 + a3 * a26 * (15.0 / 32.0) * sinI * sinI);
    const double b12 =
        -((80.0 * a17 + 32.0 * a16 + 5.0) * (a22 * eccdp * sinI * sinI * (35.0 / 576.0) * g5dg2) +
          (a8 * a21 * (35.0 / 1152.0)));

    //----------------------------I
    // COMPUTE (SEMI-MAJOR AXIS)  I
    //----------------------------I
    const double sma =
        smadp * (1.0 + gm2 * ((3.0 * theta2 - 1.0) * (eccdp2 / (cn2 * cn2 * cn2)) * (cn + (1.0 / (1.0 + cn))) +
                              ((3.0 * theta2 - 1.0) / (cn2 * cn2 * cn2)) * (eccdp * costa) *
                                  (3.0 + 3.0 * eccdp * costa + eccdp2 * costa2) +
                              3.0 * (1.0 - theta2) * adr3 * cs2gta));
    const double sn2gta = std::sin(2.0 * aopdp + 2.0 * tadp);
    const double snf2gd = std::sin(2.0 * aopdp + tadp);
    const double csf2gd = std::cos(2.0 * aopdp + tadp);
    const double sn2gd = std::sin(2.0 * aopdp);
    const double cs2gd = std::cos(2.0 * aopdp);
    const double sin3gd = std::sin(3.0 * aopdp);
    const double cs3gd = std::cos(3.0 * aopdp);
    const double sn3fgd = std::sin(3.0 * tadp + 2.0 * aopdp);
    const double cs3fgd = std::cos(3.0 * tadp + 2.0 * aopdp);
    const double sinGD = std::sin(aopdp);
    const double cosGD = std::cos(aopdp);

    //------------------------I
    // COMPUTE (L+G+H) PRIMED I
    //------------------------I
    const double bisubc = std::pow((1.0 - 5.0 * theta2), -2.0) * ((25.0 * theta4 * theta) * (gmp2 * eccdp2));
    double blghp = 0.0;
    double eccdpdl = 0.0;
    double dltI = 0.0;
    double sinDH = 0.0;
    double dlt1e = 0.0;
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
        blghp = mod(blghp, 2.0 * M_PI);
        if (blghp < 0.0)
        {
            blghp += 2.0 * M_PI;
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
    double blgh =
        blghp +
        ((1.0 / (cn + 1.0)) * (1.0 / 4.0) * eccdp * gmp2 * cn2 *
         (3.0 * (1.0 - theta2) * (sn3fgd * ((1.0 / 3.0) + adr2 * cn2 + adr) + snf2gd * (1.0 - (adr2 * cn2 + adr))) +
          2.0 * sinta * (3.0 * theta2 - 1.0) * (adr2 * cn2 + adr + 1.0))) +
        gmp2 * (3.0 / 2.0) * ((-2.0 * theta - 1.0 + 5.0 * theta2) * (eccdp * sinta + tadp - meanAnom)) +
        (3.0 + 2.0 * theta - 5.0 * theta2) * (gmp2 * (1.0 / 4.0) * (eccdp * sn3fgd + 3.0 * (sn2gta + eccdp * snf2gd)));
    blgh = mod(blgh, 2.0 * M_PI);
    if (blgh < 0.0)
    {
        blgh = blgh + 2.0 * M_PI;
    }

    const double dlte =
        dlt1e +
        ((1.0 / 2.0) * cn2 *
         ((3.0 * (1.0 / (cn2 * cn2 * cn2)) * gm2 * (1.0 - theta2) * cs2gta *
           (3.0 * eccdp * costa2 + 3.0 * costa + eccdp2 * costa * costa2 + eccdp)) -
          (gmp2 * (1.0 - theta2) * (3.0 * csf2gd + cs3fgd)) +
          (3.0 * theta2 - 1.0) * gm2 * (1.0 / (cn2 * cn2 * cn2)) *
              (eccdp * cn + (eccdp / (1.0 + cn)) + 3.0 * eccdp * costa2 + 3.0 * costa + eccdp2 * costa * costa2)));
    const double eccdpdl2 = eccdpdl * eccdpdl;
    const double eccdpde2 = (eccdp + dlte) * (eccdp + dlte);

    //-------------------------I
    // COMPUTE ECC             I
    //-------------------------I
    const double ecc = sqrt(eccdpdl2 + eccdpde2);
    const double sinDH2 = sinDH * sinDH;
    const double squar = (dltI * cosI2 * (1.0 / 2.0) + sinI2) * (dltI * cosI2 * (1.0 / 2.0) + sinI2);
    const double sqrI = sqrt(sinDH2 + squar);

    //--------------------------I
    // COMPUTE (INCLINATION) I
    //--------------------------I
    double inc = 2 * std::asin(sqrI);
    inc = mod(inc, 2.0 * M_PI);

    //-------------------------I
    // COMPUTE (MEAN ANOMALY), (LONGITUDE OF ASCENDING NODE) and (ARGUMENT  OF PERIGEE) I
    //-------------------------I
    double ma = std::numeric_limits<double>::quiet_NaN();
    double raan = std::numeric_limits<double>::quiet_NaN();
    double aop = std::numeric_limits<double>::quiet_NaN();
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
            const double arg1 = sinDH * cosraandp + sinraandp * ((1.0 / 2.0) * dltI * cosI2 + sinI2);
            const double arg2 = cosraandp * ((1.0 / 2.0) * dltI * cosI2 + sinI2) - (sinDH * sinraandp);
            raan = std::atan2(arg1, arg2);
            ma = blgh - aop - raan;
        }
    }
    else
    {
        double arg1 = eccdpdl * cosMADP + (eccdp + dlte) * sinMADP;
        double arg2 = (eccdp + dlte) * cosMADP - (eccdpdl * sinMADP);
        ma = std::atan2(arg1, arg2);
        ma = mod(ma, 2.0 * M_PI);

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
        ma += 2.0 * M_PI;
    }

    raan = mod(raan, 2.0 * M_PI);
    if (raan < 0.0)
    {
        raan += 2.0 * M_PI;
    }

    aop = mod(aop, 2.0 * M_PI);
    if (aop < 0.0)
    {
        aop += 2.0 * M_PI;
    }

    if (pseudoState != 0)
    {
        inc = M_PI - inc;
        raan = 2.0 * M_PI - raan;
    }

    return COE::FromSIVector(
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

BrouwerLyddaneMeanLong BrouwerLyddaneMeanLong::COE(const classicalOE &aCOE)
{
    if (!aCOE.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("COE");
    }

    const classicalOE::CartesianState cartesianState =
        aCOE.getCartesianState(EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF());

    return BrouwerLyddaneMeanLong::Cartesian(cartesianState, EarthGravitationalModel::EGM2008.gravitationalParameter_);
}

BrouwerLyddaneMeanLong BrouwerLyddaneMeanLong::Cartesian(
    const COE::CartesianState &aCartesianState, const Derived &aGravitationalParameter
)
{
    const auto toCOEVector = [](const Vector6d &aVector) -> Vector6d
    {
        return BrouwerLyddaneMeanLong::FromSIVector(aVector).toCOE().getSIVector(COE::AnomalyType::Mean);
    };

    return BrouwerLyddaneMeanLong::FromSIVector(
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

BrouwerLyddaneMeanLong BrouwerLyddaneMeanLong::FromSIVector(const Vector6d &aVector)
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
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
