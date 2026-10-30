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

/// @brief Spacecraft flight Profile (6 DOF)
class Profile
{
   public:
    /// @brief Represents a pointing constraint of one axis of the spacecraft towards a target.
    class PointingTarget
    {
       public:
        /// @brief Represents the type of target to point towards.
        enum class Type
        {
            // Simple pre-defined targets
            GeocentricNadir,  /// Negative of the position vector of the satellite in the ECI frame
            GeodeticNadir,    /// Negative of the geodetic normal of the satellite in the ECI frame
            VelocityECI,      /// The velocity vector of the satellite in the ECI frame
            VelocityECEF,     /// The velocity vector of the satellite in the ECEF frame
            OrbitalMomentum,  /// The orbital momentum vector of the satellite in the ECI frame
            Sun,              /// The position of the Sun
            Moon,             /// The position of the Moon

            // Pointing relative to a provided trajectory (inertial or earth-fixed)
            TrajectoryPosition,               /// Points towards the provided trajectory's position
            TrajectoryVelocity,               /// Points along the provided trajectory's velocity vector
            TrajectorySlidingGroundVelocity,  /// Points along the provided trajectory's ground velocity vector (aka the
                                              /// scan direction of the point sliding across the ground)

            // User defined pointing
            Tabulated,  /// Pointing pre-defined via a profile of vectors in the ECI frame that can be interpolated
            Generator,  /// Pointing generated at each timestep by a function that returns a vector in the ECI frame
        };

        /// @brief Constructs a PointingTarget object.
        ///
        /// @param aType The type of the target.
        /// @param aDirectionInBodyFrame The (unit) vector in the spacecraft's body frame to point relative to the
        /// target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69]
        /// for an arbitrary axis).
        PointingTarget(const Type& aType, const Vector3d& aDirectionInBodyFrame);

        Type type;                      ///< The type of the target.
        Vector3d directionInBodyFrame;  ///< The direction (unit vector) in the spacecraft's body frame of the target.
    };

    /// @brief Represents a pointing constraint of one axis of the spacecraft towards a target moving along a
    /// trajectory.
    class TrajectoryPointingTarget : public PointingTarget
    {
       public:
        /// @brief Constructs a TrajectoryPointingTarget object, pointing towards the provided trajectory's
        /// position.
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param aDirectionInBodyFrame The (unit) vector in the spacecraft's body frame to point relative to the
        /// target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69]
        /// for an arbitrary axis).
        static TrajectoryPointingTarget TargetPosition(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirectionInBodyFrame
        );

        /// @brief Constructs a TrajectoryPointingTarget object, pointing towards the provided trajectory's
        /// velocity vector.
        /// @note When choosing this as a clocking target, the resulting profile will not compensate for the rotation of
        /// the referenced celestial body (yaw compensation).
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param aDirectionInBodyFrame The (unit) vector in the spacecraft's body frame to point relative to the
        /// target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69]
        /// for an arbitrary axis).
        static TrajectoryPointingTarget TargetVelocity(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirectionInBodyFrame
        );

        /// @brief Constructs a TrajectoryPointingTarget object, pointing towards the provided trajectory's ground
        /// velocity vector (aka the scan direction of the point sliding across the ground).
        /// @note When choosing this as a clocking target, the resulting profile will compensate for the rotation of the
        /// referenced celestial body (yaw compensation).
        ///
        /// @param aTrajectory The trajectory to point towards.
        /// @param aDirectionInBodyFrame The (unit) vector in the spacecraft's body frame to point relative to the
        /// target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69]
        /// for an arbitrary axis).
        static TrajectoryPointingTarget TargetSlidingGroundVelocity(
            const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirectionInBodyFrame
        );

        ostk::astrodynamics::Trajectory trajectory;  ///< The trajectory to point towards.

       private:
        TrajectoryPointingTarget(
            const Type& aType, const ostk::astrodynamics::Trajectory& aTrajectory, const Vector3d& aDirectionInBodyFrame
        );
    };

    /// @brief Represents a pointing constraint of one axis of the spacecraft along a user defined, tabulated pointing
    /// profile in the ECI frame.
    /// @note Unlike the pre-defined target types above, the target is fully user defined: the profile is provided up
    /// front (must be known beforehand) and interpolated at each instant.
    class TabulatedPointingTarget : public PointingTarget
    {
       public:
        /// @brief Constructs a TabulatedPointingTarget object.
        ///
        /// @param aPointingProfileInECI The profile of pointing vectors in the ECI frame.
        /// @param aDirectionInBodyFrame The (unit) vector in the spacecraft's body frame to point relative to the
        /// target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69]
        /// for an arbitrary axis).
        /// @param anInterpolatorType The type of interpolator to use. Defaults to BarycentricRational.
        TabulatedPointingTarget(
            const Array<Pair<Instant, Vector3d>>& aPointingProfileInECI,
            const Vector3d& aDirectionInBodyFrame,
            const Interpolator::Type& anInterpolatorType = Interpolator::Type::BarycentricRational
        );

        /// @brief Interpolates the pointing vector in ECI at a specified instant using the profile.
        ///
        /// @param anInstant The instant at which to interpolate the pointing vector.
        /// @return The pointing vector in ECI at the specified instant.
        Vector3d interpolatePointingVectorAt(const Instant& anInstant) const;

        Array<Pair<Instant, Vector3d>> pointingProfileInECI;

       private:
        Array<Shared<const Interpolator>> interpolators_;
    };

    /// @brief Represents a pointing constraint of one axis of the spacecraft towards a user defined pointing vector
    /// generator.
    /// @note Unlike the pre-defined target types above, the target is fully user defined: the pointing vector is
    /// generated at each timestep by a function that returns a vector in the ECI frame (does not need to be known
    /// beforehand).
    class PointingTargetGenerator : public PointingTarget
    {
       public:
        /// @brief Constructs a PointingTargetGenerator object.
        ///
        /// @param aPointingProfileGeneratorInECI The function that generates the pointing vector in the ECI frame from
        /// the state at a given instant.
        /// @param aDirectionInBodyFrame The (unit) vector in the spacecraft's body frame to point relative to the
        /// target - can be a basis vector (i.e. [1, 0, 0] for the X axis), or a custom vector (i.e. [0.70, -0.15, 0.69]
        /// for an arbitrary axis).
        PointingTargetGenerator(
            std::function<Vector3d(const State&)> aPointingProfileGeneratorInECI, const Vector3d& aDirectionInBodyFrame
        );

        std::function<Vector3d(const State&)> pointingProfileGeneratorInECI;
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

    /// @brief Construct a flight profile with constrained pointing.
    ///
    /// This solves the "align and constrain" (two-vector) attitude problem at each instant of the orbit. Fully defining
    /// the spacecraft attitude requires fixing 3 rotational degrees of freedom, which is done using two constraints
    /// of decreasing priority:
    /// - Alignment (primary constraint): the body frame direction of the alignment pointing constraint is pointed
    ///   exactly towards its target (e.g. the payload boresight towards geodetic nadir). This fixes 2 degrees of
    ///   freedom, leaving the spacecraft free to rotate about its alignment direction.
    /// - Clocking (secondary constraint): the remaining degree of freedom (the rotation about the alignment direction)
    ///   is fixed by rotating the body frame direction of the clocking pointing constraint as close as possible to its
    ///   target (e.g. a body axis along the ground velocity, for yaw steering).
    ///
    /// The celestial object of the orbit is used for the target types that depend on the rotation or shape of the
    /// central body (geodetic nadir, ECEF velocity and sliding ground velocity). See AlignAndConstrainGenerator for
    /// details on how the orientation is computed.
    ///
    /// @code{.cpp}
    ///              Orbit orbit = { ... };
    ///              const Shared<const Profile::PointingTarget> alignmentPointingTargetSPtr =
    ///                  std::make_shared<const Profile::PointingTarget>(
    ///                      Profile::PointingTarget::Type::GeodeticNadir, Vector3d::UnitZ()
    ///                  );
    ///              const Shared<const Profile::PointingTarget> clockingPointingTargetSPtr =
    ///                  std::make_shared<const Profile::PointingTarget>(
    ///                      Profile::PointingTarget::Type::VelocityECEF, Vector3d::UnitX()
    ///                  );
    ///              Profile profile = Profile::AlignedAndConstrainedPointing(
    ///                  orbit, alignmentPointingTargetSPtr, clockingPointingTargetSPtr
    ///              );
    /// @endcode
    ///
    /// @param anOrbit An orbit
    /// @param anAlignmentPointingTargetSPtr An alignment pointing constraint (primary constraint, met exactly)
    /// @param aClockingPointingTargetSPtr A clocking pointing constraint (secondary constraint, met as closely as
    /// possible)
    /// @param anAngularClockingOffset An angular offset applied to the clocking target, as a right-handed rotation
    /// about the alignment target
    /// @return Flight profile
    static Profile AlignedAndConstrainedPointing(
        const trajectory::Orbit& anOrbit,
        const Shared<const PointingTarget>& anAlignmentPointingTargetSPtr,
        const Shared<const PointingTarget>& aClockingPointingTargetSPtr,
        const Angle& anAngularClockingOffset = Angle::Zero()
    );

    /// @brief Construct a flight profile with fully custom pointing
    ///
    /// @param anOrbit An orbit
    /// @param anOrientationGenerator An orientation generator
    /// @return Flight profile
    static Profile CustomPointing(
        const trajectory::Orbit& anOrbit, const std::function<Quaternion(const State&)>& anOrientationGenerator
    );

    /// @brief Generate a function that returns a quaternion constrained by alignment and clocking vectors for
    /// each given state.
    ///
    /// The generated function solves the "align and constrain" (two-vector) attitude problem: the spacecraft alignment
    /// body frame direction is pointed exactly towards the alignment target (primary constraint), and the spacecraft
    /// is then rotated about this direction so that its clocking body frame direction gets as close as possible to
    /// the clocking target (secondary constraint).
    ///
    /// For each state, with all target directions expressed in GCRF:
    /// 1. The alignment target direction is computed from the alignment pointing constraint.
    /// 2. The clocking target direction is computed from the clocking pointing constraint, then projected onto the
    ///    plane perpendicular to the alignment target direction, and rotated about the alignment target direction by
    ///    the angular clocking offset.
    /// 3. The clocking body frame direction is projected onto the plane perpendicular to the alignment body frame
    ///    direction.
    /// 4. The returned quaternion is the rotation from GCRF to the spacecraft body frame that maps the alignment
    ///    target direction onto the alignment body frame direction, and the projected clocking target direction onto
    ///    the projected clocking body frame direction.
    ///
    /// The resulting attitude is undefined if the alignment and clocking target directions are colinear at a given
    /// state, as there is no unique rotation about the alignment target direction in that case.
    ///
    /// This function is typically used in conjunction with CustomPointing, or directly through
    /// AlignedAndConstrainedPointing.
    ///
    /// @throw RuntimeError If the alignment and clocking pointing constraints have the same target type (except for
    /// trajectory position and trajectory velocity targets, which can point to different trajectories)
    /// @throw RuntimeError If the alignment and clocking body frame directions are colinear
    /// @throw RuntimeError If the alignment and clocking targets are geocentric nadir and geodetic nadir
    ///
    /// @param aCelestialSPtr A celestial object. Its body frame will be used for geodetic nadir, ECEF velocity and
    /// sliding ground velocity calculations
    /// @param anAlignmentPointingTargetSPtr An alignment pointing constraint (primary constraint, met exactly)
    /// @param aClockingPointingTargetSPtr A clocking pointing constraint (secondary constraint, met as closely as
    /// possible)
    /// @param anAngularClockingOffset An angular offset applied to the clocking target, as a right-handed rotation
    /// about the alignment target
    /// @return A function returning the orientation (rotation from GCRF to the spacecraft body frame) for a given state
    static std::function<Quaternion(const State&)> AlignAndConstrainGenerator(
        const Shared<const Celestial>& aCelestialSPtr,
        const Shared<const PointingTarget>& anAlignmentPointingTargetSPtr,
        const Shared<const PointingTarget>& aClockingPointingTargetSPtr,
        const Angle& anAngularClockingOffset = Angle::Zero()
    );

   private:
    Unique<Model> modelUPtr_;

    Profile();

    static Vector3d ComputeGeocentricNadirDirectionVector(const State& aState);

    static Vector3d ComputeGeodeticNadirDirectionVector(
        const State& aState, const Shared<const Celestial>& aCelestialSPtr
    );

    static Vector3d ComputeTargetDirectionVector(
        const State& aState, const ostk::astrodynamics::Trajectory& aTrajectory
    );

    static Vector3d ComputeTargetVelocityVector(
        const State& aState, const ostk::astrodynamics::Trajectory& aTrajectory
    );

    static Vector3d ComputeTargetSlidingGroundVelocityVector(
        const State& aState,
        const ostk::astrodynamics::Trajectory& aTrajectory,
        const Shared<const Frame>& aCelestialFrameSPtr
    );

    static Vector3d ComputeCelestialDirectionVector(const State& aState, const Celestial& aCelestial);

    static Vector3d ComputeVelocityDirectionVector_ECI(const State& aState);

    static Vector3d ComputeVelocityDirectionVector_ECEF(
        const State& aState, const Shared<const Frame>& aCelestialFrameSPtr
    );

    static Vector3d ComputeOrbitalMomentumDirectionVector(const State& aState);

    static Vector3d ComputeClockingVector(const Vector3d& anAlignmentVector, const Vector3d& aDesiredClockingVector);
};

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
