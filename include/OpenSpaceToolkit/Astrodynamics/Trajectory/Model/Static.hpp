/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static__

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

using ostk::physics::coordinate::Position;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::trajectory::Model;
using ostk::astrodynamics::trajectory::State;

/// @brief Static trajectory model
class Static : public virtual Model
{
   public:
    Static(const Position& aPosition);

    virtual Static* clone() const override;

    bool operator==(const Static& aStaticModel) const;

    bool operator!=(const Static& aStaticModel) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Static& aStaticModel);

    virtual bool isDefined() const override;

    virtual State calculateStateAt(const Instant& anInstant) const override;

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   protected:
    virtual bool operator==(const Model& aModel) const override;

    virtual bool operator!=(const Model& aModel) const override;

   private:
    Position position_;
};

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
