////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Conjunction/Messages/CCSDS/CDM.cpp
/// @author         Remy Derollez <remy@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/Messages/CCSDS/CDM.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace conjunction
{
namespace messages
{
namespace ccsds
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                CDM::CDM                                    (   const   CDM::Header&                aHeader,
                                                                                const   CDM::RelativeMetadata&      aRelativeMetadata,
                                                                                const   Array<CDM::Metadata>&       aMetadataArray,
                                                                                const   Array<CDM::Data>            aDataArray                                  )
                                :   header_(aHeader),
                                    relativeMetadata_(aRelativeMetadata),
                                    objectsMetadata_(aMetadataArray),
                                    objectsData_(aDataArray)
{

}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   CDM&                        anCDM                                       )
{

    anCDM.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            CDM::isDefined                              ( ) const
{
    return (!this->header_.ccsdsCdmVersion.isEmpty() && !this->header_.messageId.isEmpty() && this->header_.creationDate.isDefined() && !this->header_.originator.isEmpty()) ;
}

CDM::Header                     CDM::getHeader                              ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->header_ ;

}

CDM::RelativeMetadata           CDM::getRelativeMetadata                    ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->relativeMetadata_ ;

}

Array<CDM::Metadata>            CDM::getMetadataArray                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->objectsMetadata_ ;

}

Array<CDM::Data>                CDM::getDataArray                           ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->objectsData_ ;

}

CDM::Metadata                   CDM::getObjectMetadataAt                    (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->objectsMetadata_.at(anIndex) ;

}

CDM::Data                       CDM::getObjectDataAt                        (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->objectsData_.at(anIndex) ;

}

String                          CDM::getCCSDSCDMVersion                     ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->header_.ccsdsCdmVersion ;

}

Instant                         CDM::getCreationDate                        ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->header_.creationDate ;

}

String                          CDM::getOriginator                          ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->header_.originator ;

}

String                          CDM::getMessageFor                          ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->header_.messageFor ;

}

 String                         CDM::getMessageId                           ( ) const
 {

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->header_.messageId ;

 }

Instant                         CDM::getTCA                                 ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->relativeMetadata_.TCA ;

}

Length                          CDM::getMissDistance                        ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->relativeMetadata_.missDistance ;

}

Position                        CDM::getRelativePosition                    ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->relativeMetadata_.relativePosition ;

}

Velocity                        CDM::getRelativeVelocity                    ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->relativeMetadata_.relativeVelocity ;

}

Real                            CDM::getCollisionProbability                ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->relativeMetadata_.collisionProbability ;

}

String                          CDM::getCollisionProbabilityMethod          ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    return this->relativeMetadata_.collisionProbabilityMethod ;

}

Integer                         CDM::getObjectDesignator                    (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    CDM::Metadata objectMetadata = this->getObjectMetadataAt(anIndex) ;

    return objectMetadata.objectDesignator ;

}

String                          CDM::getObjectName                          (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    CDM::Metadata objectMetadata = this->getObjectMetadataAt(anIndex) ;

    return objectMetadata.objectName ;

}

String                          CDM::getObjectInternationalDesignator       (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    CDM::Metadata objectMetadata = this->getObjectMetadataAt(anIndex) ;

    return objectMetadata.internationalDesignator ;

}

CDM::ObjectType                 CDM::getObjectType                          (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    CDM::Metadata objectMetadata = this->getObjectMetadataAt(anIndex) ;

    return objectMetadata.objectType ;

}

String                          CDM::getObjectEphemerisName                 (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    CDM::Metadata objectMetadata = this->getObjectMetadataAt(anIndex) ;

    return objectMetadata.ephemerisName ;

}

String                          CDM::getObjectCovarianceMethod              (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    CDM::Metadata objectMetadata = this->getObjectMetadataAt(anIndex) ;

    return objectMetadata.covarianceMethod ;

}

String                          CDM::getObjectManeuverability               (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    CDM::Metadata objectMetadata = this->getObjectMetadataAt(anIndex) ;

    return objectMetadata.maneuverable ;

}

String                          CDM::getObjectReferenceFrame                (   const   Size&                       anIndex                                     ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CDM") ;
    }

    CDM::Metadata objectMetadata = this->getObjectMetadataAt(anIndex) ;

    return objectMetadata.refFrame ;

}

void                            CDM::print                                  (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    using ostk::core::types::String ;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Conjunction Data Message") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "CCSDS CDM Version:" << this->header_.ccsdsCdmVersion ;
    ostk::core::utils::Print::Line(anOutputStream) << "Creation Date:" << this->header_.creationDate ;
    ostk::core::utils::Print::Line(anOutputStream) << "Originator:" << this->header_.originator ;
    ostk::core::utils::Print::Line(anOutputStream) << "Message ID:" << this->header_.messageId ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

CDM                             CDM::Undefined                              ( )
{
    return {
        {
            String::Empty(),
            String::Empty(),
            Instant::Undefined(),
            String::Empty(),
            String::Empty(),
            String::Empty()
        },
        {
            String::Empty(),
            Instant::Undefined(),
            Length::Undefined(),
            Position::Undefined(),
            Velocity::Undefined(),
            Instant::Undefined(),
            Instant::Undefined(),
            String::Empty(),
            String::Empty(),
            Real::Undefined(),
            Real::Undefined(),
            Real::Undefined(),
            Instant::Undefined(),
            Instant::Undefined(),
            Real::Undefined(),
            String::Empty()
        },
        Array<CDM::Metadata>::Empty(),
        Array<CDM::Data>::Empty()
    } ;
}

CDM                             CDM::Dictionary                             (   const   ctnr::Dictionary&           aDictionary                                 )
{

    using ostk::physics::time::DateTime ;
    using ostk::physics::time::Scale ;

    // WARNING:
    // TBI: Currently adapted to handle SpaceTrack CDM (following CCSDS format loosely)
    // TBI: Only use the mandatory fields from the CCSDS reference document
    // TBI: Set the other fields to Undefined() or String::Empty() (except `MESSAGE_FOR`) for now

    // Extract Conjunction Header

    CDM::Header header = CDM::Header
    {
        aDictionary["CCSDS_CDM_VERS"].getString(),
        String::Empty(),
        Instant::DateTime(DateTime::Parse(aDictionary["CREATION_DATE"].getString(), DateTime::Format::ISO8601), Scale::UTC),
        aDictionary["ORIGINATOR"].getString(),
        aDictionary["MESSAGE_FOR"].getString(),
        aDictionary["CDM_ID"].getString()
    } ;

    // Extract Conjunction Relative Metadata

    CDM::RelativeMetadata relativeMetadata = CDM::RelativeMetadata
    {
        String::Empty(),
        Instant::DateTime(DateTime::Parse(aDictionary["TCA"].getString(), DateTime::Format::ISO8601), Scale::UTC),
        Length::Meters(std::stod(aDictionary["MISS_DISTANCE"].accessString())),
        Position::Undefined(),
        Velocity::Undefined(),
        Instant::Undefined(),
        Instant::Undefined(),
        String::Empty(),
        String::Empty(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Instant::Undefined(),
        Instant::Undefined(),
        std::stod(aDictionary["COLLISION_PROBABILITY"].accessString()),
        aDictionary["COLLISION_PROBABILITY_METHOD"].accessString()
    } ;

    // Extract Conjunction Objects Metadata

    // Extract Object 1 Metadata

    CDM::Metadata sat1ObjectMetadata = CDM::Metadata
    {
        String::Empty(),
        aDictionary["SAT1_OBJECT"].accessString(),
        Integer::Parse(aDictionary["SAT1_OBJECT_DESIGNATOR"].accessString()),
        aDictionary["SAT1_CATALOG_NAME"].accessString(),
        aDictionary["SAT1_OBJECT_NAME"].accessString(),
        aDictionary["SAT1_INTERNATIONAL_DESIGNATOR"].accessString(),
        CDM::ObjectType::Payload,
        String::Empty(),
        String::Empty(),
        String::Empty(),
        String::Empty(),
        aDictionary["SAT1_EPHEMERIS_NAME"].accessString(),
        aDictionary["SAT1_COVARIANCE_METHOD"].accessString(),
        aDictionary["SAT1_MANEUVERABLE"].accessString(),
        String::Empty(),
        aDictionary["SAT1_REF_FRAME"].accessString(),
        String::Empty(),
        String::Empty(),
        String::Empty(),
        false,
        false,
        false
    } ;

    // Extract Object 2 Metadata

    CDM::Metadata sat2ObjectMetadata = CDM::Metadata
    {
        String::Empty(),
        aDictionary["SAT2_OBJECT"].accessString(),
        Integer::Parse(aDictionary["SAT2_OBJECT_DESIGNATOR"].accessString()),
        aDictionary["SAT2_CATALOG_NAME"].accessString(),
        aDictionary["SAT2_OBJECT_NAME"].accessString(),
        aDictionary["SAT2_INTERNATIONAL_DESIGNATOR"].accessString(),
        CDM::ObjectType::Payload,
        String::Empty(),
        String::Empty(),
        String::Empty(),
        String::Empty(),
        aDictionary["SAT2_EPHEMERIS_NAME"].accessString(),
        aDictionary["SAT2_COVARIANCE_METHOD"].accessString(),
        aDictionary["SAT2_MANEUVERABLE"].accessString(),
        String::Empty(),
        aDictionary["SAT2_REF_FRAME"].accessString(),
        String::Empty(),
        String::Empty(),
        String::Empty(),
        false,
        false,
        false
    } ;

    Array<CDM::Metadata> metadataArray = { sat1ObjectMetadata, sat2ObjectMetadata } ;

    // Extract Conjunction Objects Data

    // Extract Object 1 Data

    // Construct Object 1 Covariance Matrix, TBI: Use symmetric construction

    MatrixXd object1CovarianceMatrix = MatrixXd::Zero(9, 9) ;
    object1CovarianceMatrix(0, 0) = std::stod(aDictionary["SAT1_CR_R"].accessString()) ;
    object1CovarianceMatrix(1, 0) = std::stod(aDictionary["SAT1_CT_R"].accessString()) ;
    object1CovarianceMatrix(1, 1) = std::stod(aDictionary["SAT1_CT_T"].accessString()) ;
    object1CovarianceMatrix(2, 0) = std::stod(aDictionary["SAT1_CN_R"].accessString()) ;
    object1CovarianceMatrix(2, 1) = std::stod(aDictionary["SAT1_CN_T"].accessString()) ;
    object1CovarianceMatrix(2, 2) = std::stod(aDictionary["SAT1_CN_N"].accessString()) ;
    object1CovarianceMatrix(3, 0) = std::stod(aDictionary["SAT1_CRDOT_R"].accessString()) ;
    object1CovarianceMatrix(3, 1) = std::stod(aDictionary["SAT1_CRDOT_T"].accessString()) ;
    object1CovarianceMatrix(3, 2) = std::stod(aDictionary["SAT1_CRDOT_N"].accessString()) ;
    object1CovarianceMatrix(3, 3) = std::stod(aDictionary["SAT1_CRDOT_RDOT"].accessString()) ;
    object1CovarianceMatrix(4, 0) = std::stod(aDictionary["SAT1_CTDOT_R"].accessString()) ;
    object1CovarianceMatrix(4, 1) = std::stod(aDictionary["SAT1_CTDOT_T"].accessString()) ;
    object1CovarianceMatrix(4, 2) = std::stod(aDictionary["SAT1_CTDOT_N"].accessString()) ;
    object1CovarianceMatrix(4, 3) = std::stod(aDictionary["SAT1_CTDOT_RDOT"].accessString()) ;
    object1CovarianceMatrix(4, 4) = std::stod(aDictionary["SAT1_CTDOT_TDOT"].accessString()) ;
    object1CovarianceMatrix(5, 0) = std::stod(aDictionary["SAT1_CNDOT_R"].accessString()) ;
    object1CovarianceMatrix(5, 1) = std::stod(aDictionary["SAT1_CNDOT_T"].accessString()) ;
    object1CovarianceMatrix(5, 2) = std::stod(aDictionary["SAT1_CNDOT_N"].accessString()) ;
    object1CovarianceMatrix(5, 3) = std::stod(aDictionary["SAT1_CNDOT_RDOT"].accessString()) ;
    object1CovarianceMatrix(5, 4) = std::stod(aDictionary["SAT1_CNDOT_TDOT"].accessString()) ;
    object1CovarianceMatrix(5, 5) = std::stod(aDictionary["SAT1_CNDOT_NDOT"].accessString()) ;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 1; j < 6; j++)
        {
            object1CovarianceMatrix(i, j) = object1CovarianceMatrix(j, i) ;
        }
    }

    CDM::Data sat1ObjectData = CDM::Data
    {
        Instant::Undefined(),
        Instant::Undefined(),
        Duration::Undefined(),
        Duration::Undefined(),
        Integer::Undefined(),
        Integer::Undefined(),
        Integer::Undefined(),
        Integer::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Mass::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        State(
            Instant::DateTime(DateTime::Parse(aDictionary["TCA"].getString(), DateTime::Format::ISO8601), Scale::UTC),
            Position::Meters(  // TBI: Add Position::Kilometers
                {
                    Real::Parse(aDictionary["SAT1_X"].accessString()) * 1000.0,
                    Real::Parse(aDictionary["SAT1_Y"].accessString()) * 1000.0,
                    Real::Parse(aDictionary["SAT1_Z"].accessString()) * 1000.0
                },
                Frame::ITRF()
            ),
            Velocity::MetersPerSecond(  // TBI: Add Velocity::KilometersPerSecond
                {
                    Real::Parse(aDictionary["SAT1_X_DOT"].accessString()) * 1000.0,
                    Real::Parse(aDictionary["SAT1_Y_DOT"].accessString()) * 1000.0,
                    Real::Parse(aDictionary["SAT1_Z_DOT"].accessString()) * 1000.0
                },
                Frame::ITRF()
            )
        ),
        object1CovarianceMatrix
    } ;

    // Extract Object 2 Data

    // Construct Object 2 Covariance Matrix

    MatrixXd object2CovarianceMatrix = MatrixXd::Zero(9, 9) ;
    object2CovarianceMatrix(0, 0) = std::stod(aDictionary["SAT2_CR_R"].accessString()) ;
    object2CovarianceMatrix(1, 0) = std::stod(aDictionary["SAT2_CT_R"].accessString()) ;
    object2CovarianceMatrix(1, 1) = std::stod(aDictionary["SAT2_CT_T"].accessString()) ;
    object2CovarianceMatrix(2, 0) = std::stod(aDictionary["SAT2_CN_R"].accessString()) ;
    object2CovarianceMatrix(2, 1) = std::stod(aDictionary["SAT2_CN_T"].accessString()) ;
    object2CovarianceMatrix(2, 2) = std::stod(aDictionary["SAT2_CN_N"].accessString()) ;
    object2CovarianceMatrix(3, 0) = std::stod(aDictionary["SAT2_CRDOT_R"].accessString()) ;
    object2CovarianceMatrix(3, 1) = std::stod(aDictionary["SAT2_CRDOT_T"].accessString()) ;
    object2CovarianceMatrix(3, 2) = std::stod(aDictionary["SAT2_CRDOT_N"].accessString()) ;
    object2CovarianceMatrix(3, 3) = std::stod(aDictionary["SAT2_CRDOT_RDOT"].accessString()) ;
    object2CovarianceMatrix(4, 0) = std::stod(aDictionary["SAT2_CTDOT_R"].accessString()) ;
    object2CovarianceMatrix(4, 1) = std::stod(aDictionary["SAT2_CTDOT_T"].accessString()) ;
    object2CovarianceMatrix(4, 2) = std::stod(aDictionary["SAT2_CTDOT_N"].accessString()) ;
    object2CovarianceMatrix(4, 3) = std::stod(aDictionary["SAT2_CTDOT_RDOT"].accessString()) ;
    object2CovarianceMatrix(4, 4) = std::stod(aDictionary["SAT2_CTDOT_TDOT"].accessString()) ;
    object2CovarianceMatrix(5, 0) = std::stod(aDictionary["SAT2_CNDOT_R"].accessString()) ;
    object2CovarianceMatrix(5, 1) = std::stod(aDictionary["SAT2_CNDOT_T"].accessString()) ;
    object2CovarianceMatrix(5, 2) = std::stod(aDictionary["SAT2_CNDOT_N"].accessString()) ;
    object2CovarianceMatrix(5, 3) = std::stod(aDictionary["SAT2_CNDOT_RDOT"].accessString()) ;
    object2CovarianceMatrix(5, 4) = std::stod(aDictionary["SAT2_CNDOT_TDOT"].accessString()) ;
    object2CovarianceMatrix(5, 5) = std::stod(aDictionary["SAT2_CNDOT_NDOT"].accessString()) ;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 1; j < 6; j++)
        {
            object2CovarianceMatrix(i, j) = object2CovarianceMatrix(j, i) ;
        }
    }

    CDM::Data sat2ObjectData = CDM::Data
    {
        Instant::Undefined(),
        Instant::Undefined(),
        Duration::Undefined(),
        Duration::Undefined(),
        Integer::Undefined(),
        Integer::Undefined(),
        Integer::Undefined(),
        Integer::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Mass::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        State(
            Instant::DateTime(DateTime::Parse(aDictionary["TCA"].getString(), DateTime::Format::ISO8601), Scale::UTC),
            Position::Meters(
                {
                    Real::Parse(aDictionary["SAT2_X"].accessString()) * 1000.0,
                    Real::Parse(aDictionary["SAT2_Y"].accessString()) * 1000.0,
                    Real::Parse(aDictionary["SAT2_Z"].accessString()) * 1000.0
                },
                Frame::ITRF()
            ),
            Velocity::MetersPerSecond(
                {
                    Real::Parse(aDictionary["SAT2_X_DOT"].accessString()) * 1000.0,
                    Real::Parse(aDictionary["SAT2_Y_DOT"].accessString()) * 1000.0,
                    Real::Parse(aDictionary["SAT2_Z_DOT"].accessString()) * 1000.0
                },
                Frame::ITRF()
            )
        ),
        object2CovarianceMatrix
    } ;

    Array<CDM::Data> dataArray = { sat1ObjectData, sat2ObjectData } ;

    return CDM
    {
        header,
        relativeMetadata,
        metadataArray,
        dataArray
    } ;

}

CDM                             CDM::Parse                                  (   const   String&                     aString                                     )
{

    using ostk::core::ctnr::Object ;

    return CDM::Dictionary(Object::Parse(aString, Object::Format::JSON).accessDictionary()) ;

}

CDM                             CDM::Load                                   (   const   File&                       aFile                                       )
{

    using ostk::core::ctnr::Object ;

    return CDM::Dictionary(Object::Load(aFile, Object::Format::JSON).accessDictionary()) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDM::ObjectType                 CDM::ObjectTypeFromString                   (   const   String&                     aString                                     )
{

    static const Map<String, CDM::ObjectType> stringModeMap =
    {
        { "PAYLOAD",     CDM::ObjectType::Payload },
        { "ROCKET BODY", CDM::ObjectType::RocketBody },
        { "DEBRIS",      CDM::ObjectType::Debris },
        { "UNKNOWN",     CDM::ObjectType::Unknown },
        { "OTHER",       CDM::ObjectType::Other }
    } ;

    try
    {
        return stringModeMap.at(aString) ;
    }
    catch (const std::exception&)
    {
        throw ostk::core::error::runtime::Wrong("CDM Object Type", aString) ;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
