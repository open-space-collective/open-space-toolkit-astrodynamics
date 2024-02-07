/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Tabulated__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace profile
{
namespace models
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::filesystem::File;
using ostk::core::type::Index;
using ostk::core::type::String;

using ostk::physics::coord::Axes;
using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astro::flight::profile::Model;
using ostk::astro::trajectory::State;

/// @brief Tabulated profile model
class Tabulated : public virtual Model
{
   public:
    Tabulated(const Array<State>& aStateArray);

    virtual Tabulated* clone() const override;

    bool operator==(const Tabulated& aTabulatedModel) const;

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
    Array<State> states_;
    mutable Index stateIndex_;

    Pair<const State*, const State*> accessStateRangeAt(const Instant& anInstant) const;

    Pair<const State*, const State*> accessStateRangeAtIndex(const Index& anIndex) const;
};

}  // namespace models
}  // namespace profile
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
