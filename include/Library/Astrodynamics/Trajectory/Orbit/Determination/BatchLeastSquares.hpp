////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Determination/BatchLeastSquares.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit_Determination_BatchLeastSquares__
#define __Library_Astrodynamics_Trajectory_Orbit_Determination_BatchLeastSquares__

#include <Library/Astrodynamics/Trajectory/Orbit.hpp>

#include <Library/Physics/Environment.hpp>
#include <Library/Physics/Coordinate/Position.hpp>
#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Mathematics/Objects/Vector.hpp>

#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Containers/Pair.hpp>
#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Size.hpp>
#include <Library/Core/Types/Integer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace determination
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Integer ;
using library::core::types::Size ;
using library::core::types::String ;
using library::core::ctnr::Pair ;
using library::core::ctnr::Array ;

using library::math::obj::VectorXd ;

using library::physics::time::Instant ;
using library::physics::time::Duration ;
using library::physics::time::Interval ;
using library::physics::coord::Position ;
using library::physics::Environment ;

using library::astro::trajectory::Orbit ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Orbit determination using Batch Least Squares method
///
/// @ref                        https://www.agi.com/news/blog/april-2018/batch-vs-sequential-estimation-methods-in-orbit-d

class BatchLeastSquares
{

    public:

        struct Configuration
        {

            String algorithm ;

            VectorXd initialState = VectorXd::Undefined(6) ;

            VectorXd stateLowerBound = VectorXd::Undefined(6) ;
            VectorXd stateUpperBound = VectorXd::Undefined(6) ;

            Real stoppingFunctionValue = Real::Undefined() ;

            Real relativeStateTolerance = Real::Undefined() ;

            Real relativeFunctionTolerance = Real::Undefined() ;
            Real absoluteFunctionTolerance = Real::Undefined() ;

            Integer maximumEvaluationCount = Integer::Undefined() ;
            Duration maximumDuration = Duration::Undefined() ;

            bool verbose = false ;

        } ;

        struct Output
        {

            Orbit orbit ;
            Real residuals ;
            Size evaluationCount ;
            String message ;

        } ;

                                BatchLeastSquares                           (   const   Environment&                anEnvironment,
                                                                                const   BatchLeastSquares::Configuration& aConfiguration                        ) ;

        BatchLeastSquares::Output estimateOrbit                             (   const   Array<Pair<Instant, Position>>& aMeasurementArray                       ) const ;

    private:

        Environment             environment_ ;
        BatchLeastSquares::Configuration configuration_ ;

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
