/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{
namespace sgp4
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::trajectory::State;

/// @brief SGP4 mean element set, held at full precision.
///
/// @details A TLE is a lossy serialization of this: writing one rounds the eccentricity to
/// 1e-7 and the angles to 1e-4 degrees. That rounding is harmless for propagation but fatal
/// for differentiation — on a near-circular orbit a finite-difference step in eccentricity
/// can be smaller than the quantum, so the propagator does not respond at all and the
/// Jacobian silently loses rank. Estimators should iterate on MeanElements and serialize a
/// TLE once, at the end.
///
/// @code{.cpp}
///     const MeanElements meanElements = MeanElements::FromTLE(tle);
///     const State state = meanElements.calculateStateAt(instant, Frame::TEME());
/// @endcode
class MeanElements
{
   public:
    /// @brief Constructor.
    ///
    /// @param anEpoch An epoch.
    /// @param anInclination An inclination.
    /// @param aRaan A right ascension of the ascending node.
    /// @param anEccentricity An eccentricity.
    /// @param anAop An argument of periapsis.
    /// @param aMeanAnomaly A mean anomaly.
    /// @param aMeanMotion A mean motion.
    /// @param aBStarDragTerm A B* drag term. Defaults to 0.0.
    MeanElements(
        const Instant& anEpoch,
        const Angle& anInclination,
        const Angle& aRaan,
        const Real& anEccentricity,
        const Angle& anAop,
        const Angle& aMeanAnomaly,
        const Derived& aMeanMotion,
        const Real& aBStarDragTerm = 0.0
    );

    /// @brief Equal to operator.
    ///
    /// @param aMeanElements A mean element set.
    /// @return True if both mean element sets are equal.
    bool operator==(const MeanElements& aMeanElements) const;

    /// @brief Not equal to operator.
    ///
    /// @param aMeanElements A mean element set.
    /// @return True if both mean element sets are not equal.
    bool operator!=(const MeanElements& aMeanElements) const;

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aMeanElements A mean element set.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const MeanElements& aMeanElements);

    /// @brief Check if the mean element set is defined.
    ///
    /// @return True if the mean element set is defined.
    bool isDefined() const;

    /// @brief Get the epoch.
    ///
    /// @return The epoch.
    Instant getEpoch() const;

    /// @brief Get the inclination.
    ///
    /// @return The inclination.
    Angle getInclination() const;

    /// @brief Get the right ascension of the ascending node.
    ///
    /// @return The right ascension of the ascending node.
    Angle getRaan() const;

    /// @brief Get the eccentricity.
    ///
    /// @return The eccentricity.
    Real getEccentricity() const;

    /// @brief Get the argument of periapsis.
    ///
    /// @return The argument of periapsis.
    Angle getAop() const;

    /// @brief Get the mean anomaly.
    ///
    /// @return The mean anomaly.
    Angle getMeanAnomaly() const;

    /// @brief Get the mean motion.
    ///
    /// @return The mean motion.
    Derived getMeanMotion() const;

    /// @brief Get the B* drag term.
    ///
    /// @return The B* drag term.
    Real getBStarDragTerm() const;

    /// @brief Calculate the state at a given instant.
    ///
    /// @param anInstant An instant.
    /// @param anOutputFrameSPtr An output frame. Defaults to TEME.
    /// @return The state at the given instant.
    State calculateStateAt(const Instant& anInstant, const Shared<const Frame>& anOutputFrameSPtr = Frame::TEME())
        const;

    /// @brief Calculate the states at given instants.
    ///
    /// @param anInstantArray An array of instants.
    /// @param anOutputFrameSPtr An output frame. Defaults to TEME.
    /// @return The states at the given instants.
    Array<State> calculateStatesAt(
        const Array<Instant>& anInstantArray, const Shared<const Frame>& anOutputFrameSPtr = Frame::TEME()
    ) const;

    /// @brief Print the mean element set.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display the decorator.
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Construct an undefined mean element set.
    ///
    /// @return An undefined mean element set.
    static MeanElements Undefined();

    /// @brief Construct a mean element set from a TLE.
    ///
    /// @details The values carried by the TLE have already been rounded by its text format;
    /// this does not recover the precision that was lost when the TLE was written.
    ///
    /// @param aTLE A TLE.
    /// @return The mean element set the TLE encodes.
    static MeanElements FromTLE(const TLE& aTLE);

   private:
    Instant epoch_;
    Angle inclination_;
    Angle raan_;
    Real eccentricity_;
    Angle aop_;
    Angle meanAnomaly_;
    Derived meanMotion_;
    Real bStarDragTerm_;

    class Impl;

    // Built on first propagation, then reused. Holding it by shared pointer keeps
    // MeanElements copyable and keeps the vendored propagator out of this header.
    mutable Shared<const Impl> implSPtr_;

    const Impl& accessImpl() const;
};

}  // namespace sgp4
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
