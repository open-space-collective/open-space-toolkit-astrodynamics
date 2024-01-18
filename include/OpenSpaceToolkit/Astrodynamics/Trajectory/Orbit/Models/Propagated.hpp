/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

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
using ostk::core::types::Integer;
using ostk::core::types::Real;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::orbit::Model;
using ostk::astro::Dynamics;

/// @brief Define an orbit model that is propagated using numerical propagation
class Propagated : public ostk::astro::trajectory::orbit::Model
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              Propagated propagated = { aPropagator, aState } ;
    /// @endcode
    ///
    /// @param aPropagator A propagator
    /// @param aState A state
    Propagated(const Propagator& aPropagator, const State& aState);

    /// @brief Constructor with a cached state array
    ///
    /// @code{.cpp}
    ///              Propagated propagated = { aPropagator, aCachedStateArray } ;
    /// @endcode
    ///
    /// @param aPropagator A propagator
    /// @param aCachedStateArray A state array
    Propagated(const Propagator& aPropagator, const Array<State>& aCachedStateArray);

    /// @brief Clone propagated
    ///
    /// @return Pointer to cloned propagated
    virtual Propagated* clone() const override;

    /// @brief Equal to operator
    ///
    /// @param aPropagatedModel A propagated
    /// @return True if propagateds are equal
    bool operator==(const Propagated& aPropagatedModel) const;

    /// @brief Not equal to operator
    ///
    /// @param aPropagatedModel A propagated
    /// @return True if propagateds are not equal
    bool operator!=(const Propagated& aPropagatedModel) const;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aPropagatedModel A propagated model
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Propagated& aPropagatedModel);

    /// @brief Check if propagated model is defined
    ///
    /// @return True if propagated model is defined
    virtual bool isDefined() const override;

    /// @brief Get epoch (in this case it is the first instant in the cached state array)
    ///
    /// @code{.cpp}
    ///              Instant instant = propagated.getEpoch() ;
    /// @endcode
    ///
    /// @return Instant
    virtual Instant getEpoch() const override;

    /// @brief Get revolution number at epoch (it is equal to 1)
    ///
    /// @code{.cpp}
    ///              Real real = propagated.getRevolutionNumberAtEpoch() ;
    /// @endcode
    ///
    /// @return Integer
    virtual Integer getRevolutionNumberAtEpoch() const override;

    /// @brief Calculate the state at an instant, given initial state

    /// @code{.cpp}
    ///              State state = propagated.calculateStateAt(anInstant) ;
    /// @endcode
    /// @param anInstant An instant
    /// @return State
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Calculate the state at an instant, utilizing internal cached state array to propagated
    /// shortest amount of time. Does not have macro-level sorting optimization, should not be used with disorded
    /// instant array

    /// @code{.cpp}
    ///              Array<State> states = propagated.calculateStatesAt(anInstantArray) ;
    /// @endcode
    /// @param aState An initial state
    /// @param anInstantArray An instant
    /// @return Array<State>
    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const override;

    /// @brief Calculate the revolution number at an instant
    ///
    /// @code{.cpp}
    ///              Integer integer = propagated.calculateRevolutionNumberAt(anInstant) ;
    /// @endcode
    /// @param anInstant An instant
    /// @return Integer
    virtual Integer calculateRevolutionNumberAt(const Instant& anInstant) const override;

    /// @brief Fetch internal cached state array
    ///
    /// @code{.cpp}
    ///              Array<State> stateArray = propagated.accessCachedStateArray() ;
    /// @endcode
    /// @return Array<State>&
    const Array<State>& accessCachedStateArray() const;

    /// @brief Access propagator
    ///
    /// @code{.cpp}
    ///              Propagator propagator = propagated.accessPropagator() ;
    /// @endcode
    ///
    /// @return Propagator
    const Propagator& accessPropagator() const;

    /// @brief Set internal cached state array manually
    ///
    /// @code{.cpp}
    ///              Array<State> stateArray = { ... } ;
    ///              propagated.setCachedStateArray(stateArray) ;
    /// @endcode
    /// @param aStateArray A state array
    void setCachedStateArray(const Array<State>& aStateArray);

    /// @brief Print propagated
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   protected:
    /// @brief Equal to operator
    ///
    /// @param aModel A model
    /// @return True if models are equal
    virtual bool operator==(const trajectory::Model& aModel) const override;

    /// @brief Not equal to operator
    ///
    /// @param aModel A model
    /// @return True if models are not equal
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
