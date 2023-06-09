/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_GravitationalDynamics__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_GravitationalDynamics__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Vector.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
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

using ostk::core::ctnr::Array;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;

using ostk::physics::Environment;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::data::Vector;
using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::State;

/// @brief                      Defined the acceleration experienced by a point mass due to gravity

class GravitationalDynamics : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const Celestial = { ... };
    ///                     GravitationalDynamics gravitationalDynamics = { aCelestial };
    /// @endcode
    ///
    /// @param              [in] aCelestial A Celestial Object

    GravitationalDynamics(const Celestial& aCelestial);

    /// @brief              Copy Constructor
    ///
    /// @param              [in] GravitationalDynamics A gravitational dynamics

    GravitationalDynamics(const GravitationalDynamics& aGravitationalDynamics);

    /// @brief              Destructor

    virtual ~GravitationalDynamics() override;

    /// @brief              Clone gravitational dynamics
    ///
    /// @return             Pointer to cloned gravitational dynamics

    virtual GravitationalDynamics* clone() const override;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aGravitationalDynamics A gravitational dynamics
    /// @return             True if gravitational dynamics are equal

    bool operator==(const GravitationalDynamics& aGravitationalDynamics) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aGravitationalDynamics A gravitational dynamics
    /// @return             True if gravitational dynamics are not equal

    bool operator!=(const GravitationalDynamics& aGravitationalDynamics) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aGravitationalDynamics A gravitational dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const GravitationalDynamics& aGravitationalDynamics);

    /// @brief              Check if gravitational dynamics is defined
    ///
    /// @return             True if gravitational dynamics is defined

    virtual bool isDefined() const override;

    /// @brief              Print gravitational dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    virtual void update(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant) override;

   private:
    Celestial celestial_ = Celestial::Undefined();
};

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
