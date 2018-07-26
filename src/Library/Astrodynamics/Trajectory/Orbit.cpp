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

    using library::core::types::Index ;
    using library::core::types::Real ;

    using library::math::obj::Vector3d ;

    using library::physics::time::Duration ;
    using library::physics::time::Interval ;

    // [TBI] Dead with equatorial case

    // std::cout << "aRevolutionNumber = " << aRevolutionNumber.toString() << std::endl ;
    // std::cout << "passMap_.size() = " << passMap_.size() << std::endl ;

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

        // If any, get pass with closest revolution number

        Pass const* closestPassPtr = nullptr ;

        const auto lowerBoundMapIt = passMap_.lower_bound(aRevolutionNumber) ;

        if (lowerBoundMapIt != passMap_.end()) 
        {
            // std::cout << "lowerBoundMap = " << lowerBoundMapIt->second << std::endl ;

            if (lowerBoundMapIt == passMap_.begin())
            {
                closestPassPtr = &(lowerBoundMapIt->second) ;
            }
            else
            {

                const auto closestPassMapIt = std::prev(lowerBoundMapIt) ;

                if ((aRevolutionNumber - closestPassMapIt->first) < (lowerBoundMapIt->first - aRevolutionNumber))
                {
                    closestPassPtr = &(closestPassMapIt->second) ;
                }
                else
                {
                    closestPassPtr = &(lowerBoundMapIt->second) ;
                }
                
            }

        }
        else if (passMap_.size() > 0)
        {
            // std::cout << "passMap_ IS NOT EMPTY" << std::endl ;
            closestPassPtr = &(passMap_.begin()->second) ;
        }

        Pass currentPass = Pass::Undefined() ;

        if (closestPassPtr != nullptr) // Pass with closest revolution number found
        {
            currentPass = *closestPassPtr ;
        }

        while ((!currentPass.isDefined()) || (currentPass.getRevolutionNumber() != aRevolutionNumber))
        {
            // std::cout << "currentPass = " << currentPass << std::endl ;
            static const Real tolerance = 1e-3 ; // [TBM] Param

            Integer currentRevolutionNumber = currentPass.isDefined() ? currentPass.getRevolutionNumber() : model_.getRevolutionNumberAtEpoch() ;
            // std::cout << "currentRevolutionNumber = " << currentRevolutionNumber.toString() << std::endl ;

            Instant currentInstant = currentPass.isDefined() ? currentPass.getInterval().accessEnd() : model_.getEpoch() ;
            // std::cout << "currentInstant = " << currentInstant.toString() << std::endl ;
            Duration stepDuration = currentPass.isDefined() ? currentPass.getInterval().getDuration() / 5.0 : Duration::Minutes(10.0) ; // [TBM] param
            // std::cout << "stepDuration = " << stepDuration.toString() << std::endl ;

            if (currentRevolutionNumber <= aRevolutionNumber) // Forward propagation
            {

                Real previousStateCoordinates_ECI_z = model_.calculateStateAt(currentInstant).accessPosition().accessCoordinates().z() ;
                // std::cout << "previousStateCoordinates_ECI_z = " << previousStateCoordinates_ECI_z << std::endl ;

                Real residual = Real::Undefined() ;
                // std::cout << "residual = " << residual << std::endl ;
                Index iterationCount = 0 ;
                static const Index maxIterationCount = 1000 ; // [TBM] Param
                // std::cout << "iterationCount = " << iterationCount << std::endl ;

                bool equatorCrossingDetected = false ;
                // std::cout << "equatorCrossingDetected = " << equatorCrossingDetected << std::endl ;

                while (((!residual.isDefined()) || (residual < 0.0) || (residual.abs() > tolerance)) && (iterationCount < maxIterationCount))
                {

                    currentInstant += stepDuration ;

                    const Real currentStateCoordinates_ECI_z = model_.calculateStateAt(currentInstant).accessPosition().accessCoordinates().z() ;
                    // std::cout << "currentStateCoordinates_ECI_z = " << currentStateCoordinates_ECI_z << std::endl ;

                    if ((previousStateCoordinates_ECI_z == 0.0) && (currentStateCoordinates_ECI_z == 0.0))
                    {
                        throw library::core::error::runtime::ToBeImplemented("Equatorial orbit support.") ;
                    }

                    if ((previousStateCoordinates_ECI_z < 0.0) && (currentStateCoordinates_ECI_z >= 0.0))
                    {
                        equatorCrossingDetected = true ;
                        // std::cout << "equatorCrossingDetected = " << equatorCrossingDetected << std::endl ;
                    }

                    if (equatorCrossingDetected)
                    {

                        // const Real& z1_m = previousStateCoordinates_ECI_z ;
                        // const Real& z2_m = currentStateCoordinates_ECI_z ;
                        // const Real dz_m = currentStateCoordinates_ECI_z - previousStateCoordinates_ECI_z ;
                        // std::cout << "z1_m = " << z1_m.toString() << std::endl ;
                        // std::cout << "z2_m = " << z2_m.toString() << std::endl ;
                        // std::cout << "dz_m = " << dz_m.toString() << std::endl ;

                        // const Real dt_s = stepDuration.inSeconds() ;
                        // std::cout << "dt_s = " << dt_s.toString() << std::endl ;
                        // std::cout << "-z2_m * dt_s / dz_m = " << (-z2_m * dt_s / dz_m).toString(15) << std::endl ;

                        if ((currentStateCoordinates_ECI_z - previousStateCoordinates_ECI_z) != 0.0)
                        {
                            stepDuration = Duration::Seconds(-currentStateCoordinates_ECI_z * stepDuration.inSeconds() / (currentStateCoordinates_ECI_z - previousStateCoordinates_ECI_z)) ;
                        }
                        
                        if (stepDuration.isZero())
                        {
                            stepDuration = (currentStateCoordinates_ECI_z < 0.0) ? Duration::Nanoseconds(+1.0) : Duration::Nanoseconds(-1.0) ;
                        }

                        // std::cout << "stepDuration = " << stepDuration.toString() << std::endl ;

                        residual = currentStateCoordinates_ECI_z ;
                        // std::cout << "residual= " << residual.toString() << std::endl ;

                    }

                    previousStateCoordinates_ECI_z = currentStateCoordinates_ECI_z ;

                    iterationCount++ ;

                }

                if (iterationCount > maxIterationCount)
                {
                    throw library::core::error::RuntimeError("Maximum iteration count reached.") ;
                }

                if (currentPass.isDefined())
                {
                    currentPass = { Pass::Type::Complete, currentRevolutionNumber + 1, Interval::Closed(currentPass.getInterval().accessEnd(), currentInstant) } ;
                }
                else
                {

                    if (model_.calculateStateAt(model_.getEpoch()).accessPosition().accessCoordinates().z() == 0.0)
                    {
                        currentPass = { Pass::Type::Complete, currentRevolutionNumber, Interval::Closed(model_.getEpoch(), currentInstant) } ;
                    }
                    else
                    {
                        currentPass = { Pass::Type::Partial, currentRevolutionNumber, Interval::Closed(model_.getEpoch(), currentInstant) } ;
                    }

                }

                // std::cout << "currentPass = " << currentPass << std::endl ;
                // previousStateCoordinates_ECI_z = tolerance * 2.0 ;

                currentRevolutionNumber++ ;

            }
            else // Reverse propagation
            {
                throw library::core::error::runtime::ToBeImplemented("Orbit::getPassWithRevolutionNumber > Reverse propagation") ;
            }

            if (currentPass.isDefined())
            {
                passMap_.insert({ currentPass.getRevolutionNumber(), currentPass }) ;
            }

        }

        if (currentPass.getRevolutionNumber() == aRevolutionNumber)
        {
            return currentPass ;
        }
        else
        {
            throw library::core::error::RuntimeError("Cannot get pass with revolution # [{}].", aRevolutionNumber) ;
        }

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