/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_GravityForce__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_GravityForce__

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

using ostk::math::obj::Vector3d;

using GravitationalModel = ostk::physics::environment::gravitational::Model;
using ostk::physics::coord::Position;

using ostk::astro::trajectory::Force;

class GravityForce : public Force
{
   public:

    GravityForce(const Shared<GravitationalModel>& aGravitationalModel);

    virtual GravityForce* clone() const override;

    virtual bool isDefined() const override;

    Vector3d getAcceleration(const Position& aPosition, const Instant& anInstant) const;

   private:
    Shared<GravitationalModel> gravitationalModelSPtr_;

};


}  // namespace force
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
