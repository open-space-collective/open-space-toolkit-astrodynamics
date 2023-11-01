#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster/GuidanceLaw.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{
namespace thruster
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

}  // namespace thruster
}  // namespace dynamics
}  // namespace astro
}  // namespace ostk
