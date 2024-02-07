/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_Messages_CCSDS_CDM__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_Messages_CCSDS_CDM__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Dictionary.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Providers/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

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

namespace ctnr = ostk::core::container;

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::filesystem::File;
using ostk::core::type::Index;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::coord::frame::providers::iau::Theory;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;

using ostk::astro::trajectory::State;

/// @brief CCSDS Conjunction Data Message (CDM)
/// General content sections:
/// - Header
/// - Event Relative Metadata
/// - Objects Metadata
/// - Objects Data
///
/// @ref https://public.ccsds.org/Pubs/508x0b1e2c2.pdf
// TBI: Figure out how to adapt based on providers
// TBI: Find a good way to handle comments and extra fields
// TBI: Self-contained, create a RSO class to hold Object data
// TBI: Add getters at structs level
// TBI: Improve handling of covariance matrix

class CDM
{
   public:
    enum class ObjectType
    {
        Payload,
        RocketBody,
        Debris,
        Unknown,
        Other
    };

    struct Header
    {
        String ccsdsCdmVersion;
        String comment = String::Empty();
        Instant creationDate;
        String originator;
        String messageFor = String::Empty();
        String messageId;
    };

    struct RelativeMetadata
    {
        String comment = String::Empty();

        Instant TCA;
        Length missDistance;
        Position relativePosition = Position::Undefined();  // expressed in RTN frame at TCA
        Velocity relativeVelocity = Velocity::Undefined();  // expressed in RTN frame at TCA

        Instant startScreenPeriod = Instant::Undefined();  // UTC
        Instant endScreenPeriod = Instant::Undefined();    // UTC
        String screenVolumeFrame = String::Empty();
        String screenVolumeShape = String::Empty();
        Real screenVolumeX = Real::Undefined();  // TBI: Make it a vector?
        Real screenVolumeY = Real::Undefined();
        Real screenVolumeZ = Real::Undefined();
        Instant screenEntryTime = Instant::Undefined();  // UTC
        Instant screenExitTime = Instant::Undefined();   // UTC

        Real collisionProbability = Real::Undefined();
        String collisionProbabilityMethod = String::Empty();
    };

    struct Metadata
    {
        String comment = String::Empty();
        String object;
        Integer objectDesignator;
        String catalogName;
        String objectName;
        String internationalDesignator;
        ObjectType objectType = ObjectType::Unknown;

        String operatorContactPosition = String::Empty();
        String operatorOrgnization = String::Empty();
        String operatorPhone = String::Empty();
        String operatorEmail = String::Empty();

        String ephemerisName;
        String covarianceMethod;
        String maneuverable;
        String orbitCenter = String::Empty();
        String refFrame;                        // TBI: Use Frame later
        String gravityModel = String::Empty();  // TBI: Check String or actual pointer to GravityModel
        String atmosphericModel = String::Empty();
        String nBodyPerturbations = String::Empty();
        bool solarRadiationPressure = false;
        bool earthTides = false;
        bool inTrackThrust = false;
    };

    // The CDM Data section shall be formed as logical blocks:
    //    – OD Parameters
    //    – Additional Parameters
    //    – State Vector
    //    – Covariance Matrix

    struct Data
    {
        // OD Parameters

        // String              ODComment = String::Empty() ;
        Instant timeLastObStart = Instant::Undefined();
        Instant timeLastObEnd = Instant::Undefined();
        Duration recommendedODSpan = Duration::Undefined();
        Duration actualODSpan = Duration::Undefined();
        Integer obsAvailable = Integer::Undefined();
        Integer obsUsed = Integer::Undefined();
        Integer tracksAvailable = Integer::Undefined();
        Integer tracksUsed = Integer::Undefined();
        Real residualsAccepted = Real::Undefined();
        Real weightedRMS = Real::Undefined();

        // Additional Parameters

        // String              additionParametersComment ;
        Real areaPC = Real::Undefined();  // Area, add Unit
        Real areaDrag = Real::Undefined();
        Real areaSRP = Real::Undefined();
        Mass mass = Mass::Undefined();
        Real cdAreaOverMass = Real::Undefined();
        Real crAreaOverMass = Real::Undefined();
        Real thrustAcceleration = Real::Undefined();  // Acceleration, add Unit
        Real SEDR = Real::Undefined();

        // State Vector

        // String              stateVectorComment ;
        State state;

        // Covariance Matrix

        // String              covarianceComment ;
        MatrixXd covarianceMatrix;  // Usually defined in RTN Frame
    };

    CDM(const CDM::Header& aHeader,
        const CDM::RelativeMetadata& aRelativeMetadata,
        const Array<CDM::Metadata>& aMetadataArray,
        const Array<CDM::Data> aDataArray);

    friend std::ostream& operator<<(std::ostream& anOutputStream, const CDM& anCDM);

    bool isDefined() const;

    CDM::Header getHeader() const;

    CDM::RelativeMetadata getRelativeMetadata() const;

    Array<CDM::Metadata> getMetadataArray() const;

    Array<CDM::Data> getDataArray() const;

    CDM::Metadata getObjectMetadataAt(const Index& anIndex) const;

    CDM::Data getObjectDataAt(const Index& anIndex) const;

    // Header getters

    String getCCSDSCDMVersion() const;

    Instant getCreationDate() const;

    String getOriginator() const;

    String getMessageFor() const;

    String getMessageId() const;

    // Relative Metadata getters

    Instant getTCA() const;

    Length getMissDistance() const;

    Position getRelativePosition() const;

    Velocity getRelativeVelocity() const;

    Real getCollisionProbability() const;

    String getCollisionProbabilityMethod() const;

    // Object Metadata getters. TBI: Create Resident Space Object (RSO) class and add these methods under it

    Integer getObjectDesignator(const Index& anIndex) const;

    String getObjectName(const Index& anIndex) const;

    String getObjectInternationalDesignator(const Index& anIndex) const;

    CDM::ObjectType getObjectType(const Index& anIndex) const;

    String getObjectEphemerisName(const Index& anIndex) const;

    String getObjectCovarianceMethod(const Index& anIndex) const;

    String getObjectManeuverability(const Index& anIndex) const;

    String getObjectReferenceFrame(const Index& anIndex) const;

    // Object Data getters. TBI: Create Resident Space Object (RSO) class and add these methods under it

    State getObjectStateAtTCA(const Index& anIndex) const;

    MatrixXd getObjectCovarianceMatrix(const Index& anIndex) const;

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    static CDM Undefined();

    static CDM Dictionary(const ctnr::Dictionary& aDictionary);

    static CDM Parse(const String& aString);

    static CDM Load(const File& aFile);

    static CDM::ObjectType ObjectTypeFromString(const String& aString);

   private:
    CDM::Header header_;
    CDM::RelativeMetadata relativeMetadata_;
    Array<CDM::Metadata> objectsMetadata_;
    Array<CDM::Data> objectsData_;
};

}  // namespace ccsds
}  // namespace messages
}  // namespace conjunction
}  // namespace astro
}  // namespace ostk

#endif
