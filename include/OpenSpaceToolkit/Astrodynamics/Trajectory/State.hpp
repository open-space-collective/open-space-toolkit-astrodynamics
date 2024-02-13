/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::container::Array;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Trajectory State
class State
{
   public:
    /// @brief Constructor with a pre-defined Coordinates Broker.
    ///
    /// @param anInstant An instant
    /// @param aCoordinates The coordinates at the instant in International System of Units
    /// @param aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param aCoordinateBrokerSPtr The coordinates broker associated to the coordinates
    State(
        const Instant& anInstant,
        const VectorXd& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr
    );

    /// @brief Constructor. This constructor makes a new Coordinates Broker under the hood for every
    /// State. When possible, users should prefer passing in an existing Coordinates Broker or using a StateBuilder to
    /// reduce memory footprint when constructing many states.
    ///
    /// @param anInstant An instant
    /// @param aCoordinates The coordinates at the instant in International System of Units
    /// @param aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param aCoordinateSubsetsArray The coordinates subsets associated to the coordinates
    State(
        const Instant& anInstant,
        const VectorXd& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
    );

    /// @brief Utility constructor for Position/Velocity/Attitude/Angular Velocity.
    ///
    /// @param anInstant An instant
    /// @param aPosition The Cartesian position at the instant
    /// @param aVelocity The Cartesian velocity at the instant
    /// @param anAttitude The attitude at the instant,
    /// representing the rotation required to go from the attitude reference frame to the satellite body frame
    /// @param anAngularVelocity The angular velocity at the instant, representing the angular
    /// velocity of the satellite body frame with respect to the attitude frame, expressed, in satellite body frame
    /// @param anAttitudeReferenceFrame The attitude reference frame
    State(
        const Instant& anInstant,
        const Position& aPosition,
        const Velocity& aVelocity,
        const Quaternion& anAttitude,
        const Vector3d& anAngularVelocity,
        const Shared<const Frame>& anAttitudeReferenceFrame
    );

    /// @brief Utility constructor for Position/Velocity only.
    ///
    /// @param anInstant An instant
    /// @param aPosition The cartesian position at the instant in International System of Units
    /// @param aVelocity The cartesian velocity at the instant in International System of Units
    State(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity);

    /// @brief Copy constructor.
    ///
    /// @param aState An existing State
    State(const State& aState);

    /// @brief Copy-assignment operator
    ///
    /// @param aState The State to copy
    /// @return The modified State
    State& operator=(const State& aState);

    /// @brief Equality operator.
    ///
    /// @param aState The State to compare to
    /// @return True if the States are equal, false otherwise
    bool operator==(const State& aState) const;

    /// @brief Inequality operator.
    ///
    /// @param aState The State to compare to
    /// @return True if the States are not equal, false otherwise
    bool operator!=(const State& aState) const;

    /// @brief Addition operator.
    ///
    /// @param aState The State to add to this State
    /// @return The sum of the two States
    State operator+(const State& aState) const;

    /// @brief Subtraction operator.
    ///
    /// @param aState The State to subtract from this State
    /// @return The difference between the two States
    State operator-(const State& aState) const;

    /// @brief Stream insertion operator.
    ///
    /// @param anOutputStream The output stream to insert into
    /// @param aState The State to insert
    /// @return The output stream with the State inserted
    friend std::ostream& operator<<(std::ostream& anOutputStream, const State& aState);

    /// @brief Check if the State is defined.
    ///
    /// @return True if the State is defined, false otherwise
    bool isDefined() const;

    /// @brief Accessor for the instant.
    ///
    /// @return The instant
    const Instant& accessInstant() const;

    /// @brief Accessor for the reference frame.
    ///
    /// @return The reference frame
    const Shared<const Frame> accessFrame() const;

    /// @brief Accessor for the coordinates.
    ///
    /// @return The coordinates
    const VectorXd& accessCoordinates() const;

    /// @brief Access the coordinates broker associated with the State.
    ///
    /// @return The coordinates broker associated to the State
    const Shared<const CoordinateBroker>& accessCoordinateBroker() const;

    /// @brief Get the size of the State.
    ///
    /// @return The size of the State
    Size getSize() const;

    /// @brief Get the instant associated with the State.
    ///
    /// @return The instant
    Instant getInstant() const;

    /// @brief Get the reference frame associated with the State.
    ///
    /// @return The reference frame
    Shared<const Frame> getFrame() const;

    /// @brief Get the coordinates of the State.
    /// @brief Get the cartesian position associated with the State (if present).
    ///
    /// @return The cartesian position
    Position getPosition() const;

    /// @brief Get the cartesian velocity associated with the State (if present).
    ///
    /// @return The cartesian velocity
    Velocity getVelocity() const;

    /// @brief Get the attitude quaternion associated with the State (if present).
    ///
    /// @return The attitude quaternion
    Quaternion getAttitude() const;

    /// @brief Get the angular velocity associated with the State (if present).
    ///
    /// @return The angular velocity
    Vector3d getAngularVelocity() const;

    /// @brief Get the coordinates of the State.
    ///
    /// @return The coordinates
    VectorXd getCoordinates() const;

    /// @brief Get the coordinates subsets of the State.
    ///
    /// @return The coordinates subsets
    const Array<Shared<const CoordinateSubset>> getCoordinateSubsets() const;

    /// @brief Check if the State has a given coordinates subset.
    ///
    /// @param aCoordinateSubsetSPtr the coordinates subset to be checked
    ///
    /// @return True if the coordinates subset is included in the State
    bool hasSubset(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr) const;

    /// @brief Extract the coordinates for a single subset.
    ///
    /// @param aSubsetSPtr The subset to extract the coordinates for
    /// @return The coordinates for the subset
    VectorXd extractCoordinate(const Shared<const CoordinateSubset>& aSubsetSPtr) const;

    /// @brief Extract the coordinates for multiple subsets.
    ///
    /// @param aCoordinateSubsetsArray The array of subsets to extract the coordinates for
    /// @return The coordinates for the subsets
    VectorXd extractCoordinates(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const;

    /// @brief Transform the State to a different reference frame.
    ///
    /// @param aFrameSPtr The reference frame to transform to
    /// @return The transformed State
    State inFrame(const Shared<const Frame>& aFrameSPtr) const;

    /// @brief Print the State to an output stream.
    ///
    /// @param anOutputStream The output stream to print to
    /// @param displayDecorator Whether or not to display the decorator
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Get an undefined State.
    ///
    /// @return An undefined State
    static State Undefined();

   private:
    Instant instant_;
    VectorXd coordinates_;
    Shared<const Frame> frameSPtr_;
    Shared<const CoordinateBroker> coordinatesBrokerSPtr_;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
