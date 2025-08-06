/// Apache License 2.0

#include <iostream>
#include <regex>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{
namespace sgp4
{

TLE::TLE(const String& aFirstLine, const String& aSecondLine)
    : satelliteName_(String::Empty()),
      firstLine_(aFirstLine),
      secondLine_(aSecondLine)
{
    if (((!aFirstLine.isEmpty()) || (!aSecondLine.isEmpty())) && (!TLE::CanParse(aFirstLine, aSecondLine)))
    {
        throw ostk::core::error::runtime::Wrong(
            "TLE", String::Format("First line: [{}], Second line: [{}]", aFirstLine, aSecondLine)
        );
    }
}

TLE::TLE(const String& aSatelliteName, const String& aFirstLine, const String& aSecondLine)
    : satelliteName_(aSatelliteName),
      firstLine_(aFirstLine),
      secondLine_(aSecondLine)
{
    if (((!aFirstLine.isEmpty()) || (!aSecondLine.isEmpty())) && (!TLE::CanParse(aFirstLine, aSecondLine)))
    {
        throw ostk::core::error::runtime::Wrong(
            "TLE", String::Format("First line: [{}], Second line: [{}]", aFirstLine, aSecondLine)
        );
    }
}

bool TLE::operator==(const TLE& aTle) const
{
    if ((!this->isDefined()) || (!aTle.isDefined()))
    {
        return false;
    }

    return (satelliteName_ == aTle.satelliteName_) && (firstLine_ == aTle.firstLine_) &&
           (secondLine_ == aTle.secondLine_);
}

bool TLE::operator!=(const TLE& aTle) const
{
    if ((!this->isDefined()) || (!aTle.isDefined()))
    {
        return true;
    }

    return (satelliteName_ != aTle.satelliteName_) || (firstLine_ != aTle.firstLine_) ||
           (secondLine_ != aTle.secondLine_);
}

std::ostream& operator<<(std::ostream& anOutputStream, const TLE& aTle)
{
    ostk::core::utils::Print::Header(anOutputStream, "Two-Line Elements");

    ostk::core::utils::Print::Line(anOutputStream) << "Line 1:" << aTle.getFirstLine();
    ostk::core::utils::Print::Line(anOutputStream) << "Line 2:" << aTle.getSecondLine();

    ostk::core::utils::Print::Separator(anOutputStream);

    ostk::core::utils::Print::Line(anOutputStream) << "Satellite Name:" << aTle.getSatelliteName();
    ostk::core::utils::Print::Line(anOutputStream) << "Satellite Number:" << aTle.getSatelliteNumber().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Classification:" << aTle.getClassification();
    ostk::core::utils::Print::Line(anOutputStream) << "International Designator:" << aTle.getInternationalDesignator();
    ostk::core::utils::Print::Line(anOutputStream) << "Epoch:" << aTle.getEpoch().toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Mean Motion First Time Der. / 2:" << aTle.getMeanMotionFirstTimeDerivativeDividedByTwo().toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Mean Motion Second Time Der. / 6:" << aTle.getMeanMotionSecondTimeDerivativeDividedBySix().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "B* Drag Term:" << aTle.getBStarDragTerm().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Ephemeris Type:" << aTle.getEphemerisType().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Element Set Number:" << aTle.getElementSetNumber().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Inclination:" << aTle.getInclination().toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Right Ascension of the Ascending Node :" << aTle.getRaan().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Eccentricity:" << aTle.getEccentricity().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Argument of Periapsis:" << aTle.getAop().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Mean Anomaly:" << aTle.getMeanAnomaly().toString();
    ostk::core::utils::Print::Line(anOutputStream) << "Mean Motion:" << aTle.getMeanMotion().toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Revolution Number at Epoch:" << aTle.getRevolutionNumberAtEpoch().toString();

    ostk::core::utils::Print::Footer(anOutputStream);

    return anOutputStream;
}

bool TLE::isDefined() const
{
    return (!firstLine_.isEmpty()) && (!secondLine_.isEmpty());
}

String TLE::getSatelliteName() const
{
    return satelliteName_;
}

String TLE::getFirstLine() const
{
    return firstLine_;
}

String TLE::getSecondLine() const
{
    return secondLine_;
}

Integer TLE::getSatelliteNumber() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Integer::Parse(firstLine_.getSubstring(2, 5).trim());
}

String TLE::getClassification() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return firstLine_.getSubstring(7, 1);
}

String TLE::getInternationalDesignator() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return firstLine_.getSubstring(9, 8).trim();
}

Instant TLE::getEpoch() const
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Scale;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    const String epochYearTwoDigitsString = firstLine_.getSubstring(18, 2).trim();
    const String epochDayString = firstLine_.getSubstring(20, 12).trim();

    const Integer epochYearTwoDigits = Integer::Parse(epochYearTwoDigitsString);

    // See: https://www.celestrak.com/columns/v04n03/

    const Integer epochYear =
        (((epochYearTwoDigits >= 57) && (epochYearTwoDigits <= 99)) ? 1900 : 2000) + epochYearTwoDigits;
    const Real epochDay = Real::Parse(epochDayString);

    return Instant::DateTime(DateTime(epochYear, 1, 1, 0, 0, 0), Scale::UTC) + Duration::Days(epochDay - 1.0);
}

Real TLE::getMeanMotionFirstTimeDerivativeDividedByTwo() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Real::Parse(firstLine_.getSubstring(33, 10).trim());
}

Real TLE::getMeanMotionSecondTimeDerivativeDividedBySix() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return TLE::ParseReal(firstLine_.getSubstring(44, 8).trim(), true);
}

Real TLE::getBStarDragTerm() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return TLE::ParseReal(firstLine_.getSubstring(53, 8).trim(), true);
}

Integer TLE::getEphemerisType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Integer::Parse(firstLine_.getSubstring(62, 1));
}

Integer TLE::getElementSetNumber() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Integer::Parse(firstLine_.getSubstring(64, 4).trim());
}

Integer TLE::getFirstLineChecksum() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Integer::Parse(firstLine_.getSubstring(68, 1));
}

Angle TLE::getInclination() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(8, 8).trim()));
}

Angle TLE::getRaan() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(17, 8).trim()));
}

Real TLE::getEccentricity() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Real::Parse("0." + secondLine_.getSubstring(26, 7).trim());
}

Angle TLE::getAop() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(34, 8).trim()));
}

Angle TLE::getMeanAnomaly() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Angle::Degrees(Real::Parse(secondLine_.getSubstring(43, 8).trim()));
}

Derived TLE::getMeanMotion() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Derived(
        Real::Parse(secondLine_.getSubstring(52, 11).trim()),
        Derived::Unit::AngularVelocity(Angle::Unit::Revolution, physics::unit::Time::Unit::Day)
    );
}

Integer TLE::getRevolutionNumberAtEpoch() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Integer::Parse(secondLine_.getSubstring(63, 5).trim());
}

Integer TLE::getSecondLineChecksum() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    return Integer::Parse(secondLine_.getSubstring(68, 1));
}

void TLE::setSatelliteNumber(const Integer& aSatelliteNumber)
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    if (aSatelliteNumber > 99999 || aSatelliteNumber.isNegative())
    {
        throw ostk::core::error::runtime::Wrong("Provided Satellite Number");
    }

    const String satelliteNumberString = aSatelliteNumber.toString();
    const String sanitizedSatelliteNumberString =
        String::Replicate(" ", 5 - satelliteNumberString.getLength()) + satelliteNumberString;
    const String newIntermediateFirstLine =
        firstLine_.getSubstring(0, 2) + sanitizedSatelliteNumberString + firstLine_.getSubstring(7, 62);
    const String newIntermediateSecondLine =
        secondLine_.getSubstring(0, 2) + sanitizedSatelliteNumberString + secondLine_.getSubstring(7, 62);

    const Integer firstLineNewChecksum = TLE::GenerateChecksum(newIntermediateFirstLine);
    const Integer secondLineNewChecksum = TLE::GenerateChecksum(newIntermediateSecondLine);

    firstLine_ = newIntermediateFirstLine.getSubstring(0, 68) + firstLineNewChecksum.toString();
    secondLine_ = newIntermediateSecondLine.getSubstring(0, 68) + secondLineNewChecksum.toString();

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }
}

void TLE::setEpoch(const Instant& anInstant)
{
    using ostk::physics::time::Date;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Scale;

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    const DateTime epochDateTime = anInstant.getDateTime(Scale::UTC);
    const Integer epochYear = epochDateTime.getDate().getYear();
    String epochYearTwoDigitsString;

    if (epochYear >= 1957 && epochYear <= 2056)
    {
        epochYearTwoDigitsString = epochYear.toString().getTail(2);
    }

    else
    {
        throw ostk::core::error::runtime::Wrong("Epoch Year");
    }

    const Real epochDay =
        Duration::Between(Instant::DateTime(DateTime(epochYear, 1, 1, 0, 0, 0), Scale::UTC), anInstant).inDays() +
        Duration::Days(1.0).inDays();

    const Integer epochDayInteger = epochDay.floor();
    const String epochDayIntegerString = epochDayInteger.toString();
    const String sanitizedEpochDayIntegerString =
        String::Replicate("0", 3 - epochDayIntegerString.getLength()) + epochDayIntegerString;

    const Real epochDayFraction = epochDay - Real::Integer(epochDayInteger);
    const String epochDayFractionString = epochDayFraction.toString();
    const String sanitizedEpochDayFractionString = epochDayFractionString.getSubstring(1, 9);

    const String newIntermediateFirstLine = firstLine_.getSubstring(0, 18) + epochYearTwoDigitsString +
                                            sanitizedEpochDayIntegerString + sanitizedEpochDayFractionString +
                                            firstLine_.getSubstring(32, 37);

    Integer firstLineNewChecksum = TLE::GenerateChecksum(newIntermediateFirstLine);

    firstLine_ = newIntermediateFirstLine.getSubstring(0, 68) + firstLineNewChecksum.toString();

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }
}

void TLE::setRevolutionNumberAtEpoch(const Integer& aRevolutionNumberAtEpoch)
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }

    if (aRevolutionNumberAtEpoch > 99999 || aRevolutionNumberAtEpoch.isNegative())
    {
        throw ostk::core::error::runtime::Wrong("Provided Revolution Number at Epoch");
    }

    const String revolutionNumberAtEpochString = aRevolutionNumberAtEpoch.toString();
    const String sanitizedRevolutionNumberAtEpochString =
        String::Replicate(" ", 5 - revolutionNumberAtEpochString.getLength()) + revolutionNumberAtEpochString;
    const String newIntermediateSecondLine =
        secondLine_.getSubstring(0, 63) + sanitizedRevolutionNumberAtEpochString + secondLine_.getSubstring(68, 1);

    const Integer secondLineNewChecksum = TLE::GenerateChecksum(newIntermediateSecondLine);

    secondLine_ = newIntermediateSecondLine.getSubstring(0, 68) + secondLineNewChecksum.toString();

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("TLE");
    }
}

TLE TLE::Undefined()
{
    return {String::Empty(), String::Empty(), String::Empty()};
}

bool TLE::CanParse(const String& aString)
{
    using ostk::core::container::Array;
    using ostk::core::type::Size;

    Array<String> lines = Array<String>::Empty();

    std::istringstream stringStream(aString);

    String lineBuffer;

    while (std::getline(stringStream, lineBuffer))
    {
        lineBuffer.erase(std::remove(lineBuffer.begin(), lineBuffer.end(), '\r'), lineBuffer.end());
        lineBuffer.erase(std::remove(lineBuffer.begin(), lineBuffer.end(), '\n'), lineBuffer.end());

        if (!lineBuffer.isEmpty())
        {
            lines.add(lineBuffer);
        }
    }

    if (lines.getSize() == 2)
    {
        return TLE::CanParse(lines.at(0), lines.at(1));
    }
    else if (lines.getSize() >= 3)
    {
        return TLE::CanParse(lines.at(1), lines.at(2));
    }

    return false;
}

bool TLE::CanParse(const String& aFirstLine, const String& aSecondLine)
{
    String firstLine = aFirstLine;

    firstLine.erase(std::remove(firstLine.begin(), firstLine.end(), '\r'), firstLine.end());
    firstLine.erase(std::remove(firstLine.begin(), firstLine.end(), '\n'), firstLine.end());

    String secondLine = aSecondLine;

    secondLine.erase(std::remove(secondLine.begin(), secondLine.end(), '\r'), secondLine.end());
    secondLine.erase(std::remove(secondLine.begin(), secondLine.end(), '\n'), secondLine.end());

    if ((firstLine.getLength() == 69) && (secondLine.getLength() == 69))
    {
        if (Integer::CanParse(firstLine[68]) && Integer::CanParse(secondLine[68]))
        {
            return (Integer::Parse(firstLine[68]) == TLE::GenerateChecksum(firstLine)) &&
                   (Integer::Parse(secondLine[68]) == TLE::GenerateChecksum(secondLine));
        }
    }

    return false;
}

TLE TLE::Parse(const String& aString)
{
    using ostk::core::container::Array;
    using ostk::core::type::Size;

    if (aString.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("String");
    }

    Array<String> lines = Array<String>::Empty();

    std::istringstream stringStream(aString);

    String lineBuffer;

    while (std::getline(stringStream, lineBuffer))
    {
        lineBuffer.erase(std::remove(lineBuffer.begin(), lineBuffer.end(), '\r'), lineBuffer.end());
        lineBuffer.erase(std::remove(lineBuffer.begin(), lineBuffer.end(), '\n'), lineBuffer.end());

        if (!lineBuffer.isEmpty())
        {
            lines.add(lineBuffer);
        }
    }

    if (lines.getSize() == 2)
    {
        return TLE {lines.at(0), lines.at(1)};
    }
    else if (lines.getSize() >= 3)
    {
        return TLE {lines.at(0), lines.at(1), lines.at(2)};
    }

    throw ostk::core::error::runtime::Wrong("String", aString);

    return TLE::Undefined();
}

TLE TLE::Load(const File& aFile)
{
    if (!aFile.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("File");
    }

    if (!aFile.exists())
    {
        throw ostk::core::error::RuntimeError("File [{}] does not exist.", aFile.toString());
    }

    const String tleString = aFile.getContents();

    return TLE::Parse(tleString);
}

TLE TLE::Construct(
    const String& aSatelliteName,
    const Integer& aSatelliteNumber,
    const String& aClassification,
    const String& anInternationalDesignator,
    const Instant& anEpoch,
    const Real& aMeanMotionFirstTimeDerivativeDividedByTwo,
    const Real& aMeanMotionSecondTimeDerivativeDividedBySix,
    const Real& aBStarDragTerm,
    const Integer& anEphemerisType,
    const Integer& anElementSetNumber,
    const Angle& anInclination,
    const Angle& aRaan,
    const Real& anEccentricity,
    const Angle& anAop,
    const Angle& aMeanAnomaly,
    const Derived& aMeanMotion,
    const Integer& aRevolutionNumberAtEpoch
)
{
    // https://celestrak.org/columns/v04n03/#FAQ01
    // In general, any number smaller than the maximum field size can be padded with either leading spaces or leading
    // zeros. In other words, an epoch can be represented as either 98001.12345678 or 98 1.12345678 or an inclination
    // can be represented as 28.1234 or 028.1234. Convention uses leading zeros for fields 1.5 and 1.8 and leading
    // spaces elsewhere, but either is valid.

    using ostk::physics::time::DateTime;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;
    using ostk::physics::unit::Derived;
    using ostk::physics::unit::Time;

    if (aSatelliteNumber > 99999)
    {
        throw ostk::core::error::runtime::Wrong("Satellite number", aSatelliteNumber);
    }

    if (aClassification.getLength() != 1)
    {
        throw ostk::core::error::runtime::Wrong("Classification", aClassification);
    }

    if (anInternationalDesignator.getLength() == 0)
    {
        throw ostk::core::error::runtime::Wrong("International designator", anInternationalDesignator);
    }

    if (!std::regex_match(anInternationalDesignator, std::regex("^\\d{2}\\d{3}\\w{1,3}$")))
    {
        throw ostk::core::error::runtime::Wrong("International designator", anInternationalDesignator);
    }

    if (aMeanMotionFirstTimeDerivativeDividedByTwo.abs() > 1.0)
    {
        throw ostk::core::error::runtime::Wrong(
            "Mean motion first time derivative divided by two", aMeanMotionFirstTimeDerivativeDividedByTwo
        );
    }

    if ((anEphemerisType < 0) || (anEphemerisType > 5))
    {
        throw ostk::core::error::runtime::Wrong("Ephemeris type", anEphemerisType);
    }

    if (anElementSetNumber > 99999)
    {
        throw ostk::core::error::runtime::Wrong("Element set number", anElementSetNumber);
    }

    if ((anEccentricity < 0.0) || (anEccentricity >= 1.0))
    {
        throw ostk::core::error::runtime::Wrong("Eccentricity", anEccentricity);
    }

    const Real meanMotion_revPerDay =
        aMeanMotion.in(Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day));
    if ((meanMotion_revPerDay < 0.0) || (meanMotion_revPerDay > 99.99999999))
    {
        throw ostk::core::error::runtime::Wrong("Mean motion", meanMotion_revPerDay);
    }

    if ((aRevolutionNumberAtEpoch < 0) || (aRevolutionNumberAtEpoch > 99999))
    {
        throw ostk::core::error::runtime::Wrong("Revolution number at epoch", aRevolutionNumberAtEpoch);
    }

    const DateTime epochDateTime = anEpoch.getDateTime(Scale::UTC);  // TBC: Scale

    const Integer dayOfYear =
        (anEpoch - Instant::DateTime(DateTime(epochDateTime.getDate().getYear(), 1, 1, 0, 0, 0), Scale::UTC))
            .getDays() +
        1;

    const Integer epochYear = epochDateTime.getDate().getYear() - 2000;
    const Real epochDay = Real(dayOfYear) + (epochDateTime.getTime().getHour() / 24.0) +
                          (epochDateTime.getTime().getMinute() / 1440.0) +
                          (epochDateTime.getTime().getFloatingSeconds() / 86400.0);

    const auto formatFloatingPointNotation = [](const Real& aValue) -> String
    {
        return ((aValue < 0.0) ? "-" : " ") + String::Format("{:.8f}", aValue.abs()).replace("0.", ".");
    };

    const auto formatScientificNotation = [](const Real& aValue) -> String
    {
        if (aValue == 0.0)
        {
            return " 00000-0";
        }

        String value = String::Format("{:.5e}", aValue.abs());
        value = value.replace("0.", "");

        {
            const auto end = value.find("e+");
            if (end != std::string::npos)
            {
                value = String::Format(
                    "{:0>5}+{:1}",
                    Integer::Parse(value.getHead(end).replace(".", "").getHead(5)),
                    Integer::Parse(value.getTail(value.getLength() - end - 2)) + 1
                );
            }
        }

        {
            const auto end = value.find("e-");
            if (end != std::string::npos)
            {
                value = String::Format(
                    "{:0>5}-{:1}",
                    Integer::Parse(value.getHead(end).replace(".", "").getHead(5)),
                    Integer::Parse(value.getTail(value.getLength() - end - 2)) - 1
                );
            }
        }

        value = ((aValue < 0.0) ? "-" : " ") + value;

        return value;
    };

    const String firstLine = String::Format(
        "1 {: >5}{:1} {: <8} {:0>2}{:0>12.8f} {} {} {:8} {:1}{: >5}0",
        aSatelliteNumber,
        aClassification,
        anInternationalDesignator,
        epochYear,
        epochDay,
        formatFloatingPointNotation(aMeanMotionFirstTimeDerivativeDividedByTwo),
        formatScientificNotation(aMeanMotionSecondTimeDerivativeDividedBySix),
        formatScientificNotation(aBStarDragTerm),
        anEphemerisType,
        anElementSetNumber
    );

    const String secondLine = String::Format(
        "2 {: >5} {:8.4f} {:8.4f} {} {:8.4f} {:8.4f} {:11.8f}{: >5}0",
        aSatelliteNumber,
        anInclination.inDegrees(0.0, 360.0),
        aRaan.inDegrees(0.0, 360.0),
        String::Format("{:9.7f}", anEccentricity).replace("0.", ""),
        anAop.inDegrees(0.0, 360.0),
        aMeanAnomaly.inDegrees(0.0, 360.0),
        meanMotion_revPerDay,
        aRevolutionNumberAtEpoch
    );

    return {
        aSatelliteName,
        firstLine.getHead(firstLine.getLength() - 1) + String::Format("{:1}", GenerateChecksum(firstLine)),
        secondLine.getHead(secondLine.getLength() - 1) + String::Format("{:1}", GenerateChecksum(secondLine)),
    };
}

TLE TLE::Construct(
    const Integer& aSatelliteNumber,
    const String& aClassification,
    const String& anInternationalDesignator,
    const Instant& anEpoch,
    const Real& aMeanMotionFirstTimeDerivativeDividedByTwo,
    const Real& aMeanMotionSecondTimeDerivativeDividedBySix,
    const Real& aBStarDragTerm,
    const Integer& anEphemerisType,
    const Integer& anElementSetNumber,
    const Angle& anInclination,
    const Angle& aRaan,
    const Real& anEccentricity,
    const Angle& anAop,
    const Angle& aMeanAnomaly,
    const Derived& aMeanMotion,
    const Integer& aRevolutionNumberAtEpoch
)
{
    return TLE::Construct(
        String::Empty(),
        aSatelliteNumber,
        aClassification,
        anInternationalDesignator,
        anEpoch,
        aMeanMotionFirstTimeDerivativeDividedByTwo,
        aMeanMotionSecondTimeDerivativeDividedBySix,
        aBStarDragTerm,
        anEphemerisType,
        anElementSetNumber,
        anInclination,
        aRaan,
        anEccentricity,
        anAop,
        aMeanAnomaly,
        aMeanMotion,
        aRevolutionNumberAtEpoch
    );
}

Integer TLE::GenerateChecksum(const String& aLine)
{
    using ostk::core::type::Index;

    if (aLine.getLength() != 69)
    {
        throw ostk::core::error::runtime::Wrong("Line");
    }

    int checksum = 0;

    for (Index idx = 0; idx < (aLine.getLength() - 1); ++idx)
    {
        checksum +=
            Integer::CanParse(aLine[idx]) ? static_cast<int>(Integer::Parse(aLine[idx])) : (aLine[idx] == '-' ? 1 : 0);
    }

    checksum -= (checksum / 10) * 10;  // Last digit

    return checksum;
}

Real TLE::ParseReal(const String& aString, bool isDecimalPointAssumed)
{
    using ostk::core::type::Index;

    String string = aString;

    // Add leading 0 if decimal point is assumed

    if (isDecimalPointAssumed)
    {
        if (aString[0] == '-')
        {
            string = "-0." + string.getSubstring(1, string.getLength() - 1);
        }
        else if (aString[0] == '+')
        {
            string = "+0." + string.getSubstring(1, string.getLength() - 1);
        }
        else
        {
            string = "0." + string;
        }
    }

    // Transform exponent (e.g., 123-456 to 123e-456)

    Index minusSignIndex = string.rfind('-');

    if ((minusSignIndex != String::npos) && (minusSignIndex > 0) && (string[minusSignIndex - 1] != 'e') &&
        (string[minusSignIndex - 1] != 'E'))
    {
        string.std::string::replace(minusSignIndex, 1, "e-");
    }

    Index plusSignIndex = string.rfind('+');

    if ((plusSignIndex != String::npos) && (plusSignIndex > 0) && (string[plusSignIndex - 1] != 'e') &&
        (string[plusSignIndex - 1] != 'E'))
    {
        string.std::string::replace(plusSignIndex, 1, "e+");
    }

    return Real::Parse(string);
}

}  // namespace sgp4
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
