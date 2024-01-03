/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_PositionDerivative__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_PositionDerivative__

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{

using ostk::core::types::Integer;

using ostk::astro::Dynamics;

/// @brief     Define the contribution to the position due to velocity
class PositionDerivative : public Dynamics
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  PositionDerivative positionDerivative = {};
    /// @endcode
    ///
    PositionDerivative();

    /// @brief Destructor
    virtual ~PositionDerivative() override;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aPositionDerivative A position derivative dynamics
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const PositionDerivative& aPositionDerivative);

    /// @brief Check if position derivative dynamics is defined
    ///
    /// @return True if position derivative dynamics is defined
    virtual bool isDefined() const override;

    /// @brief Return the coordinates subsets that the instance reads from
    ///
    /// @return The coordinates subsets that the instance reads from
    virtual Array<Shared<const CoordinatesSubset>> getReadCoordinatesSubsets() const override;

    /// @brief Return the coordinates subsets that the instance writes to
    ///
    /// @return The coordinates subsets that the instance writes to
    virtual Array<Shared<const CoordinatesSubset>> getWriteCoordinatesSubsets() const override;

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

    /// @brief Print
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;
};

}  // namespace dynamics
}  // namespace astro
}  // namespace ostk

#endif
