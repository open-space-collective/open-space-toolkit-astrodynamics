/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{

using ostk::core::types::String;
using ostk::core::types::Shared;
using ostk::core::ctnr::Array;

using ostk::astro::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::dynamics::thruster::GuidanceLaw;

/// @brief                      Define the acceleration experienced by a point mass due to thrust

class Thruster : public Dynamics
{
   public:
    /// @brief                  Constructor
    ///
    /// @param                  [in] aSatelliteSystem A satellite system
    /// @param                  [in] aGuidanceLaw A guidance law
    /// @param                  [in] aName A name

    Thruster(
        const SatelliteSystem& aSatelliteSystem,
        const Shared<GuidanceLaw>& aGuidanceLaw,
        const String& aName = String::Empty()
    );

    ~Thruster();

    /// @brief                  Get satellite system
    ///
    /// @return                 Satellite system

    SatelliteSystem getSatelliteSystem() const;

    /// @brief                  Get guidance law
    ///
    /// @return                 Guidance law

    Shared<GuidanceLaw> getGuidanceLaw() const;

    /// @brief                  Get name
    ///
    /// @return                 Name of thruster

    String getName() const;

    /// @brief                  Return the coordinates subsets that the instance reads from
    ///
    /// @return                 The coordinates subsets that the instance reads from

    virtual Array<Shared<const CoordinatesSubset>> getReadCoordinatesSubsets() const override;

    /// @brief                  Return the coordinates subsets that the instance writes to
    ///
    /// @return                 The coordinates subsets that the instance writes to

    virtual Array<Shared<const CoordinatesSubset>> getWriteCoordinatesSubsets() const override;

    /// @brief                  Check if constant thrust thruster dynamics is defined
    ///
    /// @return                 True if constant thrust thruster dynamics is defined

    virtual bool isDefined() const override;

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

    /// @brief                  Print thruster
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

   private:
    const SatelliteSystem satelliteSystem_;
    Shared<GuidanceLaw> guidanceLaw_;
    const String name_;
};

}  // namespace dynamics
}  // namespace astro
}  // namespace ostk

#endif
