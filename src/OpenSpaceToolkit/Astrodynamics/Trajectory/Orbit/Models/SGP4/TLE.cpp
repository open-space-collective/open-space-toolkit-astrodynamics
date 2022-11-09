////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

#include <OpenSpaceToolkit/Physics/Units/Time.hpp>
#include <OpenSpaceToolkit/Physics/Time.hpp>
#include <OpenSpaceToolkit/Physics/Time/Date.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
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
        throw ostk::core::error::runtime::Wrong("TLE") ;
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
        throw ostk::core::error::runtime::Wrong("TLE") ;
    }

}

                                TLE::TLE                                    (   const   Integer&                    aSatelliteNumber,
                                                                                const   String&                     aClassification,
                                                                                const   String&                     anInternationalDesignator,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Real&                       aFirstDerivativeMeanMotion,
                                                                                const   Real&                       aSecondDeritvativeMeanMotion,
                                                                                const   Real&                       aBStarDragTerm,
                                                                                const   Integer&                    anElementSetNumber,
                                                                                const   Angle&                      anIncliation,
                                                                                const   Real&                       anEccentricity,
                                                                                const   Angle&                      aRAAN,
                                                                                const   Angle&                      anAop,
                                                                                const   Angle&                      aMeanAnomaly,
                                                                                const   Real&                       aMeanMotion,
                                                                                const   Integer&                    aRevolutionNumber                           )
{

    firstLine_ = TLE::GenerateFirstLine(aSatelliteNumber, aClassification, anInternationalDesignator, anEpoch, aFirstDerivativeMeanMotion, aSecondDeritvativeMeanMotion, aBStarDragTerm, anElementSetNumber) ;
    secondLine_ = TLE::GenerateSecondLine(aSatelliteNumber, anIncliation, anEccentricity, aRAAN, anAop, aMeanAnomaly, aMeanMotion, aRevolutionNumber) ;

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

    ostk::core::utils::Print::Header(anOutputStream, "Two-Line Elements") ;

    ostk::core::utils::Print::Line(anOutputStream) << "Line 1:"                                                  << aTle.getFirstLine() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Line 2:"                                                  << aTle.getSecondLine() ;

    ostk::core::utils::Print::Separator(anOutputStream) ;

    ostk::core::utils::Print::Line(anOutputStream) << "Satellite Name:"                                          << aTle.getSatelliteName() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Satellite Number:"                                        << aTle.getSatelliteNumber().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Classification:"                                          << aTle.getClassification() ;
    ostk::core::utils::Print::Line(anOutputStream) << "International Designator:"                                << aTle.getInternationalDesignator() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Epoch:"                                                   << aTle.getEpoch().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Mean Motion First Time Der. / 2:"                         << aTle.getMeanMotionFirstTimeDerivativeDividedByTwo().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Mean Motion Second Time Der. / 6:"                        << aTle.getMeanMotionSecondTimeDerivativeDividedBySix().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "B* Drag Term:"                                            << aTle.getBStarDragTerm().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Ephemeris Type:"                                          << aTle.getEphemerisType().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Element Set Number:"                                      << aTle.getElementSetNumber().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Inclination:"                                             << aTle.getInclination().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Right Ascension of the Ascending Node :"                  << aTle.getRaan().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Eccentricity:"                                            << aTle.getEccentricity().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Argument of Periapsis:"                                   << aTle.getAop().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Mean Anomaly:"                                            << aTle.getMeanAnomaly().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Mean Motion:"                                             << aTle.getMeanMotion().toString() ;
    ostk::core::utils::Print::Line(anOutputStream) << "Revolution Number at Epoch:"                              << aTle.getRevolutionNumberAtEpoch().toString() ;

    ostk::core::utils::Print::Footer(anOutputStream) ;

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
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Integer::Parse(firstLine_.getSubstring(2, 5).trim()) ;

}

String                          TLE::getClassification                      ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return firstLine_.getSubstring(7, 1) ;

}

String                          TLE::getInternationalDesignator             ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return firstLine_.getSubstring(9, 8).trim() ;

}

Instant                         TLE::getEpoch                               ( ) const
{

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::DateTime ;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
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
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Real::Parse(firstLine_.getSubstring(33, 10).trim()) ;

}

Real                            TLE::getMeanMotionSecondTimeDerivativeDividedBySix ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return TLE::ParseReal(firstLine_.getSubstring(44, 8).trim(), true) ;

}

Real                            TLE::getBStarDragTerm                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return TLE::ParseReal(firstLine_.getSubstring(53, 8).trim(), true) ;

}

Integer                         TLE::getEphemerisType                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Integer::Parse(firstLine_.getSubstring(62, 1)) ;

}

Integer                         TLE::getElementSetNumber                    ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Integer::Parse(firstLine_.getSubstring(64, 4).trim()) ;

}

Integer                         TLE::getFirstLineChecksum                   ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Integer::Parse(firstLine_.getSubstring(68, 1)) ;

}

Angle                           TLE::getInclination                         ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(8, 8).trim())) ;

}

Angle                           TLE::getRaan                                ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(17, 8).trim())) ;

}

Real                            TLE::getEccentricity                        ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Real::Parse("0." + secondLine_.getSubstring(26, 7).trim()) ;

}

Angle                           TLE::getAop                                 ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(34, 8).trim())) ;

}

Angle                           TLE::getMeanAnomaly                         ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(43, 8).trim())) ;

}

Derived                         TLE::getMeanMotion                          ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Derived(Real::Parse(secondLine_.getSubstring(52, 11).trim()), Derived::Unit::AngularVelocity(Angle::Unit::Revolution, physics::units::Time::Unit::Day)) ;

}

Integer                         TLE::getRevolutionNumberAtEpoch             ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Integer::Parse(secondLine_.getSubstring(63, 5).trim()) ;

}

Integer                         TLE::getSecondLineChecksum                  ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    return Integer::Parse(secondLine_.getSubstring(68, 1)) ;

}

void                            TLE::setSatelliteNumber                     (   const   Integer&                    aSatelliteNumber                            )
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    if (aSatelliteNumber > 99999 || aSatelliteNumber.isNegative())
    {
        throw ostk::core::error::runtime::Wrong("Provided Satellite Number") ;
    }

    const String satelliteNumberString = aSatelliteNumber.toString() ;
    const String sanitizedSatelliteNumberString = String::Replicate(" ", 5 - satelliteNumberString.getLength()) + satelliteNumberString ;
    const String newIntermediateFirstLine = firstLine_.getSubstring(0, 2) + sanitizedSatelliteNumberString + firstLine_.getSubstring(7, 62) ;
    const String newIntermediateSecondLine = secondLine_.getSubstring(0, 2) + sanitizedSatelliteNumberString + secondLine_.getSubstring(7, 62) ;

    const Integer firstLineNewChecksum = TLE::GenerateChecksum(newIntermediateFirstLine) ;
    const Integer secondLineNewChecksum = TLE::GenerateChecksum(newIntermediateSecondLine) ;

    firstLine_ = newIntermediateFirstLine.getSubstring(0, 68) + firstLineNewChecksum.toString() ;
    secondLine_ = newIntermediateSecondLine.getSubstring(0, 68) + secondLineNewChecksum.toString() ;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

}

void                            TLE::setEpoch                               (   const   Instant&                    anInstant                                   )
{

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Date ;
    using ostk::physics::time::DateTime ;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    const DateTime epochDateTime = anInstant.getDateTime(Scale::UTC) ;
    const Integer epochYear = epochDateTime.getDate().getYear() ;
    String epochYearTwoDigitsString ;

    if (epochYear >= 1957 && epochYear <= 2056)
    {
        epochYearTwoDigitsString = epochYear.toString().getTail(2) ;
    }

    else
    {
        throw ostk::core::error::runtime::Wrong("Epoch Year") ;
    }

    const Real epochDay = Duration::Between(Instant::DateTime(DateTime(epochYear, 1, 1, 0, 0, 0), Scale::UTC), anInstant).inDays() + Duration::Days(1.0).inDays() ;

    const Integer epochDayInteger = epochDay.floor() ;
    const String epochDayIntegerString = epochDayInteger.toString() ;
    const String sanitizedEpochDayIntegerString = String::Replicate("0", 3 - epochDayIntegerString.getLength()) + epochDayIntegerString ;

    const Real epochDayFraction = epochDay - Real::Integer(epochDayInteger) ;
    const String epochDayFractionString = epochDayFraction.toString() ;
    const String sanitizedEpochDayFractionString = epochDayFractionString.getSubstring(1, 9) ;

    const String newIntermediateFirstLine = firstLine_.getSubstring(0, 18) + epochYearTwoDigitsString + sanitizedEpochDayIntegerString + sanitizedEpochDayFractionString + firstLine_.getSubstring(32, 37) ;

    Integer firstLineNewChecksum = TLE::GenerateChecksum(newIntermediateFirstLine) ;

    firstLine_ = newIntermediateFirstLine.getSubstring(0, 68) + firstLineNewChecksum.toString() ;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

}

void                            TLE::setRevolutionNumberAtEpoch             (   const   Integer&                    aRevolutionNumberAtEpoch                    )
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

    if (aRevolutionNumberAtEpoch > 99999 || aRevolutionNumberAtEpoch.isNegative())
    {
        throw ostk::core::error::runtime::Wrong("Provided Revolution Number at Epoch") ;
    }

    const String revolutionNumberAtEpochString = aRevolutionNumberAtEpoch.toString() ;
    const String sanitizedRevolutionNumberAtEpochString = String::Replicate("0", 5 - revolutionNumberAtEpochString.getLength()) + revolutionNumberAtEpochString ;
    const String newIntermediateSecondLine = secondLine_.getSubstring(0, 63) + sanitizedRevolutionNumberAtEpochString + secondLine_.getSubstring(68, 1) ;

    const Integer secondLineNewChecksum = TLE::GenerateChecksum(newIntermediateSecondLine) ;

    secondLine_ = newIntermediateSecondLine.getSubstring(0, 68) + secondLineNewChecksum.toString() ;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE") ;
    }

}

TLE                             TLE::Undefined                              ( )
{
    return { String::Empty(), String::Empty(), String::Empty() } ;
}

bool                            TLE::CanParse                               (   const   String&                     aString                                     )
{

    using ostk::core::types::Size ;
    using ostk::core::ctnr::Array ;

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

    using ostk::core::types::Size ;
    using ostk::core::ctnr::Array ;

    if (aString.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("String") ;
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
        return TLE { lines.at(0), lines.at(1) } ;
    }
    else if (lines.getSize() >= 3)
    {
        return TLE { lines.at(0), lines.at(1), lines.at(2) } ;
    }

    throw ostk::core::error::runtime::Wrong("String", aString) ;

    return TLE::Undefined() ;

}

TLE                             TLE::Load                                   (   const   File&                       aFile                                       )
{

    if (!aFile.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("File") ;
    }

    if (!aFile.exists())
    {
        throw ostk::core::error::RuntimeError("File [{}] does not exist.", aFile.toString()) ;
    }

    const String tleString = aFile.getContents() ;

    return TLE::Parse(tleString) ;

}

Integer                         TLE::GenerateChecksum                       (   const   String&                     aLine                                       )
{

    using ostk::core::types::Index ;

    if (aLine.getLength() != 69)
    {
        throw ostk::core::error::runtime::Wrong("Line") ;
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

    using ostk::core::types::Index ;

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

String               TLE::GenerateFirstLine                                 (   const   Integer&                    aSatelliteNumber,
                                                                                const   String&                     aClassification,
                                                                                const   String&                     anInternationalDesignator,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Real&                       aFirstDerivativeMeanMotion,
                                                                                const   Real&                       aSecondDeritvativeMeanMotion,
                                                                                const   Real&                       aBStarDragTerm,
                                                                                const   Integer&                    anElementSetNumber                          )
{

    using ostk::physics::time::Date ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::time::Time ;
    using ostk::physics::time::Scale ;

    if (std::to_string(aSatelliteNumber).length() > 5)
    {
        throw ostk::core::error::runtime::Wrong("Satellite Catalog Number greater than length 5.") ;
    }

    if ((aClassification != "U") && (aClassification != "C") && (aClassification != "S"))
    {
        throw ostk::core::error::runtime::Wrong("Classification must be 'C', 'U' or 'S'.") ;
    }

    if (anInternationalDesignator.length() > 8)
    {
        throw ostk::core::error::runtime::Wrong("International designator greater than length 8.") ;
    }

    auto getAssumedDecimalFormat = [] (const Real& aNumber) -> String
    {

        if (!aNumber.isDefined() || aNumber.isNear(0.0, 1e-12))
        {
            return " 00000-0" ;
        }

        String numberAsString = String::Format("{:.4e}", aNumber) ;

        if (aNumber < 0.0)
        {
            numberAsString = String::Format("-{}{}-{}", numberAsString.getSubstring(1, 1), numberAsString.getSubstring(3, 4), std::stoi(numberAsString.getTail(1)) - 1) ;
        } else
        {
            numberAsString = String::Format(" {}{}-{}", numberAsString.getSubstring(0, 1), numberAsString.getSubstring(2, 4), std::stoi(numberAsString.getTail(1)) - 1) ;
        }

        return numberAsString ;

    } ;

    // Epoch
    Date date = anEpoch.getDateTime(Scale::UTC).getDate() ;

    Time time = anEpoch.getDateTime(Scale::UTC).getTime();
    time.setHour(0) ;
    time.setMinute(0) ;
    time.setSecond(0) ;
    time.setMillisecond(0) ;
    time.setMicrosecond(0) ;
    time.setNanosecond(0) ;

    const String dayFraction = String::Format("{:.8f}", Real((anEpoch - Instant::DateTime(DateTime(date, time), Scale::UTC)).inSeconds()) / 86400.0) ;

    date.setDay(1) ;
    date.setMonth(1) ;

    const Integer days = (anEpoch - Instant::DateTime(DateTime(date, time), Scale::UTC)).getDays() + 1 ;

    const String epoch = String::Format("{}{:03d}{}", std::to_string(date.getYear()).substr(2,2), days, dayFraction.getSubstring(1, 9)) ;

    // First derivative mean motion

    String firstDerivativeMeanMotion = String::Format("{:.8f}", aFirstDerivativeMeanMotion) ;
    if (!aFirstDerivativeMeanMotion.isDefined() || aFirstDerivativeMeanMotion.isNear(0.0, 1e-12))
    {
        firstDerivativeMeanMotion = "0.00000000" ;
    }
    else if (aFirstDerivativeMeanMotion < 0.0)
    {
        firstDerivativeMeanMotion = String::Format("-{}", firstDerivativeMeanMotion.getSubstring(2, 9)) ;
    } else {
        firstDerivativeMeanMotion = String::Format(" {}", firstDerivativeMeanMotion.getSubstring(1, 9)) ;
    }

    // Second derivative mean motion

    const String secondDeritvativeMeanMotion = getAssumedDecimalFormat(aSecondDeritvativeMeanMotion) ;

    // BStar term

    const String bStarDragTerm = getAssumedDecimalFormat(aBStarDragTerm) ;

    const String firstLine = String::Format("1 {:05d}{} {:8} {} {} {} {} 0 {:4d}0", aSatelliteNumber, aClassification, anInternationalDesignator, epoch, firstDerivativeMeanMotion, secondDeritvativeMeanMotion, bStarDragTerm, anElementSetNumber) ;

    return firstLine.substr(0, 68) + std::to_string(TLE::GenerateChecksum(firstLine)) ;

}

        String               TLE::GenerateSecondLine                        (   const   Integer&                    aSatelliteNumber,
                                                                                const   Angle&                      anIncliation,
                                                                                const   Real&                       anEccentricity,
                                                                                const   Angle&                      aRAAN,
                                                                                const   Angle&                      anAop,
                                                                                const   Angle&                      aMeanAnomaly,
                                                                                const   Real&                       aMeanMotion,
                                                                                const   Integer&                    aRevolutionNumber                           )
{

    if (std::to_string(aSatelliteNumber).length() > 5)
    {
        throw ostk::core::error::runtime::Wrong("Satellite Catalog Number greater than length 5.") ;
    }

    if ((anEccentricity < 0.0) || (anEccentricity > 1.0))
    {
        throw ostk::core::error::runtime::Wrong("Eccentricity is invalid.") ;
    }

    auto fill = [] (const String& aString, const int& numberOfZeros) -> String
    {

        int stringLength = aString.getLength() ;

        return String(numberOfZeros - std::min(numberOfZeros, stringLength), '0') + aString ;

    } ;

    const String inclination = fill(String::Format("{:.4f}", anIncliation.inDegrees(0.0, 360.0)), 8) ;

    const String raan = fill(String::Format("{:.4f}", aRAAN.inDegrees(0.0, 360.0)), 8) ;

    const String eccentricity = fill(String::Format("{:.7f}", anEccentricity).getSubstring(2, 7), 7) ;

    const String aop = fill(String::Format("{:.4f}", anAop.inDegrees(0.0, 360.0)), 8) ;

    const String meanAnomaly = fill(String::Format("{:.4f}", aMeanAnomaly.inDegrees(0.0, 360.0)), 8) ;

    const String meanMotion = fill(String::Format("{:.8f}", aMeanMotion), 11) ;

    const String secondLine = String::Format("2 {:05d} {} {} {} {} {} {}{:05d}0", aSatelliteNumber, inclination, raan, eccentricity, aop, meanAnomaly, meanMotion, aRevolutionNumber);

    return secondLine.substr(0, 68) + std::to_string(TLE::GenerateChecksum(secondLine));

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
