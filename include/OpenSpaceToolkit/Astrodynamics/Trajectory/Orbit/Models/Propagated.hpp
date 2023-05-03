// Copyright © Loft Orbital Solutions Inc.

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::dynamics::SatelliteDynamics;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::orbit::Model;

/// @brief                      Defines an orbit model that is propagated using numerical propagation

class Propagated : public ostk::astro::trajectory::orbit::Model
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     Propagated propagated = { aSatelliteDynamics, aNumericalSolver, aState } ;
    /// @endcode
    ///
    /// @param              [in] aSatelliteDynamics A satellite dynamics object
    /// @param              [in] aNumericalSolver A numerical solver
    /// @param              [in] aState A state

    Propagated(const SatelliteDynamics& aSatelliteDynamics,
               const NumericalSolver& aNumericalSolver,
               const State& aState);

    /// @brief              Constructor with additional option of passing in an existing array of states
    ///
    /// @code
    ///                     Propagated propagated = { aSatelliteDynamics, aNumericalSolver, aCachedStateArray } ;
    /// @endcode
    ///
    /// @param              [in] aSatelliteDynamics A satellite dynamics object
    /// @param              [in] aNumericalSolver A numerical solver
    /// @param              [in] aCachedStateArray A state array

    Propagated(const SatelliteDynamics& aSatelliteDynamics,
               const NumericalSolver& aNumericalSolver,
               const Array<State>& aCachedStateArray);

    /// @brief              Clone propagated
    ///
    /// @return             Pointer to cloned propagated

    virtual Propagated* clone() const override;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aPropagatedModel A propagated
    /// @return             True if propagateds are equal

    bool operator==(const Propagated& aPropagatedModel) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aPropagatedModel A propagated
    /// @return             True if propagateds are not equal

    bool operator!=(const Propagated& aPropagatedModel) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aPropagatedModel A propagated model
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Propagated& aPropagatedModel);

    /// @brief              Check if propagated model is defined
    ///
    /// @return             True if propagated model is defined

    virtual bool isDefined() const override;

    /// @brief              Get epoch (in this case it is the first instant in the cached state array)
    ///
    /// @code
    ///                     Instant instant = propagated.getEpoch() ;
    /// @endcode
    ///
    /// @return             Instant

    virtual Instant getEpoch() const override;

    /// @brief              Get revolution number at epoch (it is equal to 1)
    ///
    /// @code
    ///                     Real real = propagated.getRevolutionNumberAtEpoch() ;
    /// @endcode
    ///
    /// @return             Integer

    virtual Integer getRevolutionNumberAtEpoch() const override;

    /// @brief              Calculate the state at an instant, utilizing internal cached state array to propagated
    /// shortest amount of time
    /// @brief              Does not have macro-level sorting optimization, should not be used with disorded instant
    /// array
    /// @code
    ///                     State state = propagated.calculateStateAt(anInstant) ;
    /// @endcode
    /// @param              [in] anInstant An instant
    /// @return             State

    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief              Calculate the state at an instant, given initial state
    /// @code
    ///                     State state = propagated.calculateStateAt(aState, anInstant) ;
    /// @endcode
    /// @param              [in] aState An initial state
    /// @param              [in] anInstant An instant
    /// @return             State

    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const override;

    /// @brief              Calculate the revolution number at an instant
    ///
    /// @code
    ///                     Integer integer = propagated.calculateRevolutionNumberAt(anInstant) ;
    /// @endcode
    /// @param              [in] anInstant An instant
    /// @return             Integer

    virtual Integer calculateRevolutionNumberAt(const Instant& anInstant) const override;

    /// @brief              Fetch internal cached state array
    ///
    /// @code
    ///                     Array<State> stateArray = propagated.accessCachedStateArray() ;
    /// @endcode

    /// @return             Array<State>&

    const Array<State>& accessCachedStateArray() const;

    /// @brief              Access propagator
    ///
    /// @code
    ///                     Propagator propagator = propagated.accessPropagator() ;
    /// @endcode
    ///
    /// @return             Propagator

    const Propagator& accessPropagator() const;

    /// @brief              Set internal cached state array manually
    ///
    /// @code
    ///                     Array<State> stateArray = { ... } ;
    ///                     propagated.setCachedStateArray(stateArray) ;
    /// @endcode
    /// @param              [in] aStateArray A state array

    void setCachedStateArray(const Array<State>& aStateArray);

    /// @brief              Print propagated
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   protected:
    /// @brief              Equal to operator
    ///
    /// @param              [in] aModel A model
    /// @return             True if models are equal

    virtual bool operator==(const trajectory::Model& aModel) const override;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aModel A model
    /// @return             True if models are not equal

    virtual bool operator!=(const trajectory::Model& aModel) const override;

   private:
    Propagator propagator_;
    mutable Array<State> cachedStateArray_;

    void sanitizeCachedArray() const;
};

}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
