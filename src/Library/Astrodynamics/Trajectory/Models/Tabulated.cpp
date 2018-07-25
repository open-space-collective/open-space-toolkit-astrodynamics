////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Models/Tabulated.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Models/Tabulated.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace models
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Tabulated::Tabulated                        (   const   Array<State>&               aStateArray                                 )
                                :   Model(),
                                    states_(aStateArray),
                                    stateIndex_(0)
{

}

Tabulated*                      Tabulated::clone                            ( ) const
{
    return new Tabulated(*this) ;
}

bool                            Tabulated::operator ==                      (   const   Tabulated&                  aTabulatedModel                             ) const
{

    if ((!this->isDefined()) || (!aTabulatedModel.isDefined()))
    {
        return false ;
    }

    return states_ == aTabulatedModel.states_ ;

}

bool                            Tabulated::operator !=                      (   const   Tabulated&                  aTabulatedModel                             ) const
{
    return !((*this) == aTabulatedModel) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Tabulated&                  aTabulatedModel                             )
{

    aTabulatedModel.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            Tabulated::isDefined                        ( ) const
{
    return !states_.isEmpty() ;
}

Interval                        Tabulated::getInterval                     ( ) const
{
    
    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Tabulated") ;
    }

    return Interval::Closed(states_.accessFirst().accessInstant(), states_.accessLast().accessInstant()) ;

}

State                           Tabulated::calculateStateAt                 (   const   Instant&                    anInstant                                   ) const
{

    using library::core::types::Real ;
    
    using library::math::obj::Vector3d ;

    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Tabulated") ;
    }

    const Pair<const State*, const State*> stateRange = this->accessStateRange(anInstant) ;

    if ((stateRange.first != nullptr) && (stateRange.second != nullptr))
    {

        const State& previousState = *(stateRange.first) ;
        const State& nextState = *(stateRange.second) ;

        const Real ratio = Duration::Between(previousState.accessInstant(), anInstant).inSeconds() / Duration::Between(previousState.accessInstant(), nextState.accessInstant()).inSeconds() ;
        
        const Vector3d x_ECI = previousState.accessPosition().accessCoordinates() + ratio * (nextState.accessPosition().accessCoordinates() - previousState.accessPosition().accessCoordinates()) ;
        const Vector3d v_ECI = previousState.accessVelocity().accessCoordinates() + ratio * (nextState.accessVelocity().accessCoordinates() - previousState.accessVelocity().accessCoordinates()) ;

        const Position position = { x_ECI, previousState.accessPosition().getUnit(), previousState.accessPosition().accessFrame() } ;
        const Velocity velocity = { v_ECI, previousState.accessVelocity().getUnit(), previousState.accessVelocity().accessFrame() } ;

        const State state = { anInstant, position, velocity } ;

        return state ;

    }

    throw library::core::error::RuntimeError("Cannot calculate state at [{}].", anInstant.toString()) ;

    return State::Undefined() ;

}

void                            Tabulated::print                            (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    using library::core::types::String ;

    displayDecorator ? library::core::utils::Print::Header(anOutputStream, "Tabulated") : void () ;

    library::core::utils::Print::Line(anOutputStream) << "Start instant:" << (this->isDefined() ? this->getInterval().accessStart().toString() : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "End instant:" << (this->isDefined() ? this->getInterval().accessEnd().toString() : "Undefined") ;

    library::core::utils::Print::Separator(anOutputStream) ;

    {

        const State firstState = this->isDefined() ? this->calculateStateAt(this->getInterval().accessStart()) : State::Undefined() ;

        library::core::utils::Print::Line(anOutputStream) << "First state:" << (firstState.isDefined() ? String::Format("{} - {} - {}", firstState.accessInstant().toString(), firstState.accessPosition().toString(), firstState.accessVelocity().toString()) : "Undefined") ;

    }

    {

        const State lastState = this->isDefined() ? this->calculateStateAt(this->getInterval().accessEnd()) : State::Undefined() ;

        library::core::utils::Print::Line(anOutputStream) << "Last state:" << (lastState.isDefined() ? String::Format("{} - {} - {}", lastState.accessInstant().toString(), lastState.accessPosition().toString(), lastState.accessVelocity().toString()) : "Undefined") ;

    }

    displayDecorator ? library::core::utils::Print::Footer(anOutputStream) : void () ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool                            Tabulated::operator ==                      (   const   Model&                      aModel                                      ) const
{

    const Tabulated* tabulatedModelPtr = dynamic_cast<const Tabulated*>(&aModel) ;

    return (tabulatedModelPtr != nullptr) && this->operator == (*tabulatedModelPtr) ;

}

bool                            Tabulated::operator !=                      (   const   Model&                      aModel                                      ) const
{
    return !((*this) == aModel) ;
}

Pair<const State*, const State*> Tabulated::accessStateRange                (   const   Instant&                    anInstant                                   ) const
{

    using library::physics::time::Scale ;

    // ... stateIndex_

    // const Real instantMjd = anInstant.getModifiedJulianDate(Scale::UTC) ;
    
    // const auto nextDataIt = data_.lower_bound(instantMjd) ;

    // if (nextDataIt == data_.end())
    // {
    //     return { &(data_.rbegin()->second), nullptr } ;
    // }
    // else if (nextDataIt == data_.begin())
    // {
    //     return { nullptr, &(nextDataIt->second) } ;
    // }
    // else
    // {

    //     const auto previousDataIt = std::prev(nextDataIt) ;

    //     return { &(previousDataIt->second), &(nextDataIt->second) } ;

    // }

    throw library::core::error::runtime::ToBeImplemented("Tabulated :: accessStateRange") ;

    return { nullptr, nullptr } ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////