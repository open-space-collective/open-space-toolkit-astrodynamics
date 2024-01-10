/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator/BarycentricRational.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{

using ostk::core::types::Shared;
using ostk::core::types::Real;
using ostk::core::ctnr::Array;

using ostk::math::object::Vector3d;
using ostk::math::object::MatrixXd;
using ostk::math::curvefitting::interpolator::BarycentricRational;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Duration;

using ostk::astro::trajectory::state::CoordinatesSubset;

/// @brief A tabulated dynamics that uses the provided contribution profile to compute the contribution to the dynamics.
class Tabulated : public Dynamics
{
   public:
    /// @brief Constructor
    ///
    /// @param anInstantArray An array of instants, must be sorted
    /// @param aContributionProfile A contribution profile, one row for each instant
    /// @param aWriteCoordinatesSubsets An array of coordinates subsets to write to
    /// @param aFrameSPtr A frame
    Tabulated(
        const Array<Instant>& anInstantArray,
        const MatrixXd& aContributionProfile,
        const Array<Shared<const CoordinatesSubset>>& aWriteCoordinatesSubsets,
        const Shared<const Frame>& aFrameSPtr
    );

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << Dynamics(...) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aTabulated A Tabulated dynamics
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aDynamics);

    /// @brief Access instants
    ///
    /// @return The instants
    const Array<Instant>& accessInstants() const;

    /// @brief Get instants
    ///
    /// @return The instants
    Array<Instant> getInstants() const;

    /// @brief Access contribution profile
    ///
    /// @return The contribution profile
    const MatrixXd& accessContributionProfile() const;

    /// @brief Get contribution profile
    ///
    /// @return The contribution profile
    MatrixXd getContributionProfile() const;

    /// @brief Access the frame
    ///
    /// @return The frame
    const Shared<const Frame>& accessFrame() const;

    /// @brief Get the frame
    ///
    /// @return The frame
    Shared<const Frame> getFrame() const;

    /// @brief Check if dynamics is defined (pure virtual)
    ///
    /// @return True if dynamics is defined
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
    /// @param anInstant An instant
    /// @param x The reduced state vector (this vector will follow the structure determined by the
    /// 'read' coordinate subsets)
    /// @param aFrameSPtr The frame in which the state vector is expressed
    ///
    /// @return The reduced derivative state vector (this vector must follow the structure determined by
    /// the 'write' coordinate subsets) expressed in the given frame
    virtual VectorXd computeContribution(
        const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
    ) const override;

    /// @brief Print Tabulated dynamics
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    const MatrixXd contributionProfile_;
    const Array<Instant> instants_;
    const Array<Shared<const CoordinatesSubset>> writeCoordinatesSubsets_;
    const Shared<const Frame> frameSPtr_;
    Array<BarycentricRational> interpolators_;
};

}  // namespace dynamics
}  // namespace astro
}  // namespace ostk

#endif
