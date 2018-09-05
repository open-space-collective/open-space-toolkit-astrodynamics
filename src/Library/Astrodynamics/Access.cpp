////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Access.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Access.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Access::Access                              (   const   Access::Type&               aType,
                                                                                const   Instant&                    anAcquisitionOfSignal,
                                                                                const   Instant&                    aTimeOfClosestApproach,
                                                                                const   Instant&                    aLossOfSignal                               )
                                :   type_(aType),
                                    acquisitionOfSignal_(anAcquisitionOfSignal),
                                    timeOfClosestApproach_(aTimeOfClosestApproach),
                                    lossOfSignal_(aLossOfSignal)
{

}

bool                            Access::operator ==                         (   const   Access&                     anAccess                                    ) const
{

    if ((!this->isDefined()) || (!anAccess.isDefined()))
    {
        return false ;
    }

    return (type_ == anAccess.type_) && (acquisitionOfSignal_ == anAccess.acquisitionOfSignal_) && (timeOfClosestApproach_ == anAccess.timeOfClosestApproach_) && (lossOfSignal_ == anAccess.lossOfSignal_) ;

}

bool                            Access::operator !=                         (   const   Access&                     anAccess                                    ) const
{
    return !((*this) == anAccess) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Access&                     anAccess                                    )
{

    using library::physics::time::Duration ;

    library::core::utils::Print::Header(anOutputStream, "Access") ;

    library::core::utils::Print::Line(anOutputStream) << "Type:"                << Access::StringFromType(anAccess.type_) ;
    
    library::core::utils::Print::Line(anOutputStream) << "Acquisition Of Signal:" << (anAccess.acquisitionOfSignal_.isDefined() ? anAccess.acquisitionOfSignal_.toString() : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Time of Closest Approach:" << (anAccess.timeOfClosestApproach_.isDefined() ? anAccess.timeOfClosestApproach_.toString() : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Loss Of Signal:"      << (anAccess.lossOfSignal_.isDefined() ? anAccess.lossOfSignal_.toString() : "Undefined") ;
    
    library::core::utils::Print::Line(anOutputStream) << "Duration:"            << ((anAccess.acquisitionOfSignal_.isDefined() && anAccess.lossOfSignal_.isDefined()) ? Duration::Between(anAccess.acquisitionOfSignal_, anAccess.lossOfSignal_).toString() : "Undefined") ;

    library::core::utils::Print::Footer(anOutputStream) ;

    return anOutputStream ;

}

bool                            Access::isDefined                           ( ) const
{
    return (type_ != Access::Type::Undefined) && acquisitionOfSignal_.isDefined() && timeOfClosestApproach_.isDefined() && lossOfSignal_.isDefined() ;
}

bool                            Access::isComplete                          ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Access") ;
    }
    
    return type_ == Access::Type::Complete ;

}

Access::Type                    Access::getType                             ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Access") ;
    }
    
    return type_ ;

}

Instant                         Access::getAcquisitionOfSignal              ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Access") ;
    }
    
    return acquisitionOfSignal_ ;

}

Instant                         Access::getTimeOfClosestApproach            ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Access") ;
    }
    
    return timeOfClosestApproach_ ;

}

Instant                         Access::getLossOfSignal                     ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Access") ;
    }
    
    return lossOfSignal_ ;

}

Interval                        Access::getInterval                         ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Access") ;
    }
    
    return Interval::Closed(acquisitionOfSignal_, lossOfSignal_) ;

}

Duration                        Access::getDuration                         ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Access") ;
    }
    
    return Duration::Between(acquisitionOfSignal_, lossOfSignal_) ;

}

Access                          Access::Undefined                           ( )
{
    return Access(Access::Type::Undefined, Instant::Undefined(), Instant::Undefined(), Instant::Undefined()) ;
}

String                          Access::StringFromType                      (   const   Access::Type&               aType                                       )
{

    switch (aType)
    {

        case Access::Type::Undefined:
            return "Undefined" ;

        case Access::Type::Complete:
            return "Complete" ;

        case Access::Type::Partial:
            return "Partial" ;

        default:
            break ;

    }

    throw library::core::error::runtime::Wrong("Type") ;

    return String::Empty() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////