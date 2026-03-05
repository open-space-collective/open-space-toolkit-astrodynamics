/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_Message_CCSDS_CDM__

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
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

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

namespace container = ostk::core::container;

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::filesystem::File;
using ostk::core::type::Index;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::iau::Theory;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::trajectory::State;

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
        Payload,     ///< Operational payload spacecraft.
        RocketBody,  ///< Rocket body or upper stage.
        Debris,      ///< Orbital debris fragment.
        Unknown,     ///< Object type is unknown.
        Other        ///< Object type not covered by other values.
    };

    /// @brief CDM header section containing message identification and provenance fields.
    struct Header
    {
        String ccsdsCdmVersion;
        String comment = String::Empty();
        Instant creationDate;
        String originator;
        String messageFor = String::Empty();
        String messageId;
    };

    /// @brief CDM relative metadata section describing the conjunction event geometry and screening parameters.
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

    /// @brief CDM per-object metadata section describing the resident space object and its propagation configuration.
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

    /// @brief CDM per-object data section containing OD parameters, physical properties, state vector, and covariance
    /// matrix.
    struct Data
    {
        // OD Parameters

        // String              ODComment = String::Empty();
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

        // String              additionParametersComment;
        Real areaPC = Real::Undefined();  // Area, add Unit
        Real areaDrag = Real::Undefined();
        Real areaSRP = Real::Undefined();
        Mass mass = Mass::Undefined();
        Real cdAreaOverMass = Real::Undefined();
        Real crAreaOverMass = Real::Undefined();
        Real thrustAcceleration = Real::Undefined();  // Acceleration, add Unit
        Real SEDR = Real::Undefined();

        // State Vector

        // String              stateVectorComment;
        State state;

        // Covariance Matrix

        // String              covarianceComment;
        MatrixXd covarianceMatrix;  // Usually defined in RTN Frame
    };

    /// @brief Construct a CDM from its constituent sections.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM(header, relativeMetadata, metadataArray, dataArray) ;
    /// @endcode
    ///
    /// @param aHeader A CDM header.
    /// @param aRelativeMetadata A CDM relative metadata.
    /// @param aMetadataArray An array of per-object metadata.
    /// @param aDataArray An array of per-object data.
    CDM(const CDM::Header& aHeader,
        const CDM::RelativeMetadata& aRelativeMetadata,
        const Array<CDM::Metadata>& aMetadataArray,
        const Array<CDM::Data> aDataArray);

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     std::cout << cdm ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param anCDM A CDM.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const CDM& anCDM);

    /// @brief Check whether the CDM is defined.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     cdm.isDefined() ;
    /// @endcode
    ///
    /// @return True if defined.
    bool isDefined() const;

    /// @brief Get the CDM header.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     CDM::Header header = cdm.getHeader() ;
    /// @endcode
    ///
    /// @return The CDM header.
    CDM::Header getHeader() const;

    /// @brief Get the CDM relative metadata.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     CDM::RelativeMetadata relativeMetadata = cdm.getRelativeMetadata() ;
    /// @endcode
    ///
    /// @return The CDM relative metadata.
    CDM::RelativeMetadata getRelativeMetadata() const;

    /// @brief Get the array of per-object metadata.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Array<CDM::Metadata> metadataArray = cdm.getMetadataArray() ;
    /// @endcode
    ///
    /// @return An array of per-object metadata.
    Array<CDM::Metadata> getMetadataArray() const;

    /// @brief Get the array of per-object data.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Array<CDM::Data> dataArray = cdm.getDataArray() ;
    /// @endcode
    ///
    /// @return An array of per-object data.
    Array<CDM::Data> getDataArray() const;

    /// @brief Get the metadata for a specific object by index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     CDM::Metadata metadata = cdm.getObjectMetadataAt(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The metadata for the object at the given index.
    CDM::Metadata getObjectMetadataAt(const Index& anIndex) const;

    /// @brief Get the data for a specific object by index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     CDM::Data data = cdm.getObjectDataAt(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The data for the object at the given index.
    CDM::Data getObjectDataAt(const Index& anIndex) const;

    // Header getters

    /// @brief Get the CCSDS CDM format version string.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String version = cdm.getCCSDSCDMVersion() ;
    /// @endcode
    ///
    /// @return The CCSDS CDM version string.
    String getCCSDSCDMVersion() const;

    /// @brief Get the message creation date.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Instant creationDate = cdm.getCreationDate() ;
    /// @endcode
    ///
    /// @return The message creation date as an Instant.
    Instant getCreationDate() const;

    /// @brief Get the message originator identifier.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String originator = cdm.getOriginator() ;
    /// @endcode
    ///
    /// @return The originator string.
    String getOriginator() const;

    /// @brief Get the optional message recipient identifier.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String messageFor = cdm.getMessageFor() ;
    /// @endcode
    ///
    /// @return The message-for string, or an empty string if not set.
    String getMessageFor() const;

    /// @brief Get the unique message identifier.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String messageId = cdm.getMessageId() ;
    /// @endcode
    ///
    /// @return The message ID string.
    String getMessageId() const;

    // Relative Metadata getters

    /// @brief Get the Time of Closest Approach (TCA).
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Instant tca = cdm.getTCA() ;
    /// @endcode
    ///
    /// @return The TCA as an Instant.
    Instant getTCA() const;

    /// @brief Get the miss distance at TCA.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Length missDistance = cdm.getMissDistance() ;
    /// @endcode
    ///
    /// @return The miss distance as a Length.
    Length getMissDistance() const;

    /// @brief Get the relative position at TCA, expressed in the RTN frame.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Position relativePosition = cdm.getRelativePosition() ;
    /// @endcode
    ///
    /// @return The relative position at TCA.
    Position getRelativePosition() const;

    /// @brief Get the relative velocity at TCA, expressed in the RTN frame.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Velocity relativeVelocity = cdm.getRelativeVelocity() ;
    /// @endcode
    ///
    /// @return The relative velocity at TCA.
    Velocity getRelativeVelocity() const;

    /// @brief Get the probability of collision.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Real collisionProbability = cdm.getCollisionProbability() ;
    /// @endcode
    ///
    /// @return The collision probability as a Real.
    Real getCollisionProbability() const;

    /// @brief Get the method used to compute the collision probability.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String method = cdm.getCollisionProbabilityMethod() ;
    /// @endcode
    ///
    /// @return The collision probability method string.
    String getCollisionProbabilityMethod() const;

    // Object Metadata getters. TBI: Create Resident Space Object (RSO) class and add these methods under it

    /// @brief Get the catalog designator for the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     Integer designator = cdm.getObjectDesignator(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The object designator as an Integer.
    Integer getObjectDesignator(const Index& anIndex) const;

    /// @brief Get the name of the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String name = cdm.getObjectName(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The object name string.
    String getObjectName(const Index& anIndex) const;

    /// @brief Get the international designator of the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String intlDesignator = cdm.getObjectInternationalDesignator(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The international designator string.
    String getObjectInternationalDesignator(const Index& anIndex) const;

    /// @brief Get the object type of the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     CDM::ObjectType objectType = cdm.getObjectType(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The object type enum value.
    CDM::ObjectType getObjectType(const Index& anIndex) const;

    /// @brief Get the ephemeris name for the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String ephemerisName = cdm.getObjectEphemerisName(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The ephemeris name string.
    String getObjectEphemerisName(const Index& anIndex) const;

    /// @brief Get the covariance method for the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String covarianceMethod = cdm.getObjectCovarianceMethod(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The covariance method string.
    String getObjectCovarianceMethod(const Index& anIndex) const;

    /// @brief Get the maneuverability status for the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String maneuverable = cdm.getObjectManeuverability(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The maneuverability string.
    String getObjectManeuverability(const Index& anIndex) const;

    /// @brief Get the reference frame for the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     String refFrame = cdm.getObjectReferenceFrame(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The reference frame string.
    String getObjectReferenceFrame(const Index& anIndex) const;

    // Object Data getters. TBI: Create Resident Space Object (RSO) class and add these methods under it

    /// @brief Get the state of the object at the TCA.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     State state = cdm.getObjectStateAtTCA(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The state of the object at TCA.
    State getObjectStateAtTCA(const Index& anIndex) const;

    /// @brief Get the covariance matrix for the object at the given index.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     MatrixXd covarianceMatrix = cdm.getObjectCovarianceMatrix(0) ;
    /// @endcode
    ///
    /// @param anIndex A zero-based object index.
    /// @return The covariance matrix, typically expressed in the RTN frame.
    MatrixXd getObjectCovarianceMatrix(const Index& anIndex) const;

    /// @brief Print the CDM to an output stream.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    ///     cdm.print(std::cout, true) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, print a decorator around the output.
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Construct an undefined CDM.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Undefined() ;
    /// @endcode
    ///
    /// @return An undefined CDM.
    static CDM Undefined();

    /// @brief Construct a CDM from a dictionary.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Dictionary(dictionary) ;
    /// @endcode
    ///
    /// @param aDictionary A dictionary containing CDM key-value pairs.
    /// @return A CDM.
    static CDM Dictionary(const container::Dictionary& aDictionary);

    /// @brief Parse a CDM from a string.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Parse(cdmString) ;
    /// @endcode
    ///
    /// @param aString A string containing CDM-formatted text.
    /// @return A CDM.
    static CDM Parse(const String& aString);

    /// @brief Load a CDM from a file.
    ///
    /// @code{.cpp}
    ///     CDM cdm = CDM::Load(File::Path(Path::Parse("path/to/cdm.txt"))) ;
    /// @endcode
    ///
    /// @param aFile A file containing a CDM.
    /// @return A CDM.
    static CDM Load(const File& aFile);

    /// @brief Convert a string to an ObjectType enum value.
    ///
    /// @code{.cpp}
    ///     CDM::ObjectType objectType = CDM::ObjectTypeFromString("PAYLOAD") ;
    /// @endcode
    ///
    /// @param aString A string representation of the object type.
    /// @return The corresponding ObjectType enum value.
    static CDM::ObjectType ObjectTypeFromString(const String& aString);

   private:
    CDM::Header header_;
    CDM::RelativeMetadata relativeMetadata_;
    Array<CDM::Metadata> objectsMetadata_;
    Array<CDM::Data> objectsData_;
};

}  // namespace ccsds
}  // namespace message
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
