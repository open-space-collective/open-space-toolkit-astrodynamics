/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

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
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::flight::system::SatelliteSystem;

/// @brief Atmospheric drag dynamics.
///
/// @details Defines the acceleration experienced by a spacecraft due to atmospheric drag.
/// The drag force depends on the atmospheric density at the spacecraft's position,
/// the spacecraft's velocity relative to the atmosphere, and its ballistic coefficient
/// (derived from the satellite system's drag coefficient, cross-sectional area, and mass).
class AtmosphericDrag : public Dynamics
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Shared<const Celestial> celestialSPtr = { ... } ;
    ///     AtmosphericDrag atmosphericDrag = { celestialSPtr } ;
    /// @endcode
    ///
    /// @param aCelestial A celestial object (with an atmospheric model).
    AtmosphericDrag(const Shared<const Celestial>& aCelestial);

    /// @brief Constructor with name.
    ///
    /// @code{.cpp}
    ///     Shared<const Celestial> celestialSPtr = { ... } ;
    ///     AtmosphericDrag atmosphericDrag = { celestialSPtr, "Earth Drag" } ;
    /// @endcode
    ///
    /// @param aCelestial A celestial object (with an atmospheric model).
    /// @param aName A name for the dynamics.
    AtmosphericDrag(const Shared<const Celestial>& aCelestial, const String& aName);

    /// @brief Destructor.
    virtual ~AtmosphericDrag() override;

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     AtmosphericDrag atmosphericDrag = { ... } ;
    ///     std::cout << atmosphericDrag ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param anAtmosphericDrag An atmospheric drag dynamics.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const AtmosphericDrag& anAtmosphericDrag);

    /// @brief Check if the atmospheric drag dynamics is defined.
    ///
    /// @code{.cpp}
    ///     AtmosphericDrag atmosphericDrag = { ... } ;
    ///     bool defined = atmosphericDrag.isDefined() ;
    /// @endcode
    ///
    /// @return True if the atmospheric drag dynamics is defined.
    virtual bool isDefined() const override;

    /// @brief Get the celestial object.
    ///
    /// @code{.cpp}
    ///     AtmosphericDrag atmosphericDrag = { ... } ;
    ///     Shared<const Celestial> celestial = atmosphericDrag.getCelestial() ;
    /// @endcode
    ///
    /// @return A shared pointer to the celestial object.
    Shared<const Celestial> getCelestial() const;

    /// @brief Get the coordinate subsets that the instance reads from.
    ///
    /// @code{.cpp}
    ///     AtmosphericDrag atmosphericDrag = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> readSubsets = atmosphericDrag.getReadCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance reads from.
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override;

    /// @brief Get the coordinate subsets that the instance writes to.
    ///
    /// @code{.cpp}
    ///     AtmosphericDrag atmosphericDrag = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> writeSubsets = atmosphericDrag.getWriteCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance writes to.
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override;

    /// @brief Compute the contribution to the state derivative.
    ///
    /// @code{.cpp}
    ///     AtmosphericDrag atmosphericDrag = { ... } ;
    ///     VectorXd contribution = atmosphericDrag.computeContribution(anInstant, x, aFrameSPtr) ;
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

    /// @brief Print the atmospheric drag dynamics.
    ///
    /// @code{.cpp}
    ///     AtmosphericDrag atmosphericDrag = { ... } ;
    ///     atmosphericDrag.print(std::cout) ;
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
