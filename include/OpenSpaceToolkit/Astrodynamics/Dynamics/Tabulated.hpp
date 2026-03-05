/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;

#define DEFAULT_TABULATED_DYNAMICS_INTERPOLATION_TYPE Interpolator::Type::BarycentricRational

/// @brief Tabulated dynamics.
///
/// @details A dynamics model that uses a pre-computed contribution profile (a time-indexed matrix of
/// values) to compute the contribution to the state derivative via interpolation. This is useful
/// for representing dynamics from pre-computed maneuver profiles or externally generated force models.
class Tabulated : public Dynamics
{
   public:
    static const Shared<const Frame> DefaultContributionFrameSPtr;

    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Array<Instant> instants = { ... } ;
    ///     MatrixXd contributionProfile = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> writeCoordinateSubsets = { ... } ;
    ///     Shared<const Frame> frameSPtr = Frame::GCRF() ;
    ///     Tabulated tabulated = { instants, contributionProfile, writeCoordinateSubsets, frameSPtr } ;
    /// @endcode
    ///
    /// @param anInstantArray An array of instants (must be sorted).
    /// @param aContributionProfile A contribution profile matrix (one row for each instant).
    /// @param aWriteCoordinateSubsets An array of coordinate subsets to write to.
    /// @param aFrameSPtr A frame in which the contribution profile is expressed.
    /// @param anInterpolationType An interpolation type (defaults to BarycentricRational).
    Tabulated(
        const Array<Instant>& anInstantArray,
        const MatrixXd& aContributionProfile,
        const Array<Shared<const CoordinateSubset>>& aWriteCoordinateSubsets,
        const Shared<const Frame>& aFrameSPtr,
        const Interpolator::Type& anInterpolationType = DEFAULT_TABULATED_DYNAMICS_INTERPOLATION_TYPE
    );

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     std::cout << tabulated ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param aTabulated A tabulated dynamics.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aDynamics);

    /// @brief Access the instants.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     const Array<Instant>& instants = tabulated.accessInstants() ;
    /// @endcode
    ///
    /// @return A const reference to the array of instants.
    const Array<Instant>& accessInstants() const;

    /// @brief Access the contribution profile.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     const MatrixXd& profile = tabulated.accessContributionProfile() ;
    /// @endcode
    ///
    /// @return A const reference to the contribution profile matrix.
    const MatrixXd& accessContributionProfile() const;

    /// @brief Get the contribution profile corresponding to a subset of coordinates.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> subsets = { ... } ;
    ///     MatrixXd profile = tabulated.getContributionProfileFromCoordinateSubsets(subsets) ;
    /// @endcode
    ///
    /// @param aCoordinateSubsetArray An array of coordinate subsets.
    /// @return The contribution profile matrix for the specified coordinate subsets.
    MatrixXd getContributionProfileFromCoordinateSubsets(
        const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetArray
    ) const;

    /// @brief Access the frame.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     const Shared<const Frame>& frame = tabulated.accessFrame() ;
    /// @endcode
    ///
    /// @return A const reference to the shared pointer to the frame.
    const Shared<const Frame>& accessFrame() const;

    /// @brief Get the interpolation type.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     Interpolator::Type type = tabulated.getInterpolationType() ;
    /// @endcode
    ///
    /// @return The interpolation type.
    Interpolator::Type getInterpolationType() const;

    /// @brief Check if the tabulated dynamics is defined.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     bool defined = tabulated.isDefined() ;
    /// @endcode
    ///
    /// @return True if the tabulated dynamics is defined.
    virtual bool isDefined() const override;

    /// @brief Get the coordinate subsets that the instance reads from.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> readSubsets = tabulated.getReadCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance reads from (empty for tabulated dynamics).
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override;

    /// @brief Get the coordinate subsets that the instance writes to.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> writeSubsets = tabulated.getWriteCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance writes to.
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override;

    /// @brief Compute the contribution to the state derivative.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     VectorXd contribution = tabulated.computeContribution(anInstant, x, aFrameSPtr) ;
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

    /// @brief Print the tabulated dynamics.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = { ... } ;
    ///     tabulated.print(std::cout) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    const Array<Instant> instants_;
    const MatrixXd contributionProfile_;
    const Array<Shared<const CoordinateSubset>> writeCoordinateSubsets_;
    const Shared<const Frame> frameSPtr_;
    Array<Shared<const Interpolator>> interpolators_;
};

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk

#endif
