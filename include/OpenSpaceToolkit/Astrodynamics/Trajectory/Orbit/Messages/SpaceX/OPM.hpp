/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Messages_SpaceX_OPM__

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
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace messages
{
namespace spacex
{

namespace ctnr = ostk::core::container;

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

using ostk::astro::trajectory::State;

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
    struct Header
    {
        Instant generationDate;
        Instant launchDate;
    };

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

        State toState() const;
    };

    OPM(const OPM::Header& aHeader, const Array<OPM::Deployment>& aDeploymentArray);

    friend std::ostream& operator<<(std::ostream& anOutputStream, const OPM& anOPM);

    bool isDefined() const;

    OPM::Header getHeader() const;

    Array<OPM::Deployment> getDeployments() const;

    OPM::Deployment getDeploymentAt(const Index& anIndex) const;

    OPM::Deployment getDeploymentWithName(const String& aName) const;

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    static OPM Undefined();

    static OPM Dictionary(const ctnr::Dictionary& aDictionary);

    static OPM Parse(const String& aString);

    static OPM Load(const File& aFile);

   private:
    OPM::Header header_;
    Array<OPM::Deployment> deployments_;
};

}  // namespace spacex
}  // namespace messages
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
