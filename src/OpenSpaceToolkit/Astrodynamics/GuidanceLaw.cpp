#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>

namespace ostk
{
namespace astrodynamics
{

GuidanceLaw::GuidanceLaw(const String& aName)
    : GuidanceLaw(aName, Duration::Undefined())
{
}

GuidanceLaw::GuidanceLaw(const String& aName, const Duration& aWeightTransitionBufferDuration)
    : name_(aName),
      weightTransitionBufferDuration_(aWeightTransitionBufferDuration),
      weightTransitionBufferEnd_(Instant::Undefined())
{
}

GuidanceLaw::~GuidanceLaw() {}

std::ostream& operator<<(std::ostream& anOutputStream, const GuidanceLaw& aGuidanceLaw)
{
    aGuidanceLaw.print(anOutputStream);
    return anOutputStream;
}

String GuidanceLaw::getName() const
{
    return name_;
}

Duration GuidanceLaw::getWeightTransitionBufferDuration() const
{
    return weightTransitionBufferDuration_;
}

void GuidanceLaw::notifyStateTransition(const Instant& anInstant) const
{
    if (weightTransitionBufferDuration_.isDefined())
    {
        weightTransitionBufferEnd_ = anInstant + weightTransitionBufferDuration_;
    }
}

bool GuidanceLaw::isInTransitionBuffer(const Instant& anInstant) const
{
    return weightTransitionBufferEnd_.isDefined() && anInstant < weightTransitionBufferEnd_;
}

void GuidanceLaw::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Guidance Law") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace astrodynamics
}  // namespace ostk
