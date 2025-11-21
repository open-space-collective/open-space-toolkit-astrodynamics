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
    // @brief Represents an axis of your spacecraft.
    enum class Axis
    {
        X,
        Y,
        Z
    };

    /// @brief Represents the type of target to point towards.
    enum class Type
    {
        // Simple pre-defined targets
        GeocentricNadir,  /// Negative of the position vector of the satellite in the ECI frame
        GeodeticNadir,    /// Negative of the geodetic normal of the satellite in the ECI frame
        Sun,              /// The position of the Sun
        Moon,             /// The position of the Moon
        VelocityECI,      /// The velocity vector in the ECI frame
        VelocityECEF,     /// The velocity vector in the ECEF frame
        OrbitalMomentum,  /// The orbital momentum vector of the satellite in the ECI frame

        // More complex targets
        TargetPosition,               /// Points towards the provided target position
        TargetVelocity,               /// Points along the provided target's velocity vector
        TargetSlidingGroundVelocity,  /// Points along the provided target's ground velocity vector (aka the scan
                                      /// direction of the point sliding across the ground)
        // Custom pointing
        CustomProfileECI,           /// Pointing pre-defined via a profile of vectors in the ECI frame
        CustomProfileGeneratorECI,  /// Pointing generated at each timestep by a function of that returns a vector in
                                    /// the ECI frame
    };

    /// @brief Represents pointing of one axis of your spacecraft towards a target.
    class Pointing
    {
       public:
        /// @brief Constructs a Pointing object.
        ///
        /// @param aType The type of the target to point towards.
        /// @param anAxis The axis of your spacecraft to point towards the target.
        /// @param isAntiDirection Whether the axis pointing is in the anti-direction.
        Pointing(const Type& aType, const Axis& anAxis, const bool& isAntiDirection = false);

        Type type;
        Axis axis;
        bool antiDirection;
    };
        /// @param anAxis The axis of your spacecraft to point towards the target.
        /// @param isAntiDirection Whether the axis pointing is in the anti-direction.
        TrajectoryPointing(const Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection = false);

        Trajectory trajectory;
    /// @brief Represents pointing of one axis of your spacecraft towards a trajectory.
    class TrajectoryPointing : public Pointing
    {
       public:
        /// @brief Constructs a TrajectoryPointing object.
        ///
        /// @deprecated Use TrajectoryTarget::TargetPosition(...) instead.
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of the target.
        /// @param isAntiDirection Whether the target is in the anti-direction.
        [[deprecated("Use TrajectoryTarget::TargetPosition(...) instead.")]]
        TrajectoryTarget(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection = false
        );

        /// @brief Constructs a TrajectoryTarget object of type TargetPosition, pointing towards a specific position.
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of your spacecraft to point towards the target.
        /// @param isAntiDirection Whether the axis pointing is in the anti-direction.
        static TrajectoryTarget TargetPosition(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection = false
        );

        /// @brief Constructs a TrajectoryTarget object of type TargetVelocity, pointing along the scan direction. When
        /// choosing this as a clocking target, the resulting profile will not be yaw compensated.
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of your spacecraft to point towards the target.
        /// @param isAntiDirection Whether the axis pointing is in the anti-direction.
        static TrajectoryTarget TargetVelocity(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Axis& anAxis, const bool& isAntiDirection = false
        );

        /// @brief Constructs a TrajectoryTarget object of type TargetSlidingGroundVelocity, pointing along the ground
        /// velocity vector (aka the scan direction of the point sliding across the ground). This will compensate for
        /// the rotation of the referenced celestial body.
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param anAxis The axis of your spacecraft to point towards the target.
        /// @param isAntiDirection Whether the axis pointing is in the anti-direction.
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

    /// @brief Represents pointing of one axis of your spacecraft towards a custom pointing profile.
    class CustomProfilePointing : public Pointing
    {
       public:
        /// @brief Constructs a CustomProfilePointing object.
        ///
        /// @param anOrientationProfile The profile of orientations to point towards in the ECI frame.
        /// @param anAxis The axis of your spacecraft to point towards the target.
        /// @param isAntiDirection Whether the axis pointing is in the anti-direction.
        CustomProfilePointing(
            const Array<Pair<Instant, Vector3d>>& aPointingProfileECI,
            const Axis& anAxis,
            const bool& isAntiDirection = false
        );

        /// @brief Interpolates the pointing vector in ECI at a specified instant using the profile.
        ///
        /// @param anInstant The instant at which to interpolate the alignment vector.
        /// @return The alignment vector in ECI at the specified instant.
        Vector3d interpolatePointingVectorAt(const Instant& anInstant) const;

        Array<Pair<Instant, Vector3d>> pointingProfileECI;

       private:
        Array<Shared<const Interpolator>> interpolators_;
    };

    /// @brief Represents pointing of one axis of your spacecraft towards a custom pointing profile generator.
    class CustomProfileGeneratorPointing : public Pointing
    {
       public:
        /// @brief Constructs a CustomProfileGeneratorPointing object.
        ///
        /// @param aPointingGeneratorECI The function that generates the pointing vector in the ECI frame from the
        /// state.
        /// @param anAxis The axis of your spacecraft to point towards the target.
        /// @param isAntiDirection Whether the axis pointing is in the anti-direction.
        CustomProfileGeneratorPointing(
            std::function<Vector3d(const State&)> aPointingProfileGeneratorECI,
            const Axis& anAxis,
            const bool& isAntiDirection = false
        );

        std::function<Vector3d(const State&)> pointingProfileGeneratorECI;
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

    /// @brief Get the spacecraft state at a given instant
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

    /// @brief Get spacecraft states at given instants
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

    /// @brief Get the spacecraft axes at a given instant
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

    /// @brief Constructs a flight profile with constant inertial pointing
    ///
    /// @param aTrajectory A trajectory
    /// @param aQuaternion A pointing in GCRF
    /// @return Flight profile
    static Profile ConstantInertialPointing(const Trajectory& aTrajectory, const Quaternion& aQuaternion);

    /// @brief Constructs a flight profile with a constant local orbital frame pointing
    ///
    /// @param anOrbit An orbit
    /// @param anOrbitalFrameType An orbital frame type
    /// @return Flight profile
    static Profile ConstantLocalOrbitalFramePointing(
        const trajectory::Orbit& anOrbit, const trajectory::Orbit::FrameType& anOrbitalFrameType
    );

    /// @brief Construct a flight profile with constrained (alignment vector + clocking vector) pointing
    ///
    /// @param anOrbit An orbit
    /// @param anAlignmentPointingSPtr An alignment pointing vector
    /// @param aClockingPointingSPtr A clocking pointing vector
    /// @param anAngularOffset An angular offset applied to the clocking pointing vector
    /// @return Flight profile
    static Profile ConstrainedPointing(
        const trajectory::Orbit& anOrbit,
        const Shared<const Pointing>& anAlignmentPointingSPtr,
        const Shared<const Pointing>& aClockingPointingSPtr,
        const Angle& anAngularOffset = Angle::Zero()
    );

    /// @brief Construct a flight profile with fully custom target pointing
    ///
    /// @param anOrbit An orbit
    /// @param anOrientationGenerator An orientation generator
    /// @return Flight profile
    static Profile CustomPointing(
        const trajectory::Orbit& anOrbit, const std::function<Quaternion(const State&)>& anOrientationGenerator
    );

    /// @brief Generate a function that returns a quaternion constrained by alignment and clocking pointing vectors
    ///
    /// @param anAlignmentPointingSPtr An alignment pointing vector
    /// @param aClockingPointingSPtr A clocking pointing vector
    /// @param anAngularOffset An angular offset applied to the clocking pointing vector

    static std::function<Quaternion(const State&)> Constrain(
        const Shared<const Pointing>& anAlignmentPointingSPtr,
        const Shared<const Pointing>& aClockingPointingSPtr,
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
