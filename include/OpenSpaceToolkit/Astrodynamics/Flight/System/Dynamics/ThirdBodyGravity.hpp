/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity__

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

using ostk::core::types::String;

using ostk::physics::env::obj::Celestial;
using ostk::physics::time::Instant;

using ostk::astro::flight::system::Dynamics;

/// @brief                      Define the acceleration experienced by a point mass due to gravity

class ThirdBodyGravity : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const aCelestial = { ... };
    ///                     ThirdBodyGravity thirdBodyGravity = { aCelestial };
    /// @endcode
    ///
    /// @param              [in] aCelestial A Celestial Object

    ThirdBodyGravity(const Shared<const Celestial>& aCelestial);

    /// @brief              Constructor
    ///
    /// @code
    ///                     const aCelestial = { ... };
    ///                     const aName = { ... };
    ///                     ThirdBodyGravity thirdBodyGravity = { aCelestial, aName };
    /// @endcode
    ///
    /// @param              [in] aCelestial A Celestial Object
    /// @param              [in] aName A name

    ThirdBodyGravity(const Shared<const Celestial>& aCelestial, const String& aName);

    /// @brief              Destructor

    virtual ~ThirdBodyGravity() override;

    /// @brief              Clone third body gravity dynamics
    ///
    /// @return             Pointer to cloned third body gravity dynamics

    virtual ThirdBodyGravity* clone() const override;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aThirdBodyGravity A third body gravity dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const ThirdBodyGravity& aThirdBodyGravity);

    /// @brief              Print third body gravity dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief              Check if third body gravity dynamics is defined
    ///
    /// @return             True if third body gravity dynamics is defined

    virtual bool isDefined() const override;

    /// @brief              Get celestial
    ///
    /// @return             A celestial

    Shared<const Celestial> getCelestial() const;

    /// @brief              Apply contributions to the state derivative
    ///
    /// @param              [in] x A state vector
    /// @param              [out] dxdt A state derivative vector
    /// @param              [in] anInstant An instant

    virtual void applyContribution(const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant) const override;

   private:
    Shared<const Celestial> celestialObjectSPtr_;
};

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
