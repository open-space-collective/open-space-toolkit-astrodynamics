////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Tabulated.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Models_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Models_Tabulated__

#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolation/BarycentricRational.hpp>
#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolation/CubicSpline.hpp>
#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolation/Linear.hpp>

#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace models
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Index ;
using ostk::core::types::Size ;
using ostk::core::types::Shared ;
using ostk::core::ctnr::Pair ;
using ostk::core::ctnr::Array ;
using ostk::core::fs::File ;

using ostk::math::obj::VectorXd ;
using ostk::math::obj::MatrixXd ;
using ostk::math::curvefitting::interp::Interpolator ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Interval ;
using ostk::physics::time::Scale ;

using ostk::astro::trajectory::Model ;
using ostk::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define                         DEFAULT_ITERPOLATION_TYPE                       Tabulated::InterpolationType::Linear

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Tabulated trajectory model
///
///                             For now, a simple linear interpolation is performed between steps.
///                             In a future release, more advanced interpolation schemes (quadratic, spline, ...) will be provided.

class Tabulated : public virtual Model
{

    public:

        enum class InterpolationType
        {

            Linear,
            BarycentricRational,
            CubicSpline

        } ;

                                Tabulated                                   (   const   Array<State>&               aStateArray,
                                                                                const   InterpolationType&          anInterpolationType                         = DEFAULT_ITERPOLATION_TYPE ) ;

        virtual Tabulated*      clone                                       ( ) const override ;

        bool                    operator ==                                 (   const   Tabulated&                  aTabulatedModel                             ) const ;

        bool                    operator !=                                 (   const   Tabulated&                  aTabulatedModel                             ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Tabulated&                  aTabulatedModel                             ) ;

        virtual bool            isDefined                                   ( ) const override ;

        Interval                getInterval                                 ( ) const ;

        Tabulated::InterpolationType getInterpolationType                   ( ) const ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        static Tabulated        Load                                        (   const   File&                       aFile                                       ) ;

    protected:

        virtual bool            operator ==                                 (   const   Model&                      aModel                                      ) const override ;

        virtual bool            operator !=                                 (   const   Model&                      aModel                                      ) const override ;

    private:

        Array<State>            states_ ;
        mutable Index           stateIndex_ ;

        InterpolationType       interpolationType_ ;
        std::function<State (const Instant&)> interpolation_ ;
        Array<Shared<Interpolator>> interpolators_ = Array<Shared<Interpolator>>::Empty() ;

        VectorXd                timestamps_ ;
        MatrixXd                coordinates_ ;

        State                   linearInterpolation                         (   const   Instant&                    anInstant                                   ) const ;
        State                   nonlinearInterpolation                      (   const   Instant&                    anInstant                                   ) const ;

        Pair<const State*, const State*> accessStateRangeAt                 (   const   Instant&                    anInstant                                   ) const ;

        Pair<const State*, const State*> accessStateRangeAtIndex            (   const   Index&                      anIndex                                     ) const ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
