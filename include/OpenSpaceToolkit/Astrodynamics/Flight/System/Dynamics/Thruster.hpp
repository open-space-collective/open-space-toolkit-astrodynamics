/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{

using ostk::core::types::Real;

using ostk::math::obj::Vector3d;
using ostk::math::geom::d3::trf::rot::RotationMatrix;

using ostk::physics::data::Direction;
using ostk::physics::time::Instant;
using ostk::physics::units::Mass;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::State;

/// @brief                      Define the acceleration experienced by a point mass due to thrust

class Thruster : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @param              [in] aName A name

    Thruster(const SatelliteSystem& aSatelliteSystem, const String& aName = String::Empty());

    /// @brief              Destructor

    virtual ~Thruster() = 0;

    /// @brief              Check if thruster is defined
    ///
    /// @return             True if thruster is defined

    virtual bool isDefined() const = 0;

    /// @brief              Get name
    ///
    /// @return             Name of thruster

    String getName() const;

    /// @brief              Get satellite system
    ///
    /// @return             Satellite system

    SatelliteSystem getSatelliteSystem() const;

    /// @brief              Print thruster
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

   private:
    const String name_;
    SatelliteSystem satelliteSystem_;  // TBI: Might want to use PropulsionSystem instead and populate it under the hood
                                       // for constant thrust case
};

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
