/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Shared;
using ostk::core::types::Size;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Instant;

/// @brief                      Trajectory state

class State
{
   public:
    /// @brief              Constructor.
    ///
    /// @param              [in] anInstant An instant
    /// @param              [in] aCoordinates The {cartesian-position, cartesian-velocity} coordinates at the instant
    /// in International System of Units
    /// @param              [in] aFrameSPtr The reference frame in which the coordinates are referenced to and resolved
    /// in

    State(const Instant& anInstant, const VectorXd& aCoordinates, const Shared<const Frame>& aFrameSPtr);

    State(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity);

    bool operator==(const State& aState) const;

    bool operator!=(const State& aState) const;

    State operator+(const State& aState) const;

    State operator-(const State& aState) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const State& aState);

    bool isDefined() const;

    const Instant& accessInstant() const;

    const Shared<const Frame> accessFrame() const;

    const VectorXd& accessCoordinates() const;

    Instant getInstant() const;

    Shared<const Frame> getFrame() const;

    Position getPosition() const;

    Velocity getVelocity() const;

    VectorXd getCoordinates() const;

    State inFrame(const Shared<const Frame>& aFrameSPtr) const;

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    static State Undefined();

   private:
    Instant instant_;
    VectorXd coordinates_;
    Shared<const Frame> frameSPtr_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
