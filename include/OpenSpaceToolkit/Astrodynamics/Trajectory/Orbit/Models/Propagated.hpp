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
#include <OpenSpaceToolkit/Astrodynamics/Flight/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>

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
using ostk::core::ctnr::Array ;

using ostk::math::obj::Vector3d ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Scale ;
using ostk::physics::units::Length ;
using ostk::physics::units::Derived ;
using ostk::physics::env::obj::Celestial ;

using ostk::astro::trajectory::State ;
using ostk::astro::flight::SatelliteSystem ;
using ostk::astro::flight::system::SatelliteDynamics ;
using ostk::astro::NumericalSolver ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Propagated : public ostk::astro::trajectory::orbit::Model
{

    public:

                                Propagated                                  (   const   SatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver                            ) ; //[TBI] Could add the passing in of a shared celestial pointer object in here insterad of in Satellite Dynamics

                                Propagated                                  (   const   SatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver,
                                                                                const   Array<State>&               aCachedStateArray                           ) ;

        virtual Propagated*     clone                                       ( ) const override ;

        bool                    operator ==                                 (   const   Propagated&                 aPropagatedModel                            ) const ;

        bool                    operator !=                                 (   const   Propagated&                 aPropagatedModel                            ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Propagated&                 aPropagatedModel                            ) ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        virtual bool            isDefined                                   ( ) const override ;

        virtual Instant         getEpoch                                    ( ) const override ;

        virtual Integer         getRevolutionNumberAtEpoch                  ( ) const override ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        virtual Integer         calculateRevolutionNumberAt                 (   const   Instant&                    anInstant                                   ) const override ;

        const Array<State>&     accessCachedStateArray                      ( ) const ;

    protected:

        virtual bool            operator ==                                 (   const   trajectory::Model&          aModel                                      ) const override ;

        virtual bool            operator !=                                 (   const   trajectory::Model&          aModel                                      ) const override ;

    private:

        mutable Array<State>    cachedStateArray_ ;
        mutable SatelliteDynamics satelliteDynamics_ ;
        mutable NumericalSolver numericalSolver_ ;

        void                    sortStateArray                              ( )     ;


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
