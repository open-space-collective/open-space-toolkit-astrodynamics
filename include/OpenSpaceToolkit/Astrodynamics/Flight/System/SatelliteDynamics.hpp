////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteDynamics.hpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteDynamics__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Data/Vector.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::types::String ;
using ostk::core::types::Shared ;
using ostk::core::ctnr::Array ;

using ostk::math::obj::Vector3d ;

using ostk::physics::Unit ;
using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;
using ostk::physics::units::Length ;
using ostk::physics::units::Mass ;
using ostk::physics::units::Derived ;
using ostk::physics::coord::Position ;
using ostk::physics::coord::Velocity ;
using ostk::physics::coord::Frame ;
using ostk::physics::env::obj::Celestial ;
using ostk::physics::env::Object ;
using ostk::physics::Environment ;
using ostk::physics::env::obj::celest::Earth ;
using ostk::physics::env::obj::celest::Sun ;
using ostk::physics::env::obj::celest::Moon ;
using ostk::physics::data::Vector ;

using ostk::astro::flight::SatelliteSystem ;
using ostk::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines a satellite in orbit subject to forces of varying fidelity. Represents a system of differential equations that can be solved by calling the NumericalSolver class.

class SatelliteDynamics
{

    public:

        enum class StateVectorDimension // Enum class values to represent dimension of state vector
        {
            PositionVelocity = 6,
            PositionVelocitywithDragCoefficient = 7
        } ;

        typedef std::vector<double> StateVector ; // Container used to hold the state vector
        typedef std::function<void(const StateVector&, StateVector&, const double)> DynamicalEquationFuncCallback ; // Function pointer type for returning dynamical equation's pointers

                                SatelliteDynamics                           (   const   Environment&                anEnvironment,
                                                                                const   SatelliteSystem&            aSatelliteSystem,
                                                                                const   State&                      aState,
                                                                                const   SatelliteDynamics::StateVectorDimension&  aStateVectorDimension                   = SatelliteDynamics::StateVectorDimension::PositionVelocity   ) ;

                                SatelliteDynamics                           (   const   SatelliteDynamics&          aSatelliteDynamics                          ) ;

        SatelliteDynamics*      clone                                       ( ) const ;

        bool                    operator ==                                 (   const   SatelliteDynamics&          aSatelliteDynamics                          ) const ;

        bool                    operator !=                                 (   const   SatelliteDynamics&          aSatelliteDynamics                          ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   SatelliteDynamics&          aSatelliteDynamics                          ) ;

        bool                    isDefined                                   ( ) const ;

        void                    print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        SatelliteDynamics::StateVectorDimension getStateVectorDimension                               ( ) const ;

        State                   getState                                    ( ) const ;

        void                    setState                                    (   const   State&                      aState                                      ) ;

        SatelliteDynamics::DynamicalEquationFuncCallback  accessDynamicalEquations ( ) const ;

        static String           StringFromStateVectorDimension              (   const   SatelliteDynamics::StateVectorDimension& aStateVectorDimension                            ) ;

    private:

        mutable Environment     environment_ ;
        Shared<const Frame>     gcrfSPtr_ ;
        SatelliteSystem         satelliteSystem_ ;
        State                   state_ ;
        SatelliteDynamics::StateVectorDimension stateVectorDimension_ ;

        // Only currently used force model that incorporates only Earth's gravity
        void                    DynamicalEquations                          (   const   SatelliteDynamics::StateVector&     x,
                                                                                        SatelliteDynamics::StateVector&     dxdt,
                                                                                const   double                              t                                   ) const ;

        // // Atmospheric perturbations only
        // void                    Exponential_Dynamics                        (   const   SatelliteDynamics::StateVector&     x,
        //                                                                                 SatelliteDynamics::StateVector&     dxdt,
        //                                                                         const   double                                                                  ) const ;

        // // Third Body perturbations only
        // void                    ThirdBodyTable_Dynamics                     (   const   SatelliteDynamics::StateVector&     x,
        //                                                                                 SatelliteDynamics::StateVector&     dxdt,
        //                                                                         const   double                              t                                   ) const ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
