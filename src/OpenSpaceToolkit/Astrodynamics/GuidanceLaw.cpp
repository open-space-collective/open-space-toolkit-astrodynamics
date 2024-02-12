#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>

namespace ostk
{
namespace astro
{

GuidanceLaw::GuidanceLaw(const String& aName)
    : name_(aName)
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

void GuidanceLaw::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Guidance Law") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace astro
}  // namespace ostk
