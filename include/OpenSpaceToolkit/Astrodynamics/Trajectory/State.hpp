/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::Size;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::CoordinatesSubset;
using ostk::astro::trajectory::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::coordinatessubsets::CartesianVelocity;

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
    /// @param              [in] aCoordinatesBrokerSPtr The coordinates broker associated to the coordinates vector

    State(
        const Instant& anInstant,
        const VectorXd& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinatesBroker> aCoordinatesBrokerSPtr
    );

    State(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity);

    bool operator==(const State& aState) const;

    bool operator!=(const State& aState) const;

    State operator+(const State& aState) const;

    State operator-(const State& aState) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const State& aState);

    const Instant& accessInstant() const;

    const Shared<const Frame> accessFrame() const;

    const VectorXd& accessCoordinates() const;

    Instant getInstant() const;

    Shared<const Frame> getFrame() const;

    Position getPosition() const;

    Velocity getVelocity() const;

    VectorXd getCoordinates() const;

    bool isDefined() const;

    State inFrame(const Shared<const Frame>& aFrameSPtr) const;

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    static State Undefined();

    static State fromStdVector(
        const Instant& anInstant,
        const std::vector<double>& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinatesBroker> aCoordinatesBrokerSPtr
    );

   private:
    static const Shared<const CartesianPosition> CARTESIAN_POSITION;
    static const Shared<const CartesianVelocity> CARTESIAN_VELOCITY;
    static const Shared<const CoordinatesBroker> CARTESIAN_POSVEL_COORDINATES_BROKER;
    Instant instant_;
    VectorXd coordinates_;
    Shared<const Frame> frameSPtr_;
    Shared<const CoordinatesBroker> coordinatesBrokerSPtr_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
