////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Models/Tabulated.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Models_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Models_Tabulated__

#include <Library/Astrodynamics/Trajectory/State.hpp>
#include <Library/Astrodynamics/Trajectory/Model.hpp>

#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/FileSystem/File.hpp>
#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Containers/Pair.hpp>
#include <Library/Core/Types/Index.hpp>

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
using ostk::core::ctnr::Pair ;
using ostk::core::ctnr::Array ;
using ostk::core::fs::File ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Interval ;

using ostk::astro::trajectory::Model ;
using ostk::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Tabulated trajectory model
///
///                             For now, a simple linear interpolation is performed between steps.
///                             In a future release, more advanced interpolation schemes (quadratic, spline, ...) will be provided.

class Tabulated : public virtual Model
{

    public:

                                Tabulated                                   (   const   Array<State>&               aStateArray                                 ) ;

        virtual Tabulated*      clone                                       ( ) const override ;

        bool                    operator ==                                 (   const   Tabulated&                  aTabulatedModel                             ) const ;

        bool                    operator !=                                 (   const   Tabulated&                  aTabulatedModel                             ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Tabulated&                  aTabulatedModel                             ) ;

        virtual bool            isDefined                                   ( ) const override ;

        Interval                getInterval                                 ( ) const ;

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
