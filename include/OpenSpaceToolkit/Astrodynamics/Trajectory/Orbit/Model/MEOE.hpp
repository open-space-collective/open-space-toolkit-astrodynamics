#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>

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

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::container::Pair;

using ostk::mathematics::object::Vector6d;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::unit::Length;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::time::Instant;
using ostk::physics::environment::object::Celestial;
using ostk::physics::coordinate::Frame;

/// @brief Modified Equinoctial Orbital Elements (MEOE)
///
/// @ref https://spsu.edu/math/npetulante/mathematica/AppliedOrbitalMechanicsSupplement.pdf

class MEOE
{
   public:
    /// @brief Constructor
    ///
    /// @param [in] p Semi-latus rectum
    /// @param [in] f X-component of eccentricity vector
    /// @param [in] g Y-component of eccentricity vector
    /// @param [in] h X-component of node vector
    /// @param [in] k Y-component of node vector
    /// @param [in] L True longitude
    MEOE(const Length& p, const Real& f, const Real& g, const Real& h, const Real& k, const Angle& L);

    /// @brief Equality operator
    ///
    /// @param [in] aMEOE An MEOE
    /// @return True if MEOEs are equal
    bool operator==(const MEOE& aMEOE) const;

    /// @brief Inequality operator
    ///
    /// @param [in] aMEOE An MEOE
    /// @return True if MEOEs are not equal
    bool operator!=(const MEOE& aMEOE) const;

    /// @brief Output stream operator
    ///
    /// @param [in] anOutputStream An output stream
    /// @param [in] aMEOE An MEOE
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const MEOE& aMEOE);

    /// @brief Check if MEOE is defined
    ///
    /// @return True if MEOE is defined
    bool isDefined() const;

    /// @brief Get semi-latus rectum
    ///
    /// @return Semi-latus rectum
    Length getSemiLatusRectum() const;

    /// @brief Get x-component of eccentricity vector
    ///
    /// @return X-component of eccentricity vector
    Real getF() const;

    /// @brief Get y-component of eccentricity vector
    ///
    /// @return Y-component of eccentricity vector
    Real getG() const;

    /// @brief Get x-component of node vector
    ///
    /// @return X-component of node vector
    Real getH() const;

    /// @brief Get y-component of node vector
    ///
    /// @return Y-component of node vector
    Real getK() const;

    /// @brief Get true longitude
    ///
    /// @return True longitude
    Angle getTrueLongitude() const;

    /// @brief Get MEOE elements as a 6D vector in SI units
    ///
    /// @return MEOE elements as a 6D vector in SI units
    Vector6d getSIVector() const;

    /// @brief Convert MEOE to Cartesian state
    ///
    /// @param [in] aGravitationalParameter Gravitational parameter
    /// @param [in] aFrameSPtr Frame in which the Cartesian state is expressed
    /// @return Cartesian state
    Pair<Position, Velocity> getCartesianState(
        const Derived& aGravitationalParameter, const Shared<const Frame>& aFrameSPtr
    ) const;

    /// @brief Print MEOE
    ///
    /// @param [in] anOutputStream An output stream
    /// @param [in] (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorators = true) const;

    /// @brief Construct an undefined MEOE
    ///
    /// @return Undefined MEOE
    static MEOE Undefined();

    /// @brief Create MEOE from Cartesian state
    ///
    /// @param [in] aCartesianState Cartesian state (position, velocity)
    /// @param [in] aGravitationalParameter Gravitational parameter
    /// @return MEOE
    static MEOE Cartesian(const Pair<Position, Velocity>& aCartesianState, const Derived& aGravitationalParameter);

    /// @brief Get string from MEOE element
    ///
    /// @param [in] anElement An MEOE element
    /// @return String from MEOE element
    static String StringFromElement(const Element& anElement);

    /// @brief MEOE element enum (used for StringFromElement)
    enum class Element
    {
        SemiLatusRectum,  ///< Semi-latus rectum (p)
        F,                ///< x-component of eccentricity vector (f)
        G,                ///< y-component of eccentricity vector (g)
        H,                ///< x-component of node vector (h)
        K,                ///< y-component of node vector (k)
        TrueLongitude     ///< True longitude (L)
    };

   private:
    Length p_;  ///< Semi-latus rectum
    Real f_;    ///< X-component of eccentricity vector
    Real g_;    ///< Y-component of eccentricity vector
    Real h_;    ///< X-component of node vector
    Real k_;    ///< Y-component of node vector
    Angle L_;   ///< True longitude
};

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif // __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE__
