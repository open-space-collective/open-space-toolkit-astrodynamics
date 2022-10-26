////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Profile.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Models/Transform.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Profile::Profile                            (   const   Model&                      aModel                                      )
                                :   modelUPtr_(aModel.clone())
{

}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Profile&                    aProfile                                    )
{

    aProfile.print(anOutputStream, true) ;

    return anOutputStream ;

}

bool                            Profile::isDefined                          ( ) const
{
    return (this->modelUPtr_ != nullptr) && this->modelUPtr_->isDefined() ;
}

State                           Profile::getStateAt                         (   const   Instant&                    anInstant                                   ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile") ;
    }

    return this->modelUPtr_->calculateStateAt(anInstant) ;

}

Array<State>                    Profile::getStatesAt                        (   const   Array<Instant>&             anInstantArray                              ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile") ;
    }

    return this->modelUPtr_->calculateStatesAt(anInstantArray) ;

}

Axes                            Profile::getAxesAt                          (   const   Instant&                    anInstant                                   ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile") ;
    }

    return modelUPtr_->getAxesAt(anInstant) ;

}

Shared<const Frame>             Profile::getBodyFrame                       (   const   String&                     aFrameName                                  ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Profile") ;
    }

    return modelUPtr_->getBodyFrame(aFrameName) ;

}

void                            Profile::print                              (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Flight Profile") : void () ;

    // TBI

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

Profile                         Profile::Undefined                          ( )
{
    return {} ;
}

Profile                         Profile::InertialPointing                   (   const   Trajectory&                 aTrajectory,
                                                                                const   Quaternion&                 aQuaternion                                 )
{

    using ostk::astro::flight::profile::models::Transform ;

    return { Transform::InertialPointing(aTrajectory, aQuaternion) } ;

}

Profile                         Profile::NadirPointing                      (   const   trajectory::Orbit&          anOrbit,
                                                                                const   trajectory::Orbit::FrameType& anOrbitalFrameType                        )
{

    using ostk::astro::flight::profile::models::Transform ;

    return { Transform::NadirPointing(anOrbit, anOrbitalFrameType) } ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Profile::Profile                            ( )
                                :   modelUPtr_(nullptr)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
