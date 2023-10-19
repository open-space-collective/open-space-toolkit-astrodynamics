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

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::ctnr::Array;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

/// @brief                      Trajectory State

class State
{
   public:
    /// @brief                  Constructor with a pre-defined Coordinates Broker.
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aCoordinates The coordinates at the instant in International System of Units
    /// @param                  [in] aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param                  [in] aCoordinatesBrokerSPtr The coordinates broker associated to the coordinates

    State(
        const Instant& anInstant,
        const VectorXd& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
    );

    /// @brief                  Constructor. This constructor makes a new Coordinates Broker under the hood for every
    /// State. When possible, users should prefer passing in an existing Coordinates Broker or using a StateBuilder to
    /// reduce memory footprint when constructing many states.
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aCoordinates The coordinates at the instant in International System of Units
    /// @param                  [in] aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param                  [in] aCoordinatesSubsetsArray The coordinates subsets associated to the coordinates

    State(
        const Instant& anInstant,
        const VectorXd& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray
    );

    /// @brief                  Utility constructor for Position/Velocity only.
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aPosition The cartesian position at the instant in International System of Units
    /// @param                  [in] aVelocity The cartesian velocity at the instant in International System of Units

    State(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity);

    /// @brief                  Copy constructor.
    ///
    /// @param                  [in] aState An existing State

    State(const State& aState);

    /// @brief                  Copy-assignment operator
    ///
    /// @param                  [in] aState The State to copy
    /// @return                 The modified State

    State& operator=(const State& aState);

    /// @brief                  Equality operator.
    ///
    /// @param                  [in] aState The State to compare to
    /// @return                 True if the States are equal, false otherwise

    bool operator==(const State& aState) const;

    /// @brief                  Inequality operator.
    ///
    /// @param                  [in] aState The State to compare to
    /// @return                 True if the States are not equal, false otherwise

    bool operator!=(const State& aState) const;

    /// @brief                  Addition operator.
    ///
    /// @param                  [in] aState The State to add to this State
    /// @return                 The sum of the two States

    State operator+(const State& aState) const;

    /// @brief                  Subtraction operator.
    ///
    /// @param                  [in] aState The State to subtract from this State
    /// @return                 The difference between the two States

    State operator-(const State& aState) const;

    /// @brief                  Stream insertion operator.
    ///
    /// @param                  [in] anOutputStream The output stream to insert into
    /// @param                  [in] aState The State to insert
    /// @return                 The output stream with the State inserted

    friend std::ostream& operator<<(std::ostream& anOutputStream, const State& aState);

    /// @brief                  Check if the State is defined.
    ///
    /// @return                 True if the State is defined, false otherwise

    bool isDefined() const;

    /// @brief                  Accessor for the instant.
    ///
    /// @return                 The instant

    const Instant& accessInstant() const;

    /// @brief                  Accessor for the reference frame.
    ///
    /// @return                 The reference frame

    const Shared<const Frame> accessFrame() const;

    /// @brief                  Accessor for the coordinates.
    ///
    /// @return                 The coordinates

    const VectorXd& accessCoordinates() const;

    /// @brief                  Access the coordinates broker associated with the State.
    ///
    /// @return                 The coordinates broker associated to the State

    const Shared<const CoordinatesBroker>& accessCoordinatesBroker() const;

    /// @brief                  Get the size of the State.
    ///
    /// @return                 The size of the State

    Size getSize() const;

    /// @brief                  Get the instant associated with the State.
    ///
    /// @return                 The instant

    Instant getInstant() const;

    /// @brief                  Get the reference frame associated with the State.
    ///
    /// @return                 The reference frame

    Shared<const Frame> getFrame() const;

    /// @brief                  Get the cartesian position associated with the State (if present).
    ///
    /// @return                 The cartesian position

    Position getPosition() const;

    /// @brief                  Get the cartesian velocity associated with the State (if present).
    ///
    /// @return                 The cartesian velocity

    Velocity getVelocity() const;

    /// @brief                  Get the coordinates of the State.
    ///
    /// @return                 The coordinates

    VectorXd getCoordinates() const;

    /// @brief                  Get the coordinates subsets of the State.
    ///
    /// @return                 The coordinates subsets

    const Array<Shared<const CoordinatesSubset>> getCoordinatesSubsets() const;

    /// @brief                  Check if the State has a given coordinates subset.
    ///
    /// @param                  [in] aCoordinatesSubsetSPtr the coordinates subset to be checked
    ///
    /// @return                 True if the coordinates subset is included in the State

    bool hasSubset(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const;

    /// @brief                  Extract the coordinates for a single subset.
    ///
    /// @param                  [in] aSubsetSPtr The subset to extract the coordinates for
    /// @return                 The coordinates for the subset

    VectorXd extractCoordinates(const Shared<const CoordinatesSubset>& aSubsetSPtr) const;

    /// @brief Extract the coordinates for multiple subsets.
    ///
    /// @param [in] aCoordinatesSubsetsArray The array of subsets to extract the coordinates for
    /// @return The coordinates for the subsets

    VectorXd extractCoordinates(const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray) const;

    /// @brief Transform the State to a different reference frame.
    ///
    /// @param [in] aFrameSPtr The reference frame to transform to
    /// @return The transformed State

    State inFrame(const Shared<const Frame>& aFrameSPtr) const;

    /// @brief Print the State to an output stream.
    ///
    /// @param [in] anOutputStream The output stream to print to
    /// @param [in] displayDecorator Whether or not to display the decorator

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Get an undefined State.
    ///
    /// @return An undefined State

    static State Undefined();

   private:
    Instant instant_;
    VectorXd coordinates_;
    Shared<const Frame> frameSPtr_;
    Shared<const CoordinatesBroker> coordinatesBrokerSPtr_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
