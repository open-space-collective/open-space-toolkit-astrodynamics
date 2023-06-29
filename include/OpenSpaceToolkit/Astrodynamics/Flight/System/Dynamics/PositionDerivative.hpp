/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative__

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{

using ostk::astro::flight::system::Dynamics;

/// @brief                  Define the contribution to the position due to velocity

class PositionDerivative : public Dynamics
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     PositionDerivative positionDerivative = {};
    /// @endcode
    ///

    PositionDerivative();

    /// @brief              Destructor

    virtual ~PositionDerivative() override;

    /// @brief              Clone position derivative dynamics
    ///
    /// @return             Pointer to cloned position derivative dynamics

    virtual PositionDerivative* clone() const override;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aPositionDerivative A position derivative dynamics
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const PositionDerivative& aPositionDerivative);

    /// @brief              Check if position derivative dynamics is defined
    ///
    /// @return             True if position derivative dynamics is defined

    virtual bool isDefined() const override;

    /// @brief              Declares the coordinates involved in the contribution calculation
    ///
    /// @param              [in] coordinatesBroker The coordinate broker mapping state coordinates and indexes

    virtual void declareCoordinates(CoordinatesBroker& coordinatesBroker) = 0;

    /// @brief              Apply contribution to the state derivative
    ///
    /// @param              [in] x A state vector
    /// @param              [out] dxdt A state derivative vector
    /// @param              [in] anInstant An instant

    virtual void applyContribution(
        const Dynamics::StateVector& x, Dynamics::StateVector& dxdt, const Instant& anInstant
    ) const override;

    /// @brief              Print
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;
};

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
