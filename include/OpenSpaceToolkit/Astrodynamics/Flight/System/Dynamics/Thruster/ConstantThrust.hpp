/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

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
namespace thruster
{

using ostk::core::types::Real;

using ostk::math::obj::Vector3d;
using ostk::math::geom::d3::trf::rot::RotationMatrix;

using ostk::physics::time::Instant;
using ostk::physics::units::Mass;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::LocalOrbitalFrameDirection;

/// @brief                      Define the acceleration experienced by a point mass due to a constant thrust thruster
/// dynamics

class ConstantThrust : public Thruster
{
   public:
    /// @brief                  Constructor
    ///
    /// @code
    ///                         const SatelliteSystem aSatelliteSystem = { ... };
    ///                         const LocalOrbitalFrameDirection aThrustDirection = { ... };
    ///                         const String aName = { ... };
    ///                         ConstantThrust constantThrustThrusterDynamics = { aSatelliteSystem, aThrustDirection,
    ///                         aName
    ///                         };
    /// @endcode
    ///
    /// @param                  [in] aSatelliteSystem A satellite system
    /// @param                  [in] aThrustDirection A local orbital frame thrust direction
    /// @param                  [in] aName A name

    ConstantThrust(
        const SatelliteSystem& aSatelliteSystem,
        const LocalOrbitalFrameDirection& aThrustDirection,
        const String& aName = String::Empty()
    );

    /// @brief                  Destructor

    virtual ~ConstantThrust() override;

    /// @brief                  Equal to operator
    ///
    /// @param                  [in] aConstantThrust A constant thrust thruster dynamics
    /// @return                 True if constant thrust thruster dynamics are equal

    bool operator==(const ConstantThrust& aConstantThrust) const;

    /// @brief                  Not equal to operator
    ///
    /// @param                  [in] aThruster A constant thrust thruster dynamics
    /// @return                 True if constant thrust thruster dynamics are not equal

    bool operator!=(const ConstantThrust& aConstantThrust) const;

    /// @brief                  Output stream operator
    ///
    /// @param                  in] anOutputStream An output stream
    /// @param                  [in] aThruster A constant thrust thruster dynamics
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const ConstantThrust& aConstantThrust);

    /// @brief                  Check if constant thrust thruster dynamics is defined
    ///
    /// @return                 True if constant thrust thruster dynamics is defined

    virtual bool isDefined() const override;

    /// @brief                  Get thrust
    ///
    /// @code
    ///                         Scalar thrust = constantThrustThruster.getThrust() ;
    /// @endcode
    ///
    /// @return                 Scalar

    Scalar getThrust() const;

    /// @brief                  Return the coordinates subsets that the instance reads from
    ///
    /// @return                 The coordinates subsets that the instance reads from

    virtual Array<Shared<const CoordinatesSubset>> getReadCoordinatesSubsets() const override;

    /// @brief                  Return the coordinates subsets that the instance writes to
    ///
    /// @return                 The coordinates subsets that the instance writes to

    virtual Array<Shared<const CoordinatesSubset>> getWriteCoordinatesSubsets() const override;

    /// @brief                  Compute the contribution to the state derivative.
    ///
    /// @param anInstant        An instant
    /// @param x                The reduced state vector (this vector will follow the structure determined by the 'read'
    /// coordinate subsets)
    /// @param aFrameSPtr       The frame in which the state vector is expressed
    ///
    /// @return                 The reduced derivative state vector (this vector must follow the structure determined by
    /// the 'write' coordinate subsets) expressed in the given frame

    virtual VectorXd computeContribution(
        const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
    ) const override;

    /// @brief                  Print constant thrust constant thrust thruster dynamics
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    LocalOrbitalFrameDirection
        localOrbitalFrameDirection_;  // TBI: Local Orbital Frame Direction definition only for thrust direction.
};

}  // namespace thruster
}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
