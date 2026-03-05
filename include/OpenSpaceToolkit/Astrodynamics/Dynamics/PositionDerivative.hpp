/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_PositionDerivative__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_PositionDerivative__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::type::Integer;

using ostk::astrodynamics::Dynamics;

/// @brief Position derivative dynamics.
///
/// @details Defines the contribution to the position state derivative due to velocity.
/// This dynamics simply maps the velocity components of the state vector to the position
/// derivative, implementing the kinematic relationship dx/dt = v.
class PositionDerivative : public Dynamics
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     PositionDerivative positionDerivative = {} ;
    /// @endcode
    PositionDerivative();

    /// @brief Destructor.
    virtual ~PositionDerivative() override;

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     PositionDerivative positionDerivative = {} ;
    ///     std::cout << positionDerivative ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param aPositionDerivative A position derivative dynamics.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const PositionDerivative& aPositionDerivative);

    /// @brief Check if the position derivative dynamics is defined.
    ///
    /// @code{.cpp}
    ///     PositionDerivative positionDerivative = {} ;
    ///     bool defined = positionDerivative.isDefined() ;
    /// @endcode
    ///
    /// @return True if the position derivative dynamics is defined.
    virtual bool isDefined() const override;

    /// @brief Get the coordinate subsets that the instance reads from.
    ///
    /// @code{.cpp}
    ///     PositionDerivative positionDerivative = {} ;
    ///     Array<Shared<const CoordinateSubset>> readSubsets = positionDerivative.getReadCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance reads from.
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override;

    /// @brief Get the coordinate subsets that the instance writes to.
    ///
    /// @code{.cpp}
    ///     PositionDerivative positionDerivative = {} ;
    ///     Array<Shared<const CoordinateSubset>> writeSubsets = positionDerivative.getWriteCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance writes to.
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override;

    /// @brief Compute the contribution to the state derivative.
    ///
    /// @code{.cpp}
    ///     PositionDerivative positionDerivative = {} ;
    ///     VectorXd contribution = positionDerivative.computeContribution(anInstant, x, aFrameSPtr) ;
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

    /// @brief Print the position derivative dynamics.
    ///
    /// @code{.cpp}
    ///     PositionDerivative positionDerivative = {} ;
    ///     positionDerivative.print(std::cout) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;
};

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk

#endif
