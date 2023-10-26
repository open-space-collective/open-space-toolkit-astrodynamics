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

GuidanceLaw::GuidanceLaw(const COE& aCOE, const Derived& aGravitationalParameter)
    : targetCOE_(aCOE),
      gravitationalParameter_(aGravitationalParameter)
{
}

GuidanceLaw::~GuidanceLaw() {}

std::ostream& operator<<(std::ostream& anOutputStream, const GuidanceLaw& aGuidanceLaw)
{
    aGuidanceLaw.print(anOutputStream);
    return anOutputStream;
}

COE GuidanceLaw::getCOE() const
{
    return targetCOE_;
}

void GuidanceLaw::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Guidance Law") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Target Orbit:" << targetCOE_;
    ostk::core::utils::Print::Line(anOutputStream) << "Gravitational Parameter:" << gravitationalParameter_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace thruster
}  // namespace dynamics
}  // namespace astro
}  // namespace ostk
