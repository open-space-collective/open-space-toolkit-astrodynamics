/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Message_SpaceX_OPM__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Message_SpaceX_OPM__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Dictionary.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace message
{
namespace spacex
{

namespace container = ostk::core::container;

using ostk::core::container::Array;
using ostk::core::filesystem::File;
using ostk::core::type::Index;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::State;

/// @brief SpaceX Orbital Parameter Message (OPM)
///
///                      # SpaceX OPM output for XXX Mission
///                      # Notes:
///                      # - ECEF velocity is Earth relative
///                      # - Apogee/Perigee altitude assumes a spherical Earth, 6378.137 km radius
///                      # - Orbital elements are computed in an inertial frame realized by inertially
///                      # freezing the WGS84 ECEF frame at time of current state
///                      # - State is post-deployment, so includes separation delta-velocity
///
///                      header:
///                        generation_date: YYYY-MM-DD-Day-HH-MM-SS
///                        launch_date: YYYY-MM-DD-Day-HH-MM-SS
///                      deployments:
///                        - name: payload-xxx
///                          sequence_number: 1
///                          mission_time_s: +XX.XX
///                          date: YYYY-MM-DD-Day-HH-MM-SS
///                          r_ecef_m: [+XXXXXX.XXX, +XXXXXX.XXX, +XXXXXX.XXX]
///                          v_ecef_m_per_s: [+XXXX.XXX, +XXXX.XXX, +XXXX.XXX]
///                          mean_perigee_altitude_km: +XXX.XXX
///                          mean_apogee_altitude_km: +XXX.XXX
///                          mean_inclination_deg: +XX.XXX
///                          mean_argument_of_perigee_deg: +XXX.XXX
///                          mean_longitude_ascending_node_deg: +XXX.XXX
///                          mean_mean_anomaly_deg: +XX.XXX
///                          ballistic_coef_kg_per_m2: +XX.XX
///
/// @ref                        Appendix E: Delivery Format Of Separation State Vector
///                      https://storage.googleapis.com/rideshare-static/Rideshare_Payload_Users_Guide.pdf
class OPM
{
   public:
    /// @brief OPM header containing generation and launch dates.
    struct Header
    {
        Instant generationDate;
        Instant launchDate;
    };

    /// @brief OPM deployment entry containing state and orbital element information for a single payload.
    struct Deployment
    {
        String name;
        Integer sequenceNumber;
        Duration missionTime;
        Instant date;
        Position position;
        Velocity velocity;
        Length meanPerigeeAltitude;
        Length meanApogeeAltitude;
        Angle meanInclination;
        Angle meanArgumentOfPerigee;
        Angle meanLongitudeAscendingNode;
        Angle meanMeanAnomaly;
        Real ballisticCoefficient;  // [kg/m2]

        /// @brief Convert deployment to a trajectory state.
        ///
        /// @code{.cpp}
        ///     OPM::Deployment deployment = ... ;
        ///     State state = deployment.toState() ;
        /// @endcode
        ///
        /// @return A trajectory state corresponding to this deployment.
        State toState() const;
    };

    /// @brief Construct an OPM from a header and an array of deployments.
    ///
    /// @code{.cpp}
    ///     OPM::Header header = ... ;
    ///     Array<OPM::Deployment> deployments = ... ;
    ///     OPM opm = {header, deployments} ;
    /// @endcode
    ///
    /// @param aHeader An OPM header.
    /// @param aDeploymentArray An array of OPM deployments.
    OPM(const OPM::Header& aHeader, const Array<OPM::Deployment>& aDeploymentArray);

    /// @brief Output stream operator.
    ///
    /// @code{.cpp}
    ///     OPM opm = ... ;
    ///     std::cout << opm ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param anOPM An OPM.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const OPM& anOPM);

    /// @brief Check if the OPM is defined.
    ///
    /// @code{.cpp}
    ///     OPM opm = ... ;
    ///     bool defined = opm.isDefined() ;
    /// @endcode
    ///
    /// @return True if the OPM is defined.
    bool isDefined() const;

    /// @brief Get the OPM header.
    ///
    /// @code{.cpp}
    ///     OPM opm = ... ;
    ///     OPM::Header header = opm.getHeader() ;
    /// @endcode
    ///
    /// @return The OPM header.
    OPM::Header getHeader() const;

    /// @brief Get all deployments in the OPM.
    ///
    /// @code{.cpp}
    ///     OPM opm = ... ;
    ///     Array<OPM::Deployment> deployments = opm.getDeployments() ;
    /// @endcode
    ///
    /// @return An array of OPM deployments.
    Array<OPM::Deployment> getDeployments() const;

    /// @brief Get the deployment at a given index.
    ///
    /// @code{.cpp}
    ///     OPM opm = ... ;
    ///     OPM::Deployment deployment = opm.getDeploymentAt(0) ;
    /// @endcode
    ///
    /// @param anIndex A deployment index.
    /// @return The OPM deployment at the given index.
    OPM::Deployment getDeploymentAt(const Index& anIndex) const;

    /// @brief Get the deployment with a given name.
    ///
    /// @code{.cpp}
    ///     OPM opm = ... ;
    ///     OPM::Deployment deployment = opm.getDeploymentWithName("payload-001") ;
    /// @endcode
    ///
    /// @param aName A deployment name.
    /// @return The OPM deployment with the given name.
    OPM::Deployment getDeploymentWithName(const String& aName) const;

    /// @brief Print the OPM to an output stream.
    ///
    /// @code{.cpp}
    ///     OPM opm = ... ;
    ///     opm.print(std::cout, true) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display a decorator around the output.
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Construct an undefined OPM.
    ///
    /// @code{.cpp}
    ///     OPM opm = OPM::Undefined() ;
    /// @endcode
    ///
    /// @return An undefined OPM.
    static OPM Undefined();

    /// @brief Construct an OPM from a dictionary.
    ///
    /// @code{.cpp}
    ///     container::Dictionary dictionary = ... ;
    ///     OPM opm = OPM::Dictionary(dictionary) ;
    /// @endcode
    ///
    /// @param aDictionary A dictionary.
    /// @return An OPM.
    static OPM Dictionary(const container::Dictionary& aDictionary);

    /// @brief Parse an OPM from a string.
    ///
    /// @code{.cpp}
    ///     String opmString = ... ;
    ///     OPM opm = OPM::Parse(opmString) ;
    /// @endcode
    ///
    /// @param aString A string.
    /// @return An OPM.
    static OPM Parse(const String& aString);

    /// @brief Load an OPM from a file.
    ///
    /// @code{.cpp}
    ///     File file = File::Path(Path::Parse("/path/to/opm.yaml")) ;
    ///     OPM opm = OPM::Load(file) ;
    /// @endcode
    ///
    /// @param aFile A file.
    /// @return An OPM.
    static OPM Load(const File& aFile);

   private:
    OPM::Header header_;
    Array<OPM::Deployment> deployments_;
};

}  // namespace spacex
}  // namespace message
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
