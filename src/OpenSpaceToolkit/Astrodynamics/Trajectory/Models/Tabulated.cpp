////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Tabulated.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Tabulated.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

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

                                Tabulated::Tabulated                        (   const   Array<State>&               aStateArray,
                                                                                const   InterpolationType&          anInterpolationType                                 )
                                :   Model(),
                                    states_(aStateArray),
                                    stateIndex_(0),
                                    interpolationType_(anInterpolationType),
                                    timestamps_(aStateArray.getSize()),
                                    coordinates_(aStateArray.getSize(), 6)
{

    using ostk::math::curvefitting::interp::CubicSpline ;
    using ostk::math::curvefitting::interp::BarycentricRational ;
    using ostk::math::curvefitting::interp::LinearInterpolator ;

    std::sort(states_.begin(), states_.end(), [] (const auto& lhs, const auto& rhs) { return lhs.getInstant() < rhs.getInstant() ; }) ;

    for (Size i = 0 ; i < states_.getSize() ; ++i)
    {

        timestamps_(i) = (states_[i].accessInstant() - states_[0].accessInstant()).inSeconds() ;

        coordinates_.row(i).segment<3>(0) = states_[i].accessPosition().accessCoordinates() ;
        coordinates_.row(i).segment<3>(3) = states_[i].accessVelocity().accessCoordinates() ;

    }

    interpolators_.reserve(coordinates_.cols()) ;

    for (Size i = 0 ; i < Size(coordinates_.cols()) ; ++i)
    {

        if (interpolationType_ == Tabulated::InterpolationType::CubicSpline)
        {
            interpolators_.add(std::make_shared<CubicSpline>(CubicSpline(timestamps_, coordinates_.col(i)))) ;
        }
        else if (interpolationType_ == Tabulated::InterpolationType::BarycentricRational)
        {
            interpolators_.add(std::make_shared<BarycentricRational>(BarycentricRational(timestamps_, coordinates_.col(i)))) ;
        }
        else if (interpolationType_ == Tabulated::InterpolationType::Linear)
        {
            interpolators_.add(std::make_shared<LinearInterpolator>(LinearInterpolator(timestamps_, coordinates_.col(i)))) ;
        }

    }

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
        throw ostk::core::error::runtime::Undefined("Tabulated") ;
    }

    return Interval::Closed(states_.accessFirst().accessInstant(), states_.accessLast().accessInstant()) ;

}

Tabulated::InterpolationType               Tabulated::getInterpolationType             ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated") ;
    }

    return interpolationType_ ;

}

State                           Tabulated::calculateStateAt                 (   const   Instant&                    anInstant                                   ) const
{

    using ostk::core::types::Size ;

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated") ;
    }

    VectorXd interpolatedCoordinates(interpolators_.getSize()) ;

    for (Size i = 0 ; i < interpolators_.getSize() ; ++i)
    {
        interpolatedCoordinates(i) = interpolators_[i]->evaluate((anInstant - states_[0].accessInstant()).inSeconds()) ;
    }

    return State(anInstant, Position::Meters(interpolatedCoordinates.segment<3>(0), states_[0].accessPosition().accessFrame()), Velocity::MetersPerSecond(interpolatedCoordinates.segment<3>(3),  states_[0].accessPosition().accessFrame())) ;

    return interpolation_(anInstant) ;

}

void                            Tabulated::print                            (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    using ostk::core::types::String ;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Tabulated") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Start instant:" << (this->isDefined() ? this->getInterval().accessStart().toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "End instant:" << (this->isDefined() ? this->getInterval().accessEnd().toString() : "Undefined") ;

    ostk::core::utils::Print::Separator(anOutputStream) ;

    {

        const State firstState = this->isDefined() ? this->calculateStateAt(this->getInterval().accessStart()) : State::Undefined() ;

        ostk::core::utils::Print::Line(anOutputStream) << "First state:" << (firstState.isDefined() ? String::Format("{} - {} - {}", firstState.accessInstant().toString(), firstState.accessPosition().toString(), firstState.accessVelocity().toString()) : "Undefined") ;

    }

    {

        const State lastState = this->isDefined() ? this->calculateStateAt(this->getInterval().accessEnd()) : State::Undefined() ;

        ostk::core::utils::Print::Line(anOutputStream) << "Last state:" << (lastState.isDefined() ? String::Format("{} - {} - {}", lastState.accessInstant().toString(), lastState.accessPosition().toString(), lastState.accessVelocity().toString()) : "Undefined") ;

    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
