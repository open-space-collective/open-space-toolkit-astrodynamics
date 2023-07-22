/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/SemiMajorAxisCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;

using ostk::astro::trajectory::orbit::models::kepler::COE;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

SemiMajorAxisCondition::SemiMajorAxisCondition(
    const Criteria& aCriteria, const Length& aLength, const Derived& aGravitationalParameter
)
    : EventCondition("Semi-Major Axis Condition", aCriteria),
      smaInMeters_(aLength.inMeters()),
      gravitationalParameterInSI_(aGravitationalParameter.in(GravitationalParameterSIUnit))
{
}

SemiMajorAxisCondition::~SemiMajorAxisCondition() {}

Length SemiMajorAxisCondition::getSemiMajorAxis() const
{
    return Length::Meters(smaInMeters_);
}

Derived SemiMajorAxisCondition::getGravitationalParameter() const
{
    return {gravitationalParameterInSI_, GravitationalParameterSIUnit};
}

Real SemiMajorAxisCondition::evaluate(const VectorXd& aStateVector, const Real& aTime) const
{
    (void)aTime;

    const Real position = aStateVector.segment(0, 3).norm();
    const Real velocity = aStateVector.segment(3, 3).norm();

    const Real smaInMeters = COE::SemiMajorAxisFromVector(position, velocity, gravitationalParameterInSI_);

    return smaInMeters - smaInMeters_;
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
