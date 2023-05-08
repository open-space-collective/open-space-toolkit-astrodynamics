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

    std::sort(states_.begin(), states_.end(), [] (const auto& lhs, const auto& rhs) { return lhs.getInstant() < rhs.getInstant() ; }) ;

    for (Size i = 0 ; i < states_.getSize() ; ++i)
    {

        timestamps_(i) = (states_[i].accessInstant() - states_[0].accessInstant()).inSeconds() ;

        coordinates_.row(i).segment<3>(0) = states_[i].accessPosition().accessCoordinates() ;
        coordinates_.row(i).segment<3>(3) = states_[i].accessVelocity().accessCoordinates() ;

    }

    if (interpolationType_ == Tabulated::InterpolationType::Linear)
    {
        interpolation_ = std::bind(&Tabulated::linearInterpolation, this, std::placeholders::_1) ;
    }
    else if (interpolationType_ == Tabulated::InterpolationType::CubicSpline)
    {
        interpolation_ = std::bind(&Tabulated::cubicSplineInterpolation, this, std::placeholders::_1) ;
    }
    else if (interpolationType_ == Tabulated::InterpolationType::BarycentricRational)
    {
        interpolation_ = std::bind(&Tabulated::barycentricRationalInterpolation, this, std::placeholders::_1) ;
    }
    else
    {
        throw ostk::core::error::runtime::Wrong("Interpolation type") ;
    }

    for (Size i = 0 ; i < Size(coordinates_.cols()) ; ++i)
    {

        if (interpolationType_ == Tabulated::InterpolationType::CubicSpline)
        {
            cubicSpline_.add(CubicSpline(timestamps_, coordinates_.col(i))) ;
        }

        if (interpolationType_ == Tabulated::InterpolationType::BarycentricRational)
        {
            barycentricRational_.add(BarycentricRational(timestamps_, coordinates_.col(i))) ;
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

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated") ;
    }

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

State                            Tabulated::linearInterpolation             (   const   Instant&                    anInstant                                   ) const
{

    using ostk::core::types::Real ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Duration ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;

    const Pair<const State*, const State*> stateRange = this->accessStateRangeAt(anInstant) ;

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
    else if (stateRange.first != nullptr)
    {
        return *(stateRange.first) ;
    }
    else if (stateRange.second != nullptr)
    {
        return *(stateRange.second) ;
    }

    throw ostk::core::error::RuntimeError("Cannot calculate state at [{}].", anInstant.toString()) ;

}

State                            Tabulated::cubicSplineInterpolation        (   const   Instant&                    anInstant                                   ) const
{

    using ostk::core::types::Size ;

    using ostk::physics::time::Scale ;

    VectorXd interpolatedCoordinates(6) ;

    for (Size i = 0 ; i < 6 ; ++i)
    {
        interpolatedCoordinates(i) = cubicSpline_[i].evaluate((anInstant - states_[0].accessInstant()).inSeconds()) ;
    }

    return State(anInstant, Position::Meters(interpolatedCoordinates.segment<3>(0), states_[0].accessPosition().accessFrame()), Velocity::MetersPerSecond(interpolatedCoordinates.segment<3>(3),  states_[0].accessPosition().accessFrame())) ;

}

State                            Tabulated::barycentricRationalInterpolation (   const   Instant&                    anInstant                                   ) const
{

    using ostk::core::types::Size ;

    using ostk::physics::time::Scale ;

    VectorXd interpolatedCoordinates(6) ;

    for (Size i = 0 ; i < 6 ; ++i)
    {
        interpolatedCoordinates(i) = barycentricRational_[i].evaluate((anInstant - states_[0].accessInstant()).inSeconds()) ;
    }

    return State(anInstant, Position::Meters(interpolatedCoordinates.segment<3>(0), states_[0].accessPosition().accessFrame()), Velocity::MetersPerSecond(interpolatedCoordinates.segment<3>(3),  states_[0].accessPosition().accessFrame())) ;

}

Pair<const State*, const State*> Tabulated::accessStateRangeAt              (   const   Instant&                    anInstant                                   ) const
{

    using ostk::core::ctnr::Unpack ;

    State const* previousStatePtr = nullptr ;
    State const* nextStatePtr = nullptr ;

    while (true) // To be improved
    {

        Unpack(previousStatePtr, nextStatePtr) = this->accessStateRangeAtIndex(stateIndex_) ; // Check index cache

        if ((previousStatePtr != nullptr) && (nextStatePtr != nullptr))
        {

            if ((previousStatePtr->accessInstant() <= anInstant) && (anInstant <= nextStatePtr->accessInstant()))
            {

                if (previousStatePtr->accessInstant() == anInstant)
                {
                    return { nullptr, previousStatePtr } ;
                }
                else if (nextStatePtr->accessInstant() == anInstant)
                {
                    return { nextStatePtr, nullptr } ;
                }

                return { previousStatePtr, nextStatePtr } ;

            }
            else
            {

                if (anInstant < previousStatePtr->accessInstant())
                {

                    if (stateIndex_ > 0)
                    {
                        stateIndex_-- ;
                    }
                    else
                    {
                        break ;
                    }

                }
                else
                {

                    if ((states_.getSize() > 0) && (stateIndex_ < (states_.getSize() - 1)))
                    {
                        stateIndex_++ ;
                    }
                    else
                    {
                        break ;
                    }

                }

            }

        }
        else if (previousStatePtr != nullptr)
        {

            if (previousStatePtr->accessInstant() == anInstant)
            {
                return { nullptr, previousStatePtr } ;
            }
            else
            {

                if (anInstant < previousStatePtr->accessInstant())
                {

                    if (stateIndex_ > 0)
                    {
                        stateIndex_-- ;
                    }
                    else
                    {
                        break ;
                    }

                }
                else
                {
                    return { nullptr, nullptr } ;
                }

            }

        }
        else
        {

            stateIndex_ = 0 ;

            break ;

        }

    }

    return { nullptr, nullptr } ;

}

Pair<const State*, const State*> Tabulated::accessStateRangeAtIndex         (   const   Index&                      anIndex                                     ) const
{

    const State* previousStatePtr = (anIndex < states_.getSize()) ? &(states_.at(anIndex)) : nullptr ;
    const State* nextStatePtr = ((anIndex + 1) < states_.getSize()) ? &(states_.at(anIndex + 1)) : nullptr ;

    return { previousStatePtr, nextStatePtr } ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
