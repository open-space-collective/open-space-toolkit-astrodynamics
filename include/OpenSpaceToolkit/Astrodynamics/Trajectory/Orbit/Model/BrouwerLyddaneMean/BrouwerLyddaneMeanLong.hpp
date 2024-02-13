/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_BrouwerLyddaneMeanLong__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_BrouwerLyddaneMeanLong__

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

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMean.hpp>
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

using classicalOE = ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMean;

/// @brief Brouwer-Lyddane Mean Orbital Elements. Short and/or secular periodic variations are
/// averaged.
///
/// @ref
/// https://space.stackexchange.com/questions/22151/whats-a-brouwer-lyddane-mean-semi-major-axis-or-any-other-for-an-orbit-in-a-l
class BrouwerLyddaneMeanLong : public BrouwerLyddaneMean
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
    BrouwerLyddaneMeanLong(
        const Length &aSemiMajorAxis,
        const Real &anEccentricity,
        const Angle &anInclination,
        const Angle &aRaan,
        const Angle &anAop,
        const Angle &aMeanAnomaly
    );

    /// @brief Construct from a COE
    ///
    /// @param aCOE A COE
    /// @return Brouwer-Lyddane Mean Short
    static BrouwerLyddaneMeanLong COE(const classicalOE &aCOE);

    /// @brief Convert BrouwerLyddaneMeanLong to COE
    ///
    /// @return COE
    virtual classicalOE toCOE() const override;

    /// @brief Constructor
    ///
    /// @param aCartesianState A cartesian state
    /// @param aGravitationalParameter A gravitational parameter
    static BrouwerLyddaneMeanLong Cartesian(
        const COE::CartesianState &aCartesianState, const Derived &aGravitationalParameter
    );

    /// @brief Construct an undefined BrouwerLyddaneMeanLong
    ///
    /// @return Undefined BrouwerLyddaneMeanLong
    static BrouwerLyddaneMeanLong Undefined();

   private:
    /// @brief Constructor
    ///
    /// @param aVector A vector
    static BrouwerLyddaneMeanLong FromSIVector(const Vector6d &aVector);
};

}  // namespace blm
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
