/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::Integer;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::container::Array;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Factory class to generate States with common reference frame and coordinates subsets
class StateBuilder
{
   public:
    /// @brief Constructor.
    ///
    /// @param aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param aCoordinateSubsetsArray The array of coordinates subsets defining the output
    /// States
    StateBuilder(
        const Shared<const Frame>& aFrameSPtr, const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
    );

    /// @brief Constructor.
    ///
    /// @param aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param aCoordinateBroker Shared pointer to an existing Coordinates Broker
    StateBuilder(const Shared<const Frame>& aFrameSPtr, const Shared<const CoordinateBroker>& aCoordinateBrokerSPtr);

    /// @brief Constructor.
    ///
    /// @param aState The state to be used as a template
    StateBuilder(const State& aState);

    /// @brief Equality operator.
    ///
    /// @param aStateBuilder The StateBuilder to compare to
    /// @return True if the  StateBuilders are equal, false otherwise
    bool operator==(const StateBuilder& aStateBuilder) const;

    /// @brief Inequality operator.
    ///
    /// @param aStateBuilder The StateBuilder to compare to
    /// @return True if the  StateBuilders are not equal, false otherwise
    bool operator!=(const StateBuilder& aStateBuilder) const;

    /// @brief Return a new StateBuilder with the additional CoordinateSubset.
    ///
    /// @param aCoordinateSubsetSPtr The CoordinateSubset to append
    ///
    /// @return A new StateBuilder
    const StateBuilder operator+(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr) const;

    /// @brief Return a new StateBuilder without the given CoordinateSubset.
    ///
    /// @param aCoordinateSubsetSPtr The CoordinateSubset to remove
    ///
    /// @return A new StateBuilder
    const StateBuilder operator-(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr) const;

    /// @brief Stream insertion operator.
    ///
    /// @param anOutputStream The output stream to insert into
    /// @param aStateBuilder The StateBuilder to insert
    /// @return The output stream with the StateBuilder inserted
    friend std::ostream& operator<<(std::ostream& anOutputStream, const StateBuilder& aStateBuilder);

    /// @brief Check if the StateBuilder is defined.
    ///
    /// @return True if the StateBuilder is defined, false otherwise
    bool isDefined() const;

    /// @brief Produce a State linked to the Frame and Coordinates Broker of the StateBuilder.
    ///
    /// @return A State linked to the Frame and Coordinates Broker of the StateBuilder
    const State build(const Instant& anInstant, const VectorXd& aCoordinates) const;

    /// @brief Produce a State with the CoordinateSubsets specified by the StateBuilder.
    ///
    /// @param aState the state from which the coordinates will be taken.
    /// @return A State with the CoordinateSubsets of the StateBuilder.
    const State reduce(const State& aState) const;

    /// @brief Produce a State with the CoordinateSubsets specified by the StateBuilder.
    ///
    /// @param aState the state from which the coordinates will be taken.
    /// @param defaultState the state from which missing coordinates will be taken.
    /// @return A State with the CoordinateSubsets of the StateBuilder.
    const State expand(const State& aState, const State& defaultState) const;

    /// @brief Accessor for the reference frame.
    ///
    /// @return The reference frame
    const Shared<const Frame> accessFrame() const;

    /// @brief Access the coordinates broker associated with the  StateBuilder.
    ///
    /// @return The coordinates broker associated to the State
    const Shared<const CoordinateBroker>& accessCoordinateBroker() const;

    /// @brief Get the reference frame associated with the  StateBuilder.
    ///
    /// @return The reference frame
    Shared<const Frame> getFrame() const;

    /// @brief Get the coordinates subsets of the  StateBuilder.
    ///
    /// @return The coordinates subsets
    const Array<Shared<const CoordinateSubset>> getCoordinateSubsets() const;

    /// @brief Print the StateBuilder to an output stream.
    ///
    /// @param anOutputStream The output stream to print to
    /// @param displayDecorator Whether or not to display the decorator
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Get an undefined StateBuilder.
    ///
    /// @return An undefined StateBuilder
    static StateBuilder Undefined();

   private:
    Shared<const Frame> frameSPtr_;
    Shared<const CoordinateBroker> coordinatesBrokerSPtr_;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
