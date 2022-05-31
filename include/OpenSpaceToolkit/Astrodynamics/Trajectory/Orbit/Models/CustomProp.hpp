////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/CustomProp.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_CustomProp__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_CustomProp__

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CustomProp : public ostk::astro::trajectory::orbit::Model
{

    public:

        enum class PerturbationType // Add astmospheric drag, lunisolar perturbs, and SRP as seperate enums for each type of perturbation type, so that they can be turned on an off independently during validation
        {

            None,
            J2,
            J4

        } ;

        /* The type of container used to hold the state vector */
        typedef std::vector< double > state_type; 

                                CustomProp                                  (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2,
                                                                                const   Real&                       aJ4,
                                                                                const   CustomProp::PerturbationType&   aPerturbationType                           ) ;

        virtual CustomProp*         clone                                   ( ) const override ;

        bool                    operator ==                                 (   const   CustomProp&                 aCustomPropModel                             ) const ;

        bool                    operator !=                                 (   const   CustomProp&                 aCustomPropModel                             ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   CustomProp&                 aCustomPropModel                             ) ;

        virtual bool            isDefined                                   ( ) const override ;

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

        static State            CalculateNoneStateAt                        (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   ) ;

        static Integer          CalculateNoneRevolutionNumberAt             (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   ) ;

        // static State            CalculateJ2StateAt                          (   const   State&                      aState,
        //                                                                         const   Instant&                    anEpoch,
        //                                                                         const   Derived&                    aGravitationalParameter,
        //                                                                         const   Instant&                    anInstant,
        //                                                                         const   Length&                     anEquatorialRadius,
        //                                                                         const   Real&                       aJ2                                         ) ;

        // static Integer          CalculateJ2RevolutionNumberAt               (   const   State&                      aState,
        //                                                                         const   Instant&                    anEpoch,
        //                                                                         const   Derived&                    aGravitationalParameter,
        //                                                                         const   Instant&                    anInstant,
        //                                                                         const   Length&                     anEquatorialRadius,
        //                                                                         const   Real&                       aJ2                                         ) ;

        // static State            CalculateJ4StateAt                          (   const   State&                      aState,
        //                                                                         const   Instant&                    anEpoch,
        //                                                                         const   Derived&                    aGravitationalParameter,
        //                                                                         const   Instant&                    anInstant,
        //                                                                         const   Length&                     anEquatorialRadius,
        //                                                                         const   Real&                       aJ2,
        //                                                                         const   Real&                       aJ4                                         ) ;

        // static Integer          CalculateJ4RevolutionNumberAt               (   const   State&                      aState,
        //                                                                         const   Instant&                    anEpoch,
        //                                                                         const   Derived&                    aGravitationalParameter,
        //                                                                         const   Instant&                    anInstant,
        //                                                                         const   Length&                     anEquatorialRadius,
        //                                                                         const   Real&                       aJ2,
        //                                                                         const   Real&                       aJ4                                         ) ;
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
