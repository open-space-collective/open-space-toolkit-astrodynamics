/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort__

#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMean.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{
namespace blm
{

using ostk::core::ctnr::Pair;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::object::Vector6d;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using classicalOE = ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMean;

/// @brief                      Brouwer-Lyddane Mean Short Orbital Elements. Short periodic variations are averaged.
///
/// @ref
/// https://space.stackexchange.com/questions/22151/whats-a-brouwer-lyddane-mean-semi-major-axis-or-any-other-for-an-orbit-in-a-l

class BrouwerLyddaneMeanShort : public BrouwerLyddaneMean
{
   public:
    /// @brief                  Constructor
    ///
    /// @param                  [in] aSemiMajorAxis A semi-major axis
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] anInclination An inclination
    /// @param                  [in] aRaan A raan
    /// @param                  [in] anAop An aop
    /// @param                  [in] aMeanAnomaly A mean anomaly

    BrouwerLyddaneMeanShort(
        const Length &aSemiMajorAxis,
        const Real &anEccentricity,
        const Angle &anInclination,
        const Angle &aRaan,
        const Angle &anAop,
        const Angle &aMeanAnomaly
    );

    /// @brief                  Construct from a COE
    ///
    /// @param                  [in] aCOE A COE
    /// @return                 Brouwer-Lyddane Mean Short

    static BrouwerLyddaneMeanShort COE(const classicalOE &aCOE);

    /// @brief                  Convert Brouwer-Lyddane Mean Short to COE
    ///
    /// @return                 COE

    virtual classicalOE toCOE() const override;

    /// @brief                  Constructor
    ///
    /// @param                  [in] aCartesianState A cartesian state
    /// @param                  [in] aGravitationalParameter A gravitational parameter

    static BrouwerLyddaneMeanShort Cartesian(
        const COE::CartesianState &aCartesianState, const Derived &aGravitationalParameter
    );

    /// @brief                  Construct an undefined BrouwerLyddaneMeanShort
    ///
    /// @return                 Undefined BrouwerLyddaneMeanLong

    static BrouwerLyddaneMeanShort Undefined();

   private:
    /// @brief                  Constructor
    ///
    /// @param                  [in] aVector A vector

    static BrouwerLyddaneMeanShort FromSIVector(const Vector6d &aVector);
};

}  // namespace blm
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
