////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/CustomProp.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_CustomProp__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_CustomProp__

// #include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

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
// using ostk::astro::trajectory::orbit::models::kepler::COE ; should we make a subclass of prop or not really since state is the only relevant "subclass"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CustomProp : public ostk::astro::trajectory::orbit::Model
{

    public:

        enum class PerturbationType // Add astmospheric drag, lunisolar perturbs, and SRP
        {

            None,
            J2,
            J4

        } ;

        typedef std::vector< double > state_type; 
        /* The type of container used to hold the state vector */

                                CustomProp                                  (   const   State&                        aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2,
                                                                                const   Real&                       aJ4,
                                                                                const   CustomProp::PerturbationType&   aPerturbationType                           ) ;

        virtual CustomProp*         clone                                   ( ) const override ;

        bool                    operator ==                                 (   const   CustomProp&                     aCustomPropModel                             ) const ;

        bool                    operator !=                                 (   const   CustomProp&                     aCustomPropModel                             ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   CustomProp&                     aCustomPropModel                             ) ;

        virtual bool            isDefined                                   ( ) const override ;

        // COE                     getClassicalOrbitalElements                 ( ) const ;

        virtual Instant         getEpoch                                    ( ) const override ;

        virtual Integer         getRevolutionNumberAtEpoch                  ( ) const override ;

        Derived                 getGravitationalParameter                   ( ) const ;

        Length                  getEquatorialRadius                         ( ) const ;

        Real                    getJ2                                       ( ) const ;

        Real                    getJ4                                       ( ) const ;

        CustomProp::PerturbationType getPerturbationType                        ( ) const ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        virtual Integer         calculateRevolutionNumberAt                 (   const   Instant&                    anInstant                                   ) const override ; // [TBR] ?

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        static String           StringFromPerturbationType                  (   const   CustomProp::PerturbationType&   aPerturbationType                           ) ;

        static void TwoBodyDynamics( const state_type &x , state_type &dxdt , const double t );

        static void PropLog( const state_type &x , const double t );

    protected:

        virtual bool            operator ==                                 (   const   trajectory::Model&          aModel                                      ) const override ;

        virtual bool            operator !=                                 (   const   trajectory::Model&          aModel                                      ) const override ;

    private:

        State                   state_ ;
        Instant                 epoch_ ;
        Derived                 gravitationalParameter_ ;
        Length                  equatorialRadius_ ;
        Real                    j2_ ;
        Real                    j4_ ;
        CustomProp::PerturbationType perturbationType_ ;

        // static COE              InertialCoeFromFixedCoe                     (   const   COE&                        aClassicalOrbitalElementSet,
        //                                                                         const   Instant&                    anEpoch,
        //                                                                         const   Celestial&                  aCelestialObject                            ) ;

        static State            CalculateStateAt                            (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   ) ;

        static Integer          CalculateRevolutionNumberAt                 (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   ) ;

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
