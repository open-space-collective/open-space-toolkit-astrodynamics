/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/Message/CCSDS/CDM.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::filesystem::File;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::conjunction::message::ccsds::CDM;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM : public ::testing::Test
{
   protected:
    // https://public.ccsds.org/Pubs/508x0b1e2c2.pdf (section 3.6.3 - page 3-12)

    void SetUp() override
    {
        this->cdm_ = CDM {
            CDM::Header {
                "1.0",
                String::Empty(),
                Instant::DateTime(DateTime(2010, 3, 12, 22, 31, 12), Scale::UTC),
                "JSPOC",
                "SATELLITE A",
                "201113719185"
            },
            CDM::RelativeMetadata {
                String::Empty(),
                Instant::DateTime(DateTime(2010, 3, 13, 22, 37, 52, 618), Scale::UTC),
                Length::Meters(715.0),
                Position::Undefined(),  // Position::Meters({ 27.4, -70.2, 711.8 }, Frame::RTN()),
                Velocity::Undefined(),  // Velocity::MetersPerSecond({ -7.2, 14692.0, -1437.2 }, Frame::RTN()),
                Instant::DateTime(DateTime(2010, 3, 12, 18, 29, 32, 212), Scale::UTC),
                Instant::DateTime(DateTime(2010, 3, 15, 18, 29, 32, 212), Scale::UTC),
                "RTN",
                "ELLIPSOID",
                200.0,
                1000.0,
                1000.0,
                Instant::DateTime(DateTime(2010, 3, 13, 22, 37, 52, 222), Scale::UTC),
                Instant::DateTime(DateTime(2010, 3, 13, 22, 37, 52, 824), Scale::UTC),
                0.0000435,
                "FOSTER-1992",
            },
            {CDM::Metadata {
                 String::Empty(),
                 "OBJECT1",
                 12345,
                 "SATCAT",
                 "SATELLITE A",
                 "1997−030E",
                 CDM::ObjectType::Payload,
                 "OSA",
                 "EUTMETSAT",
                 "+49615130312",
                 "JOHN.DOE@SOMEWHERE.NET",
                 "EPHEMERIS SATELLITE A",
                 "CALCULATED",
                 "YES",
                 String::Empty(),
                 "EME2000",
                 "EGM-96: 36D 36O",
                 "JACCHIA 70 DCA",
                 "MOON, SUN",
                 false,
                 false,
                 false
             },
             CDM::Metadata {
                 String::Empty(),
                 "OBJECT2",
                 30337,
                 "SATCAT",
                 "FENGYUN 1C DEB",
                 "1999-025AA",
                 CDM::ObjectType::Debris,
                 String::Empty(),
                 String::Empty(),
                 String::Empty(),
                 String::Empty(),
                 "NONE",
                 "CALCULATED",
                 "NO",
                 String::Empty(),
                 "EME2000",
                 "EGM-96: 36D 36O",
                 "JACCHIA 70 DCA",
                 "MOON, SUN",
                 true,
                 false,
                 false
             }},
            {CDM::Data {
                 Instant::DateTime(DateTime(2010, 3, 12, 2, 14, 12, 746), Scale::UTC),
                 Instant::DateTime(DateTime(2010, 3, 12, 2, 14, 12, 746), Scale::UTC),
                 Duration::Days(7.88),
                 Duration::Days(5.50),
                 592,
                 579,
                 123,
                 119,
                 97.8,
                 0.864,
                 5.2,
                 Real::Undefined(),
                 Real::Undefined(),
                 Mass::Kilograms(251.6),
                 0.045663,
                 0.0,
                 0.0,
                 0.000045457,
                 State::Undefined(),
                 MatrixXd(9, 9)
             },
             CDM::Data {
                 Instant::DateTime(DateTime(2010, 3, 12, 1, 14, 12, 746), Scale::UTC),
                 Instant::DateTime(DateTime(2010, 3, 12, 3, 14, 12, 746), Scale::UTC),
                 Duration::Days(2.63),
                 Duration::Days(2.63),
                 59,
                 58,
                 15,
                 15,
                 97.8,
                 0.864,
                 Real::Undefined(),
                 Real::Undefined(),
                 Real::Undefined(),
                 Mass::Kilograms(20.0),
                 Real::Undefined(),
                 Real::Undefined(),
                 Real::Undefined(),
                 Real::Undefined(),
                 State::Undefined(),
                 MatrixXd(9, 9)
             }}
        };
    }

    CDM cdm_ = CDM::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, IsDefined)
{
    {
        EXPECT_TRUE(this->cdm_.isDefined());
    }

    {
        EXPECT_FALSE(CDM::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetHeader)
{
    {
        const CDM::Header header = this->cdm_.getHeader();

        EXPECT_EQ("1.0", header.ccsdsCdmVersion);
        EXPECT_EQ(String::Empty(), header.comment);
        EXPECT_EQ(Instant::DateTime(DateTime(2010, 3, 12, 22, 31, 12), Scale::UTC), header.creationDate);
        EXPECT_EQ("JSPOC", header.originator);
        EXPECT_EQ("SATELLITE A", header.messageFor);
        EXPECT_EQ("201113719185", header.messageId);
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getHeader());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetRelativeMetadata)
{
    {
        const CDM::RelativeMetadata relativeMetadata = this->cdm_.getRelativeMetadata();

        EXPECT_EQ(String::Empty(), relativeMetadata.comment);
        EXPECT_EQ(Instant::DateTime(DateTime(2010, 3, 13, 22, 37, 52, 618), Scale::UTC), relativeMetadata.TCA);
        EXPECT_EQ(Length::Meters(715.0), relativeMetadata.missDistance);
        // EXPECT_EQ(Position::Undefined(), relativeMetadata.relativePosition) ;
        // EXPECT_EQ(Velocity::Undefined(), relativeMetadata.relativeVelocity) ;
        EXPECT_EQ(
            Instant::DateTime(DateTime(2010, 3, 12, 18, 29, 32, 212), Scale::UTC), relativeMetadata.startScreenPeriod
        );
        EXPECT_EQ(
            Instant::DateTime(DateTime(2010, 3, 15, 18, 29, 32, 212), Scale::UTC), relativeMetadata.endScreenPeriod
        );
        EXPECT_EQ("RTN", relativeMetadata.screenVolumeFrame);
        EXPECT_EQ("ELLIPSOID", relativeMetadata.screenVolumeShape);
        EXPECT_EQ(200.0, relativeMetadata.screenVolumeX);
        EXPECT_EQ(1000.0, relativeMetadata.screenVolumeY);
        EXPECT_EQ(1000.0, relativeMetadata.screenVolumeZ);
        EXPECT_EQ(
            Instant::DateTime(DateTime(2010, 3, 13, 22, 37, 52, 222), Scale::UTC), relativeMetadata.screenEntryTime
        );
        EXPECT_EQ(
            Instant::DateTime(DateTime(2010, 3, 13, 22, 37, 52, 824), Scale::UTC), relativeMetadata.screenExitTime
        );
        EXPECT_EQ(0.0000435, relativeMetadata.collisionProbability);
        EXPECT_EQ("FOSTER-1992", relativeMetadata.collisionProbabilityMethod);
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getRelativeMetadata());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetMetadataArray)
{
    {
        const Array<CDM::Metadata> objectsMetadataArray = this->cdm_.getMetadataArray();

        EXPECT_EQ(objectsMetadataArray.getSize(), 2);
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getMetadataArray());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetDataArray)
{
    {
        const Array<CDM::Data> objectsDataArray = this->cdm_.getDataArray();

        EXPECT_EQ(objectsDataArray.getSize(), 2);
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getDataArray());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectMetadataAt)
{
    {
        const CDM::Metadata objectMetadata = this->cdm_.getObjectMetadataAt(0);

        EXPECT_EQ(String::Empty(), objectMetadata.comment);
        EXPECT_EQ("OBJECT1", objectMetadata.object);
        EXPECT_EQ(12345, objectMetadata.objectDesignator);
        EXPECT_EQ("SATCAT", objectMetadata.catalogName);
        EXPECT_EQ("SATELLITE A", objectMetadata.objectName);
        EXPECT_EQ("1997−030E", objectMetadata.internationalDesignator);
        EXPECT_EQ(CDM::ObjectType::Payload, objectMetadata.objectType);
        EXPECT_EQ("OSA", objectMetadata.operatorContactPosition);
        EXPECT_EQ("EUTMETSAT", objectMetadata.operatorOrgnization);
        EXPECT_EQ("+49615130312", objectMetadata.operatorPhone);
        EXPECT_EQ("JOHN.DOE@SOMEWHERE.NET", objectMetadata.operatorEmail);
        EXPECT_EQ("EPHEMERIS SATELLITE A", objectMetadata.ephemerisName);
        EXPECT_EQ("CALCULATED", objectMetadata.covarianceMethod);
        EXPECT_EQ("YES", objectMetadata.maneuverable);
        EXPECT_EQ(String::Empty(), objectMetadata.orbitCenter);
        EXPECT_EQ("EME2000", objectMetadata.refFrame);
        EXPECT_EQ("EGM-96: 36D 36O", objectMetadata.gravityModel);
        EXPECT_EQ("JACCHIA 70 DCA", objectMetadata.atmosphericModel);
        EXPECT_EQ("MOON, SUN", objectMetadata.nBodyPerturbations);
        EXPECT_EQ(false, objectMetadata.solarRadiationPressure);
        EXPECT_EQ(false, objectMetadata.earthTides);
        EXPECT_EQ(false, objectMetadata.inTrackThrust);
    }

    {
        EXPECT_ANY_THROW(this->cdm_.getObjectMetadataAt(2));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectMetadataAt(0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectDataAt)
{
    {
        const CDM::Data objectData = this->cdm_.getObjectDataAt(0);

        EXPECT_EQ(Instant::DateTime(DateTime(2010, 3, 12, 2, 14, 12, 746), Scale::UTC), objectData.timeLastObStart);
        EXPECT_EQ(Instant::DateTime(DateTime(2010, 3, 12, 2, 14, 12, 746), Scale::UTC), objectData.timeLastObEnd);
        EXPECT_EQ(Duration::Days(7.88), objectData.recommendedODSpan);
        EXPECT_EQ(Duration::Days(5.50), objectData.actualODSpan);
        EXPECT_EQ(592, objectData.obsAvailable);
        EXPECT_EQ(579, objectData.obsUsed);
        EXPECT_EQ(123, objectData.tracksAvailable);
        EXPECT_EQ(119, objectData.tracksUsed);
        EXPECT_EQ(97.8, objectData.residualsAccepted);
        EXPECT_EQ(0.864, objectData.weightedRMS);
        EXPECT_EQ(5.2, objectData.areaPC);
        // EXPECT_EQ(Real::Undefined(), objectData.areaDrag) ;
        // EXPECT_EQ(Real::Undefined(), objectData.areaSRP) ;
        EXPECT_EQ(Mass::Kilograms(251.6), objectData.mass);
        EXPECT_EQ(0.045663, objectData.cdAreaOverMass);
        EXPECT_EQ(0.0, objectData.crAreaOverMass);
        EXPECT_EQ(0.0, objectData.thrustAcceleration);
        EXPECT_EQ(0.000045457, objectData.SEDR);
        // EXPECT_EQ(State::Undefined(), objectData.state ) ;
        // EXPECT_EQ(MatrixXd(9, 9), objectData.covarianceMatrix ) ;
    }

    {
        EXPECT_NO_THROW(this->cdm_.getObjectDataAt(1));
    }

    {
        EXPECT_ANY_THROW(this->cdm_.getObjectDataAt(2));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectDataAt(0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetCCSDSCDMVersion)
{
    {
        EXPECT_EQ("1.0", this->cdm_.getCCSDSCDMVersion());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getCCSDSCDMVersion());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, getCreationDate)
{
    {
        EXPECT_EQ(Instant::DateTime(DateTime(2010, 3, 12, 22, 31, 12), Scale::UTC), this->cdm_.getCreationDate());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getCreationDate());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetOriginator)
{
    {
        EXPECT_EQ("JSPOC", this->cdm_.getOriginator());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getOriginator());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetMessageFor)
{
    {
        EXPECT_EQ("SATELLITE A", this->cdm_.getMessageFor());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getMessageFor());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetMessageId)
{
    {
        EXPECT_EQ("201113719185", this->cdm_.getMessageId());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getMessageId());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetTCA)
{
    {
        EXPECT_EQ(Instant::DateTime(DateTime(2010, 3, 13, 22, 37, 52, 618), Scale::UTC), this->cdm_.getTCA());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getTCA());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetMissDistance)
{
    {
        EXPECT_EQ(Length::Meters(715.0), this->cdm_.getMissDistance());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getMissDistance());
    }
}

// TEST_F (OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetRelativePosition)
// {

//     {

//         EXPECT_EQ(Length.Meters(715.0), this->cdm_.getRelativePosition()) ;

//     }

//     {

//         EXPECT_ANY_THROW(CDM::Undefined().getRelativePosition()) ;

//     }

// }

// TEST_F (OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetRelativeVelocity)
// {

//     {

//         EXPECT_EQ(Length.Meters(715.0), this->cdm_.getRelativeVelocity()) ;

//     }

//     {

//         EXPECT_ANY_THROW(CDM::Undefined().getRelativeVelocity()) ;

//     }

// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetCollisionProbability)
{
    {
        EXPECT_EQ(0.0000435, this->cdm_.getCollisionProbability());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getCollisionProbability());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetCollisionProbabilityMethod)
{
    {
        EXPECT_EQ("FOSTER-1992", this->cdm_.getCollisionProbabilityMethod());
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getCollisionProbabilityMethod());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectDesignator)
{
    {
        EXPECT_EQ(30337, this->cdm_.getObjectDesignator(1));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectDesignator(1));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectName)
{
    {
        EXPECT_EQ("FENGYUN 1C DEB", this->cdm_.getObjectName(1));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectName(1));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectInternationalDesignator)
{
    {
        EXPECT_EQ("1999-025AA", this->cdm_.getObjectInternationalDesignator(1));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectInternationalDesignator(1));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectType)
{
    {
        EXPECT_EQ(CDM::ObjectType::Debris, this->cdm_.getObjectType(1));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectType(1));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectEphemerisName)
{
    {
        EXPECT_EQ("NONE", this->cdm_.getObjectEphemerisName(1));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectEphemerisName(1));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectCovarianceMethod)
{
    {
        EXPECT_EQ("CALCULATED", this->cdm_.getObjectCovarianceMethod(1));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectCovarianceMethod(1));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectManeuverability)
{
    {
        EXPECT_EQ("NO", this->cdm_.getObjectManeuverability(1));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectManeuverability(1));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectReferenceFrame)
{
    {
        EXPECT_EQ("EME2000", this->cdm_.getObjectReferenceFrame(1));
    }

    {
        EXPECT_ANY_THROW(CDM::Undefined().getObjectReferenceFrame(1));
    }
}

// TEST_F (OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectStateAtTCA)
// {

//     {

//         EXPECT_EQ("EME2000", this->cdm_.getObjectStateAtTCA(1)) ;

//     }

//     {

//         EXPECT_ANY_THROW(CDM::Undefined().getObjectStateAtTCA(1)) ;

//     }

// }

// TEST_F (OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, GetObjectCovarianceMatrix)
// {

//     {

//         EXPECT_EQ("EME2000", this->cdm_.getObjectCovarianceMatrix(1)) ;

//     }

//     {

//         EXPECT_ANY_THROW(CDM::Undefined().getObjectCovarianceMatrix(1)) ;

//     }

// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, Undefined)
{
    {
        EXPECT_NO_THROW(CDM::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, Dictionary)
{
    using ostk::core::container::Dictionary;
    using ostk::core::container::Object;

    {
        EXPECT_ANY_THROW(CDM::Dictionary(Dictionary::Empty()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, Parse)
{
    {
        EXPECT_ANY_THROW(CDM::Parse(String::Empty()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, Load)
{
    using ostk::core::filesystem::File;
    using ostk::core::filesystem::Path;

    {
        const CDM cdm = CDM::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Conjunction/Message/CCSDS/CDM/cdm.json"))
        );
    }

    {
        EXPECT_ANY_THROW(CDM::Load(File::Undefined()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM, ObjectTypeFromString)
{
    {
        EXPECT_EQ(CDM::ObjectType::Payload, CDM::ObjectTypeFromString("PAYLOAD"));
        EXPECT_EQ(CDM::ObjectType::RocketBody, CDM::ObjectTypeFromString("ROCKET BODY"));
        EXPECT_EQ(CDM::ObjectType::Debris, CDM::ObjectTypeFromString("DEBRIS"));
        EXPECT_EQ(CDM::ObjectType::Unknown, CDM::ObjectTypeFromString("UNKNOWN"));
        EXPECT_EQ(CDM::ObjectType::Other, CDM::ObjectTypeFromString("OTHER"));
    }

    {
        EXPECT_ANY_THROW(CDM::ObjectTypeFromString("UNSUPPORTED RESIDENT SPACE OBJECT TYPE"));
    }
}
