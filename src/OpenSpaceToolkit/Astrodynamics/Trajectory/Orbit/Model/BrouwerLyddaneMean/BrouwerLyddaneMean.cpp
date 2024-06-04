/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMean.hpp>

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

BrouwerLyddaneMean::BrouwerLyddaneMean(
    const Length &aSemiMajorAxis,
    const Real &anEccentricity,
    const Angle &anInclination,
    const Angle &aRaan,
    const Angle &anAop,
    const Angle &aMeanAnomaly
)
    : COE(aSemiMajorAxis, anEccentricity, anInclination, aRaan, anAop, aMeanAnomaly, COE::AnomalyType::Mean)
{
}

Angle BrouwerLyddaneMean::getMeanAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("BrouwerLyddaneMean");
    }

    return anomaly_;
}

Angle BrouwerLyddaneMean::getTrueAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("BrouwerLyddaneMean");
    }

    return COE::TrueAnomalyFromMeanAnomaly(anomaly_, eccentricity_, 1e-15);
}

Angle BrouwerLyddaneMean::getEccentricAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("BrouwerLyddaneMean");
    }

    return COE::EccentricAnomalyFromMeanAnomaly(anomaly_, eccentricity_, 1e-15);
}

COE::CartesianState BrouwerLyddaneMean::getCartesianState(
    const Derived &aGravitationalParameter, const Shared<const Frame> &aFrameSPtr
) const
{
    const COE coe = this->toCOE();
    return coe.getCartesianState(aGravitationalParameter, aFrameSPtr);
}

Vector6d BrouwerLyddaneMean::Cartesian(
    const COE::CartesianState &aCartesianState,
    const Derived &aGravitationalParameter,
    std::function<Vector6d(const Vector6d &)> toCOEVector
)
{
    const COE coe = COE::Cartesian(aCartesianState, aGravitationalParameter);

    bool possibleInaccuracyLongWritten = false;
    bool inaccuracyCriticalAngleWritten = false;

    if ((coe.getEccentricity() > 0.99) || (coe.getEccentricity() < 0.0))
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMean is applicable only if 0.0 < Ecc: [{}] < 0.99.", coe.getEccentricity()
        );
    }

    const Real perigee = coe.getPeriapsisRadius().inMeters();
    if (perigee < 3000000.0)
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMean is applicable only if Perigee radius: [{}] > 3000000.0 m.", perigee
        );
    }
    if (perigee < 6378000.0)
    {
        if (!possibleInaccuracyLongWritten)
        {
            std::cout << "Warning: For BrouwerLyddaneMean, when RadPer < 6378km, there is a "
                         "possible inaccuracy due to singularity "
                         "related with inside-of-earth orbit.\n";
            possibleInaccuracyLongWritten = true;
        }
    }

    const Real inclination_degrees = coe.getInclination().inDegrees();
    if (inclination_degrees > 180.0)
    {
        throw ostk::core::error::RuntimeError(
            "BrouwerLyddaneMean is applicable only if inclination: [{}] < 180.0 degrees.", inclination_degrees
        );
    }

    if ((58.80 < inclination_degrees && inclination_degrees < 65.78) ||
        (114.22 < inclination_degrees && inclination_degrees < 121.2))
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

    Vector6d cartesian;
    cartesian.segment(0, 3) = aCartesianState.first.accessCoordinates();
    cartesian.segment(3, 3) = aCartesianState.second.accessCoordinates();

    Vector6d coeVector = coe.getSIVector(COE::AnomalyType::Mean);

    Integer pseudoState = 0;
    if (coeVector[2] > 3.0543261909900763)  // 175.0 degrees
    {
        coeVector[2] = Real::Pi() - coeVector[2];  // INC = 180 - INC
        coeVector[3] = -coeVector[3];              // RAAN = - RAAN
        Position position = Position::Undefined();
        Velocity velocity = Velocity::Undefined();
        std::tie(position, velocity) = COE::FromSIVector(coeVector, COE::AnomalyType::Mean)
                                           .getCartesianState(aGravitationalParameter, Frame::GCRF());
        cartesian.segment(0, 3) = position.getCoordinates();
        cartesian.segment(3, 3) = velocity.getCoordinates();

        pseudoState = 1;
    }

    // TBI: Can make this a static method to convert to Modified Equinoctial elements
    // Modified Equinoctial Elements
    const auto getMEE = [](const Vector6d &aCOEVector) -> Vector6d
    {
        return {
            aCOEVector[0],
            aCOEVector[1] * std::sin(aCOEVector[3] + aCOEVector[4]),
            aCOEVector[1] * std::cos(aCOEVector[3] + aCOEVector[4]),
            std::sin(aCOEVector[2] / 2.0) * std::sin(aCOEVector[3]),
            std::sin(aCOEVector[2] / 2.0) * std::cos(aCOEVector[3]),
            aCOEVector[3] + aCOEVector[4] + aCOEVector[5],
        };
    };

    // TBI: Same as above
    const auto brouwerLyddaneMeanFromMEE = [](const Vector6d &anAeqVector) -> Vector6d
    {
        Real meanInc = Real::Undefined();

        if ((anAeqVector[3] * anAeqVector[3] + anAeqVector[4] * anAeqVector[4]) <= 1.0)
        {
            meanInc = std::acos(1.0 - 2.0 * (anAeqVector[3] * anAeqVector[3] + anAeqVector[4] * anAeqVector[4]));
        }
        else
        {
            meanInc = std::acos(1.0 - 2.0 * 1.0);
        }

        Real meanRaan = std::atan2(anAeqVector[3], anAeqVector[4]);
        if (meanRaan < 0.0)
        {
            meanRaan += Real::TwoPi();
        }

        return {
            anAeqVector[0],
            std::sqrt(anAeqVector[1] * anAeqVector[1] + anAeqVector[2] * anAeqVector[2]),
            meanInc,
            meanRaan,
            std::atan2(anAeqVector[1], anAeqVector[2]) - meanRaan,
            anAeqVector[5] - std::atan2(anAeqVector[1], anAeqVector[2]),
        };
    };

    Vector6d modifiedEquinoctialElements = getMEE(coeVector);

    Vector6d coeVector_2 = toCOEVector(coeVector);
    Vector6d modifiedEquinoctialElements_2 = getMEE(coeVector_2);

    Vector6d modifiedEquinoctialElementsMean = modifiedEquinoctialElements;
    Vector6d modifiedEquinoctialElementsMean_2 =
        modifiedEquinoctialElementsMean + (modifiedEquinoctialElements - modifiedEquinoctialElements_2);

    Vector6d brouwerLyddaneMean;

    Real errorMagnitude = 0.9;
    Real errorMagnitudePrevious = 1.0;
    Integer iter = 0;
    Vector6d cartesian_2;

    const Real tol = 1.0E-8;
    const Integer maxiter = 75;
    while (errorMagnitude > tol)
    {
        brouwerLyddaneMean = brouwerLyddaneMeanFromMEE(modifiedEquinoctialElementsMean_2);
        coeVector_2 = toCOEVector(brouwerLyddaneMean);

        const COE coe_2 = COE::FromSIVector(coeVector_2, COE::AnomalyType::Mean);
        Position position2 = Position::Undefined();
        Velocity velocity2 = Velocity::Undefined();
        std::tie(position2, velocity2) = coe_2.getCartesianState(aGravitationalParameter, Frame::GCRF());
        cartesian_2.segment(0, 3) = position2.getCoordinates();
        cartesian_2.segment(3, 3) = velocity2.getCoordinates();

        const Vector6d delta = cartesian - cartesian_2;

        errorMagnitude = delta.norm() / cartesian.norm();

        if (errorMagnitudePrevious > errorMagnitude)
        {
            errorMagnitudePrevious = errorMagnitude;
            modifiedEquinoctialElements_2 = getMEE(coeVector_2);
            modifiedEquinoctialElementsMean = modifiedEquinoctialElementsMean_2;
            modifiedEquinoctialElementsMean_2 =
                modifiedEquinoctialElementsMean + (modifiedEquinoctialElements - modifiedEquinoctialElements_2);
        }
        else
        {
            std::cout << String::Format(
                "Warning: the iterative algorithm converting from Cartesian "
                "to BrouwerLyddaneMean is not converging. So, it has been "
                "interrupted. The current relative error is {} . \n",
                errorMagnitudePrevious
            );
            break;
        }
        if (iter > maxiter)
        {
            std::cout << "Warning: Maximum iteration number has been reached. There is a possible inaccuracy.\n";
            break;
        }

        ++iter;
    }

    brouwerLyddaneMean = brouwerLyddaneMeanFromMEE(modifiedEquinoctialElementsMean);

    if (pseudoState != 0)
    {
        brouwerLyddaneMean[2] = Real::Pi() - brouwerLyddaneMean[2];
        brouwerLyddaneMean[3] = -brouwerLyddaneMean[3];
    }

    return brouwerLyddaneMean;
}

}  // namespace blm
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
