////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

#include <Library/Physics/Units/Time.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                TLE::TLE                                    (   const   String&                     aFirstLine,
                                                                                const   String&                     aSecondLine                                 )
                                :   satelliteName_(""),
                                    firstLine_(aFirstLine),
                                    secondLine_(aSecondLine)
{

    if (((!aFirstLine.isEmpty()) || (!aSecondLine.isEmpty())) && (!TLE::CanParse(aFirstLine, aSecondLine)))
    {
        throw library::core::error::runtime::Wrong("TLE") ;
    }
    
}

                                TLE::TLE                                    (   const   String&                     aSatelliteName,
                                                                                const   String&                     aFirstLine,
                                                                                const   String&                     aSecondLine                                 )
                                :   satelliteName_(aSatelliteName),
                                    firstLine_(aFirstLine),
                                    secondLine_(aSecondLine)
{

    if (((!aFirstLine.isEmpty()) || (!aSecondLine.isEmpty())) && (!TLE::CanParse(aFirstLine, aSecondLine)))
    {
        throw library::core::error::runtime::Wrong("TLE") ;
    }
    
}

bool                            TLE::operator ==                            (   const   TLE&                        aTle                                        ) const
{

    if ((!this->isDefined()) || (!aTle.isDefined()))
    {
        return false ;
    }
    
    return (satelliteName_ == aTle.satelliteName_) && (firstLine_ == aTle.firstLine_) && (secondLine_ == aTle.secondLine_) ;

}

bool                            TLE::operator !=                            (   const   TLE&                        aTle                                        ) const
{

    if ((!this->isDefined()) || (!aTle.isDefined()))
    {
        return true ;
    }

    return (satelliteName_ != aTle.satelliteName_) || (firstLine_ != aTle.firstLine_) || (secondLine_ != aTle.secondLine_) ;

}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   TLE&                        aTle                                        )
{

    library::core::utils::Print::Header(anOutputStream, "Two-Line Elements") ;

    library::core::utils::Print::Line(anOutputStream) << "Line 1:" << aTle.getFirstLine() ;
    library::core::utils::Print::Line(anOutputStream) << "Line 2:" << aTle.getSecondLine() ;

    library::core::utils::Print::Separator(anOutputStream) ;

    library::core::utils::Print::Line(anOutputStream) << "Satellite Name:" << aTle.getSatelliteName() ;
    library::core::utils::Print::Line(anOutputStream) << "Satellite Number:" << aTle.getSatelliteNumber() ;
    library::core::utils::Print::Line(anOutputStream) << "Classification:" << aTle.getClassification() ;
    library::core::utils::Print::Line(anOutputStream) << "International Designator:" << aTle.getInternationalDesignator() ;
    // library::core::utils::Print::Line(anOutputStream) << "Epoch Year:" << aTle.getEpochYear() ;
    // library::core::utils::Print::Line(anOutputStream) << "Epoch:" << aTle.getEpoch() ;
    // library::core::utils::Print::Line(anOutputStream) << "Epoch UTC (Gregorian):" << aTle.getEpochTime().getCalendarDate(Time::Scale::UTC).toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Mean Motion First Time Der. / 2:" << aTle.getMeanMotionFirstTimeDerivativeDividedByTwo() ;
    library::core::utils::Print::Line(anOutputStream) << "Mean Motion Second Time Der. / 6:" << aTle.getMeanMotionSecondTimeDerivativeDividedBySix() ;
    library::core::utils::Print::Line(anOutputStream) << "B* Drag Term:" << aTle.getBStarDragTerm() ;
    library::core::utils::Print::Line(anOutputStream) << "Ephemeris Type:" << aTle.getEphemerisType() ;
    library::core::utils::Print::Line(anOutputStream) << "Element Set Number:" << aTle.getElementSetNumber() ;
    library::core::utils::Print::Line(anOutputStream) << "Inclination:" << aTle.getInclination().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Right Ascension of the Ascending Node :" << aTle.getRaan().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Eccentricity:" << aTle.getEccentricity() ;
    library::core::utils::Print::Line(anOutputStream) << "Argument of Periapsis:" << aTle.getAop().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Mean Anomaly:" << aTle.getMeanAnomaly().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Mean Motion:" << aTle.getMeanMotion().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Revolution Number at Epoch:" << aTle.getRevolutionNumberAtEpoch() ;

    library::core::utils::Print::Footer(anOutputStream) ;

    return anOutputStream ;

}

bool                            TLE::isDefined                              ( ) const
{
    return (!firstLine_.isEmpty()) && (!secondLine_.isEmpty()) ;
}

String                          TLE::getSatelliteName                       ( ) const
{
    return satelliteName_ ;
}

String                          TLE::getFirstLine                           ( ) const
{
    return firstLine_ ;
}

String                          TLE::getSecondLine                          ( ) const
{
    return secondLine_ ;
}

Integer                         TLE::getSatelliteNumber                     ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return Integer::Parse(firstLine_.getSubstring(2, 5).trim()) ;

}

String                          TLE::getClassification                      ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return firstLine_.getSubstring(7, 1) ;

}

String                          TLE::getInternationalDesignator             ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return firstLine_.getSubstring(9, 8).trim() ;

}

// Instant                         TLE::getEpoch                               ( ) const
// {

// }

Real                            TLE::getMeanMotionFirstTimeDerivativeDividedByTwo ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return Real::Parse(firstLine_.getSubstring(33, 10).trim()) ;

}

Real                            TLE::getMeanMotionSecondTimeDerivativeDividedBySix ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return TLE::ParseReal(firstLine_.getSubstring(44, 8).trim(), true) ;

}

Real                            TLE::getBStarDragTerm                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return TLE::ParseReal(firstLine_.getSubstring(53, 8).trim(), true) ;

}

Integer                         TLE::getEphemerisType                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return Integer::Parse(firstLine_.getSubstring(62, 1)) ;

}

Integer                         TLE::getElementSetNumber                    ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return Integer::Parse(firstLine_.getSubstring(64, 4).trim()) ;

}

Integer                         TLE::getFirstLineChecksum                   ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return Integer::Parse(firstLine_.getSubstring(68, 1)) ;

}

Angle                           TLE::getInclination                         ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(8, 8).trim())) ;

}

Angle                           TLE::getRaan                                ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(17, 8).trim())) ;

}

Real                            TLE::getEccentricity                        ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }

    return Real::Parse("0." + secondLine_.getSubstring(26, 7).trim()) ;

}

Angle                           TLE::getAop                                 ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(34, 8).trim())) ;

}

Angle                           TLE::getMeanAnomaly                         ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(43, 8).trim())) ;

}

Derived                         TLE::getMeanMotion                          ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return Derived(Real::Parse(secondLine_.getSubstring(52, 11).trim()), Derived::Unit::AngularVelocity(Angle::Unit::Revolution, physics::units::Time::Unit::Day)) ;

}

Integer                         TLE::getRevolutionNumberAtEpoch             ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }
    
    return Integer::Parse(secondLine_.getSubstring(63, 5)) ;

}

TLE                             TLE::Undefined                              ( )
{
    return TLE(String::Empty(), String::Empty(), String::Empty()) ;
}

TLE                             TLE::Parse                                  (   const   String&                     aString                                     )
{

    if (aString.getLength() >= 139)
    {

        if (aString[69] == '\n')
        {
            return TLE(aString.getSubstring(0, 69), aString.getSubstring(70, 69)) ;
        }
        else if ((aString.getLength() >= 140) && (aString[69] == '\r') && (aString[70] == '\n'))
        {
            return TLE(aString.getSubstring(0, 69), aString.getSubstring(71, 69)) ;
        }
        
    }
    
    throw library::core::error::runtime::Wrong("String", aString) ;

    return TLE::Undefined() ;

}

bool                            TLE::CanParse                               (   const   String&                     aString                                     )
{
    
    if (aString.getLength() >= 139)
    {

        if (aString[69] == '\n')
        {
            return TLE::CanParse(aString.getSubstring(0, 69), aString.getSubstring(70, 69)) ;
        }
        else if ((aString.getLength() >= 140) && (aString[69] == '\r') && (aString[70] == '\n'))
        {
            return TLE::CanParse(aString.getSubstring(0, 69), aString.getSubstring(71, 69)) ;
        }
        
    }

    return false ;
    
}

bool                            TLE::CanParse                               (   const   String&                     aFirstLine,
                                                                                const   String&                     aSecondLine                                 )
{
    
    if ((aFirstLine.getLength() == 69) && (aSecondLine.getLength() == 69))
    {
        if (Integer::CanParse(aFirstLine[68]) && Integer::CanParse(aSecondLine[68]))
        {
            return (Integer::Parse(aFirstLine[68]) == TLE::GenerateChecksum(aFirstLine)) && (Integer::Parse(aSecondLine[68]) == TLE::GenerateChecksum(aSecondLine)) ;
        } 
    }

    return false ;
    
}

Integer                         TLE::GenerateChecksum                       (   const   String&                     aLine                                       )
{

    using library::core::types::Index ;

    if (aLine.getLength() != 69)
    {
        throw library::core::error::runtime::Wrong("Line") ;
    }

    int checksum = 0 ;

    for (Index idx = 0; idx < (aLine.getLength() - 1); ++idx)
    {
        checksum += Integer::CanParse(aLine[idx]) ? static_cast<int>(Integer::Parse(aLine[idx])) : (aLine[idx] == '-' ? 1 : 0) ;
    }
    
    checksum -= (checksum / 10) * 10 ; // Last digit

    return checksum ;

}

Real                            TLE::ParseReal                              (   const   String&                     aString,
                                                                                        bool                        isDecimalPointAssumed                       )
{

    using library::core::types::Index ;

    String string = aString ;

    // Add leading 0 if decimal point is assumed

    if (isDecimalPointAssumed)
    {
        
        if (aString[0] == '-')
        {
            string = "-0." + string.getSubstring(1, string.getLength() - 1) ;
        }
        else if (aString[0] == '+')
        {
            string = "+0." + string.getSubstring(1, string.getLength() - 1) ;
        }
        else
        {
            string = "0." + string ;
        }

    }

    // Transform exponent (e.g., 123-456 to 123e-456)
    
    Index minusSignIndex = string.rfind('-') ;

    if ((minusSignIndex != String::npos) && (minusSignIndex > 0) && (string[minusSignIndex - 1] != 'e') && (string[minusSignIndex - 1] != 'E'))
    {
        string.replace(minusSignIndex, 1, "e-") ;
    }

    Index plusSignIndex = string.rfind('+') ;
    
    if ((plusSignIndex != String::npos) && (plusSignIndex > 0) && (string[plusSignIndex - 1] != 'e') && (string[plusSignIndex - 1] != 'E'))
    {
        string.replace(plusSignIndex, 1, "e+") ;
    }

    return Real::Parse(string) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////