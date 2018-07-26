////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const Real Tolerance = 1e-30 ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Kepler::Kepler                              (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2,
                                                                                const   Kepler::PerturbationType&   aPerturbationType                           )
                                :   Model(),
                                    coe_(aClassicalOrbitalElementSet),
                                    epoch_(anEpoch),
                                    gravitationalParameter_(aGravitationalParameter),
                                    equatorialRadius_(anEquatorialRadius),
                                    j2_(aJ2),
                                    perturbationType_(aPerturbationType)
{

}

                                Kepler::Kepler                              (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Celestial&                  aCelestialObject,
                                                                                const   Kepler::PerturbationType&   aPerturbationType                           )
                                :   Model(),
                                    coe_(aClassicalOrbitalElementSet),
                                    epoch_(anEpoch),
                                    gravitationalParameter_(aCelestialObject.getGravitationalConstant()),
                                    equatorialRadius_(aCelestialObject.getEquatorialRadius()),
                                    j2_(aCelestialObject.getJ2()),
                                    perturbationType_(aPerturbationType)
{

}

Model*                          Kepler::clone                               ( ) const
{
    return new Kepler(*this) ;
}

bool                            Kepler::operator ==                         (   const   Kepler&                     aKeplerianModel                             ) const
{

    if ((!this->isDefined()) || (!aKeplerianModel.isDefined()))
    {
        return false ;
    }

    return (coe_ == aKeplerianModel.coe_) 
        && (epoch_ == aKeplerianModel.epoch_) 
        && (gravitationalParameter_ == aKeplerianModel.gravitationalParameter_) 
        && (perturbationType_ == aKeplerianModel.perturbationType_) ;

}

bool                            Kepler::operator !=                         (   const   Kepler&                     aKeplerianModel                             ) const
{
    return !((*this) == aKeplerianModel) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Kepler&                     aKeplerianModel                             )
{

    aKeplerianModel.print(anOutputStream) ;

    return anOutputStream ;
    
}

bool                            Kepler::isDefined                           ( ) const
{
    return coe_.isDefined() && epoch_.isDefined() && gravitationalParameter_.isDefined() ;
}

COE                             Kepler::getClassicalOrbitalElements         ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Kepler") ;
    }
    
    return coe_ ;

}

Instant                         Kepler::getEpoch                            ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Kepler") ;
    }
    
    return epoch_ ;

}

Derived                         Kepler::getGravitationalParameter           ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Kepler") ;
    }
    
    return gravitationalParameter_ ;

}

Length                          Kepler::getEquatorialRadius                 ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Kepler") ;
    }
    
    return equatorialRadius_ ;

}

Real                            Kepler::getJ2                               ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Kepler") ;
    }
    
    return j2_ ;

}

Kepler::PerturbationType        Kepler::getPerturbationType                 ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Kepler") ;
    }
    
    return perturbationType_ ;

}

State                           Kepler::calculateStateAt                    (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Kepler") ;
    }

    switch (perturbationType_)
    {

        case Kepler::PerturbationType::None:
            return Kepler::CalculateNoneStateAt(coe_, epoch_, gravitationalParameter_, anInstant) ;

        case Kepler::PerturbationType::J2:
            return Kepler::CalculateJ2StateAt(coe_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_) ;

        default:
            throw library::core::error::runtime::Wrong("Perturbation type") ;

    }

    return State::Undefined() ;

}

Integer                         Kepler::calculateRevolutionNumberAt         (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Kepler") ;
    }

    if (anInstant == epoch_)
    {
        return 1 ;
    }

    switch (perturbationType_)
    {

        case Kepler::PerturbationType::None:
            return Kepler::CalculateNoneRevolutionNumberAt(coe_, epoch_, gravitationalParameter_, anInstant) ;

        case Kepler::PerturbationType::J2:
            return Kepler::CalculateJ2RevolutionNumberAt(coe_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_) ;

        default:
            throw library::core::error::runtime::Wrong("Perturbation type") ;

    }

    return Integer::Undefined() ;

}

void                            Kepler::print                               (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? library::core::utils::Print::Header(anOutputStream, "Kepler") : void () ;

    library::core::utils::Print::Line(anOutputStream) << "Epoch:"               << (epoch_.isDefined() ? epoch_.toString() : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Gravitational parameter:" << (gravitationalParameter_.isDefined() ? gravitationalParameter_.toString() : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Type:"                << Kepler::StringFromPerturbationType(perturbationType_) ;

    library::core::utils::Print::Separator(anOutputStream, "Classical Orbital Elements") ;

    coe_.print(anOutputStream, false) ;

    displayDecorator ? library::core::utils::Print::Footer(anOutputStream) : void () ;

}

String                          Kepler::StringFromPerturbationType          (   const   Kepler::PerturbationType&   aPerturbationType                           )
{

    switch (aPerturbationType)
    {

        case Kepler::PerturbationType::None:
            return "None" ;

        case Kepler::PerturbationType::J2:
            return "J2" ;
        
        default:
            throw library::core::error::runtime::Wrong("Perturbation type") ;

    }

    return String::Empty() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool                            Kepler::operator ==                         (   const   trajectory::Model&          aModel                                      ) const
{

    const Kepler* keplerianModelPtr = dynamic_cast<const Kepler*>(&aModel) ;

    return (keplerianModelPtr != nullptr) && this->operator == (*keplerianModelPtr) ;

}

bool                            Kepler::operator !=                         (   const   trajectory::Model&          aModel                                      ) const
{
    return !((*this) == aModel) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

State                           Kepler::CalculateNoneStateAt                (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   )
{

    using library::physics::units::Time ;
    using library::physics::units::Derived ;
    using library::physics::units::Angle ;
    using library::physics::time::Duration ;

    // Duration from epoch

    const Real durationFromEpoch_s = Duration::Between(anEpoch, anInstant).inSeconds() ;

    // Orbital parameters

    const Real semiMajorAxis_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters() ;
    const Real eccentricity = aClassicalOrbitalElementSet.getEccentricity() ;
    const Real inclination_rad = aClassicalOrbitalElementSet.getInclination().inRadians() ;
    const Real raan_rad = aClassicalOrbitalElementSet.getRaan().inRadians() ;
    const Real aop_rad = aClassicalOrbitalElementSet.getAop().inRadians() ;

    Real trueAnomaly_rad = aClassicalOrbitalElementSet.getTrueAnomaly().inRadians() ;
    
    // Mean motion
    
    const Real meanMotion_radSec = aClassicalOrbitalElementSet.getMeanMotion(aGravitationalParameter).in(Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Second)) ;
    
    if (eccentricity.abs() < Tolerance) // Circular orbit
    {
        trueAnomaly_rad += meanMotion_radSec * durationFromEpoch_s ;
    }
    else
    {
    
        // Mean anomaly

        Real meanAnomaly_rad = aClassicalOrbitalElementSet.getMeanAnomaly().inRadians() ;
        
        meanAnomaly_rad += meanMotion_radSec * durationFromEpoch_s ;
        meanAnomaly_rad = std::fmod(meanAnomaly_rad, 2.0 * M_PI) ;

        // Eccentric anomaly    
        
        const Angle eccentricAnomaly = COE::EccentricAnomalyFromMeanAnomaly(Angle::Radians(meanAnomaly_rad), eccentricity, Tolerance) ;
        
        // True anomaly    
        
        trueAnomaly_rad = COE::TrueAnomalyFromEccentricAnomaly(eccentricAnomaly, eccentricity).inRadians() ;
    
    }

    const COE coe = { Length::Meters(semiMajorAxis_m), eccentricity, Angle::Radians(inclination_rad), Angle::Radians(raan_rad), Angle::Radians(aop_rad), Angle::Radians(trueAnomaly_rad) } ;

    const COE::CartesianState cartesianState = coe.getCartesianState(aGravitationalParameter) ;

    const Position& position = cartesianState.first ;
    const Velocity& velocity = cartesianState.second ;
    
    const State state = { anInstant, position, velocity } ;
    
    return state ;

}

Integer                         Kepler::CalculateNoneRevolutionNumberAt     (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   )
{

    using library::physics::time::Duration ;

    const Duration orbitalPeriod = aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter) ;

    const Duration durationFromEpoch = Duration::Between(anEpoch, anInstant) ;

    return (durationFromEpoch.inSeconds() / orbitalPeriod.inSeconds()).floor() + 1 ;

}

State                           Kepler::CalculateJ2StateAt                  (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2                                         )
{

    using library::physics::units::Mass ;
    using library::physics::units::Time ;
    using library::physics::units::Derived ;
    using library::physics::units::Angle ;
    using library::physics::time::Duration ;

    // Setup

    const Real equatorialRadius_m = anEquatorialRadius.inMeters() ;

    static const Derived::Unit gravitationParameterSIUnit = { Length::Unit::Meter, Derived::Order(3), Mass::Unit::Undefined, Derived::Order::Zero(), Time::Unit::Second, Derived::Order(-2), Angle::Unit::Undefined, Derived::Order::Zero() } ;

	const Real gravitationParameter_SI = aGravitationalParameter.in(gravitationParameterSIUnit) ;

    // Duration from epoch

    const Real durationFromEpoch_s = Duration::Between(anEpoch, anInstant).inSeconds() ;

    // Orbital parameters at epoch

    const Real semiMajorAxisAtEpoch_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters() ;
    const Real eccentricityAtEpoch = aClassicalOrbitalElementSet.getEccentricity() ;
    const Real inclinationAtEpoch_rad = aClassicalOrbitalElementSet.getInclination().inRadians() ;
    const Real raanAtEpoch_rad = aClassicalOrbitalElementSet.getRaan().inRadians() ;
    const Real aopAtEpoch_rad = aClassicalOrbitalElementSet.getAop().inRadians() ;
    const Real meanAnomalyAtEpoch_rad = aClassicalOrbitalElementSet.getMeanAnomaly().inRadians() ;
    
    // Calculation
    // Ref: http://www.ltas-vis.ulg.ac.be/cmsms/uploads/File/Lecture06_AnalyticNumeric_2016-2017.pdf

    const Real n = std::sqrt(gravitationParameter_SI / (semiMajorAxisAtEpoch_m * semiMajorAxisAtEpoch_m * semiMajorAxisAtEpoch_m)) ;
    const Real p = semiMajorAxisAtEpoch_m * (1.0 - eccentricityAtEpoch * eccentricityAtEpoch) ;

    const Real cosInclination = std::cos(inclinationAtEpoch_rad) ;
    const Real sinInclination = std::sin(inclinationAtEpoch_rad) ;
    const Real sinInclinationSquared = sinInclination * sinInclination ;

    const Real expr = (3.0 / 2.0) * aJ2 * std::pow((equatorialRadius_m / p), 2) ;

    const Real n_bar = n * (1.0 + expr * std::sqrt(1.0 - eccentricityAtEpoch * eccentricityAtEpoch) * (1.0 - (3.0 / 2.0) * sinInclinationSquared)) ;
    
    const Real aop_bar_rad = aopAtEpoch_rad + expr * (2.0 - (5.0 / 2.0) * sinInclinationSquared) * n_bar * durationFromEpoch_s ;
    const Real raan_bar_rad = raanAtEpoch_rad - expr * cosInclination * n_bar * durationFromEpoch_s ;
    const Real meanAnomaly_rad = meanAnomalyAtEpoch_rad + n_bar * durationFromEpoch_s ;

    // Orbital parameters at instant

    const Real semiMajorAxis_m = semiMajorAxisAtEpoch_m ;
    const Real eccentricity = eccentricityAtEpoch ;
    const Real inclination_rad = inclinationAtEpoch_rad ;
    const Real raan_rad = raan_bar_rad ;
    const Real aop_rad = aop_bar_rad ;
    const Real trueAnomaly_rad = COE::TrueAnomalyFromEccentricAnomaly(COE::EccentricAnomalyFromMeanAnomaly(Angle::Radians(meanAnomaly_rad), eccentricity, Tolerance), eccentricity).inRadians() ;

    const COE coe = { Length::Meters(semiMajorAxis_m), eccentricity, Angle::Radians(inclination_rad), Angle::Radians(raan_rad), Angle::Radians(aop_rad), Angle::Radians(trueAnomaly_rad) } ;

    const COE::CartesianState cartesianState = coe.getCartesianState(aGravitationalParameter) ;

    const Position& position = cartesianState.first ;
    const Velocity& velocity = cartesianState.second ;
    
    const State state = { anInstant, position, velocity } ;
    
    return state ;

}

Integer                         Kepler::CalculateJ2RevolutionNumberAt       (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2                                         )
{

    using library::physics::time::Duration ;

    return Integer::Undefined() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////