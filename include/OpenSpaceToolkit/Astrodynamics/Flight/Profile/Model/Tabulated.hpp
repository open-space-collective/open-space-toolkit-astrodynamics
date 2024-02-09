/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model_Tabulated__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace profile
{
namespace model
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::filesystem::File;
using ostk::core::type::Index;
using ostk::core::type::String;

using ostk::physics::coordinate::Axes;
using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::mathematics::curvefitting::Interpolator;

using ostk::astrodynamics::flight::profile::Model;
using ostk::astrodynamics::flight::profile::State;
using ostk::astrodynamics::flight::profile::StateBuilder;
using ostk::astrodynamics::trajectory::State;

/// @brief Tabulated profile model
class Tabulated : public virtual Model
{
   public:
    Tabulated(const Array<State>& aStateArray, const Interpolator::Type& anInterpolatorType);

    virtual Tabulated* clone() const override;

    bool operator==(const Tabulated& aTabulatedModel) const;

    bool operator!=(const Tabulated& aTabulatedModel) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aTabulatedModel);

    virtual bool isDefined() const override;

    Interval getInterval() const;

    virtual State calculateStateAt(const Instant& anInstant) const override;

    virtual Axes getAxesAt(const Instant& anInstant) const override;

    virtual Shared<const Frame> getBodyFrame(const String& aFrameName) const override;

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    static Tabulated Load(const File& aFile);

   protected:
    virtual bool operator==(const Model& aModel) const override;

    virtual bool operator!=(const Model& aModel) const override;

   private:
    State firstState_;
    State lastState_;

    StateBuilder stateBuilder_;

    Array<Shared<Interpolator>> interpolator_ = Array<Shared<Interpolator>>::Empty();
};

}  // namespace model
}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
