/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model__

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{

using ostk::core::types::Integer;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::State;

class Model : public virtual ostk::astro::trajectory::Model
{
   public:
    Model();

    virtual ~Model() = 0;

    virtual Model* clone() const = 0;

    virtual bool isDefined() const = 0;

    virtual Instant getEpoch() const = 0;

    virtual Integer getRevolutionNumberAtEpoch() const = 0;

    virtual State calculateStateAt(const Instant& anInstant) const = 0;

    virtual Integer calculateRevolutionNumberAt(const Instant& anInstant) const = 0;  // [TBR]

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const = 0;
};

}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
