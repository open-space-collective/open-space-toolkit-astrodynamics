/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model_Transform__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model_Transform__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

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
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Axes;
using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;

using ostk::astrodynamics::flight::profile::Model;
using ostk::astrodynamics::trajectory::State;

/// @brief Transform provided profile model
class Transform : public virtual Model
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Transform transform(aDynamicTransformProvider, aFrameSPtr) ;
    /// @endcode
    ///
    /// @param aDynamicTransformProvider A dynamic transform provider.
    /// @param aFrameSPtr A shared pointer to a reference frame.
    Transform(const DynamicProvider& aDynamicTransformProvider, const Shared<const Frame>& aFrameSPtr);

    /// @brief Clone the transform model.
    ///
    /// @code{.cpp}
    ///     Transform* clonedTransform = transform.clone() ;
    /// @endcode
    ///
    /// @return A pointer to the cloned transform model.
    virtual Transform* clone() const override;

    /// @brief Output stream operator.
    ///
    /// @code{.cpp}
    ///     std::cout << transform ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param aTransformModel A transform model.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Transform& aTransformModel);

    /// @brief Check if the transform model is defined.
    ///
    /// @code{.cpp}
    ///     bool defined = transform.isDefined() ;
    /// @endcode
    ///
    /// @return True if the transform model is defined.
    virtual bool isDefined() const override;

    /// @brief Calculate the state at a given instant.
    ///
    /// @code{.cpp}
    ///     State state = transform.calculateStateAt(anInstant) ;
    /// @endcode
    ///
    /// @param anInstant An instant.
    /// @return The state at the given instant.
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Get the axes at a given instant.
    ///
    /// @code{.cpp}
    ///     Axes axes = transform.getAxesAt(anInstant) ;
    /// @endcode
    ///
    /// @param anInstant An instant.
    /// @return The axes at the given instant.
    virtual Axes getAxesAt(const Instant& anInstant) const override;

    /// @brief Construct a body frame with a given name.
    ///
    /// @code{.cpp}
    ///     Shared<const Frame> bodyFrame = transform.constructBodyFrame(aFrameName) ;
    /// @endcode
    ///
    /// @param aFrameName A frame name.
    /// @return A shared pointer to the constructed body frame.
    virtual Shared<const Frame> constructBodyFrame(const String& aFrameName) const override;

    /// @brief Print the transform model to an output stream.
    ///
    /// @code{.cpp}
    ///     transform.print(anOutputStream, true) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display a decorator.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Construct an undefined transform model.
    ///
    /// @code{.cpp}
    ///     Transform transform = Transform::Undefined() ;
    /// @endcode
    ///
    /// @return An undefined transform model.
    static Transform Undefined();

    /// @brief Constructs a flight profile with inertial pointing
    ///
    /// @param aTrajectory A trajectory
    /// @param aQuaternion A pointing in GCRF
    /// @return Flight profile
    static Transform InertialPointing(const Trajectory& aTrajectory, const Quaternion& aQuaternion);

    /// @brief Constructs a flight profile with local orbital frame pointing
    ///
    /// @param anOrbit An orbit
    /// @param anOrbitalFrameType An orbital frame type
    /// @return Flight profile
    static Transform LocalOrbitalFramePointing(
        const trajectory::Orbit& anOrbit, const trajectory::Orbit::FrameType& anOrbitalFrameType
    );

   protected:
    virtual bool operator==(const Model& aModel) const override;

   private:
    DynamicProvider transformProvider_;
    Shared<const Frame> frameSPtr_;
};

}  // namespace model
}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
