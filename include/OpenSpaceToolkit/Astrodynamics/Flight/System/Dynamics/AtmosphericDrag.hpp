/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag__

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

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

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::Dynamics;

/// @brief                      Define the acceleration experienced by a point mass due to atmospheric drag

class AtmosphericDrag : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     const Celestial = { ... };
    ///                     const SatelliteSystem = { ... };
    ///                     AtmosphericDrag atmosphericDrag = { aCelestial, aSatelliteSystem };
    /// @endcode
    ///
    /// @param              [in] aCelestial A celestial object
    /// @param              [in] aSatelliteSystem A satellite system

    AtmosphericDrag(const Shared<const Celestial>& aCelestial, const SatelliteSystem& aSatelliteSystem);

    /// @brief              Constructor
    ///
    /// @code
    ///                     const Celestial = { ... };
    ///                     const SatelliteSystem = { ... };
    ///                     const aName = { ... };
    ///                     AtmosphericDrag atmosphericDrag = { aCelestial, aSatelliteSystem, aName };
    /// @endcode
    ///
    /// @param              [in] aCelestial A celestial object
    /// @param              [in] aSatelliteSystem A satellite system
    /// @param              [in] aName A name

    AtmosphericDrag(
        const Shared<const Celestial>& aCelestial, const SatelliteSystem& aSatelliteSystem, const String& aName
    );

    /// @brief              Destructor

    virtual ~AtmosphericDrag() override;

    /// @brief              Clone atmospheric drag dynamics
    ///
    /// @return             Pointer to cloned atmospheric drag dynamics

    virtual AtmosphericDrag* clone() const override;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] anAtmosphericDrag An atmospheric drag dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const AtmosphericDrag& anAtmosphericDrag);

    /// @brief              Check if atmospheric drag dynamics is defined
    ///
    /// @return             True if atmospheric drag dynamics is defined

    virtual bool isDefined() const override;

    /// @brief              Get celestial
    ///
    /// @return             A celestial object

    Shared<const Celestial> getCelestial() const;

    /// @brief              Get satellite system
    ///
    /// @return             A satellite system

    SatelliteSystem getSatelliteSystem() const;

    virtual Array<Shared<const CoordinatesSubset>> getReadCoordinateSubsets() const override;

    virtual Array<Shared<const CoordinatesSubset>> getWriteCoordinateSubsets() const override;

    virtual VectorXd computeContribution(const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrame)
        const override;

    /// @brief              Print atmospheric drag dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    Shared<const Celestial> celestialObjectSPtr_;
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Integer positionIndex_ = Integer::Undefined();
    Integer velocityIndex_ = Integer::Undefined();
};

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
