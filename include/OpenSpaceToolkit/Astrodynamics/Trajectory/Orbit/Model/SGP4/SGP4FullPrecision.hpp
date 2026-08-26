/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision__

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

/// @brief An Orbit Model that holds SGP4 mean elements at full precision.
///
/// @details This uses the SGP4 propagator, but with the orbital elements stored at machine-precision rather
/// than the truncated precision of the TLE format.
///
/// @code{.cpp}
///     // Elements computed rather than read: they are taken as continuous values
///     const SGP4FullPrecision sgp4FullPrecision = {
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
///     const State state = sgp4FullPrecision.calculateStateAt(Instant::Now());
/// @endcode
class SGP4FullPrecision : public ostk::astrodynamics::trajectory::orbit::Model
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     const SGP4FullPrecision sgp4FullPrecision = {
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
    SGP4FullPrecision(
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

    /// @brief Copy constructor.
    ///
    /// @details The copy propagates independently of its source: it gets its own propagator
    /// state rather than sharing it.
    ///
    /// @param aSGP4FullPrecision A full precision sgp4 model.
    SGP4FullPrecision(const SGP4FullPrecision& aSGP4FullPrecision);

    /// @brief Copy assignment operator.
    ///
    /// @param aSGP4FullPrecision A full precision sgp4 model.
    /// @return A reference to this full precision sgp4 model.
    SGP4FullPrecision& operator=(const SGP4FullPrecision& aSGP4FullPrecision);

    /// @brief Move constructor.
    SGP4FullPrecision(SGP4FullPrecision&& aSGP4FullPrecision) = default;

    /// @brief Move assignment operator.
    ///
    /// @return A reference to this full precision sgp4 model.
    SGP4FullPrecision& operator=(SGP4FullPrecision&& aSGP4FullPrecision) = default;

    /// @brief Clone the full precision sgp4 model.
    ///
    /// @code{.cpp}
    ///     SGP4FullPrecision* sgp4FullPrecisionPtr = sgp4FullPrecision.clone();
    /// @endcode
    ///
    /// @return A pointer to the cloned full precision sgp4 model.
    virtual SGP4FullPrecision* clone() const override;

    /// @brief Equal to operator.
    ///
    /// @code{.cpp}
    ///     bool isEqual = SGP4FullPrecision::FromTLE(tle) == SGP4FullPrecision::FromTLE(tle);  // True
    /// @endcode
    ///
    /// @param aSGP4FullPrecision A full precision sgp4 model.
    /// @return True if both full precision sgp4 models are equal.
    bool operator==(const SGP4FullPrecision& aSGP4FullPrecision) const;

    /// @brief Not equal to operator.
    ///
    /// @code{.cpp}
    ///     bool isNotEqual = SGP4FullPrecision::FromTLE(tle) != SGP4FullPrecision::Undefined();  // True
    /// @endcode
    ///
    /// @param aSGP4FullPrecision A full precision sgp4 model.
    /// @return True if both full precision sgp4 models are not equal.
    bool operator!=(const SGP4FullPrecision& aSGP4FullPrecision) const;

    /// @brief Output stream operator.
    ///
    /// @code{.cpp}
    ///     std::cout << sgp4FullPrecision << std::endl;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param aSGP4FullPrecision A full precision sgp4 model.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const SGP4FullPrecision& aSGP4FullPrecision);

    /// @brief Check if the full precision sgp4 model is defined.
    ///
    /// @code{.cpp}
    ///     bool isDefined = sgp4FullPrecision.isDefined();
    /// @endcode
    ///
    /// @return True if the full precision sgp4 model is defined.
    virtual bool isDefined() const override;

    /// @brief Get the epoch.
    ///
    /// @code{.cpp}
    ///     Instant epoch = sgp4FullPrecision.getEpoch();
    /// @endcode
    ///
    /// @return The epoch.
    virtual Instant getEpoch() const override;

    /// @brief Get the inclination.
    ///
    /// @code{.cpp}
    ///     Angle inclination = sgp4FullPrecision.getInclination();
    /// @endcode
    ///
    /// @return The inclination.
    Angle getInclination() const;

    /// @brief Get the right ascension of the ascending node.
    ///
    /// @code{.cpp}
    ///     Angle raan = sgp4FullPrecision.getRaan();
    /// @endcode
    ///
    /// @return The right ascension of the ascending node.
    Angle getRaan() const;

    /// @brief Get the eccentricity.
    ///
    /// @code{.cpp}
    ///     Real eccentricity = sgp4FullPrecision.getEccentricity();
    /// @endcode
    ///
    /// @return The eccentricity.
    Real getEccentricity() const;

    /// @brief Get the argument of periapsis.
    ///
    /// @code{.cpp}
    ///     Angle aop = sgp4FullPrecision.getAop();
    /// @endcode
    ///
    /// @return The argument of periapsis.
    Angle getAop() const;

    /// @brief Get the mean anomaly.
    ///
    /// @code{.cpp}
    ///     Angle meanAnomaly = sgp4FullPrecision.getMeanAnomaly();
    /// @endcode
    ///
    /// @return The mean anomaly.
    Angle getMeanAnomaly() const;

    /// @brief Get the mean motion.
    ///
    /// @code{.cpp}
    ///     Real meanMotion_revPerDay = sgp4FullPrecision.getMeanMotion().in(
    ///         Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)
    ///     );
    /// @endcode
    ///
    /// @return The mean motion.
    Derived getMeanMotion() const;

    /// @brief Get the B* drag term.
    ///
    /// @code{.cpp}
    ///     Real bStarDragTerm = sgp4FullPrecision.getBStarDragTerm();
    /// @endcode
    ///
    /// @return The B* drag term.
    Real getBStarDragTerm() const;

    /// @brief Get the revolution number at epoch.
    ///
    /// @code{.cpp}
    ///     Integer revolutionNumber = sgp4FullPrecision.getRevolutionNumberAtEpoch();
    /// @endcode
    ///
    /// @return The revolution number at epoch.
    virtual Integer getRevolutionNumberAtEpoch() const override;

    /// @brief Get output frame.
    ///
    /// @code{.cpp}
    ///     Shared<const Frame> outputFrameSPtr = sgp4FullPrecision.getOutputFrame();
    /// @endcode
    ///
    /// @return The output frame.
    Shared<const Frame> getOutputFrame() const;

    /// @brief Calculate the state at a given instant.
    ///
    /// @code{.cpp}
    ///     const State state = sgp4FullPrecision.calculateStateAt(Instant::Now());
    /// @endcode
    ///
    /// @param anInstant An instant.
    /// @return The state at the given instant, expressed in the output frame.
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Calculate the states at given instants.
    ///
    /// @code{.cpp}
    ///     const Array<State> states = sgp4FullPrecision.calculateStatesAt({
    ///         sgp4FullPrecision.getEpoch(),
    ///         sgp4FullPrecision.getEpoch() + Duration::Minutes(10.0),
    ///     });
    /// @endcode
    ///
    /// @param anInstantArray An array of instants.
    /// @return The states at the given instants, expressed in the output frame.
    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const override;

    /// @brief Print the full precision sgp4 model.
    ///
    /// @code{.cpp}
    ///     sgp4FullPrecision.print(std::cout, true);
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display the decorator.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Construct an undefined full precision sgp4 model.
    ///
    /// @code{.cpp}
    ///     const SGP4FullPrecision sgp4FullPrecision = SGP4FullPrecision::Undefined();  // isDefined() is false
    /// @endcode
    ///
    /// @return An undefined full precision sgp4 model.
    static SGP4FullPrecision Undefined();

    /// @brief Construct a full precision sgp4 model from a TLE.
    ///
    /// @details The values carried by the TLE have already been rounded by its text format;
    /// this does not recover the precision that was lost when the TLE was written.
    ///
    /// @code{.cpp}
    ///     const SGP4FullPrecision sgp4FullPrecision = SGP4FullPrecision::FromTLE(tle);
    ///     const SGP4FullPrecision sgp4FullPrecisionGCRF = SGP4FullPrecision::FromTLE(tle, Frame::GCRF());
    /// @endcode
    ///
    /// @param aTLE A TLE.
    /// @param anOutputFrameSPtr An output frame. Defaults to Frame::TEME().
    /// @return The full precision sgp4 model the TLE encodes.
    static SGP4FullPrecision FromTLE(const TLE& aTLE, const Shared<const Frame>& anOutputFrameSPtr = Frame::TEME());

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

    // Holds the vendored propagator, by pointer so that it stays out of this header. Null
    // exactly when the element set is undefined.
    //
    // The propagator carries state across calls — where its deep-space integrator has reached —
    // so this is not shared between copies: copying the model copies it. Shared rather than
    // unique only because that keeps the pointee const and the class copyable in one step.
    Shared<const Impl> implSPtr_;

    static Shared<const Impl> CopyImpl(const Shared<const Impl>& anImplSPtr);
};

}  // namespace sgp4
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
