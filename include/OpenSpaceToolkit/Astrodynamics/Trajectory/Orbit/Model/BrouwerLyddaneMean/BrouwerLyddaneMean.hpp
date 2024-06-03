/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_BrouwerLyddaneMean_BrouwerLyddaneMean__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_BrouwerLyddaneMean_BrouwerLyddaneMean__

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{
namespace blm
{

using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector6d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

/// @brief Brouwer-Lyddane Mean Orbital Elements. Short and/or secular periodic variations are
/// averaged.
///
/// @ref
/// https://space.stackexchange.com/questions/22151/whats-a-brouwer-lyddane-mean-semi-major-axis-or-any-other-for-an-orbit-in-a-l
class BrouwerLyddaneMean : public COE
{
   public:
    /// @brief Constructor
    ///
    /// @param aSemiMajorAxis A semi-major axis
    /// @param anEccentricity An eccentricity
    /// @param anInclination An inclination
    /// @param aRaan A raan
    /// @param anAop An aop
    /// @param aMeanAnomaly A mean anomaly
    BrouwerLyddaneMean(
        const Length &aSemiMajorAxis,
        const Real &anEccentricity,
        const Angle &anInclination,
        const Angle &aRaan,
        const Angle &anAop,
        const Angle &aMeanAnomaly
    );

    /// @brief Get Mean anomaly
    ///
    /// @return Mean anomaly
    virtual Angle getMeanAnomaly() const override;

    /// @brief Get True anomaly
    ///
    /// @return True anomaly
    virtual Angle getTrueAnomaly() const override;

    /// @brief Get Eccentric anomaly
    ///
    /// @return Eccentric anomaly
    virtual Angle getEccentricAnomaly() const override;

    /// @brief Get cartesian state
    ///
    /// @param aGravitationalParameter A gravitational parameter
    /// @param aFrameSPtr A frame
    /// @return Cartesian state
    COE::CartesianState getCartesianState(const Derived &aGravitationalParameter, const Shared<const Frame> &aFrameSPtr)
        const;

    /// @brief Convert to COE
    ///
    /// @return COE
    virtual COE toCOE() const = 0;

   protected:
    /// @brief Convert cartesian state to Vector
    ///
    /// @param aCartesianState A cartesian state
    /// @param aGravitationalParameter A gravitational parameter
    /// @param toCOEVector A converting function
    static Vector6d Cartesian(
        const COE::CartesianState &aCartesianState,
        const Derived &aGravitationalParameter,
        std::function<Vector6d(const Vector6d &)> toCOEVector
    );
};

}  // namespace blm
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
