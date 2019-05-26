////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/Propagator.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Propagator.hpp>

#include <Library/Physics/Coordinate/Transform.hpp>
#include <Library/Physics/Time/Duration.hpp>
#include <Library/Physics/Units/Mass.hpp>

#include <Library/Mathematics/Objects/Vector.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

#include <boost/numeric/odeint.hpp>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::math::obj::Vector3d ;

using library::physics::units::Mass ;
using library::physics::time::Duration ;
using library::physics::coord::Transform ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Propagator::Propagator                      (   const   State&                      anInitialState,
                                                                                const   Integer&                    anInitialRevolutionNumber,
                                                                                const   System&                     aSystem,
                                                                                const   Solver&                     aSolver                                     )
                                :   Model(),
                                    initialState_(anInitialState),
                                    initialRevolutionNumber_(anInitialRevolutionNumber),
                                    system_(aSystem),
                                    solver_(aSolver)
{

    this->setup() ;

}

                                Propagator::Propagator                      (   const   Propagator&                 aPropagatorModel                            )
                                :   Model(aPropagatorModel),
                                    initialState_(aPropagatorModel.initialState_),
                                    initialRevolutionNumber_(aPropagatorModel.initialRevolutionNumber_),
                                    system_(aPropagatorModel.system_),
                                    solver_(aPropagatorModel.solver_)
{

}

                                Propagator::~Propagator                     ( )
{

}

Propagator&                     Propagator::operator =                      (   const   Propagator&                 aPropagatorModel                            )
{

    if (this != &aPropagatorModel)
    {

        Model::operator = (aPropagatorModel) ;

        initialState_ = aPropagatorModel.initialState_ ;
        initialRevolutionNumber_ = aPropagatorModel.initialRevolutionNumber_ ;
        system_ = aPropagatorModel.system_ ;
        solver_ = aPropagatorModel.solver_ ;

    }

    return *this ;

}

Propagator*                     Propagator::clone                           ( ) const
{
    return new Propagator(*this) ;
}

bool                            Propagator::operator ==                     (   const   Propagator&                 aPropagatorModel                            ) const
{

    if ((!this->isDefined()) || (!aPropagatorModel.isDefined()))
    {
        return false ;
    }

    return (initialState_ == aPropagatorModel.initialState_)
        && (initialRevolutionNumber_ == aPropagatorModel.initialRevolutionNumber_)
        && solver_.getType() == aPropagatorModel.solver_.getType() ; // [TBI] Add System comparison too

}

bool                            Propagator::operator !=                     (   const   Propagator&                 aPropagatorModel                            ) const
{
    return !((*this) == aPropagatorModel) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Propagator&                 aPropagatorModel                            )
{

    aPropagatorModel.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            Propagator::isDefined                       ( ) const
{

    return initialState_.isDefined()
        && initialRevolutionNumber_.isDefined()
        && system_.isDefined()
        && solver_.isDefined() ;

}

State                           Propagator::getInitialState                 ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Propagator") ;
    }

    return initialState_ ;

}

Integer                         Propagator::getInitialRevolutionNumber      ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Propagator") ;
    }

    return initialRevolutionNumber_ ;

}

System                          Propagator::getSystem                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Propagator") ;
    }

    return system_ ;

}

Solver                          Propagator::getSolver                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Propagator") ;
    }

    return solver_ ;

}

Instant                         Propagator::getEpoch                        ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Propagator") ;
    }

    return initialState_.getInstant() ;

}

Integer                         Propagator::getRevolutionNumberAtEpoch      ( ) const
{
    return this->getInitialRevolutionNumber() ;
}

State                           Propagator::calculateStateAt                (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Propagator") ;
    }

    if (system_.getInstant() != anInstant)
    {

        const double stepDuration_s = Duration::Between(system_.getInstant(), anInstant).inSeconds() ;

        solver_.step(system_, stepDuration_s) ;

    }

    const Vector3d x_GCRF = { system_.accessState()[0], system_.accessState()[1], system_.accessState()[2] } ;
    const Vector3d v_GCRF = { system_.accessState()[3], system_.accessState()[4], system_.accessState()[5] } ;

    const State currentState =
    {
        system_.getInstant(),
        Position::Meters(x_GCRF, Frame::GCRF()),
        Velocity::MetersPerSecond(v_GCRF, Frame::GCRF())
    } ;

    return currentState ;

}

Integer                         Propagator::calculateRevolutionNumberAt     (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Propagator") ;
    }

    if (anInstant == this->getEpoch())
    {
        return this->getRevolutionNumberAtEpoch() ;
    }

    throw library::core::error::runtime::ToBeImplemented("Propagator :: calculateRevolutionNumberAt") ; // [TBI]

    return Integer::Undefined() ;

}

void                            Propagator::print                           (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? library::core::utils::Print::Header(anOutputStream, "Propagator") : void () ;

    displayDecorator ? library::core::utils::Print::Footer(anOutputStream) : void () ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool                            Propagator::operator ==                     (   const   trajectory::Model&          aModel                                      ) const
{

    const Propagator* propagatorModelPtr = dynamic_cast<const Propagator*>(&aModel) ;

    return (propagatorModelPtr != nullptr) && this->operator == (*propagatorModelPtr) ;

}

bool                            Propagator::operator !=                     (   const   trajectory::Model&          aModel                                      ) const
{
    return !((*this) == aModel) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            Propagator::setup                           ( )
{

    const Vector3d x_GCRF = initialState_.inFrame(Frame::GCRF()).getPosition().inUnit(Position::Unit::Meter).getCoordinates() ;
    const Vector3d v_GCRF = initialState_.inFrame(Frame::GCRF()).getVelocity().inUnit(Velocity::Unit::MeterPerSecond).getCoordinates() ;

    const std::vector<double> state =
    {
        x_GCRF.x(), x_GCRF.y(), x_GCRF.z(),
        v_GCRF.x(), v_GCRF.y(), v_GCRF.z()
    } ;

    system_.setInstant(initialState_.getInstant()) ;
    system_.setState(state) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
