////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated__

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

using ostk::math::obj::Vector3d ;

using ostk::physics::time::Instant ;
using ostk::physics::units::Length ;
using ostk::physics::units::Derived ;
using ostk::physics::env::obj::Celestial ;

using ostk::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Propagated : public ostk::astro::trajectory::orbit::Model
{

    public:

        enum class GravitationalPerturbationType
        {

            None,
            J2,
            TenByTen,
            FourtyByFourty

        } ;

        enum class AtmosphericPerturbationType
        {

            None,
            Exponential,
            JacchiaRoberts,
            NRLMISIS00

        } ;

        enum class ThirdBodyPerturbationType
        {

            None,
            Luni,
            Solar,
            LuniSolar

        } ;

        enum class IntegrationStepperType
        {
            RungeKuttaCashKarp54
        } ;

        enum class IntegrationLogType
        {
            NoLog,
            LogConstant,
            LogAdaptive
        } ;

        /* The type of container used to hold the state vector */
        typedef std::vector<double> state_type;

                                Propagated                                  (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Propagated::GravitationalPerturbationType&       aGravitationalPerturbationType,
                                                                                const   Propagated::AtmosphericPerturbationType&      anAtmosphericPerturbationType,
                                                                                const   Propagated::ThirdBodyPerturbationType&  aThirdBodyPerturbationType,
                                                                                const   Propagated::IntegrationStepperType& anIntegrationStepperType            = Propagated::IntegrationStepperType::RungeKuttaCashKarp54,
                                                                                const   Propagated::IntegrationLogType& anIntegrationLogType                    = Propagated::IntegrationLogType::NoLog,
                                                                                const   Real&                       aRelativeTolerance                          = 1.0e-15,
                                                                                const   Real&                       anAbsoluteTolerance                         = 1.0e-15   ) ;

                                Propagated                                  (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Celestial&                  aCelestialObject,
                                                                                const   Propagated::GravitationalPerturbationType&       aGravitationalPerturbationType,
                                                                                const   Propagated::AtmosphericPerturbationType&      anAtmosphericPerturbationType,
                                                                                const   Propagated::ThirdBodyPerturbationType&  aThirdBodyPerturbationType,
                                                                                const   Propagated::IntegrationStepperType& anIntegrationStepperType            = Propagated::IntegrationStepperType::RungeKuttaCashKarp54,
                                                                                const   Propagated::IntegrationLogType& anIntegrationLogType                    = Propagated::IntegrationLogType::NoLog,
                                                                                const   Real&                       aRelativeTolerance                          = 1.0e-15,
                                                                                const   Real&                       anAbsoluteTolerance                         = 1.0e-15   ) ;

        virtual Propagated*     clone                                       ( ) const override ;

        bool                    operator ==                                 (   const   Propagated&                 aPropagatedModel                             ) const ;

        bool                    operator !=                                 (   const   Propagated&                 aPropagatedModel                             ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Propagated&                 aPropagatedModel                             ) ;

        virtual bool            isDefined                                   ( ) const override ;

        virtual Instant         getEpoch                                    ( ) const override ;

        virtual Integer         getRevolutionNumberAtEpoch                  ( ) const override ;

        Derived                 getGravitationalParameter                   ( ) const ;

        Length                  getEquatorialRadius                         ( ) const ;

        Propagated::GravitationalPerturbationType getGravitationalPerturbationType            ( ) const ;

        Propagated::AtmosphericPerturbationType getAtmosphericPerturbationType          ( ) const ;

        Propagated::ThirdBodyPerturbationType getThirdBodyPerturbationType  ( ) const ;

        Propagated::IntegrationStepperType getIntegrationStepperType        ( ) const ;

        Propagated::IntegrationLogType getIntegrationLogType                ( ) const ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        virtual Integer         calculateRevolutionNumberAt                 (   const   Instant&                    anInstant                                   ) const override ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        static String           StringFromGravitationalPerturbationType     (   const   Propagated::GravitationalPerturbationType&       aGravitationalPerturbationType           ) ;

        static String           StringFromAtmosphericPerturbationType       (   const   Propagated::AtmosphericPerturbationType&      anAtmosphericPerturbationType          ) ;

        static String           StringFromThirdBodyPerturbationType         (   const   Propagated::ThirdBodyPerturbationType&  aThirdBodyPerturbationType      ) ;

        static String           StringFromIntegrationStepperType            (   const   Propagated::IntegrationStepperType&  aIntegrationStepperType            ) ;

        static String           StringFromIntegrationLogType                (   const   Propagated::IntegrationLogType&  aIntegrationLogType                    ) ;

    protected:

        virtual bool            operator ==                                 (   const   trajectory::Model&          aModel                                      ) const override ;

        virtual bool            operator !=                                 (   const   trajectory::Model&          aModel                                      ) const override ;

    private:

        State                   state_ ;
        Instant                 epoch_ ;
        Derived                 gravitationalParameter_ ;
        Length                  equatorialRadius_ ;
        Propagated::GravitationalPerturbationType gravitationalPerturbationType_ ;
        Propagated::AtmosphericPerturbationType atmosphericPerturbationType_ ;
        Propagated::ThirdBodyPerturbationType thirdBodyPerturbationType_ ;
        Propagated::IntegrationStepperType integrationStepperType_ ;
        Propagated::IntegrationLogType integrationLogType_ ;
        Real relativeTolerance_ ;
        Real absoluteTolerance_ ;

        static void             TwoBodyDynamics                             (   const   state_type&                 x,
                                                                                        state_type&                 dxdt,
                                                                                const   double                      t,
                                                                                const   double&                     mu_SI                                       ) ;

        static void             NumericalIntegrationLogger                  (   const   state_type&                 x,
                                                                                const   double                      t                                           ) ;

        static State            CalculateNoneStateAt                        (   const   State&                      aState,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant,
                                                                                const   Propagated::IntegrationStepperType& anIntegrationStepperType,
                                                                                const   Propagated::IntegrationLogType& anIntegrationLogType,
                                                                                const   Real&                       aRelativeTolerance,
                                                                                const   Real&                       anAbsoluteTolerance                         ) ;

        static Integer          CalculateNoneRevolutionNumberAt             (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   ) ;
        //[TBI] add more complicated combinations of force models
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
