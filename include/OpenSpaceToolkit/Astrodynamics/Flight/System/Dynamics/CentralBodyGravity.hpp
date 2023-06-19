/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity__

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{

using ostk::physics::env::obj::Celestial;
using ostk::physics::time::Instant;

using ostk::astro::flight::system::Dynamics;

/// @brief                      Define the acceleration experienced by a point mass due to gravity

class CentralBodyGravity : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const Celestial = { ... };
    ///                     CentralBodyGravity centralBodyGravity = { aCelestial };
    /// @endcode
    ///
    /// @param              [in] aCelestial A Celestial Object

    CentralBodyGravity(const Shared<const Celestial>& aCelestial);

    /// @brief              Destructor

    virtual ~CentralBodyGravity() override;

    /// @brief              Clone central body gravity dynamics
    ///
    /// @return             Pointer to cloned central body gravity dynamics

    virtual CentralBodyGravity* clone() const override;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aCentralBodyGravity A central body gravity dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const CentralBodyGravity& aCentralBodyGravity);

    /// @brief              Check if central body gravity dynamics is defined
    ///
    /// @return             True if central body gravity dynamics is defined

    virtual bool isDefined() const override;

    /// @brief              Print central body gravity dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief              Get celestial
    ///
    /// @return             A celestial object

    Shared<const Celestial> getCelestial() const;

    /// @brief              Update the state derivative
    ///
    /// @param              [in] x A state vector
    /// @param              [in] dxdt A state derivative vector
    /// @param              [in] anInstant An instant

    virtual void update(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant) override;

   private:
    Shared<const Celestial> celestialObjectSPtr_;
};

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
