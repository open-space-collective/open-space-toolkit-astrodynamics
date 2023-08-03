/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalProvider.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

LOFProvider::LOFProvider(const LOFProvider::Generator& aGenerator)
    : generator_(aGenerator)
{
}

LOFProvider::~LOFProvider() {}

LOFProvider* LOFProvider::clone() const
{
    return new LOFProvider(*this);
}

bool LOFProvider::isDefined() const
{
    return !!generator_;
}

Transform LOFProvider::getTransformAt(const Instant& anInstant) const
{
    (void)(anInstant);
}

Transform LOFProvider::getTransformAt(const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("LOFProvider provider");
    }

    return generator_(anInstant, aPosition, aVelocity);
}

LOFProvider LOFProvider::Undefined()
{
    return {{}};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
