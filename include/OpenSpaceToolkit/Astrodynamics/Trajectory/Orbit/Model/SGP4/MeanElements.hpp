/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
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
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::trajectory::State;

/// @brief SGP4 mean element set, held at full precision.
///
/// @details A TLE is a lossy serialization of this set: writing one rounds the eccentricity to 1e-7,
/// the angles to 1e-4 deg, the mean motion to 1e-8 rev/day and the epoch to 1e-8 day.
/// That rounding is harmless for propagation but fatal for differentiation — on a near-circular
/// orbit a finite-difference step in eccentricity can be smaller than 1e-7, so the
/// propagator does not respond at all and the Jacobian silently loses rank.
///
/// Estimators should iterate on `MeanElements` and serialize a TLE once, at the end, which costs a few
/// tens of millimetres.
///
/// To propagate a TLE that was published, received, or read from a file, use the `SGP4` class.
///
/// @code{.cpp}
///     // Elements computed rather than read: they are taken as continuous values
///     const MeanElements meanElements = {
///         Instant::DateTime(DateTime::Parse("2008-09-20 12:25:40.104"), Scale::UTC),
///         Angle::Degrees(51.6416),   // inclination
///         Angle::Degrees(247.4627),  // RAAN
///         0.0006703,                 // eccentricity
///         Angle::Degrees(130.5360),  // AoP
///         Angle::Degrees(325.0288),  // mean anomaly
///         Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
///         -1.1606e-5,                // B*
///     };
///
///     const State state = meanElements.calculateStateAt(Instant::Now());
/// @endcode
class MeanElements : public ostk::astrodynamics::trajectory::orbit::Model
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     const MeanElements meanElements = {
    ///         epoch,
    ///         Angle::Degrees(51.6416),
    ///         Angle::Degrees(247.4627),
    ///         0.0006703,
    ///         Angle::Degrees(130.5360),
    ///         Angle::Degrees(325.0288),
    ///         Derived(15.72125391, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)),
    ///         -1.1606e-5,
    ///     };
    /// @endcode
    ///
    /// @param anEpoch An epoch.
    /// @param anInclination An inclination.
    /// @param aRaan A right ascension of the ascending node.
    /// @param anEccentricity An eccentricity.
    /// @param anAop An argument of periapsis.
    /// @param aMeanAnomaly A mean anomaly.
    /// @param aMeanMotion A mean motion.
    /// @param aBStarDragTerm A B* drag term.
    /// @param aRevolutionNumberAtEpoch A revolution number at epoch. Defaults to 1.
    /// @param anOutputFrameSPtr An output frame. Defaults to Frame::TEME().
    /// @throw ostk::core::error::RuntimeError If the element set cannot be propagated.
    MeanElements(
        const Instant& anEpoch,
        const Angle& anInclination,
        const Angle& aRaan,
        const Real& anEccentricity,
        const Angle& anAop,
        const Angle& aMeanAnomaly,
        const Derived& aMeanMotion,
        const Real& aBStarDragTerm,
        const Integer& aRevolutionNumberAtEpoch = 1,
        const Shared<const Frame>& anOutputFrameSPtr = Frame::TEME()
    );

    /// @brief Clone the mean element set.
    ///
    /// @code{.cpp}
    ///     MeanElements* meanElementsPtr = meanElements.clone();
    /// @endcode
    ///
    /// @return A pointer to the cloned mean element set.
    virtual MeanElements* clone() const override;

    /// @brief Equal to operator.
    ///
    /// @code{.cpp}
    ///     bool isEqual = MeanElements::FromTLE(tle) == MeanElements::FromTLE(tle);  // True
    /// @endcode
    ///
    /// @param aMeanElements A mean element set.
    /// @return True if both mean element sets are equal.
    bool operator==(const MeanElements& aMeanElements) const;

    /// @brief Not equal to operator.
    ///
    /// @code{.cpp}
    ///     bool isNotEqual = MeanElements::FromTLE(tle) != MeanElements::Undefined();  // True
    /// @endcode
    ///
    /// @param aMeanElements A mean element set.
    /// @return True if both mean element sets are not equal.
    bool operator!=(const MeanElements& aMeanElements) const;

    /// @brief Output stream operator.
    ///
    /// @code{.cpp}
    ///     std::cout << meanElements << std::endl;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param aMeanElements A mean element set.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const MeanElements& aMeanElements);

    /// @brief Check if the mean element set is defined.
    ///
    /// @code{.cpp}
    ///     bool isDefined = meanElements.isDefined();
    /// @endcode
    ///
    /// @return True if the mean element set is defined.
    virtual bool isDefined() const override;

    /// @brief Get the epoch.
    ///
    /// @code{.cpp}
    ///     Instant epoch = meanElements.getEpoch();
    /// @endcode
    ///
    /// @return The epoch.
    virtual Instant getEpoch() const override;

    /// @brief Get the inclination.
    ///
    /// @code{.cpp}
    ///     Angle inclination = meanElements.getInclination();
    /// @endcode
    ///
    /// @return The inclination.
    Angle getInclination() const;

    /// @brief Get the right ascension of the ascending node.
    ///
    /// @code{.cpp}
    ///     Angle raan = meanElements.getRaan();
    /// @endcode
    ///
    /// @return The right ascension of the ascending node.
    Angle getRaan() const;

    /// @brief Get the eccentricity.
    ///
    /// @code{.cpp}
    ///     Real eccentricity = meanElements.getEccentricity();
    /// @endcode
    ///
    /// @return The eccentricity.
    Real getEccentricity() const;

    /// @brief Get the argument of periapsis.
    ///
    /// @code{.cpp}
    ///     Angle aop = meanElements.getAop();
    /// @endcode
    ///
    /// @return The argument of periapsis.
    Angle getAop() const;

    /// @brief Get the mean anomaly.
    ///
    /// @code{.cpp}
    ///     Angle meanAnomaly = meanElements.getMeanAnomaly();
    /// @endcode
    ///
    /// @return The mean anomaly.
    Angle getMeanAnomaly() const;

    /// @brief Get the mean motion.
    ///
    /// @code{.cpp}
    ///     Real meanMotion_revPerDay = meanElements.getMeanMotion().in(
    ///         Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)
    ///     );
    /// @endcode
    ///
    /// @return The mean motion.
    Derived getMeanMotion() const;

    /// @brief Get the B* drag term.
    ///
    /// @code{.cpp}
    ///     Real bStarDragTerm = meanElements.getBStarDragTerm();
    /// @endcode
    ///
    /// @return The B* drag term.
    Real getBStarDragTerm() const;

    /// @brief Get the revolution number at epoch.
    ///
    /// @code{.cpp}
    ///     Integer revolutionNumber = meanElements.getRevolutionNumberAtEpoch();
    /// @endcode
    ///
    /// @return The revolution number at epoch.
    virtual Integer getRevolutionNumberAtEpoch() const override;

    /// @brief Get output frame.
    ///
    /// @code{.cpp}
    ///     Shared<const Frame> outputFrameSPtr = meanElements.getOutputFrame();
    /// @endcode
    ///
    /// @return The output frame.
    Shared<const Frame> getOutputFrame() const;

    /// @brief Calculate the state at a given instant.
    ///
    /// @code{.cpp}
    ///     const State state = meanElements.calculateStateAt(Instant::Now());
    /// @endcode
    ///
    /// @param anInstant An instant.
    /// @return The state at the given instant, expressed in the output frame.
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Calculate the states at given instants.
    ///
    /// @code{.cpp}
    ///     const Array<State> states = meanElements.calculateStatesAt({
    ///         meanElements.getEpoch(),
    ///         meanElements.getEpoch() + Duration::Minutes(10.0),
    ///     });
    /// @endcode
    ///
    /// @param anInstantArray An array of instants.
    /// @return The states at the given instants, expressed in the output frame.
    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const override;

    /// @brief Print the mean element set.
    ///
    /// @code{.cpp}
    ///     meanElements.print(std::cout, true);
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display the decorator.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Construct an undefined mean element set.
    ///
    /// @code{.cpp}
    ///     const MeanElements meanElements = MeanElements::Undefined();  // isDefined() is false
    /// @endcode
    ///
    /// @return An undefined mean element set.
    static MeanElements Undefined();

    /// @brief Construct a mean element set from a TLE.
    ///
    /// @details The values carried by the TLE have already been rounded by its text format;
    /// this does not recover the precision that was lost when the TLE was written.
    ///
    /// @code{.cpp}
    ///     const MeanElements meanElements = MeanElements::FromTLE(tle);
    ///     const MeanElements meanElementsGCRF = MeanElements::FromTLE(tle, Frame::GCRF());
    /// @endcode
    ///
    /// @param aTLE A TLE.
    /// @param anOutputFrameSPtr An output frame. Defaults to Frame::TEME().
    /// @return The mean element set the TLE encodes.
    static MeanElements FromTLE(const TLE& aTLE, const Shared<const Frame>& anOutputFrameSPtr = Frame::TEME());

   protected:
    virtual bool operator==(const trajectory::Model& aModel) const override;

    virtual bool operator!=(const trajectory::Model& aModel) const override;

   private:
    Instant epoch_;
    Angle inclination_;
    Angle raan_;
    Real eccentricity_;
    Angle aop_;
    Angle meanAnomaly_;
    Derived meanMotion_;
    Real bStarDragTerm_;
    Integer revolutionNumberAtEpoch_;
    Shared<const Frame> outputFrameSPtr_;

    class Impl;

    // Built at construction and never touched again, so copies can share it freely. Holding it
    // by shared pointer keeps MeanElements copyable and keeps the vendored propagator out of
    // this header. Null exactly when the element set is undefined.
    Shared<const Impl> implSPtr_;
};

}  // namespace sgp4
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
