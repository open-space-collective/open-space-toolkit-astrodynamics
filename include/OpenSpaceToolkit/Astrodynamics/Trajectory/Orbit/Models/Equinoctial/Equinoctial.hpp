/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_BrouwerLyddaneMean_BrouwerLyddaneMean__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_BrouwerLyddaneMean_BrouwerLyddaneMean__

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

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

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
namespace equinoctial
{

using ostk::core::ctnr::Pair;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::object::Vector6d;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using classicalOE = ostk::astro::trajectory::orbit::models::kepler::COE;

/// @brief                      Equinoctial Orbital Elements. A representation of the Keplerian element set which
/// removes the mathematical singularities at eccentricity = 0 and inclination = 0 and 90 degrees. This element set has
/// a singularity at an inclination of 180 degrees.
///
/// @ref
/// https://ai-solutions.com/_help_Files/orbit_element_types.htm
/// Applied Non Singular Astrodynamics - Optimal Low-Thrust Orbit Transfer - Jean Albert Kechichian - Chapter 1
///
///                             a - semi-major axis (same as kepler classical orbital elements)
///                             h - projection of eccentricity vector onto y-axis
///                             k - projection of eccentricity vector onto x-axis
///                             p - projection of ascending node vector onto y-axis
///                             q - projection of ascending node vector onto x-axis
///                             lambda - longitude of the ascending node

class Equinoctial
{
   public:
    enum class LongitudeType
    {
        Mean,
        Eccentric,
    };

    /// @brief                  Constructor
    ///
    /// @param                  [in] aSemiMajorAxis A semi-major axis length
    /// @param                  [in] aH A h real value
    /// @param                  [in] aK A k real value
    /// @param                  [in] aP A p real value
    /// @param                  [in] aQ A q real value
    /// @param                  [in] aMeanLongitude A mean longitude angle

    Equinoctial(
        const Length& aSemiMajorAxis,
        const Real& aH,
        const Real& aK,
        const Real& aP,
        const Real& aQ,
        const Angle& aMeanLongitude
    );

    /// @brief                  Equal to operator
    ///
    /// @param                  [in] anEquinoctial A set of Equinoctial Orbital Elements (EOE)
    /// @return                 True if EOEs are equal

    bool operator==(const Equinoctial& anEquinoctial) const;

    /// @brief                  Not Equal to operator
    ///
    /// @param                  [in] anEquinoctial A set of COE
    /// @return                 True if EOEs are not equal

    bool operator!=(const Equinoctial& anEquinoctial) const;

    /// @brief                  Output stream operator
    ///
    /// @code
    ///                         std::cout << Equinoctial(...) ;
    /// @endcode
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] anEquinoctial A set of EOE
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Equinoctial& anEquinoctial);

    /// @brief                  Check if EOE is defined
    ///
    /// @return                 True if EOE is defined

    bool isDefined() const;

    /// @brief                  Get semi-major axis
    ///
    /// @return                 Semi-major axis

    Length getSemiMajorAxis() const;

    /// @brief                  Get h
    ///
    /// @return                 h

    Real getH() const;

    /// @brief                  Get k
    ///
    /// @return                 k

    Real getK() const;

    /// @brief                  Get p
    ///
    /// @return                 p

    Real getP() const;

    /// @brief                  Get q
    ///
    /// @return                 q

    Real getQ() const;

    /// @brief                  Get true longitude
    ///
    /// @return                 True longitude

    // Angle getTrueLongitude() const;

    /// @brief                  Get mean longitude
    ///
    /// @return                 Mean Longitude

    Angle getMeanLongitude() const;

    /// @brief                  Get eccentric longitude
    ///
    /// @return                 Eccentric Longitude

    Angle getEccentricLongitude() const;

    /// @brief                  Get Cartesian state
    ///
    /// @param                  [in] aGravitationalParameter A gravitational parameter
    /// @param                  [in] aFrameSPtr A frame
    /// @return                 Cartesian state

    classicalOE::CartesianState getCartesianState(
        const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr
    ) const;

    /// @brief                  Get vector of elements in SI units
    ///
    /// @param                  [in] aLongitudeType An longitude type
    /// @return                 Vector

    Vector6d getSIVector(const LongitudeType& aLongitudeType) const;

    /// @brief                  Print EOE
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Construct from a COE
    ///
    /// @param                  [in] aCOE A COE
    /// @return                 Equinoctial

    static Equinoctial COE(const classicalOE& aCOE);

    /// @brief                  Construct an undefined Equinoctial
    ///
    /// @code
    ///                         Equinoctial equinoctial = Equinoctial::Undefined();
    /// @endcode
    ///
    /// @return                 Undefined equinoctial

    static Equinoctial Undefined();

    /// @brief                  Constructor
    ///
    /// @param                  [in] aCartesianState A cartesian state
    /// @param                  [in] aGravitationalParameter A gravitational parameter

    static Equinoctial Cartesian(
        const classicalOE::CartesianState& aCartesianState, const Derived& aGravitationalParameter
    );

    /// @brief                  Construct from a vector of elements in SI units
    ///
    /// @param                  [in] aVector A vector
    /// @return                 Equinoctial

    static Equinoctial FromSIVector(const Vector6d& aVector);

    /// @brief                  Convert Equinoctial to COE
    ///
    /// @return                 COE

    classicalOE toCOE() const;

    /// @brief                  Convert Eccentric longitude to Mean longitude
    ///
    /// @param                  [in] aMeanAnomaly A mean longitude
    /// @param                  [in] aH A h real value
    /// @param                  [in] aK A k real value
    /// @return                 Mean longitude

    static Angle MeanLongitudeFromEccentricLongitude(const Angle& anEccentricLongitude, const Real& aH, const Real& aK);

    /// @brief                  Convert Mean longitude to Eccentric longitude
    ///
    /// @param                  [in] anEccentricLongitude An eccentric longitude
    /// @param                  [in] aH A h real value
    /// @param                  [in] aK A k real value
    /// @param                  [in] aTolerance a tolerance
    /// @return                 Mean longitude

    static Angle EccentricLongitudeFromMeanLongitude(
        const Angle& aMeanLongitude, const Real& aH, const Real& aK, const Real& aTolerance
    );

   private:
    Length semiMajorAxis_;
    Real h_;
    Real k_;
    Real p_;
    Real q_;
    Angle longitude_;
    LongitudeType longitudeType_;

    /// @brief                  Convert longitude
    ///
    /// @param                  [in] anAnomaly An longitude
    /// @param                  [in] anEccentricity An eccentricity
    /// @param                  [in] fromAnomalyType An longitude type
    /// @param                  [in] toAnomalyType An longitude type
    /// @param                  [in] aTolerance a tolerance
    /// @return                 Anomaly

    static Angle ConvertLongitude(
        const Angle& aLongitude,
        const Real& aH,
        const Real& aK,
        const LongitudeType& fromLongitudeType,
        const LongitudeType& toLongitudeType,
        const Real& aTolerance
    );
};

}  // namespace equinoctial
}  // namespace models
}  // namespace orbit
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
