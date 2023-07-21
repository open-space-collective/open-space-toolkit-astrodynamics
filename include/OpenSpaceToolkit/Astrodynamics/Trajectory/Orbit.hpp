/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit__

#include <mutex>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Map.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Unique.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Map;
using ostk::core::types::Index;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::Unique;

using ostk::physics::coord::Frame;
using ostk::physics::env::obj::Celestial;
using ostk::physics::time::Instant;
using ostk::physics::time::Time;
using ostk::physics::units::Angle;
using ostk::physics::units::Length;

using ostk::astro::Trajectory;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::orbit::Pass;

/// @brief                      Gravitationally curved trajectory of an object
///
/// @ref                        https://en.wikipedia.org/wiki/Orbit

class Orbit : public Trajectory
{
   public:
    enum class FrameType
    {

        Undefined,  ///< Undefined frame
        NED,        ///< North-East-Down (NED) frame
        LVLH,    ///< Local Vertical, Local Horizontal (LVLH) frame (X axis aligned with position, Z axis aligned with
                 ///< orbital momentum)
        VVLH,    ///< Vehicle Velocity, Local Horizontal (VVLH) frame (Z axis aligned with opposite of position, Y axis
                 ///< aligned with opposite of orbital momentum)
        LVLHGD,  ///< Local Vertical, Local Horizontal GeoDetic (LVLHGD) frame
        QSW,     ///< QSW frame (X axis aligned with position, Z axis aligned with orbital momentum)
        TNW,     ///< TNW frame (X axis aligned with velocity, Z axis aligned with orbital momentum)
        VNC  ///< Velocity - Normal - Co-normal (VNC) frame (X axis aligned with velocity, Y axis aligned with orbital
             ///< momentum)

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

    /// @brief              Constructs an undefined orbit
    ///
    ///                     Undefined orbit

    static Orbit Undefined();

    /// @brief              Constructs a circular orbit
    ///
    ///                     Model: Kepler (No Perturbation).
    ///
    /// @param              [in] anEpoch An orbit epoch
    /// @param              [in] anAltitude An orbit altitude (wrt. equatorial radius)
    /// @param              [in] anInclination An orbit inclination
    /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return             Circular orbit

    static Orbit Circular(
        const Instant& anEpoch,
        const Length& anAltitude,
        const Angle& anInclination,
        const Shared<const Celestial>& aCelestialObjectSPtr
    );

    /// @brief              Constructs an equatorial orbit
    ///
    ///                     Model: Kepler (No Perturbation).
    ///
    /// @param              [in] anEpoch An orbit epoch
    /// @param              [in] anApoapsisAltitude An orbit apoapsis altitude (wrt. equatorial radius)
    /// @param              [in] aPeriapsisAltitude An orbit periapsis altitude (wrt. equatorial radius)
    /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return             Equatorial orbit

    static Orbit Equatorial(
        const Instant& anEpoch,
        const Length& anApoapsisAltitude,
        const Length& aPeriapsisAltitude,
        const Shared<const Celestial>& aCelestialObjectSPtr
    );

    /// @brief              Constructs a circular-equatorial orbit
    ///
    ///                     Model: Kepler (No Perturbation).
    ///
    /// @param              [in] anEpoch An orbit epoch
    /// @param              [in] anAltitude An orbit altitude (wrt. equatorial radius)
    /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return             Circular-equatorial orbit

    static Orbit CircularEquatorial(
        const Instant& anEpoch, const Length& anAltitude, const Shared<const Celestial>& aCelestialObjectSPtr
    );

    /// @brief              Constructs a geosynchronous orbit
    ///
    ///                     Model: Kepler (J2 Perturbation).
    ///
    /// @param              [in] anEpoch An orbit epoch
    /// @param              [in] anInclination An orbit inclination
    /// @param              [in] aLongitude A longitude above the surface
    /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return             Circular orbit

    static Orbit GeoSynchronous(
        const Instant& anEpoch,
        const Angle& anInclination,
        const Angle& aLongitude,
        const Shared<const Celestial>& aCelestialObjectSPtr
    );

    /// @brief              Constructs a Sun-synchronous orbit
    ///
    ///                     Model: Kepler (J2 Perturbation).
    ///
    /// @param              [in] anEpoch An orbit epoch
    /// @param              [in] anAltitude An orbit altitude (wrt. equatorial radius)
    /// @param              [in] aLocalTimeAtDescendingNode A local time at descending node
    /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @return             Sun-synchronous orbit

    static Orbit SunSynchronous(
        const Instant& anEpoch,
        const Length& anAltitude,
        const Time& aLocalTimeAtDescendingNode,
        const Shared<const Celestial>& aCelestialObjectSPtr
    );

    static String StringFromFrameType(const Orbit::FrameType& aFrameType);

   private:
    const orbit::Model* modelPtr_;

    Shared<const Celestial> celestialObjectSPtr_;

    mutable std::mutex mutex_;
    mutable Map<Integer, Pass> passMap_;

    String generateFrameName(const Orbit::FrameType& aFrameType) const;

    static Map<Index, Pass> GeneratePassMap(const Array<State>& aStateArray, const Integer& anInitialRevolutionNumber);

    static Array<State> GenerateStates(const Model& aModel, const Array<Instant>& anInstantGrid);
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
