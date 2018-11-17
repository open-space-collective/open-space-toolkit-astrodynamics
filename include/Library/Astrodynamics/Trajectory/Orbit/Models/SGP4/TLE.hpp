////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE__
#define __Library_Astrodynamics_Trajectory_Orbit_Models_SGP4_TLE__

#include <Library/Physics/Time/Instant.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Derived/Angle.hpp>

#include <Library/Core/FileSystem/File.hpp>
#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Integer.hpp>

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

using library::core::types::Integer ;
using library::core::types::Real ;
using library::core::types::String ;
using library::core::fs::File ;

using library::physics::units::Angle ;
using library::physics::units::Derived ;
using library::physics::time::Instant ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      A Two-Line Element set (TLE) is data format encoding a list of orbital elements
///                             of an Earth-orbiting object for a given point in time
///
/// @ref                        https://en.wikipedia.org/wiki/Two-line_element_set

class TLE
{

    public:

        /// @brief              Constructor
        ///
        /// @code
        ///                     TLE tle("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        ///                             "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537") ;
        /// @endcode
        ///
        /// @param              [in] aFirstLine A first line
        /// @param              [in] aSecondLine A second line

                                TLE                                         (   const   String&                     aFirstLine,
                                                                                const   String&                     aSecondLine                                 ) ;

        /// @brief              Constructor
        ///
        /// @code
        ///                     TLE tle("ISS (ZARYA)",
        ///                             "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        ///                             "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537") ;
        /// @endcode
        ///
        /// @param              [in] aSatelliteName A satellite name
        /// @param              [in] aFirstLine A first line
        /// @param              [in] aSecondLine A second line

                                TLE                                         (   const   String&                     aSatelliteName,
                                                                                const   String&                     aFirstLine,
                                                                                const   String&                     aSecondLine                                 ) ;

        /// @brief              Equal to operator
        ///
        /// @code
        ///                     TLE("...", "...") == TLE("...", "...") ;
        /// @endcode
        ///
        /// @param              [in] aTle A TLE
        /// @return             True if TLEs are equal

        bool                    operator ==                                 (   const   TLE&                        aTle                                        ) const ;

        /// @brief              Not equal to operator
        ///
        /// @code
        ///                     TLE("...", "...") != TLE("...", "...") ;
        /// @endcode
        ///
        /// @param              [in] aTle A TLE
        /// @return             True if TLEs are not equal

        bool                    operator !=                                 (   const   TLE&                        aTle                                        ) const ;

        /// @brief              Output stream operator
        ///
        /// @code
        ///                     TLE tle("...", "...") ;
        ///                     std::cout << tle ;
        /// @endcode
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] aTle A TLE
        /// @return             A reference to output stream

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   TLE&                        aTle                                        ) ;

        /// @brief              Check if TLE is defined
        ///
        /// @code
        ///                     TLE("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        ///                         "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537").isDefined() ; // True
        /// @endcode
        ///
        /// @return             True if TLE is defined

        bool                    isDefined                                   ( ) const ;

        /// @brief              Get satellite name
        ///
        /// @code
        ///                     TLE("ISS (ZARYA)",
        ///                         "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        ///                         "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537").getSatelliteName() ; // "ISS (ZARYA)"
        /// @endcode
        ///
        /// @return             Satellite name

        String                  getSatelliteName                            ( ) const ;

        /// @brief              Get first line
        ///
        /// @code
        ///                     TLE("ISS (ZARYA)",
        ///                         "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        ///                         "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537").getSatelliteName() ; // "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927"
        /// @endcode
        ///
        /// @return             First line
        
        String                  getFirstLine                                ( ) const ;

        /// @brief              Get second line
        ///
        /// @code
        ///                     TLE("ISS (ZARYA)",
        ///                         "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        ///                         "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537").getSatelliteName() ; // "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537"
        /// @endcode
        ///
        /// @return             Second line

        String                  getSecondLine                               ( ) const ;

        /// @brief              Get satellite number
        ///
        /// @return             Satellite number

        Integer                 getSatelliteNumber                          ( ) const ;

        /// @brief              Get classification
        ///
        /// @return             Classification

        String                  getClassification                           ( ) const ;

        /// @brief              Get international designator
        ///
        /// @return             International designator

        String                  getInternationalDesignator                  ( ) const ;

        /// @brief              Get epoch
        ///
        /// @return             Epoch

        Instant                 getEpoch                                    ( ) const ;

        /// @brief              Get first time derivative of the mean motion divided by two
        ///
        /// @return             First time derivative of the mean motion divided by two

        Real                    getMeanMotionFirstTimeDerivativeDividedByTwo ( ) const ;

        /// @brief              Get second time derivative of mean motion divided by six
        ///
        /// @return             Second time derivative of mean motion divided by six

        Real                    getMeanMotionSecondTimeDerivativeDividedBySix ( ) const ;

        /// @brief              Get B* drag term
        ///
        /// @return             B* drag term
        /// @ref                https://en.wikipedia.org/wiki/BSTAR

        Real                    getBStarDragTerm                            ( ) const ;

        /// @brief              Get ephemeris type (0)
        ///
        /// @return             Ephemeris type

        Integer                 getEphemerisType                            ( ) const ;

        /// @brief              Get element set number (incremented when a new TLE is generated for this object)
        ///
        /// @return             Element set number

        Integer                 getElementSetNumber                         ( ) const ;

        /// @brief              Get checksum of first line
        ///
        /// @return             Checksum of first line

        Integer                 getFirstLineChecksum                        ( ) const ;

        /// @brief              Get inclination
        ///
        /// @return             Inclination

        Angle                   getInclination                              ( ) const ;

        /// @brief              Get Right Ascension of the Ascending Node [RAAN]
        ///
        /// @return             Right Ascension of the Ascending Node

        Angle                   getRaan                                     ( ) const ;

        /// @brief              Get eccentricity
        ///
        /// @return             Eccentricity

        Real                    getEccentricity                             ( ) const ;

        /// @brief              Get Argument of Perigee [AOP]
        ///
        /// @return             Argument of Perigee

        Angle                   getAop                                      ( ) const ;

        /// @brief              Get Mean Anomaly
        ///
        /// @return             Mean Anomaly

        Angle                   getMeanAnomaly                              ( ) const ;

        /// @brief              Get Mean Motion
        ///
        /// @return             Mean Motion

        Derived                 getMeanMotion                               ( ) const ;

        /// @brief              Get revolution number at epoch
        ///
        /// @return             Revolution number at epoch

        Integer                 getRevolutionNumberAtEpoch                  ( ) const ;

        /// @brief              Constructs an undefined TLE
        ///
        /// @code
        ///                     TLE tle = TLE::Undefined() ; // Undefined
        /// @endcode
        ///
        /// @return             Undefined TLE

        static TLE              Undefined                                   ( ) ;

        /// @brief              Returns true if TLE can be generated from the given string
        ///
        /// @code
        ///                     TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n
        ///                                    2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537") ; // True
        /// @endcode
        ///
        /// @param              [in] aString A string
        /// @return             True if TLE can be generated from the given string

        static bool             CanParse                                    (   const   String&                     aString                                     ) ;

        /// @brief              Returns true if TLE can be generated from the given lines
        ///
        /// @code
        ///                     TLE::CanParse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        ///                                   "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537") ; // True
        /// @endcode
        ///
        /// @param              [in] aFirstLine A first line
        /// @param              [in] aSecondLine A second line
        /// @return             True if TLE can be generated from the given lines

        static bool             CanParse                                    (   const   String&                     aFirstLine,
                                                                                const   String&                     aSecondLine                                 ) ;

        /// @brief              Constructs a TLE from a given string
        ///
        /// @code
        ///                     TLE tle = TLE::Parse("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927\n
        ///                                           2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537") ;
        /// @endcode
        ///
        /// @param              [in] aString A string
        /// @return             TLE

        static TLE              Parse                                       (   const   String&                     aString                                     ) ;

        /// @brief              Load a TLE from a given file
        ///
        /// @code
        ///                     TLE tle = TLE::File(File::Path(Path::String("/path/to/file.tle"))) ;
        /// @endcode
        ///
        /// @param              [in] aFile A file
        /// @return             TLE

        static TLE              Load                                        (   const   File&                       aFile                                       ) ;

    private:

        String                  satelliteName_ ;
        String                  firstLine_ ;
        String                  secondLine_ ;

        static Integer          GenerateChecksum                            (   const   String&                     aLine                                       ) ;

        static Real             ParseReal                                   (   const   String&                     aString,
                                                                                        bool                        isDecimalPointAssumed                       ) ;

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