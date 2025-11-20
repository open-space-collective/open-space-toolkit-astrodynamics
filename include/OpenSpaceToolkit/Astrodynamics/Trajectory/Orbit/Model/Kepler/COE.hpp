/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_COE__

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

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
namespace kepler
{

using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector6d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

/// @brief Classical Orbital Elements (COE)
///
/// @ref https://en.wikipedia.org/wiki/Orbital_elements
/// @ref http://help.agi.com/stk/index.htm#stk/vehSat_coordType_classical.htm
class COE
{
   public:
    enum class Element
    {
        SemiMajorAxis,
        Eccentricity,
        Inclination,
        Raan,
        Aop,
        TrueAnomaly,
        MeanAnomaly,
        EccentricAnomaly,
        ArgumentOfLatitude
    };

    enum class AnomalyType
    {
        True,
        Mean,
        Eccentric,
    };

    typedef Pair<Position, Velocity> CartesianState;

    /// @brief Constructor
    ///
    /// @param aSemiMajorAxis A semi-major axis
    /// @param anEccentricity An eccentricity
    /// @param anInclination An inclination
    /// @param aRaan A raan
    /// @param anAop An aop
    /// @param aTrueAnomaly A true anomaly
    COE(const Length& aSemiMajorAxis,
        const Real& anEccentricity,
        const Angle& anInclination,
        const Angle& aRaan,
        const Angle& anAop,
        const Angle& aTrueAnomaly);

    /// @brief Equal to operator
    ///
    /// @param aCOE A set of COE
    /// @return True if COEs are equal
    bool operator==(const COE& aCOE) const;

    /// @brief Not Equal to operator
    ///
    /// @param aCOE A set of COE
    /// @return True if COEs are not equal
    bool operator!=(const COE& aCOE) const;

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << COE(...);
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aCOE A set of COE
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const COE& aCOE);

    /// @brief Check if COE is defined
    ///
    /// @return True if COE is defined
    bool isDefined() const;

    /// @brief Get Semi Major Axis
    ///
    /// @return Semi Major Axis
    Length getSemiMajorAxis() const;

    /// @brief Get Eccentricity
    ///
    /// @return Eccentricity
    Real getEccentricity() const;

    /// @brief Get Inclination
    ///
    /// @return Inclination
    Angle getInclination() const;

    /// @brief Get Right ascenscion of the ascending node
    ///
    /// @return Right ascenscion of the ascending node
    Angle getRaan() const;

    /// @brief Get Argument of periapsis
    ///
    /// @return Argument of periapsis
    Angle getAop() const;

    /// @brief Get Argument of latitude
    ///
    /// @return Argument of latitude
    Angle getArgumentOfLatitude() const;

    /// @brief Get True anomaly
    ///
    /// @return True anomaly
    virtual Angle getTrueAnomaly() const;

    /// @brief Get Mean anomaly
    ///
    /// @return Mean anomaly
    virtual Angle getMeanAnomaly() const;

    /// @brief Get Eccentric anomaly
    ///
    /// @return Eccentric anomaly
    virtual Angle getEccentricAnomaly() const;

    /// @brief Get Periapsis radius
    ///
    /// @return Periapsis radius
    Length getPeriapsisRadius() const;

    /// @brief Get Apoapsis radius
    ///
    /// @return Apoapsis radius
    Length getApoapsisRadius() const;

    /// @brief Get Semi-Latus Rectum
    ///
    /// @return Semi-Latus Rectum
    Length getSemiLatusRectum() const;

    /// @brief Get Radial Distance
    ///
    /// @return Radiaul Distance
    Length getRadialDistance() const;

    /// @brief Get Angular Momentum
    ///
    /// @param aGravitationalParameter A gravitational parameter
    /// @return Angular Momentum
    Derived getAngularMomentum(const Derived& aGravitationalParameter) const;

    /// @brief Get Mean motion
    ///
    /// @param aGravitationalParameter A gravitational parameter
    /// @return Mean motion
    Derived getMeanMotion(const Derived& aGravitationalParameter) const;

    /// @brief Get Nodal precession rate
    ///
    /// @param aGravitationalParameter A gravitational parameter
    /// @param anEquatorialRadius An equatorial radius
    /// @param aJ2Parameter
    /// @return Nodal precession rate
    Derived getNodalPrecessionRate(
        const Derived& aGravitationalParameter, const Length& anEquatorialRadius, const Real& aJ2Parameter
    ) const;

    /// @brief Get Orbital period
    ///
    /// @param aGravitationalParameter A gravitational parameter
    /// @return Orbital period
    Duration getOrbitalPeriod(const Derived& aGravitationalParameter) const;

    /// @brief Get Cartesian state
    ///
    /// @param aGravitationalParameter A gravitational parameter
    /// @param aFrameSPtr A frame
    /// @return Cartesian state
    COE::CartesianState getCartesianState(const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr)
        const;

    /// @brief Get vector of elements in SI units
    ///
    /// @param anAnomalyType An anomaly type
    /// @return Vector
    Vector6d getSIVector(const AnomalyType& anAnomalyType) const;

    /// @brief Print COE
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Construct an undefined COE
    ///
    /// @code{.cpp}
    ///                  COE coe = COE::Undefined();
    /// @endcode
    ///
    /// @return Undefined coe
    static COE Undefined();

    /// @brief Construct a COE from a cartesian state
    ///
    /// @param aCartesianState A cartesian state
    /// @param aGravitationalParameter A gravitational parameter
    /// @return COE
    static COE Cartesian(const COE::CartesianState& aCartesianState, const Derived& aGravitationalParameter);

    /// @brief Construct a COE from a vector
    ///
    /// @param aCOEVector A vector
    /// @param anAnomalyType An anomaly type
    /// @return COE
    static COE FromSIVector(const Vector6d& aCOEVector, const AnomalyType& anAnomalyType);

    /// @brief Construct a frozen orbit from an incomplete set of COEs
    ///
    /// The critical angles for inclination are 63.4349 degrees and 116.5651 degrees.
    /// The critical angles for AoP are 90.0 degrees and 270.0 degrees.
    ///
    /// At a minimum, a semi-major axis and shared pointer to a central celestial body with a defined J2 and J3
    /// must be provided. In this case, the inclination and AoP are set to critical angles, and the eccentricity
    /// is derived from inclination. RAAN and true anomaly default to zero degrees.
    ///
    /// Additionally, the following combinations of inputs are supported:
    /// - AoP (inclination set to critical value, eccentricity derived)
    /// - AoP and eccentricity (inclination derived)
    /// - AoP and inclination, but at least one of them must be a critical value (eccentricity derived)
    /// - Inclination (AoP set to critical value, eccentricity derived)
    /// - Eccentricity (AoP set to critical value, inclination derived)
    ///
    /// Note that inclination and eccentricity cannot both be provided.
    ///
    /// RAAN and True Anomaly may be provided alongside any of these arguments, and will be passed through
    /// to the resulting COE as they do not impact the frozen orbit condition.
    ///
    /// @param aSemiMajorAxis A semi-major axis
    /// @param aCelestialObjectSPtr A shared pointer to a central celestial body
    /// @param anEccentricity An eccentricity
    /// @param anInclination An inclination
    /// @param aRaan A raan
    /// @param anAop An aop
    /// @param aTrueAnomaly A true anomaly
    /// @return COE
    static COE FrozenOrbit(
        const Length& aSemiMajorAxis,
        const Shared<const Celestial>& aCelestialObjectSPtr,
        const Real& anEccentricity = Real::Undefined(),
        const Angle& anInclination = Angle::Undefined(),
        const Angle& aRaan = Angle::Degrees(0.0),
        const Angle& anAop = Angle::Undefined(),
        const Angle& aTrueAnomaly = Angle::Degrees(0.0)
    );

    /// @brief Construct a frozen orbit from an incomplete set of COEs
    ///
    /// The critical angles for inclination are 63.4349 degrees and 116.5651 degrees.
    /// The critical angles for AoP are 90.0 degrees and 270.0 degrees.
    ///
    /// At a minimum, a semi-major axis, equatorial radius, J2, and J3 must be provided. In this case,
    /// the inclination and AoP are set to critical angles, and the eccentricity is derived from inclination.
    /// RAAN and true anomaly default to zero degrees.
    ///
    /// Additionally, the following combinations of inputs are supported:
    /// - AoP (inclination set to critical value, eccentricity derived)
    /// - AoP and eccentricity (inclination derived)
    /// - AoP and inclination, but at least one of them must be a critical value (eccentricity derived)
    /// - Inclination (AoP set to critical value, eccentricity derived)
    /// - Eccentricity (AoP set to critical value, inclination derived)
    ///
    /// Note that inclination and eccentricity cannot both be provided.
    ///
    /// RAAN and True Anomaly may be provided alongside any of these arguments, and will be passed through
    /// to the resulting COE as they do not impact the frozen orbit condition.
    ///
    /// @param aSemiMajorAxis A semi-major axis
    /// @param anEquatorialRadius An equatorial radius
    /// @param aJ2 A J2
    /// @param aJ3 A J3
    /// @param anEccentricity An eccentricity
    /// @param anInclination An inclination
    /// @param aRaan A raan
    /// @param anAop An aop
    /// @param aTrueAnomaly A true anomaly
    /// @return COE
    static COE FrozenOrbit(
        const Length& aSemiMajorAxis,
        const Length& anEquatorialRadius,
        const Real& aJ2,
        const Real& aJ3,
        const Real& anEccentricity = Real::Undefined(),
        const Angle& anInclination = Angle::Undefined(),
        const Angle& aRaan = Angle::Degrees(0.0),
        const Angle& anAop = Angle::Undefined(),
        const Angle& aTrueAnomaly = Angle::Degrees(0.0)
    );

    /// @brief Convert True anomaly to Eccentric anomaly
    ///
    /// @param aTrueAnomaly A true anomaly
    /// @param anEccentricity An eccentricity
    /// @return Eccentric anomaly
    static Angle EccentricAnomalyFromTrueAnomaly(const Angle& aTrueAnomaly, const Real& anEccentricity);

    /// @brief Convert Eccentric anomaly to True anomaly
    ///
    /// @param anEccentricAnomaly An eccentric anomaly
    /// @param anEccentricity An eccentricity
    /// @return True anomaly
    static Angle TrueAnomalyFromEccentricAnomaly(const Angle& anEccentricAnomaly, const Real& anEccentricity);

    /// @brief Convert Mean anomaly to Eccentric anomaly
    ///
    /// @param aMeanAnomaly A mean anomaly
    /// @param anEccentricity An eccentricity
    /// @return Eccentric anomaly
    static Angle MeanAnomalyFromEccentricAnomaly(const Angle& anEccentricAnomaly, const Real& anEccentricity);

    /// @brief Convert Eccentric anomaly to Mean anomaly
    ///
    /// @param anEccentricAnomaly An eccentric anomaly
    /// @param anEccentricity An eccentricity
    /// @param aTolerance a tolerance
    /// @return Mean anomaly
    static Angle EccentricAnomalyFromMeanAnomaly(
        const Angle& aMeanAnomaly, const Real& anEccentricity, const Real& aTolerance
    );

    /// @brief Convert Mean anomaly to True anomaly
    ///
    /// @param aMeanAnomaly A mean anomaly
    /// @param anEccentricity An eccentricity
    /// @param aTolerance a tolerance
    /// @return True anomaly
    static Angle TrueAnomalyFromMeanAnomaly(
        const Angle& aMeanAnomly, const Real& anEccentricity, const Real& aTolerance
    );

    /// @brief Compute the semi-latus rectum of the orbit.
    ///
    /// @param aSemiMajorAxis Semi-major axis of the orbit in meters.
    /// @param anEccentricity Eccentricity of the orbit.
    /// @return Semi-latus rectum in meters.
    static Real ComputeSemiLatusRectum(const Real& aSemiMajorAxis, const Real& anEccentricity);

    /// @brief Compute the angular momentum of the orbit.
    ///
    /// @param aSemiMajorAxis Semi-major axis of the orbit in meters.
    /// @param anEccentricity Eccentricity of the orbit.
    /// @param aGravitationalParameter Gravitational parameter.
    /// @return Angular momentum.
    static Real ComputeAngularMomentum(
        const Real& aSemiMajorAxis, const Real& anEccentricity, const Derived& aGravitationalParameter
    );

    /// @brief Compute the angular momentum of the orbit.
    ///
    /// @param aSemiLatusRectum Semi-latus rectum of the orbit in meters.
    /// @param aGravitationalParameter Gravitational parameter.
    /// @return Angular momentum.
    static Real ComputeAngularMomentum(const Real& aSemiLatusRectum, const Derived& aGravitationalParameter);

    /// @brief Compute the radial distance at a given true anomaly.
    ///
    /// @param aSemiMajorAxis Semi-major axis of the orbit in meters.
    /// @param anEccentricity Eccentricity of the orbit.
    /// @param trueAnomaly True anomaly in radians.
    /// @return Radial distance in meters.
    static Real ComputeRadialDistance(const Real& aSemiMajorAxis, const Real& anEccentricity, const Real& trueAnomaly);

    /// @brief Compute Mean Local Time of the Ascending Node (MLTAN) from RAAN and instant
    ///
    /// @param raan Right Ascension of the Ascending Node (mean element, not osculating element)
    /// @param anInstant The instant at which to compute LTAN
    /// @return Mean Local Time of the Ascending Node (MLTAN) in hours
    static Time ComputeMeanLTAN(const Angle& raan, const Instant& anInstant, const Sun& sun = Sun::Default());

    /// @brief Compute Mean Local Time of the Descending Node (MLTDN) from RAAN and instant
    ///
    /// @param raan Right Ascension of the Ascending Node (mean element, not osculating element)
    /// @param anInstant The instant at which to compute LTAN
    /// @return Mean Local Time of the Descending Node (MLTDN) in hours
    static Time ComputeMeanLTDN(const Angle& raan, const Instant& anInstant, const Sun& sun = Sun::Default());

    /// @brief Compute Local Time of the Ascending Node (LTAN) from RAAN and instant
    ///
    /// @param raan Right Ascension of the Ascending Node (mean element, not osculating element)
    /// @param anInstant The instant at which to compute LTAN
    /// @return Local Time of the Ascending Node (LTAN) in hours
    static Time ComputeLTAN(const Angle& raan, const Instant& anInstant, const Sun& sun = Sun::Default());

    /// @brief Compute Local Time of the Descending Node (LTDN) from RAAN and instant
    ///
    /// @param raan Right Ascension of the Ascending Node (mean element, not osculating element)
    /// @param anInstant The instant at which to compute LTAN
    /// @return Local Time of the Descending Node (LTDN) in hours
    static Time ComputeLTDN(const Angle& raan, const Instant& anInstant, const Sun& sun = Sun::Default());

    /// @brief Convert element to string
    ///
    /// @param anElement An element
    /// @return String representing the element
    static String StringFromElement(const COE::Element& anElement);

   protected:
    Length semiMajorAxis_;
    Real eccentricity_;
    Angle inclination_;
    Angle raan_;
    Angle aop_;
    Angle anomaly_;
    AnomalyType anomalyType_;

    /// @brief Constructor
    ///
    /// @param aSemiMajorAxis A semi-major axis
    /// @param anEccentricity An eccentricity
    /// @param anInclination An inclination
    /// @param aRaan A raan
    /// @param anAop An aop
    /// @param anAnomaly An anomaly
    /// @param anAnomalyType An anomaly type
    COE(const Length& aSemiMajorAxis,
        const Real& anEccentricity,
        const Angle& anInclination,
        const Angle& aRaan,
        const Angle& anAop,
        const Angle& anAnomaly,
        const AnomalyType& anAnomalyType);

   private:
    /// @brief Convert anomaly
    ///
    /// @param anAnomaly An anomaly
    /// @param anEccentricity An eccentricity
    /// @param fromAnomalyType An anomaly type
    /// @param toAnomalyType An anomaly type
    /// @param aTolerance a tolerance
    /// @return Anomaly
    static Angle ConvertAnomaly(
        const Angle& anAnomaly,
        const Real& anEccentricity,
        const AnomalyType& fromAnomalyType,
        const AnomalyType& toAnomalyType,
        const Real& aTolerance
    );

    /// @brief Compute the equation of time
    ///
    /// @param anInstant The instant at which to compute the equation of time
    /// @return The equation of time as an angle
    static Angle ComputeEquationOfTime(const Instant& anInstant);
};

}  // namespace kepler
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
