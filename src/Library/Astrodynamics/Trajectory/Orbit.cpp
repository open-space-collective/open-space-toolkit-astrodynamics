////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Tabulated.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Orbit::Orbit                                (   const   orbit::Model&               aModel                                      )
                                :   Trajectory(aModel),
                                    model_(dynamic_cast<const orbit::Model&>(this->accessModel()))
{

}

                                Orbit::Orbit                                (   const   Array<State>&               aStateArray,
                                                                                const   Integer&                    anInitialRevolutionNumber                   )
                                :   Trajectory(orbit::models::Tabulated(aStateArray, anInitialRevolutionNumber)),
                                    model_(dynamic_cast<const orbit::Model&>(this->accessModel()))
{

}

                                Orbit::Orbit                                (   const   Orbit&                      anOrbit                                     )
                                :   Trajectory(anOrbit),
                                    model_(dynamic_cast<const orbit::Model&>(this->accessModel()))
{

}

bool                            Orbit::operator ==                          (   const   Orbit&                      anOrbit                                     ) const
{

    if ((!this->isDefined()) || (!anOrbit.isDefined()))
    {
        return false ;
    }

    return Trajectory::operator == (anOrbit) ;

}

bool                            Orbit::operator !=                          (   const   Orbit&                      anOrbit                                     ) const
{
    return !((*this) == anOrbit) ;
}

bool                            Orbit::isDefined                            ( ) const
{
    return Trajectory::isDefined() ;
}

Integer                         Orbit::getRevolutionNumberAt                (   const   Instant&                    anInstant                                   ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Orbit") ;
    }

    return model_.calculateRevolutionNumberAt(anInstant) ;

}

Pass                            Orbit::getPassAt                            (   const   Instant&                    anInstant                                   ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Orbit") ;
    }

    return this->getPassWithRevolutionNumber(this->getRevolutionNumberAt(anInstant)) ;
    
}

Pass                            Orbit::getPassWithRevolutionNumber          (   const   Integer&                    aRevolutionNumber                           ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Orbit") ;
    }

    std::lock_guard<std::mutex> lock(mutex_) ;

    const auto passMapIt = passMap_.find(aRevolutionNumber) ;

    if (passMapIt != passMap_.end())
    {
        return passMapIt->second ;
    }
    else
    {

        throw library::core::error::runtime::ToBeImplemented("Orbit :: getPassWithRevolutionNumber") ;

    }

    return Pass::Undefined() ;
    
}

void                            Orbit::print                                (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? library::core::utils::Print::Header(anOutputStream, "Orbit") : void () ;

    Trajectory::print(anOutputStream, false) ;

    std::lock_guard<std::mutex> lock(mutex_) ;

    for (const auto& passIt : passMap_)
    {

        const Pass& pass = passIt.second ;

        library::core::utils::Print::Separator(anOutputStream, String::Format("Pass #{}", pass.getRevolutionNumber())) ;

        library::core::utils::Print::Line(anOutputStream) << "Type:"            << Pass::StringFromType(pass.getType()) ;
        library::core::utils::Print::Line(anOutputStream) << "Revolution #:"    << pass.getRevolutionNumber().toString() ;
        library::core::utils::Print::Line(anOutputStream) << "Start time:"      << pass.getInterval().accessStart().toString() ;
        library::core::utils::Print::Line(anOutputStream) << "End time:"        << pass.getInterval().accessEnd().toString() ;
        library::core::utils::Print::Line(anOutputStream) << "Duration:"        << pass.getInterval().getDuration().toString() ;

    }

    displayDecorator ? library::core::utils::Print::Footer(anOutputStream) : void () ;

}

Orbit                           Orbit::Undefined                            ( )
{
    return Orbit(Array<State>::Empty(), Integer::Undefined()) ;
}

// Map<Index, Pass>                Orbit::GeneratePassMap                      (   const   Array<State>&               aStateArray,
//                                                                                 const   Integer&                    anInitialRevolutionNumber                   )
// {

//     using library::core::types::Real ;

//     using library::math::obj::Vector3d ;

//     using library::physics::time::Duration ;

//     if (!anInitialRevolutionNumber.isDefined())
//     {
//         throw library::core::error::runtime::Undefined("Initial revolution number") ;
//     }

//     // [TBI] Deal w/ z_ECI always equal to 0.0 case (equatorial orbit)

//     Map<Index, Pass> passMap ;

//     Index index = 0 ;
//     Integer revolutionNumber = anInitialRevolutionNumber ;
//     Instant previousPassEndInstant = Instant::Undefined() ;

//     Index currentIndex = 0 ;
//     State const* previousStatePtr = nullptr ;

//     for (const auto& state : aStateArray)
//     {

//         bool passHasBeenAdded = false ;

//         if (previousStatePtr != nullptr)
//         {

//             const Vector3d& previousStateCoordinates_ECI = previousStatePtr->accessPosition().accessCoordinates() ;
//             const Vector3d& currentStateCoordinates_ECI = state.accessPosition().accessCoordinates() ;

//             if ((previousStateCoordinates_ECI.z() < 0.0) && (currentStateCoordinates_ECI.z() >= 0.0))
//             {

//                 const Pass::Type passType = ((!previousPassEndInstant.isDefined()) && aStateArray.accessFirst().accessPosition().accessCoordinates().z() != 0.0) ? Pass::Type::Partial : Pass::Type::Complete ;

//                 const Instant passStartInstant = previousPassEndInstant.isDefined() ? previousPassEndInstant : aStateArray.accessFirst().accessInstant() ;
//                 const Instant passEndInstant = previousStatePtr->accessInstant() - Duration::Between(previousStatePtr->accessInstant(), state.accessInstant()) * (previousStateCoordinates_ECI.z() / (currentStateCoordinates_ECI.z() - previousStateCoordinates_ECI.z())) ;

//                 const Interval passInterval = Interval::Closed(passStartInstant, passEndInstant) ;

//                 const Pass pass = { passType, revolutionNumber, passInterval } ;

//                 passMap.insert({ index, pass }) ;

//                 index = currentIndex ;
//                 revolutionNumber++ ;

//                 previousPassEndInstant = passEndInstant ;
//                 passHasBeenAdded = true ;

//             }

//         }

//         if ((!passHasBeenAdded) && (&state == &aStateArray.accessLast())) // Last state
//         {

//             const Pass::Type passType = ((previousStatePtr != nullptr) && (previousStatePtr->accessPosition().accessCoordinates().z() < 0.0) && (state.accessPosition().accessCoordinates().z() == 0.0)) ? Pass::Type::Complete : Pass::Type::Partial ;

//             const Instant passStartInstant = previousPassEndInstant.isDefined() ? previousPassEndInstant : aStateArray.accessFirst().accessInstant() ;
//             const Instant passEndInstant = state.accessInstant() ;

//             const Interval passInterval = Interval::Closed(passStartInstant, passEndInstant) ;

//             const Pass pass = { passType, revolutionNumber, passInterval } ;

//             passMap.insert({ index, pass }) ;

//         }

//         previousStatePtr = &state ;
//         currentIndex++ ;

//     }

//     return passMap ;

// }

// Array<State>                    Orbit::GenerateStates                       (   const   Model&                      aModel,
//                                                                                 const   Array<Instant>&             anInstantGrid                               )
// {

//     Array<State> states = Array<State>::Empty() ;

//     states.reserve(anInstantGrid.getSize()) ;

//     for (const auto& instant : anInstantGrid)
//     {
//         states.add(aModel.calculateStateAt(instant)) ;
//     }

//     return states ;

// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////