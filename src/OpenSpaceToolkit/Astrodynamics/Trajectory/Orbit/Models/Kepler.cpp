/// Apache License 2.0  

#include <iostream>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>

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

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

static const Real Tolerance = 1e-8;
static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

Kepler::Kepler(
    const COE& aClassicalOrbitalElementSet,
    const Instant& anEpoch,
    const Derived& aGravitationalParameter,
    const Length& anEquatorialRadius,
    const Real& aJ2,
    const Real& aJ4,
    const Kepler::PerturbationType& aPerturbationType
)
    : Model(),
      coe_(aClassicalOrbitalElementSet),
      epoch_(anEpoch),
      gravitationalParameter_(aGravitationalParameter),
      equatorialRadius_(anEquatorialRadius),
      j2_(aJ2),
      j4_(aJ4),
      perturbationType_(aPerturbationType)
{
}

Kepler::Kepler(
    const COE& aClassicalOrbitalElementSet,
    const Instant& anEpoch,
    const Celestial& aCelestialObject,
    const Kepler::PerturbationType& aPerturbationType,
    const bool inFixedFrame
)
    : Model(),
      coe_(
          inFixedFrame ? Kepler::InertialCoeFromFixedCoe(aClassicalOrbitalElementSet, anEpoch, aCelestialObject)
                       : aClassicalOrbitalElementSet
      ),
      epoch_(anEpoch),
      gravitationalParameter_(aCelestialObject.getGravitationalParameter()),
      equatorialRadius_(aCelestialObject.getEquatorialRadius()),
      j2_(aCelestialObject.getJ2()),
      j4_(aCelestialObject.getJ4()),
      perturbationType_(aPerturbationType)
{
}

Kepler* Kepler::clone() const
{
    return new Kepler(*this);
}

bool Kepler::operator==(const Kepler& aKeplerianModel) const
{
    if ((!this->isDefined()) || (!aKeplerianModel.isDefined()))
    {
        return false;
    }

    return (coe_ == aKeplerianModel.coe_) && (epoch_ == aKeplerianModel.epoch_) &&
           (gravitationalParameter_ == aKeplerianModel.gravitationalParameter_) &&
           (perturbationType_ == aKeplerianModel.perturbationType_);
}

bool Kepler::operator!=(const Kepler& aKeplerianModel) const
{
    return !((*this) == aKeplerianModel);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Kepler& aKeplerianModel)
{
    aKeplerianModel.print(anOutputStream);

    return anOutputStream;
}

bool Kepler::isDefined() const
{
    return coe_.isDefined() && epoch_.isDefined() && gravitationalParameter_.isDefined();
}

COE Kepler::getClassicalOrbitalElements() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    return coe_;
}

Instant Kepler::getEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    return epoch_;
}

Integer Kepler::getRevolutionNumberAtEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    return 1;  // [TBI] With param
}

Derived Kepler::getGravitationalParameter() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    return gravitationalParameter_;
}

Length Kepler::getEquatorialRadius() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    return equatorialRadius_;
}

Real Kepler::getJ2() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    return j2_;
}

Real Kepler::getJ4() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    return j4_;
}

Kepler::PerturbationType Kepler::getPerturbationType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    return perturbationType_;
}

State Kepler::calculateStateAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    switch (perturbationType_)
    {
        case Kepler::PerturbationType::None:
            return Kepler::CalculateNoneStateAt(coe_, epoch_, gravitationalParameter_, anInstant);

        case Kepler::PerturbationType::J2:
            return Kepler::CalculateJ2StateAt(coe_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_);

        case Kepler::PerturbationType::J4:
            return Kepler::CalculateJ4StateAt(
                coe_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_, j4_
            );

        default:
            throw ostk::core::error::runtime::Wrong("Perturbation type");
    }

    return State::Undefined();
}

Integer Kepler::calculateRevolutionNumberAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Kepler");
    }

    if (anInstant == epoch_)
    {
        return this->getRevolutionNumberAtEpoch();
    }

    switch (perturbationType_)
    {
        case Kepler::PerturbationType::None:
            return Kepler::CalculateNoneRevolutionNumberAt(coe_, epoch_, gravitationalParameter_, anInstant);

        case Kepler::PerturbationType::J2:
            return Kepler::CalculateJ2RevolutionNumberAt(
                coe_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_
            );

        case Kepler::PerturbationType::J4:
            return Kepler::CalculateJ4RevolutionNumberAt(
                coe_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_, j4_
            );

        default:
            throw ostk::core::error::runtime::Wrong("Perturbation type");
    }

    return Integer::Undefined();
}

void Kepler::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Kepler") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Epoch:" << (epoch_.isDefined() ? epoch_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Gravitational parameter:"
        << (gravitationalParameter_.isDefined() ? gravitationalParameter_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream) << "Type:" << Kepler::StringFromPerturbationType(perturbationType_);

    ostk::core::utils::Print::Separator(anOutputStream, "Classical Orbital Elements");

    coe_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

String Kepler::StringFromPerturbationType(const Kepler::PerturbationType& aPerturbationType)
{
    switch (aPerturbationType)
    {
        case Kepler::PerturbationType::None:
            return "None";

        case Kepler::PerturbationType::J2:
            return "J2";

        case Kepler::PerturbationType::J4:
            return "J4";

        default:
            throw ostk::core::error::runtime::Wrong("Perturbation type");
    }

    return String::Empty();
}

bool Kepler::operator==(const trajectory::Model& aModel) const
{
    const Kepler* keplerianModelPtr = dynamic_cast<const Kepler*>(&aModel);

    return (keplerianModelPtr != nullptr) && this->operator==(*keplerianModelPtr);
}

bool Kepler::operator!=(const trajectory::Model& aModel) const
{
    return !((*this) == aModel);
}

COE Kepler::InertialCoeFromFixedCoe(
    const COE& aClassicalOrbitalElementSet, const Instant& anEpoch, const Celestial& aCelestialObject
)
{
    using ostk::physics::coord::Transform;

    const Shared<const Frame> fixedFrame = aCelestialObject.accessFrame();

    static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

    const Transform fixedFrameToInertialFrameTransform = fixedFrame->getTransformTo(gcrfSPtr, anEpoch);

    const COE::CartesianState cartesianStateInFixedFrame =
        aClassicalOrbitalElementSet.getCartesianState(aCelestialObject.getGravitationalParameter(), fixedFrame);

    const Position& positionInFixedFrame = cartesianStateInFixedFrame.first;
    const Velocity& velocityInFixedFrame = cartesianStateInFixedFrame.second;

    const Position positionInInertialFrame = Position::Meters(
        fixedFrameToInertialFrameTransform.applyToPosition(positionInFixedFrame.accessCoordinates()), gcrfSPtr
    );
    const Velocity velocityInInertialFrame = Velocity::MetersPerSecond(
        fixedFrameToInertialFrameTransform.applyToVelocity({0.0, 0.0, 0.0}, velocityInFixedFrame.accessCoordinates()),
        gcrfSPtr
    );

    const COE::CartesianState cartesianStateInInertialFrame = {positionInInertialFrame, velocityInInertialFrame};

    return COE::Cartesian(cartesianStateInInertialFrame, aCelestialObject.getGravitationalParameter());
}

State Kepler::CalculateNoneStateAt(
    const COE& aClassicalOrbitalElementSet,
    const Instant& anEpoch,
    const Derived& aGravitationalParameter,
    const Instant& anInstant
)
{
    using ostk::physics::time::Duration;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Time;

    // Duration from epoch

    const Real durationFromEpoch_s = Duration::Between(anEpoch, anInstant).inSeconds();

    // Orbital parameters

    const Real semiMajorAxis_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters();
    const Real eccentricity = aClassicalOrbitalElementSet.getEccentricity();
    const Real inclination_rad = aClassicalOrbitalElementSet.getInclination().inRadians();
    const Real raan_rad = aClassicalOrbitalElementSet.getRaan().inRadians();
    const Real aop_rad = aClassicalOrbitalElementSet.getAop().inRadians();

    Real trueAnomaly_rad = aClassicalOrbitalElementSet.getTrueAnomaly().inRadians();

    // Mean motion

    const Real meanMotion_radSec = aClassicalOrbitalElementSet.getMeanMotion(aGravitationalParameter)
                                       .in(Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Second));

    if (eccentricity.abs() < Tolerance)  // Circular orbit
    {
        trueAnomaly_rad += meanMotion_radSec * durationFromEpoch_s;
    }
    else
    {
        // Mean anomaly

        Real meanAnomaly_rad = aClassicalOrbitalElementSet.getMeanAnomaly().inRadians();

        meanAnomaly_rad += meanMotion_radSec * durationFromEpoch_s;
        meanAnomaly_rad = std::fmod(meanAnomaly_rad, 2.0 * M_PI);

        // Eccentric anomaly

        const Angle eccentricAnomaly =
            COE::EccentricAnomalyFromMeanAnomaly(Angle::Radians(meanAnomaly_rad), eccentricity, Tolerance);

        // True anomaly

        trueAnomaly_rad = COE::TrueAnomalyFromEccentricAnomaly(eccentricAnomaly, eccentricity).inRadians();
    }

    const COE coe = {
        Length::Meters(semiMajorAxis_m),
        eccentricity,
        Angle::Radians(inclination_rad),
        Angle::Radians(raan_rad),
        Angle::Radians(aop_rad),
        Angle::Radians(trueAnomaly_rad)};

    static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

    const COE::CartesianState cartesianState = coe.getCartesianState(aGravitationalParameter, gcrfSPtr);

    const Position& position = cartesianState.first;
    const Velocity& velocity = cartesianState.second;

    const State state = {anInstant, position, velocity};

    return state;
}

Integer Kepler::CalculateNoneRevolutionNumberAt(
    const COE& aClassicalOrbitalElementSet,
    const Instant& anEpoch,
    const Derived& aGravitationalParameter,
    const Instant& anInstant
)
{
    using ostk::physics::time::Duration;

    const Duration orbitalPeriod = aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter);

    const Duration durationFromEpoch = Duration::Between(anEpoch, anInstant);

    return (durationFromEpoch.inSeconds() / orbitalPeriod.inSeconds()).floor() + 1;
}

State Kepler::CalculateJ2StateAt(
    const COE& aClassicalOrbitalElementSet,
    const Instant& anEpoch,
    const Derived& aGravitationalParameter,
    const Instant& anInstant,
    const Length& anEquatorialRadius,
    const Real& aJ2
)
{
    using ostk::physics::time::Duration;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Mass;
    using ostk::physics::units::Time;

    // Setup

    const Real equatorialRadius_m = anEquatorialRadius.inMeters();

    const Real gravitationalParameter_SI = aGravitationalParameter.in(GravitationalParameterSIUnit);

    // Duration from epoch

    const Real durationFromEpoch_s = Duration::Between(anEpoch, anInstant).inSeconds();

    // Orbital parameters at epoch

    const Real semiMajorAxisAtEpoch_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters();
    const Real eccentricityAtEpoch = aClassicalOrbitalElementSet.getEccentricity();
    const Real inclinationAtEpoch_rad = aClassicalOrbitalElementSet.getInclination().inRadians();
    const Real raanAtEpoch_rad = aClassicalOrbitalElementSet.getRaan().inRadians();
    const Real aopAtEpoch_rad = aClassicalOrbitalElementSet.getAop().inRadians();
    const Real meanAnomalyAtEpoch_rad = aClassicalOrbitalElementSet.getMeanAnomaly().inRadians();

    // Calculation
    // Ref: http://www.s3l.be/usr/files/di/fi/2/Lecture06_AnalyticNumeric_2018-2019_201811142121.pdf

    const Real n = std::sqrt(
        gravitationalParameter_SI / (semiMajorAxisAtEpoch_m * semiMajorAxisAtEpoch_m * semiMajorAxisAtEpoch_m)
    );
    const Real p = semiMajorAxisAtEpoch_m * (1.0 - eccentricityAtEpoch * eccentricityAtEpoch);

    const Real cosInclination = std::cos(inclinationAtEpoch_rad);
    const Real sinInclination = std::sin(inclinationAtEpoch_rad);
    const Real sinInclinationSquared = sinInclination * sinInclination;

    const Real expr = (3.0 / 2.0) * aJ2 * std::pow((equatorialRadius_m / p), 2);

    const Real n_bar = n * (1.0 + expr * std::sqrt(1.0 - eccentricityAtEpoch * eccentricityAtEpoch) *
                                      (1.0 - (3.0 / 2.0) * sinInclinationSquared));

    const Real aop_bar_rad =
        aopAtEpoch_rad + expr * (2.0 - (5.0 / 2.0) * sinInclinationSquared) * n_bar * durationFromEpoch_s;
    const Real raan_bar_rad = raanAtEpoch_rad - expr * cosInclination * n_bar * durationFromEpoch_s;
    const Real meanAnomaly_rad = meanAnomalyAtEpoch_rad + n_bar * durationFromEpoch_s;

    // Orbital parameters at instant

    const Real semiMajorAxis_m = semiMajorAxisAtEpoch_m;
    const Real eccentricity = eccentricityAtEpoch;
    const Real inclination_rad = inclinationAtEpoch_rad;
    const Real raan_rad = raan_bar_rad;
    const Real aop_rad = aop_bar_rad;
    const Real trueAnomaly_rad =
        COE::TrueAnomalyFromEccentricAnomaly(
            COE::EccentricAnomalyFromMeanAnomaly(Angle::Radians(meanAnomaly_rad), eccentricity, Tolerance), eccentricity
        )
            .inRadians();

    const COE coe = {
        Length::Meters(semiMajorAxis_m),
        eccentricity,
        Angle::Radians(inclination_rad),
        Angle::Radians(raan_rad),
        Angle::Radians(aop_rad),
        Angle::Radians(trueAnomaly_rad)};

    static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

    const COE::CartesianState cartesianState = coe.getCartesianState(aGravitationalParameter, gcrfSPtr);

    const Position& position = cartesianState.first;
    const Velocity& velocity = cartesianState.second;

    const State state = {anInstant, position, velocity};

    return state;
}

Integer Kepler::CalculateJ2RevolutionNumberAt(
    const COE& aClassicalOrbitalElementSet,
    const Instant& anEpoch,
    const Derived& aGravitationalParameter,
    const Instant& anInstant,
    const Length& anEquatorialRadius,
    const Real& aJ2
)
{
    using ostk::physics::time::Duration;

    const Real R_m = anEquatorialRadius.inMeters();

    const Real a_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters();
    const Real e = aClassicalOrbitalElementSet.getEccentricity();
    const Real i_rad = aClassicalOrbitalElementSet.getInclination().inRadians();

    // std::cout << "T0 = " << aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter).toString() << " ==
    // " << aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter).inSeconds().toString() << " [s]" <<
    // std::endl ;

    Duration orbitalPeriod = Duration::Undefined();

    // {

    //     // https://en.wikipedia.org/wiki/Nodal_period
    //     //
    //     https://space.stackexchange.com/questions/25868/equation-for-orbital-period-around-oblate-bodies-based-on-j2

    //     if (e > 0.1)
    //     {
    //         throw ostk::core::error::runtime::ToBeImplemented("Kepler::CalculateJ2RevolutionNumberAt -> Only
    //         near-circular orbits are supported.") ;
    //     }

    //     const Real w_rad = aClassicalOrbitalElementSet.getAop().inRadians() ;

    //     const Real sin_i = std::sin(i_rad) ;
    //     const Real sin_i_2 = sin_i * sin_i ;

    //     const Real correctionFactor = 1.0
    //                                 - ((3.0 * aJ2 * (4.0 - 5.0 * sin_i_2)) / (4.0 * std::pow(a_m / R_m, 2) *
    //                                 std::sqrt(1.0 - e * e) * std::pow(1.0 + e * std::cos(w_rad), 2)))
    //                                 - ((3.0 * aJ2 * std::pow(1.0 - e * std::cos(w_rad), 3)) / (2.0 * std::pow(a_m /
    //                                 R_m, 2) * std::pow(1.0 - e * e, 3))) ;

    //     orbitalPeriod = aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter) * correctionFactor ;

    //     std::cout << "B - T = " << orbitalPeriod.toString() << " == " << orbitalPeriod.inSeconds().toString() << "
    //     [s]" << std::endl ; std::cout << "A - DT = " << (orbitalPeriod -
    //     aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter)).toString() << std::endl ;

    // }

    // {

    //     // https://www.mathworks.com/matlabcentral/fileexchange/39550-orbital-periods-of-a-satellite

    //     using ostk::physics::units::Mass ;
    //     using ostk::physics::units::Time ;
    //     using ostk::physics::units::Derived ;
    //     using ostk::physics::units::Angle ;

    //     const Real a_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters() ;
    //     const Real ecc = aClassicalOrbitalElementSet.getEccentricity() ;
    //     const Real inc = aClassicalOrbitalElementSet.getInclination().inRadians() ;
    //     const Real argper = aClassicalOrbitalElementSet.getAop().inRadians() ;

    //     static const Derived::Unit gravitationalParameterSIUnit = { Length::Unit::Meter, Derived::Order(3),
    //     Mass::Unit::Undefined, Derived::Order::Zero(), Time::Unit::Second, Derived::Order(-2),
    //     Angle::Unit::Undefined, Derived::Order::Zero() } ;

    //     const Real gravitationalParameter_SI = aGravitationalParameter.in(gravitationalParameterSIUnit) ;

    //     const double sma = a_m ;
    //     const double mu = gravitationalParameter_SI ;
    //     const double req = R_m ;
    //     const double j2 = aJ2 ;

    //     const double tkepler = 2.0 * M_PI * sma * std::sqrt(sma / mu) ;
    //     const double mm = 2.0 * M_PI / tkepler ;

    //     const double slr = sma * (1.0 - ecc * ecc) ;

    //     const double b = std::sqrt(1.0 - ecc * ecc) ;
    //     const double c = req / slr ;
    //     const double d = c * c ;
    //     const double e = std::sin(inc) * std::sin(inc) ;

    //     const double pmm = mm * (1.0 + 1.5 * j2 * d * std::sqrt(1.0 - ecc * ecc) * (1.0 - 1.5 * e));

    //     const double ar = (sma / req) * (sma / req) ;
    //     const double ep = 1.0 - ecc * ecc ;
    //     const double sw = std::sin(argper) ;
    //     const double pp = 1.0 + ecc * std::cos(argper) ;

    //     const double a = -0.75 * j2 * (4.0 - 5.0 * e) / (ar * std::sqrt(ep) * pp * pp) ;
    //     const double bb = -1.5 * j2 * pp * pp * pp / (ar * ep * ep * ep) ;

    //     const double tnodal = tkepler * (1.0 + a + bb) ;

    //     std::cout << "tnodal = " << tnodal << std::endl ;

    // }

    // {

    //     //
    //     https://books.google.com/books?id=tLU-wjQktoMC&pg=PA50&lpg=PA50&dq=draconic+period+j2&source=bl&ots=GF-KGM4jIr&sig=NG77REHR3SpC4FhRLpX8AfoeBAo&hl=en&sa=X&ved=2ahUKEwia5YTd6L3cAhUDKnwKHfSoCwEQ6AEwAnoECAQQAQ#v=onepage&q=draconic%20period%20j2&f=false

    //     const double T_0 = aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter).inSeconds() ;

    //     const double n_0 = Real::TwoPi() / T_0 ;
    //     const double n = n_0 - (3.0 / 4.0) * n_0 * aJ2 * std::pow(R_m / a_m, 2) * (1.0 - 3.0 * std::cos(i_rad) *
    //     std::cos(i_rad)) ; const double w_dot = - (3.0 / 4.0) * n_0 * aJ2 * std::pow(R_m / a_m, 2) * (1.0 - 5.0 *
    //     std::cos(i_rad) * std::cos(i_rad)) ;

    //     const double T = Real::TwoPi() / (n + w_dot) ;

    //     orbitalPeriod = Duration::Seconds(T) ;

    //     std::cout << "B - T = " << orbitalPeriod.toString() << " == " << orbitalPeriod.inSeconds().toString() << "
    //     [s]" << std::endl ; std::cout << "B - DT = " << (orbitalPeriod -
    //     aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter)).toString() << std::endl ;

    // }

    {
        // https://arxiv.org/pdf/1609.00915.pdf (7.37)

        const double T_0 = aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter).inSeconds();

        const double n_0 = Real::TwoPi() / T_0;
        const double n = n_0 - (3.0 / 4.0) * n_0 * aJ2 * std::pow(R_m / a_m, 2) / std::pow(1.0 - e * e, 3.0 / 2.0) *
                                   (1.0 - 3.0 * std::cos(i_rad) * std::cos(i_rad));
        const double w_dot = -(3.0 / 4.0) * n_0 * aJ2 * std::pow(R_m / a_m, 2) / std::pow(1.0 - e * e, 2) *
                             (1.0 - 5.0 * std::cos(i_rad) * std::cos(i_rad));

        const double T = Real::TwoPi() / (n + w_dot);

        orbitalPeriod = Duration::Seconds(T);

        // std::cout << "B - T = " << orbitalPeriod.toString() << " == " << orbitalPeriod.inSeconds().toString() << "
        // [s]" << std::endl ; std::cout << "B - DT = " << (orbitalPeriod -
        // aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter)).toString() << std::endl ;
    }

    const Duration durationFromEpoch = Duration::Between(anEpoch, anInstant);

    return (durationFromEpoch.inSeconds() / orbitalPeriod.inSeconds()).floor() + 1;
}

State Kepler::CalculateJ4StateAt(
    const COE& aClassicalOrbitalElementSet,
    const Instant& anEpoch,
    const Derived& aGravitationalParameter,
    const Instant& anInstant,
    const Length& anEquatorialRadius,
    const Real& aJ2,
    const Real& aJ4
)
{
    using ostk::physics::time::Duration;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Mass;
    using ostk::physics::units::Time;

    // Setup

    const Real equatorialRadius_m = anEquatorialRadius.inMeters();

    const Real gravitationalParameter_SI = aGravitationalParameter.in(GravitationalParameterSIUnit);

    // Duration from epoch

    const Real durationFromEpoch_s = Duration::Between(anEpoch, anInstant).inSeconds();

    // Orbital parameters at epoch

    const Real semiMajorAxisAtEpoch_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters();
    const Real eccentricityAtEpoch = aClassicalOrbitalElementSet.getEccentricity();
    const Real inclinationAtEpoch_rad = aClassicalOrbitalElementSet.getInclination().inRadians();
    const Real raanAtEpoch_rad = aClassicalOrbitalElementSet.getRaan().inRadians();
    const Real aopAtEpoch_rad = aClassicalOrbitalElementSet.getAop().inRadians();
    const Real meanAnomalyAtEpoch_rad = aClassicalOrbitalElementSet.getMeanAnomaly().inRadians();

    // Calculation
    // Ref: Vallado, D. A (2013). Fundamentals of Astrodynamics and Applications.
    // Ref: Escobal, P. R (1965). Methods of Orbit Determination.

    const Real n = std::sqrt(
        gravitationalParameter_SI / (semiMajorAxisAtEpoch_m * semiMajorAxisAtEpoch_m * semiMajorAxisAtEpoch_m)
    );
    const Real p = semiMajorAxisAtEpoch_m * (1.0 - eccentricityAtEpoch * eccentricityAtEpoch);

    const Real cosInclination = std::cos(inclinationAtEpoch_rad);
    const Real sinInclination = std::sin(inclinationAtEpoch_rad);
    const Real cosInclinationSquared = cosInclination * cosInclination;
    const Real sinInclinationSquared = sinInclination * sinInclination;

    const Real eccentricityAtEpochSquared = eccentricityAtEpoch * eccentricityAtEpoch;
    const Real sqrtBeta = std::sqrt(1.0 - eccentricityAtEpochSquared);

    const Real expr = (3.0 / 2.0) * aJ2 * std::pow((equatorialRadius_m / p), 2);

    const Real n_bar =
        n *
        (1.0 + expr * sqrtBeta * ((1.0 - (3.0 / 2.0) * sinInclinationSquared)) +
         3.0 / 128.0 * aJ2 * aJ2 * std::pow(equatorialRadius_m / p, 4) * sqrtBeta *
             (16.0 * sqrtBeta + 25.0 * (1.0 - eccentricityAtEpochSquared) - 15.0 +
              (30.0 - 96.0 * sqrtBeta - 90.0 * (1.0 - eccentricityAtEpochSquared)) * cosInclinationSquared +
              (105.0 + 144.0 * sqrtBeta + 25.0 * (1.0 - eccentricityAtEpochSquared)) * std::pow(cosInclination, 4)) -
         45.0 / 128.0 * aJ4 * eccentricityAtEpochSquared * std::pow(equatorialRadius_m / p, 4) * sqrtBeta *
             (3.0 - 30.0 * cosInclinationSquared + 35.0 * std::pow(cosInclination, 4)));

    const Real raan_dot =
        -n_bar * expr * cosInclination *
            (1.0 +
             expr * ((3.0 / 2.0) + eccentricityAtEpochSquared / 6.0 - 2.0 * sqrtBeta -
                     (5.0 / 3.0 - 5.0 / 24.0 * eccentricityAtEpochSquared - 3.0 * sqrtBeta) * sinInclinationSquared)) -
        35.0 / 8.0 * n * aJ4 * std::pow(equatorialRadius_m / p, 4) * cosInclination *
            (1.0 + (3.0 / 2.0) * eccentricityAtEpochSquared) * (12.0 - 21.0 * sinInclinationSquared) / 14.0;

    const Real aop_dot =
        n_bar * expr * (2.0 - (5.0 / 2.0) * sinInclinationSquared) *
            (1.0 + expr * (2.0 + eccentricityAtEpochSquared / 2.0 - 2.0 * sqrtBeta -
                           (43.0 / 24.0 - eccentricityAtEpochSquared / 48.0 - 3.0 * sqrtBeta) * sinInclinationSquared)
            ) -
        45.0 / 36.0 * aJ2 * aJ2 * n * std::pow(equatorialRadius_m / p, 4) * eccentricityAtEpochSquared *
            std::pow(cosInclination, 4) -
        35.0 / 8.0 * n * aJ4 * std::pow(equatorialRadius_m / p, 4) *
            (12.0 / 7.0 - 93.0 / 14.0 * sinInclinationSquared + 21.0 / 4.0 * std::pow(sinInclination, 4) +
             eccentricityAtEpochSquared *
                 (27.0 / 14.0 - 189.0 / 28.0 * sinInclinationSquared + 81.0 / 16.0 * std::pow(sinInclination, 4)));

    const Real raan_bar_rad = raanAtEpoch_rad + raan_dot * durationFromEpoch_s;
    const Real aop_bar_rad = aopAtEpoch_rad + aop_dot * durationFromEpoch_s;
    const Real meanAnomaly_rad = meanAnomalyAtEpoch_rad + n_bar * durationFromEpoch_s;

    // Orbital parameters at instant

    const Real semiMajorAxis_m = semiMajorAxisAtEpoch_m;
    const Real eccentricity = eccentricityAtEpoch;
    const Real inclination_rad = inclinationAtEpoch_rad;
    const Real raan_rad = raan_bar_rad;
    const Real aop_rad = aop_bar_rad;
    const Real trueAnomaly_rad =
        COE::TrueAnomalyFromEccentricAnomaly(
            COE::EccentricAnomalyFromMeanAnomaly(Angle::Radians(meanAnomaly_rad), eccentricity, Tolerance), eccentricity
        )
            .inRadians();

    const COE coe = {
        Length::Meters(semiMajorAxis_m),
        eccentricity,
        Angle::Radians(inclination_rad),
        Angle::Radians(raan_rad),
        Angle::Radians(aop_rad),
        Angle::Radians(trueAnomaly_rad)};

    static const Shared<const Frame> gcrfSPtr = Frame::GCRF();

    const COE::CartesianState cartesianState = coe.getCartesianState(aGravitationalParameter, gcrfSPtr);

    const Position& position = cartesianState.first;
    const Velocity& velocity = cartesianState.second;

    const State state = {anInstant, position, velocity};

    return state;
}

Integer Kepler::CalculateJ4RevolutionNumberAt(
    const COE& aClassicalOrbitalElementSet,
    const Instant& anEpoch,
    const Derived& aGravitationalParameter,
    const Instant& anInstant,
    const Length& anEquatorialRadius,
    const Real& aJ2,
    const Real& aJ4
)
{
    using ostk::physics::time::Duration;

    const Real R_m = anEquatorialRadius.inMeters();

    const Real a_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters();
    const Real e = aClassicalOrbitalElementSet.getEccentricity();
    const Real i_rad = aClassicalOrbitalElementSet.getInclination().inRadians();

    const Real p = a_m * (1.0 - e * e);

    Duration orbitalPeriod = Duration::Undefined();

    {
        const double T_0 = aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter).inSeconds();

        const double n_0 = Real::TwoPi() / T_0;

        const Real expr = (3.0 / 2.0) * aJ2 * std::pow((R_m / p), 2);

        const Real n_bar =
            n_0 *
            (1.0 + expr * std::sqrt(1.0 - e * e) * ((1.0 - (3.0 / 2.0) * std::pow(std::sin(i_rad), 2))) +
             3.0 / 128.0 * aJ2 * aJ2 * std::pow(R_m / p, 4) * std::sqrt(1.0 - e * e) *
                 (16.0 * std::sqrt(1.0 - e * e) + 25.0 * (1.0 - e * e) - 15.0 +
                  (30.0 - 96.0 * std::sqrt(1.0 - e * e) - 90.0 * (1.0 - e * e)) * std::pow(std::cos(i_rad), 2) +
                  (105.0 + 144.0 * std::sqrt(1.0 - e * e) + 25.0 * (1.0 - e * e)) * std::pow(std::cos(i_rad), 4)) -
             45.0 / 128.0 * aJ4 * e * e * std::pow(R_m / p, 4) * std::sqrt(1.0 - e * e) *
                 (3.0 - 30.0 * std::pow(std::cos(i_rad), 2) + 35.0 * std::pow(std::cos(i_rad), 4)));

        const Real w_dot =
            n_bar * expr * (2.0 - (5.0 / 2.0) * std::pow(std::sin(i_rad), 2)) *
                (1.0 +
                 expr * (2.0 + e * e / 2.0 - 2.0 * std::sqrt(1.0 - e * e) -
                         (43.0 / 24.0 - e * e / 48.0 - 3.0 * std::sqrt(1.0 - e * e)) * std::pow(std::sin(i_rad), 2))) -
            45.0 / 36.0 * aJ2 * aJ2 * n_0 * std::pow(R_m / p, 4) * e * e * std::pow(std::cos(i_rad), 4) -
            35.0 / 8.0 * n_0 * aJ4 * std::pow(R_m / p, 4) *
                (12.0 / 7.0 - 93.0 / 14.0 * std::pow(std::sin(i_rad), 2) + 21.0 / 4.0 * std::pow(std::sin(i_rad), 4) +
                 e * e *
                     (27.0 / 14.0 - 189.0 / 28.0 * std::pow(std::sin(i_rad), 2) +
                      81.0 / 16.0 * std::pow(std::sin(i_rad), 4)));

        const double T = Real::TwoPi() / (n_bar + w_dot);

        orbitalPeriod = Duration::Seconds(T);
    }

    const Duration durationFromEpoch = Duration::Between(anEpoch, anInstant);

    return (durationFromEpoch.inSeconds() / orbitalPeriod.inSeconds()).floor() + 1;
}

}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
