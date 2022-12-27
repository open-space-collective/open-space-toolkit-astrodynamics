////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Conjunction/Messages/CCSDS/CDM.hpp
/// @author         Remy Derollez <remy@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_Messages_CCSDS_CDM__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_Messages_CCSDS_CDM__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Matrix.hpp>

#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Containers/Dictionary.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

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

namespace ctnr = ostk::core::ctnr ;

using ostk::core::types::Integer ;
using ostk::core::types::Size ;
using ostk::core::types::Real ;
using ostk::core::types::String ;
using ostk::core::ctnr::Array ;
using ostk::core::fs::File ;

using ostk::math::obj::Matrix3d ;
using ostk::math::obj::MatrixXd ; // Test out

using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;
using ostk::physics::units::Length ;
using ostk::physics::units::Mass ;
using ostk::physics::units::Angle ;
using ostk::physics::coord::Position ;
using ostk::physics::coord::Velocity ;
using ostk::physics::coord::Frame ;

using ostk::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      CCSDS Conjunction Data Message (CDM)
///
///                             General content sections:
///                             - header
///                             - relativeMetadata
///                             - objectsMetadata
///                             - data
///
/// @ref                        https://public.ccsds.org/Pubs/508x0b1e2c2.pdf

class CDM
{

    public:

        struct Header
        {

            String              ccsdsCdmVersion ;
            String              comment = String::Undefined() ;
            Instant             creationDate ;
            String              originator ;
            String              messageFor = String::Undefined() ;
            String              messageId ;

        } ;

        struct RelativeMetadata
        {

            String              comment = String::Undefined() ;

            Instant             TCA ;
            Length              missDistance ;
            Velocity            relativeVelocity = Velocity::Undefined() ;  // expressed in RTN frame

            Instant             startScreenPeriod = Instant::Undefined() ;  // UTC
            Instant             endScreenPeriod = Instant::Undefined() ;    // UTC
            String              screenVolumeFrame = String::Undefined();
            String              screenVolumeShape = String::Undefined();
            Real                screenVolumeX = Real::Undefined() ;  // Make it a vector?
            Real                screenVolumeY = Real::Undefined() ;
            Real                screenVolumeZ = Real::Undefined() ;
            Instant             screenEntryTime = Instant::Undefined() ;  // UTC
            Instant             screenExitTime = Instant::Undefined() ;  // UTC

            Real                collisionProbility = Real::Undefined() ;
            String              collisionProbilityMethod = String::Undefined() ;

        } ;

        // Exists per object concerned with the CDM

        struct Metadata
        {

            String              comment ;
            String              object ;
            Integer             objectDesignator ;
            String              catalogName ;
            String              objectName ;
            String              internationalDesignator ;
            ObjectType          objectType = ;  // ObjectType to be created

            String              operatorContactPosition = String::Undefined() ;
            String              operatorOrgnization = String::Undefined() ;
            String              operatorPhone = String::Undefined() ;
            String              operatorName = String::Undefined() ;

            String              ephemerisName = String::Undefined();
            String              covarianceMethod = String::Undefined();
            bool                maneuverable ;
            String              orbitCenter = String::Undefined();
            Frame               refFrame ;
            String              gravityModel = String::Undefined() ;  // Check String or actual pointer to GravityModel
            String              atmosphericModel = String::Undefined();
            String              nBodyPerturbations = String::Undefined();
            bool                solarRadiationPressure ;
            bool                earthTides ;
            bool                intrackThrust ;

        } ;

        // The CDM Data section shall be formed as logical blocks:
        //    – OD Parameters;
        //    – Additional Parameters;
        //    – State Vector; and
        //    – Covariance Matrix.

        struct Data
        {

            // OD Parameters

            // String              comment ;
            Instant             timeLastObStart ;
            Instant             timeLastObEnd ;
            Duration            recommendedODSpan ;
            Duration            actualODSpan ;
            Integer             obsAvailable ;
            Integer             obsUsed ;
            Integer             tracksAvailable ;
            Integer             tracksUsed ;
            Real                residualsAccepted ;
            Real                weightedRMS ;

            // Additional Parameters

            // String              comment ;
            Real                areaPC ;  // Area, add Unit
            Real                areaDrag ;
            Real                areaSRP ;
            Mass                mass ;
            Real                cdAreaOverMass ;
            Real                crAreaOverMass ;
            Real                thrustAcceleration ;  // Acceleration, add Unit
            Real                SEDR ;

            // State Vector

            // String              comment ;
            State               stateVector ;  // Frame? (RTN)

            // Covariance Matrix

            // String              comment ;
            MatrixXd            covarianceMatrix ;  // Check how to accomodate 6x6 vs 9x9, Should be 6d

        } ;




                                CDM                                         (   const   CDM::Header&                aHeader,
                                                                                const   CDM::RelativeMetadata&      aRelativeMetadata,
                                                                                const   Array<CDM::Metadata>&       aMetadataArray,
                                                                                const   Array<CDM::Data>            aDataArray                                  ) ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   CDM&                        anCDM                                       ) ;

        bool                    isDefined                                   ( ) const ;

        CDM::Header             getHeader                                   ( ) const ;

        CDM::RelativeMetadata   getRelativeMetadata                         ( ) const ;

        Array<CDM::Metadata>    getMetadataArray                            ( ) const ;

        Array<CDM::Data>        getDataArray                                ( ) const ;

        CDM::Metadata           getObjectMetadataAt                         (   const   Size&                       anIndex                                     ) const ;

        CDM::Data               getObjectDataAt                             (   const   Size&                       anIndex                                     ) const ;

        void                    print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        static CDM              Undefined                                   ( ) ;

        static CDM              Dictionary                                  (   const   ctnr::Dictionary&           aDictionary                                 ) ;

        static CDM              Parse                                       (   const   String&                     aString                                     ) ;

        static CDM              Load                                        (   const   File&                       aFile                                       ) ;

    private:

        CDM::Header             header_ ;
        CDM::RelativeMetadata   relativeMetadata_ ;
        Array<CDM::Metadata>    objectsMetadata_ ;
        Array<CDM::Data>        objectsdata_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
