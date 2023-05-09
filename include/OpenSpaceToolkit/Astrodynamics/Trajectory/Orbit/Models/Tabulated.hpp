// Copyright © Loft Orbital Solutions Inc.

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Tabulated__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

using ostk::core::ctnr::Array;
using ostk::core::types::Integer;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::State;

class Tabulated : public virtual trajectory::orbit::Model, public trajectory::models::Tabulated
{
   public:
    Tabulated(
        const Array<State>& aStateArray,
        const Integer& anInitialRevolutionNumber,
        const InterpolationType& aType = DEFAULT_TABULATED_INTERPOLATION_TYPE
    );

    virtual Tabulated* clone() const override;

    bool operator==(const Tabulated& aTabulatedModel) const;

    bool operator!=(const Tabulated& aTabulatedModel) const;

    virtual bool isDefined() const override;

    virtual Instant getEpoch() const override;

    virtual Integer getRevolutionNumberAtEpoch() const override;

    virtual State calculateStateAt(const Instant& anInstant) const override;

    virtual Integer calculateRevolutionNumberAt(const Instant& anInstant) const override;

    virtual void print(std::ostream& anOutputStream, bool displayDecorator) const override;

   protected:
    virtual bool operator==(const trajectory::Model& aModel) const override;

    virtual bool operator!=(const trajectory::Model& aModel) const override;

   private:
    Integer initialRevolutionNumber_;
};

}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
