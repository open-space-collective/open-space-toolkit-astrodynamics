/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity__

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

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

using ostk::core::types::Integer;
using ostk::core::types::String;

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
    ///                     const aCelestial = { ... };
    ///                     CentralBodyGravity centralBodyGravity = { aCelestial };
    /// @endcode
    ///
    /// @param              [in] aCelestial A Celestial Object

    CentralBodyGravity(const Shared<const Celestial>& aCelestial);

    /// @brief              Constructor
    ///
    /// @code
    ///                     const aCelestial = { ... };
    ///                     const aName = { ... };
    ///                     CentralBodyGravity centralBodyGravity = { aCelestial, aName };
    /// @endcode
    ///
    /// @param              [in] aCelestial A celestial object
    /// @param              [in] aName A name

    CentralBodyGravity(const Shared<const Celestial>& aCelestial, const String& aName);

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

    /// @brief              Get celestial
    ///
    /// @return             A celestial object

    Shared<const Celestial> getCelestial() const;

    virtual Array<Shared<const CoordinatesSubset>> getReadCoordinateSubsets() const override;

    virtual Array<Shared<const CoordinatesSubset>> getWriteCoordinateSubsets() const override;

    virtual VectorXd computeContribution(const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrame)
        const override;

    /// @brief              Print central body gravity dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    Shared<const Celestial> celestialObjectSPtr_;
    Integer positionIndex_ = Integer::Undefined();
    Integer velocityIndex_ = Integer::Undefined();
};

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
