/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access_Generator__
#define __OpenSpaceToolkit_Astrodynamics_Access_Generator__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Interval.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/AER.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace access
{

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Interval;
using ostk::mathematics::object::Matrix3d;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::AER;
using ostk::physics::Environment;
using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::Access;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::State;

#define DEFAULT_STEP Duration::Minutes(1.0)
#define DEFAULT_TOLERANCE Duration::Microseconds(1.0)

/// @brief Represents the configuration for a ground target, including azimuth, elevation, and range intervals, as well
/// as position and LLA (Latitude, Longitude, Altitude).
///
/// @details This class provides methods to retrieve the trajectory, position, LLA, and intervals for azimuth,
/// elevation, and range. It also includes a method to get the SEZ (South-East-Zenith) rotation matrix.
struct GroundTargetConfiguration
{
    /// @brief Constructor
    /// @param anAzimuthInterval An azimuth interval [deg]
    /// @param anElevationInterval An elevation interval [deg]
    /// @param aRangeInterval A range interval [m]
    /// @param aPosition A position
    GroundTargetConfiguration(
        const Interval<Real>& anAzimuthInterval,
        const Interval<Real>& anElevationInterval,
        const Interval<Real>& aRangeInterval,
        const Position& aPosition
    );

    /// @brief Constructor
    /// @param anAzimuthInterval An azimuth interval [deg]
    /// @param anElevationInterval An elevation interval [deg]
    /// @param aRangeInterval A range interval [m]
    /// @param aLLA An LLA
    GroundTargetConfiguration(
        const Interval<Real>& anAzimuthInterval,
        const Interval<Real>& anElevationInterval,
        const Interval<Real>& aRangeInterval,
        const LLA& aLLA
    );

    /// @brief Get the trajectory
    ///
    /// @code{.cpp}
    ///              GroundStationConfiguration groundStationConfiguration = { ... } ;
    ///              Trajectory trajectory = groundStationConfiguration.getTrajectory();
    /// @endcode
    ///
    /// @return The trajectory
    Trajectory getTrajectory() const;

    /// @brief Get the position
    ///
    /// @code{.cpp}
    ///              GroundStationConfiguration groundStationConfiguration = { ... } ;
    ///              Position position = groundStationConfiguration.getPosition();
    /// @endcode
    ///
    /// @return The position
    Position getPosition() const;

    /// @brief Get the latitude, longitude, and altitude (LLA)
    ///
    /// @code{.cpp}
    ///              GroundStationConfiguration groundStationConfiguration = { ... } ;
    ///              LLA lla = groundStationConfiguration.getLLA();
    /// @endcode
    ///
    /// @return The latitude, longitude, and altitude (LLA)
    LLA getLLA() const;

    /// @brief Get the azimuth interval
    ///
    /// @code{.cpp}
    ///              GroundStationConfiguration groundStationConfiguration = { ... } ;
    ///              Interval<Real> groundStationConfiguration = generator.getAzimuthInterval();
    /// @endcode
    ///
    /// @return The azimuth interval
    Interval<Real> getAzimuthInterval() const;

    /// @brief Get the elevation interval
    ///
    /// @code{.cpp}
    ///              GroundStationConfiguration groundStationConfiguration = { ... } ;
    ///              Interval<Real> groundStationConfiguration = generator.getElevationInterval();
    /// @endcode
    ///
    /// @return The elevation interval
    Interval<Real> getElevationInterval() const;

    /// @brief Get the range interval
    ///
    /// @code{.cpp}
    ///              GroundStationConfiguration groundStationConfiguration = { ... } ;
    ///              Interval<Real> groundStationConfiguration = generator.getRangeInterval();
    /// @endcode
    ///
    /// @return The range interval
    Interval<Real> getRangeInterval() const;

    /// @brief Get the rotation matrix (Matrix3d) from ECEF (Earth-Centered-Earth-Fixed) to SEZ (South-East-Zenith)
    /// frame
    ///
    /// @code{.cpp}
    ///              GroundStationConfiguration groundStationConfiguration = { ... } ;
    ///              Matrix3d sezRotation = groundStationConfiguration.getR_SEZ_ECEF();
    /// @endcode
    ///
    /// @return The SEZ rotation matrix
    Matrix3d getR_SEZ_ECEF() const;

   private:
    Interval<Real> azimuthInterval_;
    Interval<Real> elevationInterval_;
    Interval<Real> rangeInterval_;

    Position position_;
    LLA lla_;
};

class Generator
{
   public:
    Generator(
        const Environment& anEnvironment,
        const Duration& aStep = DEFAULT_STEP,
        const Duration& aTolerance = DEFAULT_TOLERANCE
    );

    Generator(
        const Environment& anEnvironment,
        const std::function<bool(const AER&)>& anAerFilter,
        const std::function<bool(const Access&)>& anAccessFilter = {},
        const std::function<bool(const State&, const State&)>& aStateFilter = {},
        const Duration& aStep = DEFAULT_STEP,
        const Duration& aTolerance = DEFAULT_TOLERANCE
    );

    bool isDefined() const;

    Duration getStep() const;

    Duration getTolerance() const;

    std::function<bool(const AER&)> getAerFilter() const;

    std::function<bool(const Access&)> getAccessFilter() const;

    std::function<bool(const State&, const State&)> getStateFilter() const;

    std::function<bool(const Instant&)> getConditionFunction(
        const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
    ) const;

    Array<Access> computeAccesses(
        const physics::time::Interval& anInterval, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
    ) const;

    Array<Array<Access>> computeAccessesWithGroundTargets(
        const physics::time::Interval& anInterval,
        const Array<GroundTargetConfiguration>& someGroundTargetConfigurations,
        const Trajectory& aToTrajectory
    ) const;

    void setStep(const Duration& aStep);

    void setTolerance(const Duration& aTolerance);

    void setAerFilter(const std::function<bool(const AER&)>& anAerFilter);

    void setAccessFilter(const std::function<bool(const Access&)>& anAccessFilter);

    void setStateFilter(const std::function<bool(const State&, const State&)>& aStateFilter);

    static Generator Undefined();

    /// @brief Construct an access generator with defined AER ranges
    ///
    /// @param anAzimuthRange An azimuth interval [deg]
    /// @param anElevationRange An elevation interval [deg]
    /// @param aRangeRange A range interval [m]
    /// @param anEnvironment An environment
    /// @return An access generator
    static Generator AerRanges(
        const Interval<Real>& anAzimuthRange,
        const Interval<Real>& anElevationRange,
        const Interval<Real>& aRangeRange,
        const Environment& anEnvironment
    );

    /// @brief Construct an access generator with a defined AER mask
    ///
    /// @param anAzimuthElevationMask An azimuth-elevation mask [deg]
    /// @param aRangeRange A range interval [m]
    /// @param anEnvironment An environment
    /// @return An access generator
    static Generator AerMask(
        const Map<Real, Real>& anAzimuthElevationMask,
        const Interval<Real>& aRangeRange,
        const Environment& anEnvironment
    );

   private:
    Environment environment_;

    Duration step_;
    Duration tolerance_;

    std::function<bool(const AER&)> aerFilter_;
    std::function<bool(const Access&)> accessFilter_;
    std::function<bool(const State&, const State&)> stateFilter_;

    Array<Access> generateAccessesFromIntervals(
        const Array<physics::time::Interval>& someIntervals,
        const physics::time::Interval& anInterval,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory
    ) const;

    static Access GenerateAccess(
        const physics::time::Interval& anAccessInterval,
        const physics::time::Interval& aGlobalInterval,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const Shared<const Celestial> anEarthSPtr,
        const Duration& aTolerance
    );

    static Instant FindTimeOfClosestApproach(
        const physics::time::Interval& anAccessInterval,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const Duration& aTolerance
    );

    static Angle CalculateElevationAt(
        const Instant& anInstant,
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const Shared<const Celestial> anEarthSPtr
    );
};

class GeneratorContext
{
   public:
    GeneratorContext(
        const Trajectory& aFromTrajectory,
        const Trajectory& aToTrajectory,
        const Environment& anEnvironment,
        const Generator& aGenerator
    );

    bool isAccessActive(const Instant& anInstant);

    static Pair<State, State> GetStatesAt(
        const Instant& anInstant, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
    );

    static Pair<Position, Position> GetPositionsFromStates(const State& aFromState, const State& aToState);

    static AER CalculateAer(
        const Instant& anInstant,
        const Position& aFromPosition,
        const Position& aToPosition,
        const Shared<const Celestial> anEarthSPtr
    );

   private:
    Trajectory fromTrajectory_;
    Trajectory toTrajectory_;
    Environment environment_;
    const Shared<const Celestial> earthSPtr_;

    Generator generator_;
};

}  // namespace access
}  // namespace astrodynamics
}  // namespace ostk

#endif
