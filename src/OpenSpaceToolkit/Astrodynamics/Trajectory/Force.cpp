/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Force.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

Force::Force() {}

Force::~Force() {}

std::ostream& operator<<(std::ostream& anOutputStream, const Force& aForce)
{
    aForce.print(anOutputStream);

    return anOutputStream;
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
