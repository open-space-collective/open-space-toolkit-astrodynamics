/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit__

#include <mutex>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Unique.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::container::Map;
using ostk::core::type::Index;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Unique;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Instant;
using ostk::physics::time::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::orbit::Pass;

/// @brief Gravitationally curved trajectory of an object
///
/// @ref                        https://en.wikipedia.org/wiki/Orbit
class Orbit : public Trajectory
{
   public:
    enum class FrameType
    {

        Undefined,  // Undefined frame
        NED,        // North-East-Down (NED) frame
        LVLH,       // Local Vertical, Local Horizontal (LVLH) frame (X axis aligned with position, Z axis aligned with
                    // orbital momentum)
        VVLH,       // Vehicle Velocity, Local Horizontal (VVLH) frame (Z axis aligned with opposite of position, Y axis
                    // aligned with opposite of orbital momentum)
        LVLHGD,     // Local Vertical, Local Horizontal GeoDetic (LVLHGD) frame
        LVLHGDGT,   // Local Vertical, Local Horizontal GeoDetic GroundTrack (LVLHGDGT) frame
        QSW,        // QSW frame (X axis aligned with position, Z axis aligned with orbital momentum)
        TNW,        // TNW frame (X axis aligned with velocity, Z axis aligned with orbital momentum)
        VNC  // Velocity - Normal - Co-normal (VNC) frame (X axis aligned with velocity, Y axis aligned with orbital
             // momentum)
    };

    typedef Array<Pass>::ConstIterator ConstPassIterator;

    Orbit(const orbit::Model& aModel, const Shared<const Celestial>& aCelestialObjectSPtr);

    Orbit(
        const Array<State>& aStateArray,
        const Integer& anInitialRevolutionNumber,
        const Shared<const Celestial>& aCelestialObjectSPtr
    );

    Orbit(const Orbit& anOrbit);

    ~Orbit();

    Orbit& operator=(const Orbit& anOrbit);

    bool operator==(const Orbit& anOrbit) const;

    bool operator!=(const Orbit& anOrbit) const;

    bool isDefined() const;

    Integer getRevolutionNumberAt(const Instant& anInstant) const;

    Pass getPassAt(const Instant& anInstant) const;

    Pass getPassWithRevolutionNumber(const Integer& aRevolutionNumber) const;

    Shared<const Frame> getOrbitalFrame(const Orbit::FrameType& aFrameType) const;

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Constructs an undefined orbit
    ///
    ///              Undefined orbit
    static Orbit Undefined();

    /// @brief Constructs a circular orbit
    ///
    ///              Model: Kepler (No Perturbation).
    ///
    /// @param anEpoch An orbit epoch
    /// @param anAltitude An orbit altitude (wrt. equatorial radius)
    /// @param anInclination An orbit inclination
    /// @param aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return Circular orbit
    static Orbit Circular(
        const Instant& anEpoch,
        const Length& anAltitude,
        const Angle& anInclination,
        const Shared<const Celestial>& aCelestialObjectSPtr
    );

    /// @brief Constructs an equatorial orbit
    ///
    ///              Model: Kepler (No Perturbation).
    ///
    /// @param anEpoch An orbit epoch
    /// @param anApoapsisAltitude An orbit apoapsis altitude (wrt. equatorial radius)
    /// @param aPeriapsisAltitude An orbit periapsis altitude (wrt. equatorial radius)
    /// @param aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return Equatorial orbit
    static Orbit Equatorial(
        const Instant& anEpoch,
        const Length& anApoapsisAltitude,
        const Length& aPeriapsisAltitude,
        const Shared<const Celestial>& aCelestialObjectSPtr
    );

    /// @brief Constructs a circular-equatorial orbit
    ///
    ///              Model: Kepler (No Perturbation).
    ///
    /// @param anEpoch An orbit epoch
    /// @param anAltitude An orbit altitude (wrt. equatorial radius)
    /// @param aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return Circular-equatorial orbit
    static Orbit CircularEquatorial(
        const Instant& anEpoch, const Length& anAltitude, const Shared<const Celestial>& aCelestialObjectSPtr
    );

    /// @brief Constructs a geosynchronous orbit
    ///
    ///              Model: Kepler (J2 Perturbation).
    ///
    /// @param anEpoch An orbit epoch
    /// @param anInclination An orbit inclination
    /// @param aLongitude A longitude above the surface
    /// @param aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return Circular orbit
    static Orbit GeoSynchronous(
        const Instant& anEpoch,
        const Angle& anInclination,
        const Angle& aLongitude,
        const Shared<const Celestial>& aCelestialObjectSPtr
    );

    /// @brief Constructs a Sun-synchronous orbit
    ///
    ///              Model: Kepler (J2 Perturbation).
    ///
    /// @param anEpoch An orbit epoch
    /// @param anAltitude An orbit altitude (wrt. equatorial radius)
    /// @param aLocalTimeAtDescendingNode A local time at descending node
    /// @param aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @param anArgumentOfLatitude An argument of latitude
    /// @return Sun-synchronous orbit
    static Orbit SunSynchronous(
        const Instant& anEpoch,
        const Length& anAltitude,
        const Time& aLocalTimeAtDescendingNode,
        const Shared<const Celestial>& aCelestialObjectSPtr,
        const Angle& anArgumentOfLatitude = Angle::Zero()
    );

    static String StringFromFrameType(const Orbit::FrameType& aFrameType);

    static Array<Pair<Index, Pass>> ComputePasses(
        const Array<State>& aStateArray, const Integer& anInitialRevolutionNumber
    );

   private:
    const orbit::Model* modelPtr_;

    Shared<const Celestial> celestialObjectSPtr_;

    mutable std::mutex mutex_;
    mutable Map<Integer, Pass> passMap_;

    String generateFrameName(const Orbit::FrameType& aFrameType) const;

    /// @brief Find the Instant at which the return value of `getValue` crosses zero.
    /// Use a bisection search to find the Instant between `previousInstant` and `nextInstant` at which the return value
    /// of `getValue` crosses zero.
    ///
    /// @param anEpoch An orbit epoch from which to measure
    /// @param previousInstant lower bound of search
    /// @param currentInstant upper bound of search
    /// @param getValue function that receives a duration [s] from `anEpoch` and returns a real value
    /// @return Instant at which the crossing occurs
    static Instant GetCrossingInstant(
        const Instant& anEpoch,
        const Instant& previousInstant,
        const Instant& currentInstant,
        const std::function<double(double)>& getValue
    );
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
