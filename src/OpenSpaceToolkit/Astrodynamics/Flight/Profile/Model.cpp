////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{
namespace profile
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Model::Model                                ( )
{

}

                                Model::~Model                               ( )
{

}

bool                            Model::operator !=                          (   const   Model&                      aModel                                      ) const
{
    return !((*this) == aModel) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Model&                      aModel                                      )
{

    aModel.print(anOutputStream) ;

    return anOutputStream ;

}

Array<State>                    Model::calculateStatesAt                    (   const   Array<Instant>&             anInstantArray                              ) const
{

    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty() ;
    }

    Array<State> stateArray = Array<State>::Empty() ;

    stateArray.reserve(anInstantArray.getSize()) ;

    for (const auto& instant : anInstantArray)
    {
        stateArray.add(this->calculateStateAt(instant)) ;
    }

    return stateArray ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
