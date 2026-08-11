/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Instant;

using ostk::astrodynamics::Dynamics;

/// @brief Analytical third-body gravitational dynamics.
///
/// @details Defines the gravitational acceleration experienced by a spacecraft due to a third body
/// (i.e., a celestial object other than the central body), using the third-body perturbation
/// formulation, which accounts for both the direct attraction of the third body on the spacecraft
/// and the indirect effect from the attraction on the central body. The third body is treated as a
/// point mass, using the gravitational parameter of the celestial object.
///
/// Unlike ThirdBodyGravity, the position of the third body is obtained from
/// Celestial::computeAnalyticalPosition instead of from the ephemeris of the celestial object.
/// This is orders of magnitude faster than a SPICE-based ephemeris, at the cost of a lower position
/// accuracy (< 0.1 deg in direction), which is generally negligible with respect to the magnitude of
/// the third-body perturbation itself.
///
/// Only supports the celestial objects providing an analytical model (Sun and Moon).
class AnalyticalThirdBodyGravity : public Dynamics
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Shared<const Celestial> celestialSPtr = std::make_shared<Moon>(Moon::Spherical()) ;
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { celestialSPtr } ;
    /// @endcode
    ///
    /// @param aCelestial A celestial object representing the third body.
    AnalyticalThirdBodyGravity(const Shared<const Celestial>& aCelestial);

    /// @brief Constructor with name.
    ///
    /// @code{.cpp}
    ///     Shared<const Celestial> celestialSPtr = std::make_shared<Moon>(Moon::Spherical()) ;
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { celestialSPtr, "Moon Gravity" } ;
    /// @endcode
    ///
    /// @param aCelestial A celestial object representing the third body.
    /// @param aName A name for the dynamics.
    AnalyticalThirdBodyGravity(const Shared<const Celestial>& aCelestial, const String& aName);

    /// @brief Destructor.
    virtual ~AnalyticalThirdBodyGravity() override;

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { ... } ;
    ///     std::cout << analyticalThirdBodyGravity ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param anAnalyticalThirdBodyGravity An analytical third-body gravity dynamics.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(
        std::ostream& anOutputStream, const AnalyticalThirdBodyGravity& anAnalyticalThirdBodyGravity
    );

    /// @brief Check if the analytical third-body gravity dynamics is defined.
    ///
    /// @code{.cpp}
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { ... } ;
    ///     bool defined = analyticalThirdBodyGravity.isDefined() ;
    /// @endcode
    ///
    /// @return True if the analytical third-body gravity dynamics is defined.
    virtual bool isDefined() const override;

    /// @brief Get the celestial object.
    ///
    /// @code{.cpp}
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { ... } ;
    ///     Shared<const Celestial> celestial = analyticalThirdBodyGravity.getCelestial() ;
    /// @endcode
    ///
    /// @return A shared pointer to the celestial object.
    Shared<const Celestial> getCelestial() const;

    /// @brief Get the coordinate subsets that the instance reads from.
    ///
    /// @code{.cpp}
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> readSubsets =
    ///         analyticalThirdBodyGravity.getReadCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance reads from.
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override;

    /// @brief Get the coordinate subsets that the instance writes to.
    ///
    /// @code{.cpp}
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> writeSubsets =
    ///         analyticalThirdBodyGravity.getWriteCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance writes to.
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override;

    /// @brief Compute the contribution to the state derivative.
    ///
    /// @code{.cpp}
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { ... } ;
    ///     VectorXd contribution = analyticalThirdBodyGravity.computeContribution(anInstant, x, aFrameSPtr) ;
    /// @endcode
    ///
    /// @param anInstant An instant.
    /// @param x The reduced state vector (follows the structure determined by the read coordinate subsets).
    /// @param aFrameSPtr The frame in which the state vector is expressed.
    /// @return The reduced derivative state vector (follows the structure determined by the write coordinate subsets)
    /// expressed in the given frame.
    virtual VectorXd computeContribution(
        const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
    ) const override;

    /// @brief Print the analytical third-body gravity dynamics.
    ///
    /// @code{.cpp}
    ///     AnalyticalThirdBodyGravity analyticalThirdBodyGravity = { ... } ;
    ///     analyticalThirdBodyGravity.print(std::cout) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    Shared<const Celestial> celestialObjectSPtr_;
    Real gravitationalParameter_SI_;
};

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk

#endif
