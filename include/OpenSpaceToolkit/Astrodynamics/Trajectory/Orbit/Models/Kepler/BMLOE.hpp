/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE__

#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

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
namespace kepler
{

using ostk::core::ctnr::Pair;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using Vector6d = Eigen::Matrix<double, 1, 6>;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::trajectory::orbit::models::kepler::COE;

/// @brief                      Brouwer Mean Long Orbital Elements (BMLOE)
///
/// TBI: add ref

class BMLOE
{
   public:
    BMLOE(
        const Length &aSemiMajorAxis,
        const Real &anEccentricity,
        const Angle &anInclination,
        const Angle &aRaan,
        const Angle &anAop,
        const Angle &aMeanAnomaly
    );

    // bool operator==(const BMLOE &aBMLOE) const;

    // bool operator!=(const BMLOE &aBMLOE) const;

    // friend std::ostream &operator<<(std::ostream &anOutputStream, const COE &aCOE);

    // bool isDefined() const;

    // Length getSemiMajorAxis() const;

    // Real getEccentricity() const;

    // Angle getInclination() const;

    // Angle getRaan() const;

    // Angle getAop() const;

    // Angle getMeanAnomaly() const;

    // Angle getEccentricAnomaly() const;

    // Length getPeriapsisRadius() const;

    // Length getApoapsisRadius() const;

    // Derived getMeanMotion(const Derived &aGravitationalParameter) const;

    // Duration getOrbitalPeriod(const Derived &aGravitationalParameter) const;

    // COE::CartesianState getCartesianState(const Derived &aGravitationalParameter, const Shared<const Frame>
    // &aFrameSPtr) const;

    Vector6d asVector() const;

    // void print(std::ostream &anOutputStream, bool displayDecorator = true) const;

    // static BMLOE Undefined();

    COE toCOE() const;

    static BMLOE Cartesian(const COE::CartesianState &aCartesianState, const Derived &aGravitationalParameter);

    static BMLOE FromVector(const Vector6d &aCOEVector);

   private:
    Length semiMajorAxis_;
    Real eccentricity_;
    Angle inclination_;
    Angle raan_;
    Angle aop_;
    Angle meanAnomaly_;
};

}  // namespace kepler
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
