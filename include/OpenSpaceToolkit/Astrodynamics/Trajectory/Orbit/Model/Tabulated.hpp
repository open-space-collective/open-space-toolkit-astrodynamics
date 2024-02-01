/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{

using ostk::core::container::Array;
using ostk::core::type::Integer;

using ostk::mathematics::curvefitting::Interpolator;

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::State;

class Tabulated : public virtual trajectory::orbit::Model, public trajectory::model::Tabulated
{
   public:
    Tabulated(
        const Array<State>& aStateArray,
        const Integer& anInitialRevolutionNumber,
        const Interpolator::Type& aType = DEFAULT_TABULATED_TRAJECTORY_INTERPOLATION_TYPE
    );

    virtual Tabulated* clone() const override;

    bool operator==(const Tabulated& aTabulatedModel) const;

    bool operator!=(const Tabulated& aTabulatedModel) const;

    virtual bool isDefined() const override;

    virtual Instant getEpoch() const override;

    virtual Integer getRevolutionNumberAtEpoch() const override;

    virtual State calculateStateAt(const Instant& anInstant) const override;

    virtual void print(std::ostream& anOutputStream, bool displayDecorator) const override;

   protected:
    virtual bool operator==(const trajectory::Model& aModel) const override;

    virtual bool operator!=(const trajectory::Model& aModel) const override;

   private:
    Integer initialRevolutionNumber_;
};

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
