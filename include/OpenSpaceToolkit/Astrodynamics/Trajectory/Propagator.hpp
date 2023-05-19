/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__

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
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::ctnr::Array;
using ostk::core::types::Integer;
using ostk::core::types::Real;

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astro::NumericalSolver;
using ostk::astro::trajectory::State;
using ostk::astro::flight::system::Dynamics;

/// @brief                      Defines a propagator to be used for numerical integration

class Propagator
{
   public:
    typedef std::function<void(const Dynamics::StateVector&, Dynamics::StateVector&, const double)>
        DynamicalEquationWrapper;
    /// @brief              Constructor
    ///
    /// @code
    ///                     Propagator propagator = { aDynamicsArray, aNumericalSolver } ;
    /// @endcode
    ///
    /// @param              [in] aDynamicsArray A dynamics array
    /// @param              [in] aNumericalSolver A numerical solver

    Propagator(const Array<Shared<Dynamics>>& aDynamicsArray, const NumericalSolver& aNumericalSolver);

    /// @brief              Clone propagator
    ///
    /// @return             Pointer to cloned propagator

    Propagator* clone() const;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aPropagator A propagator
    /// @return             True if propagators are equal

    bool operator==(const Propagator& aPropagator) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aPropagator A propagator
    /// @return             True if propagators are not equal

    bool operator!=(const Propagator& aPropagator) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aPropagator A propagator
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Propagator& aPropagator);

    /// @brief              Check if propagator is defined
    ///
    /// @return             True if propagator is defined

    bool isDefined() const;

    /// @brief              Calculate the state at an instant, given initial state
    /// @code
    ///                     State state = propagator.calculateStateAt(aState, anInstant) ;
    /// @endcode
    /// @param              [in] aState An initial state
    /// @param              [in] anInstant An instant
    /// @return             State

    void setDynamics(const Array<Shared<Dynamics>>& aDynamicsArray);

    Array<Shared<Dynamics>> getDynamics() const;

    void addDynamics(const Shared<Dynamics>& aDynamics);

    State calculateStateAt(const State& aState, const Instant& anInstant) const;

    /// @brief              Calculate the states at an array of instants, given an initial state
    /// @brief              Can only be used with sorted instant array
    /// @code
    ///                     Array<State> states = propagator.calculateStatesAt(aState, anInstantArray) ;
    /// @endcode
    /// @param              [in] anInstantArray An instant array
    /// @return             Array<State>

    Array<State> calculateStatesAt(const State& aState, const Array<Instant>& anInstantArray) const;

    /// @brief              Print propagator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    DynamicalEquationWrapper getDynamicalEquations(const Instant& anInstant) const;

    static void DynamicalEquations(
        const Dynamics::StateVector& x,
        Dynamics::StateVector& dxdt,
        const double t,
        const Array<Shared<Dynamics>>& dynamics,
        const Instant& anInstant
    );

   private:
    mutable Array<Shared<Dynamics>> dynamics_;
    mutable NumericalSolver numericalSolver_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
