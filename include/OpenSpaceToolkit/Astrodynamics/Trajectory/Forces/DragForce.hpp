/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_DragForce__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_DragForce__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
// #include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
// #include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
// #include <OpenSpaceToolkit/Physics/Units/Length.hpp>

// #include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
// #include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Force.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace force
{

using ostk::core::types::Shared;
using ostk::physics::env::obj::Celestial;
using ostk::astro::trajectory::Force;

class DragForce : public Force
{
   public:

    DragForce(const Shared<const Celestial>& aCelestialSPtr);

    /// @brief              Destructor

    virtual ~DragForce() override;

    /// @brief              Clone gravity force
    ///
    /// @return             Pointer to cloned gravity force

    virtual DragForce* clone() const override;

    // virtual bool isDefined() const override;

    virtual Vector3d getContribution(const Position& aPosition, const Instant& anInstant, const Velocity& aVelocity, const Real& aSurfaceArea, const Real& aDragCoefficient, const Real& aMass) const override;

   private:
    Shared<const Celestial> celestialSPtr_;  // TBI: Ideally we would pass "less" information than this

};


}  // namespace force
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
