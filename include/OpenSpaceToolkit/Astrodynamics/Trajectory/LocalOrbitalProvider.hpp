/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalProvider__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalProvider__

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::physics::time::Instant;
using ostk::physics::coord::frame::Provider;
using ostk::physics::coord::Transform;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

/// @brief                      Local orbital provider

class LOFProvider : public Provider
{
   public:
    typedef std::function<Transform(const Instant&, const Position&, const Velocity&)> Generator;

    LOFProvider(const LOFProvider::Generator& aGenerator);

    virtual ~LOFProvider() override;

    virtual LOFProvider* clone() const override;

    virtual bool isDefined() const override;

    virtual Transform getTransformAt(const Instant& anInstant) const override;

    virtual Transform getTransformAt(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity) const;

    static LOFProvider Undefined();

   private:
    LOFProvider::Generator generator_;
};


}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
