/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Axes.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Unique;

using ostk::math::geometry::d3::transformation::rotation::Quaternion;
using ostk::math::object::Vector3d;

using ostk::physics::coord::Axes;
using ostk::physics::coord::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astro::Trajectory;
using ostk::astro::flight::profile::Model;
using ostk::astro::flight::profile::State;

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
    ///              std::cout << Profile(...) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aProfile A flight profile
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Profile& aProfile);

    /// @brief Check if profile is defined
    ///
    /// @code{.cpp}
    ///              Profile(...).isDefined() ;
    /// @endcode
    ///
    /// @return True if profile is defined
    bool isDefined() const;

    /// @brief Get state at a given instant
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... } ;
    ///              Instant instant = { ... } ;
    ///              State state = profile.getStateAt(instant) ;
    /// @endcode
    ///
    /// @param anInstant An instant
    /// @return State
    State getStateAt(const Instant& anInstant) const;

    /// @brief Get states at a given instants
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... } ;
    ///              Array<Instant> instants = { ... } ;
    ///              Array<State> state = profile.getStatesAt(instants) ;
    /// @endcode
    ///
    /// @param anInstantArray An array of instants
    /// @return Array of states
    Array<State> getStatesAt(const Array<Instant>& anInstantArray) const;

    /// @brief Get axes at a given instant
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... } ;
    ///              Instant instant = { ... } ;
    ///              Axes axes = profile.getAxesAt(instant) ;
    /// @endcode
    ///
    /// @param anInstant An instant
    /// @return Axes
    Axes getAxesAt(const Instant& anInstant) const;

    /// @brief Get body frame
    ///
    /// @param aFrameName A body frame name
    /// @return Shared pointer to body frame
    Shared<const Frame> getBodyFrame(const String& aFrameName) const;

    /// @brief Print flight profile to output stream
    ///
    /// @code{.cpp}
    ///              Profile profile = { ... } ;
    ///              profile.print(std::cout, true) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param displayDecorator If true, display decorator
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Constructs an undefined flight profile
    ///
    /// @code{.cpp}
    ///              Profile profile = Profile::Undefined() ; // Undefined
    /// @endcode
    ///
    /// @return Undefined profile
    static Profile Undefined();

    /// @brief Constructs a flight profile with inertial pointing
    ///
    /// @param aTrajectory A trajectory
    /// @param aQuaternion A pointing in GCRF
    /// @return Flight profile
    static Profile InertialPointing(const Trajectory& aTrajectory, const Quaternion& aQuaternion);

    /// @brief Constructs a flight profile with nadir pointing
    ///
    /// @param anOrbit An orbit
    /// @param anOrbitalFrameType An orbital frame type
    /// @return Flight profile
    static Profile NadirPointing(
        const trajectory::Orbit& anOrbit, const trajectory::Orbit::FrameType& anOrbitalFrameType
    );

   private:
    Unique<Model> modelUPtr_;

    Profile();
};

}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
