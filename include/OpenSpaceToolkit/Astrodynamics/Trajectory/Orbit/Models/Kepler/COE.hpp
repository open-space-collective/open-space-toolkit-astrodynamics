/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE__

#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{
namespace kepler
{

using ostk::core::ctnr::Pair;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::Vector6d;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

class BrouwerLyddaneMeanLong;   // Forward declaration
class BrouwerLyddaneMeanShort;  // Forward declaration

/// @brief                      Classical Orbital Elements (COE)
///
/// @ref                        https://en.wikipedia.org/wiki/Orbital_elements
/// @ref                        http://help.agi.com/stk/index.htm#stk/vehSat_coordType_classical.htm

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
        EccentricAnomaly
    };

    enum class AnomalyType
    {
        True,
        Mean,
        Eccentric,
    };

    typedef Pair<Position, Velocity> CartesianState;

    /// @brief                  Constructor
    ///
    /// @param                  [in] aSemiMajorAxis A semi-major axis
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] anInclination An inclination
    /// @param                  [in] aRaan A raan
    /// @param                  [in] anAop An aop
    /// @param                  [in] aTrueAnomaly A true anomaly

    COE(const Length& aSemiMajorAxis,
        const Real& anEccentricity,
        const Angle& anInclination,
        const Angle& aRaan,
        const Angle& anAop,
        const Angle& aTrueAnomaly);

    /// @brief                  Equal to operator
    ///
    /// @param                  [in] aCOE A set of COE
    /// @return                 True if COEs are equal

    bool operator==(const COE& aCOE) const;

    /// @brief                  Not Equal to operator
    ///
    /// @param                  [in] aCOE A set of COE
    /// @return                 True if COEs are not equal

    bool operator!=(const COE& aCOE) const;

    /// @brief                  Output stream operator
    ///
    /// @code
    ///                         std::cout << COE(...) ;
    /// @endcode
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] aCOE A set of COE
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const COE& aCOE);

    /// @brief                  Check if COE is defined
    ///
    /// @return                 True if COE is defined

    bool isDefined() const;

    /// @brief                  Get Semi Major Axis
    ///
    /// @return                 Semi Major Axis

    Length getSemiMajorAxis() const;

    /// @brief                  Get Eccentricity
    ///
    /// @return                 Eccentricity

    Real getEccentricity() const;

    /// @brief                  Get Inclination
    ///
    /// @return                 Inclination

    Angle getInclination() const;

    /// @brief                  Get Right ascenscion of the ascending node
    ///
    /// @return                 Right ascenscion of the ascending node

    Angle getRaan() const;

    /// @brief                  Get Argument of periapsis
    ///
    /// @return                 Argument of periapsis

    Angle getAop() const;

    /// @brief                  Get True anomaly
    ///
    /// @return                 True anomaly

    virtual Angle getTrueAnomaly() const;

    /// @brief                  Get Mean anomaly
    ///
    /// @return                 Mean anomaly

    virtual Angle getMeanAnomaly() const;

    /// @brief                  Get Eccentric anomaly
    ///
    /// @return                 Eccentric anomaly

    virtual Angle getEccentricAnomaly() const;

    /// @brief                  Get Periapsis radius
    ///
    /// @return                 Periapsis radius

    Length getPeriapsisRadius() const;

    /// @brief                  Get Apoapsis radius
    ///
    /// @return                 Apoapsis radius

    Length getApoapsisRadius() const;

    /// @brief                  Get Semi-Latus Rectum
    ///
    /// @return                 Semi-Latus Rectum

    Length getSemiLatusRectum() const;

    /// @brief                  Get Radial Distance
    ///
    /// @return                 Radiaul Distance

    Length getRadialDistance() const;

    /// @brief                  Get Angular Momentum
    ///
    /// @return                 Angular Momentum

    Derived getAngularMomentum(const Derived& aGravitationalParameter) const;

    /// @brief                  Get Mean motion
    ///
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    /// @return                 Mean motion

    Derived getMeanMotion(const Derived& aGravitationalParameter) const;

    /// @brief                  Get Orbital period
    ///
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    /// @return                 Orbital period

    Duration getOrbitalPeriod(const Derived& aGravitationalParameter) const;

    /// @brief                  Get Cartesian state
    ///
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    /// @param                  [in] aFrameSPtr A frame
    /// @return                 Cartesian state

    COE::CartesianState getCartesianState(const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr)
        const;

    /// @brief                  Get vector of elements in SI units
    ///
    /// @param                  [in] anAnomalyType An anomaly type
    /// @return                 Vector

    Vector6d getSIVector(const AnomalyType& anAnomalyType) const;

    /// @brief                  Print COE
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Construct an undefined COE
    ///
    /// @code
    ///                         COE coe = COE::Undefined();
    /// @endcode
    ///
    /// @return                 Undefined coe

    static COE Undefined();

    /// @brief                  Construct a COE from a cartesian state
    ///
    /// @param                  [in] aCartesianState A cartesian state
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    /// @return                 COE

    static COE Cartesian(const COE::CartesianState& aCartesianState, const Derived& aGravitationalParameter);

    /// @brief                  Construct a COE from a vector
    ///
    /// @param                  [in] aCOEVector A vector
    /// @param                  [in] anAnomalyType An anomaly type
    /// @return                 COE

    static COE FromSIVector(const Vector6d& aCOEVector, const AnomalyType& anAnomalyType);

    /// @brief                  Convert True anomaly to Eccentric anomaly
    ///
    /// @param                  [in] aTrueAnomaly A true anomaly
    /// @param                  [in] anEccentricity An eccentricity
    /// @return                 Eccentric anomaly

    static Angle EccentricAnomalyFromTrueAnomaly(const Angle& aTrueAnomaly, const Real& anEccentricity);

    /// @brief                  Convert Eccentric anomaly to True anomaly
    ///
    /// @param                  [in] anEccentricAnomaly An eccentric anomaly
    /// @param                  [in] anEccentricity An eccentricity
    /// @return                 True anomaly

    static Angle TrueAnomalyFromEccentricAnomaly(const Angle& anEccentricAnomaly, const Real& anEccentricity);

    /// @brief                  Convert Mean anomaly to Eccentric anomaly
    ///
    /// @param                  [in] aMeanAnomaly A mean anomaly
    /// @param                  [in] anEccentricity An eccentricity
    /// @return                 Eccentric anomaly

    static Angle MeanAnomalyFromEccentricAnomaly(const Angle& anEccentricAnomaly, const Real& anEccentricity);

    /// @brief                  Convert Eccentric anomaly to Mean anomaly
    ///
    /// @param                  [in] anEccentricAnomaly An eccentric anomaly
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] aTolerance a tolerance
    /// @return                 Mean anomaly

    static Angle EccentricAnomalyFromMeanAnomaly(
        const Angle& aMeanAnomaly, const Real& anEccentricity, const Real& aTolerance
    );

    /// @brief                  Convert Mean anomaly to True anomaly
    ///
    /// @param                  [in] aMeanAnomaly A mean anomaly
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] aTolerance a tolerance
    /// @return                 True anomaly

    static Angle TrueAnomalyFromMeanAnomaly(
        const Angle& aMeanAnomly, const Real& anEccentricity, const Real& aTolerance
    );

    /// @brief                  Compute the semi-latus rectum of the orbit.
    ///
    /// @param                  [in] aSemiMajorAxis Semi-major axis of the orbit in meters.
    /// @param                  [in] anEccentricity Eccentricity of the orbit.
    /// @return                 Real Semi-latus rectum.

    static Real ComputeSemiLatusRectum(const Real& aSemiMajorAxis, const Real& anEccentricity);

    /// @brief                  Compute the angular momentum of the orbit.
    ///
    /// @param                  [in] aSemiMajorAxis Semi-major axis of the orbit in meters.
    /// @param                  [in] anEccentricity Eccentricity of the orbit.
    /// @param                  [in] aGravitationalParameter Gravitational parameter.
    /// @return                 Angular momentum.

    static Real ComputeAngularMomentum(
        const Real& aSemiMajorAxis, const Real& anEccentricity, const Derived& aGravitationalParameter
    );

    /// @brief                  Compute the angular momentum of the orbit.
    ///
    /// @param                  [in] aSemiLatusRectum Semi-latus rectum of the orbit in meters.
    /// @param                  [in] aGravitationalParameter Gravitational parameter.
    /// @return                 Angular momentum.

    static Real ComputeAngularMomentum(const Real& aSemiLatusRectum, const Derived& aGravitationalParameter);

    /// @brief                  Compute the radial distance at a given true anomaly.
    ///
    /// @param                  [in] aSemiMajorAxis Semi-major axis of the orbit in meters.
    /// @param                  [in] anEccentricity Eccentricity of the orbit.
    /// @param                  [in] trueAnomaly True anomaly in radians.
    /// @return                 Radial distance.

    static Real ComputeRadialDistance(const Real& aSemiMajorAxis, const Real& anEccentricity, const Real& trueAnomaly);

    /// @brief                  Convert element to string
    ///
    /// @param                  [in] anElement An element
    /// @return                 String representing the element

    static String StringFromElement(const COE::Element& anElement);

   protected:
    Length semiMajorAxis_;
    Real eccentricity_;
    Angle inclination_;
    Angle raan_;
    Angle aop_;
    Angle anomaly_;
    AnomalyType anomalyType_;

    /// @brief                  Constructor
    ///
    /// @param                  [in] aSemiMajorAxis A semi-major axis
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] anInclination An inclination
    /// @param                  [in] aRaan A raan
    /// @param                  [in] anAop An aop
    /// @param                  [in] anAnomaly An anomaly
    /// @param                  [in] anAnomalyType An anomaly type

    COE(const Length& aSemiMajorAxis,
        const Real& anEccentricity,
        const Angle& anInclination,
        const Angle& aRaan,
        const Angle& anAop,
        const Angle& anAnomaly,
        const AnomalyType& anAnomalyType);

   private:
    /// @brief                  Convert anomaly
    ///
    /// @param                  [in] anAnomaly An anomaly
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] fromAnomalyType An anomaly type
    /// @param                  [in] toAnomalyType An anomaly type
    /// @param                  [in] aTolerance a tolerance
    /// @return                 Anomaly

    static Angle ConvertAnomaly(
        const Angle& anAnomaly,
        const Real& anEccentricity,
        const AnomalyType& fromAnomalyType,
        const AnomalyType& toAnomalyType,
        const Real& aTolerance
    );
};

}  // namespace kepler
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
