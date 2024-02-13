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

/// @brief Define the acceleration experienced by a point mass due to gravity
class CentralBodyGravity : public Dynamics
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  const aCelestial = { ... };
    ///                  CentralBodyGravity centralBodyGravity = { aCelestial };
    /// @endcode
    ///
    /// @param aCelestial A Celestial Object
    CentralBodyGravity(const Shared<const Celestial>& aCelestial);

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  const aCelestial = { ... };
    ///                  const aName = { ... };
    ///                  CentralBodyGravity centralBodyGravity = { aCelestial, aName };
    /// @endcode
    ///
    /// @param aCelestial A celestial object
    /// @param aName A name
    CentralBodyGravity(const Shared<const Celestial>& aCelestial, const String& aName);

    /// @brief Destructor
    virtual ~CentralBodyGravity() override;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aCentralBodyGravity A central body gravity dynamics
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const CentralBodyGravity& aCentralBodyGravity);

    /// @brief Check if central body gravity dynamics is defined
    ///
    /// @return True if central body gravity dynamics is defined
    virtual bool isDefined() const override;

    /// @brief Get celestial
    ///
    /// @return A celestial object
    Shared<const Celestial> getCelestial() const;

    /// @brief Return the coordinate subsets that the instance reads from
    ///
    /// @return The coordinate subsets that the instance reads from
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override;

    /// @brief Return the coordinate subsets that the instance writes to
    ///
    /// @return The coordinate subsets that the instance writes to
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override;

    /// @brief Compute the contribution to the state derivative.
    ///
    /// @param anInstant        An instant
    /// @param x                The reduced state vector (this vector will follow the structure determined by the 'read'
    /// coordinate subsets)
    /// @param aFrameSPtr       The frame in which the state vector is expressed
    ///
    /// @return The reduced derivative state vector (this vector must follow the structure determined by
    /// the 'write' coordinate subsets) expressed in the given frame
    virtual VectorXd computeContribution(
        const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
    ) const override;

    /// @brief Print central body gravity dynamics
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    Shared<const Celestial> celestialObjectSPtr_;
};

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk

#endif
