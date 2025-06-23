#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial__

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

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
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using KeplerianCOE = ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

/// @brief Modified Equinoctial Orbital Elements (ModifiedEquinoctial)
///
/// @ref https://spsu.edu/math/npetulante/mathematica/AppliedOrbitalMechanicsSupplement.pdf

class ModifiedEquinoctial
{
   public:
    /// @brief ModifiedEquinoctial element enum
    enum class Element
    {
        SemiLatusRectum,  ///< Semi-latus rectum (p)
        EccentricityX,    ///< x-component of eccentricity vector (f)
        EccentricityY,    ///< y-component of eccentricity vector (g)
        NodeX,            ///< x-component of node vector (h)
        NodeY,            ///< y-component of node vector (k)
        TrueLongitude     ///< True longitude (L)
    };

    /// @brief Constructor
    ///
    /// @param [in] semiLatusRectum Semi-latus rectum
    /// @param [in] eccentricityX X-component of eccentricity vector
    /// @param [in] eccentricityY Y-component of eccentricity vector
    /// @param [in] nodeX X-component of node vector
    /// @param [in] nodeY Y-component of node vector
    /// @param [in] trueLongitude True longitude
    ModifiedEquinoctial(
        const Length& semiLatusRectum,
        const Real& eccentricityX,
        const Real& eccentricityY,
        const Real& nodeX,
        const Real& nodeY,
        const Angle& trueLongitude
    );

    /// @brief Equality operator
    ///
    /// @param [in] aMEOE An ModifiedEquinoctial
    /// @return True if MEOEs are equal
    bool operator==(const ModifiedEquinoctial& aMEOE) const;

    /// @brief Inequality operator
    ///
    /// @param [in] aMEOE An ModifiedEquinoctial
    /// @return True if MEOEs are not equal
    bool operator!=(const ModifiedEquinoctial& aMEOE) const;

    /// @brief Output stream operator
    ///
    /// @param [in] anOutputStream An output stream
    /// @param [in] aMEOE An ModifiedEquinoctial
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const ModifiedEquinoctial& aMEOE);

    /// @brief Check if ModifiedEquinoctial is defined
    ///
    /// @return True if ModifiedEquinoctial is defined
    bool isDefined() const;

    /// @brief Get semi-latus rectum
    ///
    /// @return Semi-latus rectum
    Length getSemiLatusRectum() const;

    /// @brief Get x-component of eccentricity vector
    ///
    /// @return X-component of eccentricity vector
    Real getEccentricityX() const;

    /// @brief Get y-component of eccentricity vector
    ///
    /// @return Y-component of eccentricity vector
    Real getEccentricityY() const;

    /// @brief Get x-component of node vector
    ///
    /// @return X-component of node vector
    Real getNodeX() const;

    /// @brief Get y-component of node vector
    ///
    /// @return Y-component of node vector
    Real getNodeY() const;

    /// @brief Get true longitude
    ///
    /// @return True longitude
    Angle getTrueLongitude() const;

    /// @brief Get ModifiedEquinoctial elements as a 6D vector in SI units
    ///
    /// @return ModifiedEquinoctial elements as a 6D vector in SI units
    Vector6d getSIVector() const;

    /// @brief Convert ModifiedEquinoctial to Cartesian state
    ///
    /// @param [in] aGravitationalParameter Gravitational parameter
    /// @param [in] aFrameSPtr Frame in which the Cartesian state is expressed
    /// @return Cartesian state
    Pair<Position, Velocity> getCartesianState(
        const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr
    ) const;

    /// @brief Print ModifiedEquinoctial
    ///
    /// @param [in] anOutputStream An output stream
    /// @param [in] (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorators = true) const;

    /// @brief Construct an undefined ModifiedEquinoctial
    ///
    /// @return Undefined ModifiedEquinoctial
    static ModifiedEquinoctial Undefined();

    /// @brief Create ModifiedEquinoctial from Cartesian state
    /// @ref https://spsweb.fltops.jpl.nasa.gov/portaldataops/mpg/MPG_Docs/Source%20Docs/EquinoctalElements-modified.pdf
    ///
    /// @param [in] aCartesianState Cartesian state (position, velocity)
    /// @param [in] aGravitationalParameter Gravitational parameter
    /// @return ModifiedEquinoctial
    static ModifiedEquinoctial Cartesian(
        const Pair<Position, Velocity>& aCartesianState, const Derived& aGravitationalParameter
    );

    /// @brief Create ModifiedEquinoctial from Classical Orbital Elements
    ///
    /// @param [in] aClassicalOrbitalElements Classical Orbital Elements
    /// @return ModifiedEquinoctial
    static ModifiedEquinoctial COE(const KeplerianCOE& aKeplerianElements);

    /// @brief Get string from ModifiedEquinoctial element
    ///
    /// @param [in] anElement An ModifiedEquinoctial element
    /// @return String from ModifiedEquinoctial element
    static String StringFromElement(const Element& anElement);

   private:
    Length semiLatusRectum_;  ///< Semi-latus rectum
    Real eccentricityX_;      ///< X-component of eccentricity vector
    Real eccentricityY_;      ///< Y-component of eccentricity vector
    Real nodeX_;              ///< X-component of node vector
    Real nodeY_;              ///< Y-component of node vector
    Angle trueLongitude_;     ///< True longitude
};

}  // namespace kepler
}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif  // __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial__
