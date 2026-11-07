/// Apache License 2.0

#include <cmath>
#include <limits>
#include <optional>

#include <boost/lexical_cast.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/Message/CCSDS/CDM.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace message
{
namespace ccsds
{

namespace
{

/// @brief Parse a string into a double
///
/// "Inf", "+Inf" and "-Inf" map to infinities. Any other non-number (including "NaN" and "Undefined") is rejected.
///
/// @param [in] aString A string
/// @return Parsed value
double ParseDouble(const String& aString)
{
    if (aString.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("String");
    }

    if ((aString == "Inf") || (aString == "+Inf"))
    {
        return std::numeric_limits<double>::infinity();
    }

    if (aString == "-Inf")
    {
        return -std::numeric_limits<double>::infinity();
    }

    double value = 0.0;

    if ((!boost::conversion::try_lexical_convert<double>(aString, value)) || std::isnan(value))
    {
        throw ostk::core::error::RuntimeError("Cannot cast string [" + aString + "] to Real.");
    }

    return value;
}

/// @brief Parse a string into an int
///
/// @param [in] aString A string
/// @return Parsed value
int ParseInt(const String& aString)
{
    if (aString.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("String");
    }

    int value = 0;

    if (!boost::conversion::try_lexical_convert<int>(aString, value))
    {
        throw ostk::core::error::RuntimeError("Cannot cast string [" + aString + "] to Integer.");
    }

    return value;
}

}  // namespace

CDM::CDM(
    const CDM::Header& aHeader,
    const CDM::RelativeMetadata& aRelativeMetadata,
    const Array<CDM::Metadata>& aMetadataArray,
    const Array<CDM::Data> aDataArray
)
    : header_(aHeader),
      relativeMetadata_(aRelativeMetadata),
      objectsMetadata_(aMetadataArray),
      objectsData_(aDataArray)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const CDM& anCDM)
{
    anCDM.print(anOutputStream);

    return anOutputStream;
}

const CDM::Header& CDM::getHeader() const
{
    return this->header_;
}

const CDM::RelativeMetadata& CDM::getRelativeMetadata() const
{
    return this->relativeMetadata_;
}

const Array<CDM::Metadata>& CDM::getMetadataArray() const
{
    return this->objectsMetadata_;
}

const Array<CDM::Data>& CDM::getDataArray() const
{
    return this->objectsData_;
}

const CDM::Metadata& CDM::getObjectMetadataAt(const Index& anIndex) const
{
    return this->objectsMetadata_.at(anIndex);
}

const CDM::Data& CDM::getObjectDataAt(const Index& anIndex) const
{
    return this->objectsData_.at(anIndex);
}

const String& CDM::getCCSDSCDMVersion() const
{
    return this->header_.ccsdsCdmVersion;
}

const Instant& CDM::getCreationDate() const
{
    return this->header_.creationDate;
}

const String& CDM::getOriginator() const
{
    return this->header_.originator;
}

const String& CDM::getMessageFor() const
{
    return this->header_.messageFor;
}

const String& CDM::getMessageId() const
{
    return this->header_.messageId;
}

const Instant& CDM::getTCA() const
{
    return this->relativeMetadata_.TCA;
}

const Length& CDM::getMissDistance() const
{
    return this->relativeMetadata_.missDistance;
}

const std::optional<Position>& CDM::getRelativePosition() const
{
    return this->relativeMetadata_.relativePosition;
}

const std::optional<Velocity>& CDM::getRelativeVelocity() const
{
    return this->relativeMetadata_.relativeVelocity;
}

std::optional<double> CDM::getCollisionProbability() const
{
    return this->relativeMetadata_.collisionProbability;
}

const String& CDM::getCollisionProbabilityMethod() const
{
    return this->relativeMetadata_.collisionProbabilityMethod;
}

int CDM::getObjectDesignator(const Index& anIndex) const
{
    const CDM::Metadata& objectMetadata = this->getObjectMetadataAt(anIndex);

    return objectMetadata.objectDesignator;
}

const String& CDM::getObjectName(const Index& anIndex) const
{
    const CDM::Metadata& objectMetadata = this->getObjectMetadataAt(anIndex);

    return objectMetadata.objectName;
}

const String& CDM::getObjectInternationalDesignator(const Index& anIndex) const
{
    const CDM::Metadata& objectMetadata = this->getObjectMetadataAt(anIndex);

    return objectMetadata.internationalDesignator;
}

CDM::ObjectType CDM::getObjectType(const Index& anIndex) const
{
    const CDM::Metadata& objectMetadata = this->getObjectMetadataAt(anIndex);

    return objectMetadata.objectType;
}

const String& CDM::getObjectEphemerisName(const Index& anIndex) const
{
    const CDM::Metadata& objectMetadata = this->getObjectMetadataAt(anIndex);

    return objectMetadata.ephemerisName;
}

const String& CDM::getObjectCovarianceMethod(const Index& anIndex) const
{
    const CDM::Metadata& objectMetadata = this->getObjectMetadataAt(anIndex);

    return objectMetadata.covarianceMethod;
}

const String& CDM::getObjectManeuverability(const Index& anIndex) const
{
    const CDM::Metadata& objectMetadata = this->getObjectMetadataAt(anIndex);

    return objectMetadata.maneuverable;
}

const String& CDM::getObjectReferenceFrame(const Index& anIndex) const
{
    const CDM::Metadata& objectMetadata = this->getObjectMetadataAt(anIndex);

    return objectMetadata.refFrame;
}

void CDM::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::type::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Conjunction Data Message") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "CCSDS CDM Version:" << this->header_.ccsdsCdmVersion;
    ostk::core::utils::Print::Line(anOutputStream) << "Creation Date:" << this->header_.creationDate;
    ostk::core::utils::Print::Line(anOutputStream) << "Originator:" << this->header_.originator;
    ostk::core::utils::Print::Line(anOutputStream) << "Message ID:" << this->header_.messageId;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

CDM CDM::Dictionary(const container::Dictionary& aDictionary)
{
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Scale;

    // WARNING:
    // TBI: Currently adapted to handle SpaceTrack CDM (following CCSDS format loosely)
    // TBI: Only use the mandatory fields from the CCSDS reference document
    // TBI: Set the other fields to std::nullopt or String::Empty() (except `MESSAGE_FOR`) for now

    // Utility Function (to be moved later)
    const auto referenceFrameStringToFrame = [](const String& referenceFrameString) -> Shared<const Frame>
    {
        if (referenceFrameString == "ITRF")
        {
            return Frame::ITRF();
        }

        else if (referenceFrameString == "GCRF")
        {
            return Frame::GCRF();
        }

        else if (referenceFrameString == "EME2000")
        {
            return Frame::J2000(Theory::IAU_2006);
        }

        else
        {
            throw ostk::core::error::runtime::Wrong("Frame type", referenceFrameString);
        }
    };

    // Extract Conjunction Header

    CDM::Header header = CDM::Header {
        aDictionary["CCSDS_CDM_VERS"].getString(),
        String::Empty(),
        Instant::DateTime(
            DateTime::Parse(aDictionary["CREATION_DATE"].getString(), DateTime::Format::ISO8601), Scale::UTC
        ),
        aDictionary["ORIGINATOR"].getString(),
        aDictionary["MESSAGE_FOR"].getString(),
        aDictionary["CDM_ID"].getString()
    };

    // Extract Conjunction Relative Metadata

    const std::optional<double> collisionProbability =
        (aDictionary.hasValueForKey("COLLISION_PROBABILITY") &&
         !aDictionary["COLLISION_PROBABILITY"].getString().isEmpty())
            ? std::optional<double>(ParseDouble(aDictionary["COLLISION_PROBABILITY"].getString()))
            : std::nullopt;

    CDM::RelativeMetadata relativeMetadata = CDM::RelativeMetadata {
        String::Empty(),
        Instant::DateTime(DateTime::Parse(aDictionary["TCA"].getString(), DateTime::Format::ISO8601), Scale::UTC),
        Length::Meters(ParseDouble(aDictionary["MISS_DISTANCE"].getString())),
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        String::Empty(),
        String::Empty(),
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        collisionProbability,
        aDictionary["COLLISION_PROBABILITY_METHOD"].getString()
    };

    // Extract Conjunction Objects Metadata

    // Extract Object 1 Metadata

    CDM::Metadata sat1ObjectMetadata = CDM::Metadata {
        String::Empty(),
        aDictionary["SAT1_OBJECT"].getString(),
        ParseInt(aDictionary["SAT1_OBJECT_DESIGNATOR"].getString()),
        aDictionary["SAT1_CATALOG_NAME"].getString(),
        aDictionary["SAT1_OBJECT_NAME"].getString(),
        aDictionary["SAT1_INTERNATIONAL_DESIGNATOR"].getString(),
        CDM::ObjectType::Payload,
        String::Empty(),
        String::Empty(),
        String::Empty(),
        String::Empty(),
        aDictionary["SAT1_EPHEMERIS_NAME"].getString(),
        aDictionary["SAT1_COVARIANCE_METHOD"].getString(),
        aDictionary["SAT1_MANEUVERABLE"].getString(),
        String::Empty(),
        aDictionary["SAT1_REF_FRAME"].getString(),
        String::Empty(),
        String::Empty(),
        String::Empty(),
        false,
        false,
        false
    };

    // Extract Object 2 Metadata

    CDM::Metadata sat2ObjectMetadata = CDM::Metadata {
        String::Empty(),
        aDictionary["SAT2_OBJECT"].getString(),
        ParseInt(aDictionary["SAT2_OBJECT_DESIGNATOR"].getString()),
        aDictionary["SAT2_CATALOG_NAME"].getString(),
        aDictionary["SAT2_OBJECT_NAME"].getString(),
        aDictionary["SAT2_INTERNATIONAL_DESIGNATOR"].getString(),
        CDM::ObjectType::Payload,
        String::Empty(),
        String::Empty(),
        String::Empty(),
        String::Empty(),
        aDictionary["SAT2_EPHEMERIS_NAME"].getString(),
        aDictionary["SAT2_COVARIANCE_METHOD"].getString(),
        aDictionary["SAT2_MANEUVERABLE"].getString(),
        String::Empty(),
        aDictionary["SAT2_REF_FRAME"].getString(),
        String::Empty(),
        String::Empty(),
        String::Empty(),
        false,
        false,
        false
    };

    Array<CDM::Metadata> metadataArray = {sat1ObjectMetadata, sat2ObjectMetadata};

    // Extract Conjunction Objects Data

    // Extract Object 1 Data

    // Construct Object 1 Covariance Matrix, TBI: Use symmetric construction

    MatrixXd object1CovarianceMatrix = MatrixXd::Zero(9, 9);
    object1CovarianceMatrix(0, 0) = ParseDouble(aDictionary["SAT1_CR_R"].getString());
    object1CovarianceMatrix(1, 0) = ParseDouble(aDictionary["SAT1_CT_R"].getString());
    object1CovarianceMatrix(1, 1) = ParseDouble(aDictionary["SAT1_CT_T"].getString());
    object1CovarianceMatrix(2, 0) = ParseDouble(aDictionary["SAT1_CN_R"].getString());
    object1CovarianceMatrix(2, 1) = ParseDouble(aDictionary["SAT1_CN_T"].getString());
    object1CovarianceMatrix(2, 2) = ParseDouble(aDictionary["SAT1_CN_N"].getString());
    object1CovarianceMatrix(3, 0) = ParseDouble(aDictionary["SAT1_CRDOT_R"].getString());
    object1CovarianceMatrix(3, 1) = ParseDouble(aDictionary["SAT1_CRDOT_T"].getString());
    object1CovarianceMatrix(3, 2) = ParseDouble(aDictionary["SAT1_CRDOT_N"].getString());
    object1CovarianceMatrix(3, 3) = ParseDouble(aDictionary["SAT1_CRDOT_RDOT"].getString());
    object1CovarianceMatrix(4, 0) = ParseDouble(aDictionary["SAT1_CTDOT_R"].getString());
    object1CovarianceMatrix(4, 1) = ParseDouble(aDictionary["SAT1_CTDOT_T"].getString());
    object1CovarianceMatrix(4, 2) = ParseDouble(aDictionary["SAT1_CTDOT_N"].getString());
    object1CovarianceMatrix(4, 3) = ParseDouble(aDictionary["SAT1_CTDOT_RDOT"].getString());
    object1CovarianceMatrix(4, 4) = ParseDouble(aDictionary["SAT1_CTDOT_TDOT"].getString());
    object1CovarianceMatrix(5, 0) = ParseDouble(aDictionary["SAT1_CNDOT_R"].getString());
    object1CovarianceMatrix(5, 1) = ParseDouble(aDictionary["SAT1_CNDOT_T"].getString());
    object1CovarianceMatrix(5, 2) = ParseDouble(aDictionary["SAT1_CNDOT_N"].getString());
    object1CovarianceMatrix(5, 3) = ParseDouble(aDictionary["SAT1_CNDOT_RDOT"].getString());
    object1CovarianceMatrix(5, 4) = ParseDouble(aDictionary["SAT1_CNDOT_TDOT"].getString());
    object1CovarianceMatrix(5, 5) = ParseDouble(aDictionary["SAT1_CNDOT_NDOT"].getString());

    for (int i = 0; i < 5; i++)
    {
        for (int j = 1; j < 6; j++)
        {
            object1CovarianceMatrix(i, j) = object1CovarianceMatrix(j, i);
        }
    }

    String sat1RefFrameString = aDictionary["SAT1_REF_FRAME"].getString();
    Shared<const Frame> sat1RefFramePtr = referenceFrameStringToFrame(sat1RefFrameString);

    CDM::Data sat1ObjectData = CDM::Data {
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        State(
            Instant::DateTime(DateTime::Parse(aDictionary["TCA"].getString(), DateTime::Format::ISO8601), Scale::UTC),
            Position::Meters(  // TBI: Add Position::Kilometers
                {ParseDouble(aDictionary["SAT1_X"].getString()) * 1000.0,
                 ParseDouble(aDictionary["SAT1_Y"].getString()) * 1000.0,
                 ParseDouble(aDictionary["SAT1_Z"].getString()) * 1000.0},
                sat1RefFramePtr
            ),
            Velocity::MetersPerSecond(  // TBI: Add Velocity::KilometersPerSecond
                {ParseDouble(aDictionary["SAT1_X_DOT"].getString()) * 1000.0,
                 ParseDouble(aDictionary["SAT1_Y_DOT"].getString()) * 1000.0,
                 ParseDouble(aDictionary["SAT1_Z_DOT"].getString()) * 1000.0},
                sat1RefFramePtr
            )
        ),
        object1CovarianceMatrix
    };

    // Extract Object 2 Data

    // Construct Object 2 Covariance Matrix

    MatrixXd object2CovarianceMatrix = MatrixXd::Zero(9, 9);
    object2CovarianceMatrix(0, 0) = ParseDouble(aDictionary["SAT2_CR_R"].getString());
    object2CovarianceMatrix(1, 0) = ParseDouble(aDictionary["SAT2_CT_R"].getString());
    object2CovarianceMatrix(1, 1) = ParseDouble(aDictionary["SAT2_CT_T"].getString());
    object2CovarianceMatrix(2, 0) = ParseDouble(aDictionary["SAT2_CN_R"].getString());
    object2CovarianceMatrix(2, 1) = ParseDouble(aDictionary["SAT2_CN_T"].getString());
    object2CovarianceMatrix(2, 2) = ParseDouble(aDictionary["SAT2_CN_N"].getString());
    object2CovarianceMatrix(3, 0) = ParseDouble(aDictionary["SAT2_CRDOT_R"].getString());
    object2CovarianceMatrix(3, 1) = ParseDouble(aDictionary["SAT2_CRDOT_T"].getString());
    object2CovarianceMatrix(3, 2) = ParseDouble(aDictionary["SAT2_CRDOT_N"].getString());
    object2CovarianceMatrix(3, 3) = ParseDouble(aDictionary["SAT2_CRDOT_RDOT"].getString());
    object2CovarianceMatrix(4, 0) = ParseDouble(aDictionary["SAT2_CTDOT_R"].getString());
    object2CovarianceMatrix(4, 1) = ParseDouble(aDictionary["SAT2_CTDOT_T"].getString());
    object2CovarianceMatrix(4, 2) = ParseDouble(aDictionary["SAT2_CTDOT_N"].getString());
    object2CovarianceMatrix(4, 3) = ParseDouble(aDictionary["SAT2_CTDOT_RDOT"].getString());
    object2CovarianceMatrix(4, 4) = ParseDouble(aDictionary["SAT2_CTDOT_TDOT"].getString());
    object2CovarianceMatrix(5, 0) = ParseDouble(aDictionary["SAT2_CNDOT_R"].getString());
    object2CovarianceMatrix(5, 1) = ParseDouble(aDictionary["SAT2_CNDOT_T"].getString());
    object2CovarianceMatrix(5, 2) = ParseDouble(aDictionary["SAT2_CNDOT_N"].getString());
    object2CovarianceMatrix(5, 3) = ParseDouble(aDictionary["SAT2_CNDOT_RDOT"].getString());
    object2CovarianceMatrix(5, 4) = ParseDouble(aDictionary["SAT2_CNDOT_TDOT"].getString());
    object2CovarianceMatrix(5, 5) = ParseDouble(aDictionary["SAT2_CNDOT_NDOT"].getString());

    for (int i = 0; i < 5; i++)
    {
        for (int j = 1; j < 6; j++)
        {
            object2CovarianceMatrix(i, j) = object2CovarianceMatrix(j, i);
        }
    }

    String sat2RefFrameString = aDictionary["SAT2_REF_FRAME"].getString();
    Shared<const Frame> sat2RefFramePtr = referenceFrameStringToFrame(sat2RefFrameString);

    CDM::Data sat2ObjectData = CDM::Data {
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        State(
            Instant::DateTime(DateTime::Parse(aDictionary["TCA"].getString(), DateTime::Format::ISO8601), Scale::UTC),
            Position::Meters(
                {ParseDouble(aDictionary["SAT2_X"].getString()) * 1000.0,
                 ParseDouble(aDictionary["SAT2_Y"].getString()) * 1000.0,
                 ParseDouble(aDictionary["SAT2_Z"].getString()) * 1000.0},
                sat2RefFramePtr
            ),
            Velocity::MetersPerSecond(
                {ParseDouble(aDictionary["SAT2_X_DOT"].getString()) * 1000.0,
                 ParseDouble(aDictionary["SAT2_Y_DOT"].getString()) * 1000.0,
                 ParseDouble(aDictionary["SAT2_Z_DOT"].getString()) * 1000.0},
                sat2RefFramePtr
            )
        ),
        object2CovarianceMatrix
    };

    Array<CDM::Data> dataArray = {sat1ObjectData, sat2ObjectData};

    return CDM {header, relativeMetadata, metadataArray, dataArray};
}

CDM CDM::Parse(const String& aString)
{
    using ostk::core::container::Object;

    return CDM::Dictionary(Object::Parse(aString, Object::Format::JSON).getDictionary());
}

CDM CDM::Load(const File& aFile)
{
    using ostk::core::container::Object;

    return CDM::Dictionary(Object::Load(aFile, Object::Format::JSON).getDictionary());
}

CDM::ObjectType CDM::ObjectTypeFromString(const String& aString)
{
    static const Map<String, CDM::ObjectType> stringModeMap = {
        {"PAYLOAD", CDM::ObjectType::Payload},
        {"ROCKET BODY", CDM::ObjectType::RocketBody},
        {"DEBRIS", CDM::ObjectType::Debris},
        {"UNKNOWN", CDM::ObjectType::Unknown},
        {"OTHER", CDM::ObjectType::Other}
    };

    try
    {
        return stringModeMap.at(aString);
    }
    catch (const std::exception&)
    {
        throw ostk::core::error::runtime::Wrong("CDM Object Type", aString);
    }
}

}  // namespace ccsds
}  // namespace message
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
