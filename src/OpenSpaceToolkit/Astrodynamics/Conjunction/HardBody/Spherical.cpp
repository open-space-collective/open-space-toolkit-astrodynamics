/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Object/Point.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace hardbody
{

using ostk::core::container::Array;

using ostk::mathematics::geometry::d2::object::Point;

Spherical::Spherical(const Real& aRadius)
    : HardBody(),
      radius_(aRadius)
{
    if (!radius_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Radius");
    }

    if (radius_ < 0.0)
    {
        throw ostk::core::error::RuntimeError("Radius must be greater than or equal to 0.");
    }
}

Real Spherical::getRadius() const
{
    return radius_;
}

HardBody::CrossSection Spherical::calculateCrossSectionAt(
    [[maybe_unused]] const State& aState, [[maybe_unused]] const Shared<const Frame>& aFrameSPtr
) const
{
    return {Array<Point>({Point::Origin()}), radius_};
}

}  // namespace hardbody
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
