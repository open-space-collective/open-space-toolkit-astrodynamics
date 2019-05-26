////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Determination/BatchLeastSquares.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Determination_BatchLeastSquares__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Determination_BatchLeastSquares__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
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
namespace determination
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Integer ;
using ostk::core::types::Size ;
using ostk::core::types::String ;
using ostk::core::ctnr::Pair ;
using ostk::core::ctnr::Array ;

using ostk::math::obj::VectorXd ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;
using ostk::physics::time::Interval ;
using ostk::physics::coord::Position ;
using ostk::physics::Environment ;

using ostk::astro::trajectory::Orbit ;

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
