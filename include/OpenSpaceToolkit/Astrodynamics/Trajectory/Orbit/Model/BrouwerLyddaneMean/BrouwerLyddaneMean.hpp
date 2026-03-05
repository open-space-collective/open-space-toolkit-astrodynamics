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
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     BrouwerLyddaneMean blm = {
    ///         Length::Kilometers(7000.0), 0.001,
    ///         Angle::Degrees(97.5), Angle::Degrees(0.0),
    ///         Angle::Degrees(0.0), Angle::Degrees(0.0)
    ///     } ;
    /// @endcode
    ///
    /// @param aSemiMajorAxis A semi-major axis.
    /// @param anEccentricity An eccentricity.
    /// @param anInclination An inclination.
    /// @param aRaan A right ascension of ascending node.
    /// @param anAop An argument of periapsis.
    /// @param aMeanAnomaly A mean anomaly.
    BrouwerLyddaneMean(
        const Length &aSemiMajorAxis,
        const Real &anEccentricity,
        const Angle &anInclination,
        const Angle &aRaan,
        const Angle &anAop,
        const Angle &aMeanAnomaly
    );

    /// @brief Get the mean anomaly.
    ///
    /// @code{.cpp}
    ///     BrouwerLyddaneMean blm = { ... } ;
    ///     Angle meanAnomaly = blm.getMeanAnomaly() ;
    /// @endcode
    ///
    /// @return The mean anomaly.
    virtual Angle getMeanAnomaly() const override;

    /// @brief Get the true anomaly.
    ///
    /// @code{.cpp}
    ///     BrouwerLyddaneMean blm = { ... } ;
    ///     Angle trueAnomaly = blm.getTrueAnomaly() ;
    /// @endcode
    ///
    /// @return The true anomaly.
    virtual Angle getTrueAnomaly() const override;

    /// @brief Get the eccentric anomaly.
    ///
    /// @code{.cpp}
    ///     BrouwerLyddaneMean blm = { ... } ;
    ///     Angle eccentricAnomaly = blm.getEccentricAnomaly() ;
    /// @endcode
    ///
    /// @return The eccentric anomaly.
    virtual Angle getEccentricAnomaly() const override;

    /// @brief Get the cartesian state (position, velocity) from the mean elements.
    ///
    /// @code{.cpp}
    ///     BrouwerLyddaneMean blm = { ... } ;
    ///     Derived mu = Derived(3.986004418e14, Derived::Unit::GravitationalParameter()) ;
    ///     COE::CartesianState cartesianState = blm.getCartesianState(mu, Frame::GCRF()) ;
    /// @endcode
    ///
    /// @param aGravitationalParameter A gravitational parameter.
    /// @param aFrameSPtr A frame.
    /// @return Cartesian state (position, velocity pair).
    COE::CartesianState getCartesianState(const Derived &aGravitationalParameter, const Shared<const Frame> &aFrameSPtr)
        const;

    /// @brief Convert to classical orbital elements (COE).
    ///
    /// @code{.cpp}
    ///     BrouwerLyddaneMeanLong blmLong = { ... } ;
    ///     COE coe = blmLong.toCOE() ;
    /// @endcode
    ///
    /// @return Classical orbital elements.
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
