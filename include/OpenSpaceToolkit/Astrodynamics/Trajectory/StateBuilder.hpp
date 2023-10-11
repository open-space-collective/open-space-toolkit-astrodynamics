/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Map.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Integer;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::ctnr::Array;
using ostk::core::ctnr::Map;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

/// @brief                      Factory class to generate States with common reference frame and coordinates subsets

class StateBuilder
{
   public:
    /// @brief                  Constructor.
    ///
    /// @param                  [in] aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param                  [in] aCoordinatesSubsetsArray The array of coordinates subsets defining the output
    /// States

    StateBuilder(
        const Shared<const Frame>& aFrameSPtr, const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray
    );

    /// @brief                  Constructor.
    ///
    /// @param                  [in] aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param                  [in] aCoordinatesBroker Shared pointer to an existing Coordinates Broker

    StateBuilder(const Shared<const Frame>& aFrameSPtr, const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr);

    /// @brief                  Equality operator.
    ///
    /// @param                  [in] aStateBuilder The StateBuilder to compare to
    /// @return                 True if the  StateBuilders are equal, false otherwise

    bool operator==(const StateBuilder& aStateBuilder) const;

    /// @brief                  Inequality operator.
    ///
    /// @param                  [in] aStateBuilder The StateBuilder to compare to
    /// @return                 True if the  StateBuilders are not equal, false otherwise

    bool operator!=(const StateBuilder& aStateBuilder) const;

    /// @brief                  Stream insertion operator.
    ///
    /// @param                  [in] anOutputStream The output stream to insert into
    /// @param                  [in] aStateBuilder The StateBuilder to insert
    /// @return                 The output stream with the StateBuilder inserted

    friend std::ostream& operator<<(std::ostream& anOutputStream, const StateBuilder& aStateBuilder);

    /// @brief                  Check if the StateBuilder is defined.
    ///
    /// @return                 True if the StateBuilder is defined, false otherwise

    bool isDefined() const;

    /// @brief                  Produce a State linked to the Frame and Coordinates Broker of the StateBuilder.
    ///
    /// @return                 A State linked to the Frame and Coordinates Broker of the StateBuilder

    const State buildState(const Instant& anInstant, const VectorXd& aCoordinates) const;

    const State build(
        const State& aState,
        const Map<const Shared<const CoordinatesSubset>, const VectorXd>& anAdditionalCoordinatesMap
    ) const;

    /// @brief                  Accessor for the reference frame.
    ///
    /// @return                 The reference frame

    const Shared<const Frame> accessFrame() const;

    /// @brief                  Access the coordinates broker associated with the  StateBuilder.
    ///
    /// @return                 The coordinates broker associated to the State

    const Shared<const CoordinatesBroker>& accessCoordinatesBroker() const;

    /// @brief                  Get the reference frame associated with the  StateBuilder.
    ///
    /// @return                 The reference frame

    Shared<const Frame> getFrame() const;

    /// @brief                  Get the coordinates subsets of the  StateBuilder.
    ///
    /// @return                 The coordinates subsets

    const Array<Shared<const CoordinatesSubset>> getCoordinatesSubsets() const;

    /// @brief                  Print the StateBuilder to an output stream.
    ///
    /// @param                  [in] anOutputStream The output stream to print to
    /// @param                  [in] displayDecorator Whether or not to display the decorator

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Return a new StateBuilder with the additional CoordinatesSubset.
    ///
    /// @param                  [in] aCoordinatesSubsetSPtr The CoordinatesSubset to append
    ///
    /// @return                 A new StateBuilder

    const StateBuilder expand(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const;

    /// @brief                   Return a new StateBuilder without the given CoordinatesSubset.
    ///
    /// @param                  [in] aCoordinatesSubsetSPtr The CoordinatesSubset to remove
    ///
    /// @return                 A new StateBuilder

    const StateBuilder contract(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const;

    /// @brief                  Get an undefined StateBuilder.
    ///
    /// @return                 An undefined StateBuilder

    static StateBuilder Undefined();

   private:
    Shared<const Frame> frameSPtr_;
    Shared<const CoordinatesBroker> coordinatesBrokerSPtr_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
