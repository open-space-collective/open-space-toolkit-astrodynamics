/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_CentralBodyGravity__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_CentralBodyGravity__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>

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
using ostk::core::type::String;

using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Instant;

using ostk::astrodynamics::Dynamics;

/// @brief Central body gravitational dynamics.
///
/// @details Defines the gravitational acceleration experienced by a spacecraft due to the central
/// body. The central body's gravitational model (e.g., spherical, EGM96, EGM2008) is obtained
/// from the associated Celestial object.
class CentralBodyGravity : public Dynamics
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Shared<const Celestial> celestialSPtr = { ... } ;
    ///     CentralBodyGravity centralBodyGravity = { celestialSPtr } ;
    /// @endcode
    ///
    /// @param aCelestial A celestial object representing the central body.
    CentralBodyGravity(const Shared<const Celestial>& aCelestial);

    /// @brief Constructor with name.
    ///
    /// @code{.cpp}
    ///     Shared<const Celestial> celestialSPtr = { ... } ;
    ///     CentralBodyGravity centralBodyGravity = { celestialSPtr, "Earth Gravity" } ;
    /// @endcode
    ///
    /// @param aCelestial A celestial object representing the central body.
    /// @param aName A name for the dynamics.
    CentralBodyGravity(const Shared<const Celestial>& aCelestial, const String& aName);

    /// @brief Destructor.
    virtual ~CentralBodyGravity() override;

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     CentralBodyGravity centralBodyGravity = { ... } ;
    ///     std::cout << centralBodyGravity ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param aCentralBodyGravity A central body gravity dynamics.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const CentralBodyGravity& aCentralBodyGravity);

    /// @brief Check if the central body gravity dynamics is defined.
    ///
    /// @code{.cpp}
    ///     CentralBodyGravity centralBodyGravity = { ... } ;
    ///     bool defined = centralBodyGravity.isDefined() ;
    /// @endcode
    ///
    /// @return True if the central body gravity dynamics is defined.
    virtual bool isDefined() const override;

    /// @brief Get the celestial object.
    ///
    /// @code{.cpp}
    ///     CentralBodyGravity centralBodyGravity = { ... } ;
    ///     Shared<const Celestial> celestial = centralBodyGravity.getCelestial() ;
    /// @endcode
    ///
    /// @return A shared pointer to the celestial object.
    Shared<const Celestial> getCelestial() const;

    /// @brief Get the coordinate subsets that the instance reads from.
    ///
    /// @code{.cpp}
    ///     CentralBodyGravity centralBodyGravity = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> readSubsets = centralBodyGravity.getReadCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance reads from.
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override;

    /// @brief Get the coordinate subsets that the instance writes to.
    ///
    /// @code{.cpp}
    ///     CentralBodyGravity centralBodyGravity = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> writeSubsets = centralBodyGravity.getWriteCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance writes to.
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override;

    /// @brief Compute the contribution to the state derivative.
    ///
    /// @code{.cpp}
    ///     CentralBodyGravity centralBodyGravity = { ... } ;
    ///     VectorXd contribution = centralBodyGravity.computeContribution(anInstant, x, aFrameSPtr) ;
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

    /// @brief Print the central body gravity dynamics.
    ///
    /// @code{.cpp}
    ///     CentralBodyGravity centralBodyGravity = { ... } ;
    ///     centralBodyGravity.print(std::cout) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    Shared<const Celestial> celestialObjectSPtr_;
};

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk

#endif
