/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Define the acceleration experienced by a point mass due to thrust
class Thruster : public Dynamics
{
   public:
    /// @brief Default contribution frame shared pointer
    static const Shared<const Frame> DefaultContributionFrameSPtr;
    /// @brief Constructor
    ///
    /// @param aSatelliteSystem A satellite system
    /// @param aGuidanceLaw A guidance law
    /// @param aName A name
    Thruster(
        const SatelliteSystem& aSatelliteSystem,
        const Shared<const GuidanceLaw>& aGuidanceLaw,
        const String& aName = "Thruster"
    );

    ~Thruster();

    /// @brief Get satellite system
    ///
    /// @return Satellite system
    SatelliteSystem getSatelliteSystem() const;

    /// @brief Get guidance law
    ///
    /// @return Guidance law
    Shared<const GuidanceLaw> getGuidanceLaw() const;

    /// @brief Return the coordinate subsets that the instance reads from
    ///
    /// @return The coordinate subsets that the instance reads from
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override;

    /// @brief Return the coordinate subsets that the instance writes to
    ///
    /// @return The coordinate subsets that the instance writes to
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override;

    /// @brief Check if constant thrust thruster dynamics is defined
    ///
    /// @return True if constant thrust thruster dynamics is defined
    virtual bool isDefined() const override;

    /// @brief Compute the contribution to the state derivative.
    ///
    /// @param anInstant        An instant
    /// @param x The reduced state vector (this vector will follow the structure determined by the
    /// 'read' coordinate subsets)
    /// @param aFrameSPtr The frame in which the state vector is expressed
    ///
    /// @return The reduced derivative state vector (this vector must follow the structure determined by
    /// the 'write' coordinate subsets) expressed in the given frame
    virtual VectorXd computeContribution(const Instant& anInstant, const VectorXd& x) const override;

    /// @brief Print thruster
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

   private:
    const SatelliteSystem satelliteSystem_;
    const Shared<const GuidanceLaw> guidanceLaw_;
    const String name_;

    const Real massFlowRateCache_;
};

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk

#endif
