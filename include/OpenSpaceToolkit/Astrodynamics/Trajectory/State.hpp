/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::CoordinatesSubset;

/// @brief                      Trajectory state

class State
{
   public:
    State(
        const Instant& anInstant,
        const VectorXd& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinatesBroker> aCoordinatesBroker
    );

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

    const Position accessPosition() const;

    const Velocity accessVelocity() const;

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
    Shared<const CoordinatesBroker> broker_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
