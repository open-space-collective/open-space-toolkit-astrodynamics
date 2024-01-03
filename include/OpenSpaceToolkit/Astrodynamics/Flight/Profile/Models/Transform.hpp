/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Models_Transform__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Providers/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

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

using ostk::core::ctnr::Array;
using ostk::core::types::String;

using ostk::physics::coord::Axes;
using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using DynamicProvider = ostk::physics::coord::frame::provider::Dynamic;

using ostk::astro::flight::profile::Model;
using ostk::astro::flight::profile::State;

/// @brief     Transform provided profile model
class Transform : public virtual Model
{
   public:
    Transform(const DynamicProvider& aDynamicTransformProvider, const Shared<const Frame>& aFrameSPtr);

    virtual Transform* clone() const override;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Transform& aTransformModel);

    virtual bool isDefined() const override;

    virtual State calculateStateAt(const Instant& anInstant) const override;

    virtual Axes getAxesAt(const Instant& anInstant) const override;

    virtual Shared<const Frame> getBodyFrame(const String& aFrameName) const override;

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    static Transform Undefined();

    /// @brief Constructs a flight profile with inertial pointing
    ///
    /// @param aTrajectory A trajectory
    /// @param aQuaternion A pointing in GCRF
    /// @return Flight profile
    static Transform InertialPointing(const Trajectory& aTrajectory, const Quaternion& aQuaternion);

    /// @brief Constructs a flight profile with nadir pointing
    ///
    /// @param anOrbit An orbit
    /// @param anOrbitalFrameType An orbital frame type
    /// @return Flight profile
    static Transform NadirPointing(
        const trajectory::Orbit& anOrbit, const trajectory::Orbit::FrameType& anOrbitalFrameType
    );

   protected:
    virtual bool operator==(const Model& aModel) const override;

   private:
    DynamicProvider transformProvider_;
    Shared<const Frame> frameSPtr_;
};

}  // namespace models
}  // namespace profile
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
