/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile__

#include <set>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Axes.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Pair;
using ostk::core::container::Tuple;
using ostk::core::type::Index;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;
using ostk::core::type::Unique;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Axes;
using ostk::physics::coordinate::Frame;
using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::unit::Angle;

using ostk::astrodynamics::flight::profile::Model;
using ostk::astrodynamics::trajectory::State;

/// @brief Spacecraft flight profile
class Profile
{
   public:
    enum class PointingMode
    {

        Undefined,  ///< Undefined pointing mode
        Inertial,   ///< Inertial pointing mode (the spacecraft points to a celestial object)
        Nadir,      ///< Nadir pointing mode (the spacecraft points points "directly down")
        Target,     ///< Target pointing mode (the spacecraft points to a given target position)
        Custom      ///< Custom pointing mode
    };

    enum class Axis
    {
        X,
        Y,
        Z
    };

    enum class TargetType
    {
        GeocentricNadir,              /// Negative of the position vector of the satellite in the ECI frame
        GeodeticNadir,                /// Negative of the geodetic normal of the satellite in the ECI frame
        Trajectory,                   /// DEPRECATED: Use TargetPosition instead.
        TargetPosition,               /// Points towards the provided target position
        TargetVelocity,               /// Points along the provided target's velocity vector
        TargetSlidingGroundVelocity,  /// Points along the provided target's ground velocity vector (aka the scan
                                      /// direction of the point sliding across the ground)
        Sun,                          /// The position of the Sun
        Moon,                         /// The position of the Moon
        VelocityECI,                  /// The velocity vector in the ECI frame
        VelocityECEF,                 /// The velocity vector in the ECEF frame
        OrbitalMomentum,              /// The orbital momentum vector of the satellite in the ECI frame
        OrientationProfile,           /// Points towards a profile of orientations in the ECI frame
        Custom,                       /// Custom target
    };

    /// @brief Represents a target for alignment or pointing purposes.
    class Target
    {
       public:
        /// @brief Constructs a Target object.
        ///
        /// @param aType The type of the target.
        /// @param anAxis The axis of the target.
        /// @param isAntiDirection Whether the target is in the anti-direction.
        Target(const TargetType& aType, const Axis& anAxis, const bool& isAntiDirection = false);

        TargetType type;     ///< The type of the target.
        Axis axis;           ///< The axis of the target.
        bool antiDirection;  ///< Whether the target is in the anti-direction.
    };

    /// @brief Represents a target that points towards a trajectory.
    class TrajectoryTarget : public Target
    {
       public:
        /// @brief Constructs a TrajectoryTarget object.
        ///
        /// @deprecated Use TrajectoryTarget::TargetPosition(...) instead.
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of the target.
        /// @param isAntiDirection Whether the target is in the anti-direction.
        [[deprecated("Use TrajectoryTarget::TargetPosition(...) instead.")]]
        TrajectoryTarget(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection = false
        );

        /// @brief Constructs a TrajectoryTarget object of type Trajectory, pointing towards a specific position.
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of the target.
        /// @param isAntiDirection Whether the target is in the anti-direction.
        static TrajectoryTarget TargetPosition(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection = false
        );

        /// @brief Constructs a TrajectoryTarget object of type TargetVelocity, pointing along the scan direction. When
        /// choosing this as a clocking target, the resulting profile will not be yaw compensated.
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of the target.
        /// @param isAntiDirection Whether the target is in the anti-direction.
        static TrajectoryTarget TargetVelocity(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection = false
        );

        /// @brief Constructs a TrajectoryTarget object of type TargetSlidingGroundVelocity, pointing along the ground
        /// velocity vector (aka the scan direction of the point sliding across the ground). This will compensate for
        /// the rotation of the referenced celestial body.
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of the target.
        /// @param isAntiDirection Whether the target is in the anti-direction.
        static TrajectoryTarget TargetSlidingGroundVelocity(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection = false
        );

        ostk::astrodynamics::Trajectory trajectory;  ///< The trajectory to point towards.

       private:
        /// @brief Constructs a TrajectoryTarget object.
        ///
        /// @param aType The type of the target.
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of the target.
        /// @param isAntiDirection Whether the target is in the anti-direction.
        TrajectoryTarget(
            const TargetType& aType,
            const ostk::astrodynamics::Trajectory& aTrajectory,
            const Axis& anAxis,
            const bool& isAntiDirection = false
        );
    };

    /// @brief Represents a target that points towards a profile of orientations.
    class OrientationProfileTarget : public Target
    {
       public:
        /// @brief Constructs an OrientationProfileTarget object.
        ///
        /// @param anOrientationProfile The profile of orientations.
        /// @param anAxis The axis of the target.
        /// @param isAntiDirection Whether the target is in the anti-direction.
        OrientationProfileTarget(
            const Array<Pair<Instant, Vector3d>>& anOrientationProfile,
            const Axis& anAxis,
            const bool& isAntiDirection = false
        );

        /// @brief Gets the alignment vector at a specific instant.
        ///
        /// @param anInstant The instant at which to get the alignment vector.
        /// @return The alignment vector at the specified instant.
        Vector3d getAlignmentVectorAt(const Instant& anInstant) const;

        Array<Pair<Instant, Vector3d>> orientationProfile;  ///< The profile of orientations.

       private:
        Array<Shared<const Interpolator>> interpolators_;  ///< Interpolators for the orientation profile.
    };

    class CustomTarget : public Target
    {
       public:
        CustomTarget(
            std::function<Vector3d(const State&)> anOrientationGenerator,
            const Axis& anAxis,
            const bool& isAntiDirection = false
        );

        std::function<Vector3d(const State&)> orientationGenerator;
    };

    /// @brief Constructor
    ///
    /// @param aModel A model
    Profile(const Model& aModel);

    /// @brief Copy constructor
    ///
    /// @param aProfile A flight profile
    Profile(const Profile& aProfile);

    /// @brief Copy assignment operator
    ///
    /// @param aProfile A flight profile
    /// @return Reference to flight profile
    Profile& operator=(const Profile& aProfile);

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///              std::cout << Profile(...);
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aProfile A flight profile
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Profile& aProfile);

    /// @brief Check if profile is defined
    ///
    /// @code{.cpp}
    ///              Profile(...).isDefined();
    /// @endcode
    ///
    /// @return True if profile is defined
    bool isDefined() const;

    /// @brief Access profile model
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... };
    ///              const Model& model = profile.accessModel();
    /// @endcode
    ///
    /// @return Const reference to the profile model
    const Model& accessModel() const;

    /// @brief Get state at a given instant
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... };
    ///              Instant instant = { ... };
    ///              State state = profile.getStateAt(instant);
    /// @endcode
    ///
    /// @param anInstant An instant
    /// @return State
    State getStateAt(const Instant& anInstant) const;

    /// @brief Get states at a given instants
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... };
    ///              Array<Instant> instants = { ... };
    ///              Array<State> state = profile.getStatesAt(instants);
    /// @endcode
    ///
    /// @param anInstantArray An array of instants
    /// @return Array of states
    Array<State> getStatesAt(const Array<Instant>& anInstantArray) const;

    /// @brief Get axes at a given instant
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... };
    ///              Instant instant = { ... };
    ///              Axes axes = profile.getAxesAt(instant);
    /// @endcode
    ///
    /// @param anInstant An instant
    /// @return Axes
    Axes getAxesAt(const Instant& anInstant) const;

    /// @brief Get body frame
    ///
    /// @param aFrameName A body frame name
    /// @return Shared pointer to body frame
    [[deprecated("Use profile.construct_body_frame(...) instead.")]]
    Shared<const Frame> getBodyFrame(const String& aFrameName) const;

    /// @brief Construct body frame
    ///
    /// @param aFrameName A body frame name
    /// @param overwrite If true, destruct existing frame with same name
    /// @return Shared pointer to body frame
    Shared<const Frame> constructBodyFrame(const String& aFrameName, const bool& overwrite = false) const;

    /// @brief Print flight profile to output stream
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... };
    ///              profile.print(std::cout, true);
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param displayDecorator If true, display decorator
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Constructs an undefined flight profile
    ///
    /// @code{.cpp}
    ///              Profile profile = Profile::Undefined(); // Undefined
    /// @endcode
    ///
    /// @return Undefined profile
    static Profile Undefined();

    /// @brief Constructs a flight profile with inertial pointing
    ///
    /// @param aTrajectory A trajectory
    /// @param aQuaternion A pointing in GCRF
    /// @return Flight profile
    static Profile InertialPointing(const ostk::astrodynamics::Trajectory& aTrajectory, const Quaternion& aQuaternion);

    /// @brief Constructs a flight profile with local orbital frame pointing
    ///
    /// @param anOrbit An orbit
    /// @param anOrbitalFrameType An orbital frame type
    /// @return Flight profile
    static Profile LocalOrbitalFramePointing(
        const trajectory::Orbit& anOrbit, const trajectory::Orbit::FrameType& anOrbitalFrameType
    );

    /// @brief Construct a flight profile with custom target pointing
    ///
    /// @param anOrbit An orbit
    /// @param anOrientationGenerator An orientation generator
    /// @return Flight profile
    static Profile CustomPointing(
        const trajectory::Orbit& anOrbit, const std::function<Quaternion(const State&)>& anOrientationGenerator
    );

    /// @brief Construct a flight profile with custom target pointing
    ///
    /// @param anOrbit An orbit
    /// @param anAlignmentTarget An alignment target
    /// @param aClockingTarget A clocking target
    /// @param anAngularOffset An angular offset
    /// @return Flight profile
    static Profile CustomPointing(
        const trajectory::Orbit& anOrbit,
        const Shared<const Target>& anAlignmentTargetSPtr,
        const Shared<const Target>& aClockingTargetSPtr,
        const Angle& anAngularOffset = Angle::Zero()
    );

    /// @brief Generate a function that provides a quaternion that aligns and constrains for a given state.
    ///
    /// @param anAlignmentAxis An alignment axis
    /// @param aClockingAxis A clocking axis
    /// @param anAngularOffset An angular offset applied to the clocking axis

    static std::function<Quaternion(const State&)> AlignAndConstrain(
        const Shared<const Target>& anAlignmentTargetSPtr,
        const Shared<const Target>& aClockingTargetSPtr,
        const Angle& anAngularOffset = Angle::Zero()
    );

   private:
    Unique<Model> modelUPtr_;

    Profile();

    static Vector3d ComputeGeocentricNadirDirectionVector(const State& aState);

    static Vector3d ComputeGeodeticNadirDirectionVector(const State& aState);

    static Vector3d ComputeTargetDirectionVector(
        const State& aState, const ostk::astrodynamics::Trajectory& aTrajectory
    );

    static Vector3d ComputeTargetVelocityVector(
        const State& aState, const ostk::astrodynamics::Trajectory& aTrajectory
    );
    static Vector3d ComputeTargetSlidingGroundVelocityVector(
        const State& aState, const ostk::astrodynamics::Trajectory& aTrajectory
    );

    static Vector3d ComputeCelestialDirectionVector(const State& aState, const Celestial& aCelestial);

    static Vector3d ComputeVelocityDirectionVector_ECI(const State& aState);

    static Vector3d ComputeVelocityDirectionVector_ECEF(const State& aState);

    static Vector3d ComputeOrbitalMomentumDirectionVector(const State& aState);

    static Vector3d ComputeClockingAxisVector(const Vector3d& anAlignmentAxisVector, const Vector3d& aClockingVector);

    static Quaternion ComputeBodyToECIQuaternion(
        const Axis& anAlignmentAxis,
        const Axis& aClockingAxis,
        const Vector3d& anAlignmentAxisVector,
        const Vector3d& aClockingAxisVector
    );
};

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
