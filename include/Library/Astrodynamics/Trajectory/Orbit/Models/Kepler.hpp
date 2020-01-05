////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler__

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <Library/Astrodynamics/Trajectory/State.hpp>

#include <Library/Physics/Environment/Objects/Celestial.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Length.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Integer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::types::String ;

using ostk::physics::time::Instant ;
using ostk::physics::units::Length ;
using ostk::physics::units::Derived ;
using ostk::physics::env::obj::Celestial ;

using ostk::astro::trajectory::State ;
using ostk::astro::trajectory::orbit::models::kepler::COE ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Kepler : public ostk::astro::trajectory::orbit::Model
{

    public:

        enum class PerturbationType
        {

            None,
            J2

        } ;

                                Kepler                                      (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2,
                                                                                const   Kepler::PerturbationType&   aPerturbationType                           ) ;

                                Kepler                                      (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Celestial&                  aCelestialObject,
                                                                                const   Kepler::PerturbationType&   aPerturbationType,
                                                                                const   bool                        inFixedFrame                                =   false ) ;

        virtual Kepler*         clone                                       ( ) const override ;

        bool                    operator ==                                 (   const   Kepler&                     aKeplerianModel                             ) const ;

        bool                    operator !=                                 (   const   Kepler&                     aKeplerianModel                             ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Kepler&                     aKeplerianModel                             ) ;

        virtual bool            isDefined                                   ( ) const override ;

        COE                     getClassicalOrbitalElements                 ( ) const ;

        virtual Instant         getEpoch                                    ( ) const override ;

        virtual Integer         getRevolutionNumberAtEpoch                  ( ) const override ;

        Derived                 getGravitationalParameter                   ( ) const ;

        Length                  getEquatorialRadius                         ( ) const ;

        Real                    getJ2                                       ( ) const ;

        Kepler::PerturbationType getPerturbationType                        ( ) const ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        virtual Integer         calculateRevolutionNumberAt                 (   const   Instant&                    anInstant                                   ) const override ; // [TBR] ?

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        static String           StringFromPerturbationType                  (   const   Kepler::PerturbationType&   aPerturbationType                           ) ;

    protected:

        virtual bool            operator ==                                 (   const   trajectory::Model&          aModel                                      ) const override ;

        virtual bool            operator !=                                 (   const   trajectory::Model&          aModel                                      ) const override ;

    private:

        COE                     coe_ ;
        Instant                 epoch_ ;
        Derived                 gravitationalParameter_ ;
        Length                  equatorialRadius_ ;
        Real                    j2_ ;
        Kepler::PerturbationType perturbationType_ ;

        static COE              InertialCoeFromFixedCoe                     (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Celestial&                  aCelestialObject                            ) ;

        static State            CalculateNoneStateAt                        (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   ) ;

        static Integer          CalculateNoneRevolutionNumberAt             (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   ) ;

        static State            CalculateJ2StateAt                          (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2                                         ) ;

        static Integer          CalculateJ2RevolutionNumberAt               (   const   COE&                        aClassicalOrbitalElementSet,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2                                         ) ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
