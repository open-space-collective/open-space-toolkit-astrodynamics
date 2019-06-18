////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

#include <Library/Physics/Units/Time.hpp>

#include <Library/Core/Containers/Array.hpp>
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
namespace orbit
{
namespace models
{
namespace sgp4
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

    library::core::utils::Print::Line(anOutputStream) << "Line 1:"                                                  << aTle.getFirstLine() ;
    library::core::utils::Print::Line(anOutputStream) << "Line 2:"                                                  << aTle.getSecondLine() ;

    library::core::utils::Print::Separator(anOutputStream) ;

    library::core::utils::Print::Line(anOutputStream) << "Satellite Name:"                                          << aTle.getSatelliteName() ;
    library::core::utils::Print::Line(anOutputStream) << "Satellite Number:"                                        << aTle.getSatelliteNumber().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Classification:"                                          << aTle.getClassification() ;
    library::core::utils::Print::Line(anOutputStream) << "International Designator:"                                << aTle.getInternationalDesignator() ;
    library::core::utils::Print::Line(anOutputStream) << "Epoch:"                                                   << aTle.getEpoch().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Mean Motion First Time Der. / 2:"                         << aTle.getMeanMotionFirstTimeDerivativeDividedByTwo().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Mean Motion Second Time Der. / 6:"                        << aTle.getMeanMotionSecondTimeDerivativeDividedBySix().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "B* Drag Term:"                                            << aTle.getBStarDragTerm().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Ephemeris Type:"                                          << aTle.getEphemerisType().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Element Set Number:"                                      << aTle.getElementSetNumber().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Inclination:"                                             << aTle.getInclination().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Right Ascension of the Ascending Node :"                  << aTle.getRaan().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Eccentricity:"                                            << aTle.getEccentricity().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Argument of Periapsis:"                                   << aTle.getAop().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Mean Anomaly:"                                            << aTle.getMeanAnomaly().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Mean Motion:"                                             << aTle.getMeanMotion().toString() ;
    library::core::utils::Print::Line(anOutputStream) << "Revolution Number at Epoch:"                              << aTle.getRevolutionNumberAtEpoch().toString() ;

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

Instant                         TLE::getEpoch                               ( ) const
{

    using library::physics::time::Scale ;
    using library::physics::time::Duration ;
    using library::physics::time::DateTime ;

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("TLE") ;
    }

    const String epochYearTwoDigitsString = firstLine_.getSubstring(18, 2).trim() ;
    const String epochDayString = firstLine_.getSubstring(20, 12).trim() ;

    const Integer epochYearTwoDigits = Integer::Parse(epochYearTwoDigitsString) ;

    // See: https://www.celestrak.com/columns/v04n03/

    const Integer epochYear = (((epochYearTwoDigits >= 57) && (epochYearTwoDigits <= 99)) ? 1900 : 2000) + epochYearTwoDigits ;
    const Real epochDay = Real::Parse(epochDayString) ;

    return Instant::DateTime(DateTime(epochYear, 1, 1, 0, 0, 0), Scale::UTC) + Duration::Days(epochDay - 1.0) ;

}

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

bool                            TLE::CanParse                               (   const   String&                     aString                                     )
{

    using library::core::types::Size ;
    using library::core::ctnr::Array ;

    Array<String> lines = Array<String>::Empty() ;

    std::istringstream stringStream(aString) ;

    String lineBuffer ;

    while (std::getline(stringStream, lineBuffer))
    {

        lineBuffer.erase(std::remove(lineBuffer.begin(), lineBuffer.end(), '\r'), lineBuffer.end()) ;
        lineBuffer.erase(std::remove(lineBuffer.begin(), lineBuffer.end(), '\n'), lineBuffer.end()) ;

        if (!lineBuffer.isEmpty())
        {
            lines.add(lineBuffer) ;
        }

    }

    if (lines.getSize() == 2)
    {
        return TLE::CanParse(lines.at(0), lines.at(1)) ;
    }
    else if (lines.getSize() >= 3)
    {
        return TLE::CanParse(lines.at(1), lines.at(2)) ;
    }

    return false ;

}

bool                            TLE::CanParse                               (   const   String&                     aFirstLine,
                                                                                const   String&                     aSecondLine                                 )
{

    String firstLine = aFirstLine ;

    firstLine.erase(std::remove(firstLine.begin(), firstLine.end(), '\r'), firstLine.end()) ;
    firstLine.erase(std::remove(firstLine.begin(), firstLine.end(), '\n'), firstLine.end()) ;

    String secondLine = aSecondLine ;

    secondLine.erase(std::remove(secondLine.begin(), secondLine.end(), '\r'), secondLine.end()) ;
    secondLine.erase(std::remove(secondLine.begin(), secondLine.end(), '\n'), secondLine.end()) ;

    if ((firstLine.getLength() == 69) && (secondLine.getLength() == 69))
    {
        if (Integer::CanParse(firstLine[68]) && Integer::CanParse(secondLine[68]))
        {
            return (Integer::Parse(firstLine[68]) == TLE::GenerateChecksum(firstLine)) && (Integer::Parse(secondLine[68]) == TLE::GenerateChecksum(secondLine)) ;
        }
    }

    return false ;

}

TLE                             TLE::Parse                                  (   const   String&                     aString                                     )
{

    using library::core::types::Size ;
    using library::core::ctnr::Array ;

    if (aString.isEmpty())
    {
        throw library::core::error::runtime::Undefined("String") ;
    }

    Array<String> lines = Array<String>::Empty() ;

    std::istringstream stringStream(aString) ;

    String lineBuffer ;

    while (std::getline(stringStream, lineBuffer))
    {

        lineBuffer.erase(std::remove(lineBuffer.begin(), lineBuffer.end(), '\r'), lineBuffer.end()) ;
        lineBuffer.erase(std::remove(lineBuffer.begin(), lineBuffer.end(), '\n'), lineBuffer.end()) ;

        if (!lineBuffer.isEmpty())
        {
            lines.add(lineBuffer) ;
        }

    }

    if (lines.getSize() == 2)
    {
        return TLE(lines.at(0), lines.at(1)) ;
    }
    else if (lines.getSize() >= 3)
    {
        return TLE(lines.at(0), lines.at(1), lines.at(2)) ;
    }

    throw library::core::error::runtime::Wrong("String", aString) ;

    return TLE::Undefined() ;

}

TLE                             TLE::Load                                   (   const   File&                       aFile                                       )
{

    if (!aFile.isDefined())
    {
        throw library::core::error::runtime::Undefined("File") ;
    }

    if (!aFile.exists())
    {
        throw library::core::error::RuntimeError("File [{}] does not exist.", aFile.toString()) ;
    }

    const String tleString = aFile.getContents() ;

    return TLE::Parse(tleString) ;

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
        string.std::string::replace(minusSignIndex, 1, "e-") ;
    }

    Index plusSignIndex = string.rfind('+') ;

    if ((plusSignIndex != String::npos) && (plusSignIndex > 0) && (string[plusSignIndex - 1] != 'e') && (string[plusSignIndex - 1] != 'E'))
    {
        string.std::string::replace(plusSignIndex, 1, "e+") ;
    }

    return Real::Parse(string) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////