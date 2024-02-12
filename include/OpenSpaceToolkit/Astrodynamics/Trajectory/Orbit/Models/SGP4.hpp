/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Type/Unique.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>
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

using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::String;
using ostk::core::type::Unique;

using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Instant;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::orbit::model::sgp4::TLE;

class SGP4 : public ostk::astro::trajectory::orbit::Model
{
   public:
    SGP4(const TLE& aTle);

    SGP4(const SGP4& aSGP4Model);

    ~SGP4();

    SGP4& operator=(const SGP4& aSGP4Model);

    virtual SGP4* clone() const override;

    bool operator==(const SGP4& aSGP4Model) const;

    bool operator!=(const SGP4& aSGP4Model) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const SGP4& aSGP4Model);

    virtual bool isDefined() const override;

    TLE getTle() const;

    virtual Instant getEpoch() const override;

    virtual Integer getRevolutionNumberAtEpoch() const override;

    virtual State calculateStateAt(const Instant& anInstant) const override;

    virtual Integer calculateRevolutionNumberAt(const Instant& anInstant) const override;  // [TBR] ?

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   protected:
    virtual bool operator==(const trajectory::Model& aModel) const override;

    virtual bool operator!=(const trajectory::Model& aModel) const override;

   private:
    class Impl;

    TLE tle_;

    Unique<SGP4::Impl> implUPtr_;
};

}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
