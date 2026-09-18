/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace probabilityofcollisionalgorithm
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Frisbee 2015 maximum probability of collision algorithm as described in:
///
/// Frisbee, J.H. (2015). An Upper Bound On High Speed Satellite Collision Probability When Only One Object Has Position
/// Uncertainty Information. AAS 15-717.
///
/// @details The algorithm computes an upper bound on the probability of collision for a high-relative velocity
/// encounter when the position error covariance of only one object is known. The unknown covariance is replaced by the
/// critical degenerate covariance that maximizes the probability of collision.
///
/// The result is an upper bound: a value below an action threshold shows that no risk mitigation is required, while a
/// value above it is inconclusive.
///
/// The following assumptions apply:
/// - High-relative velocity encounter
/// - Position uncertainty available for Object 1, or for at least one object when falling back to Object 2 covariance
///
/// @ref https://ntrs.nasa.gov/citations/20150014495
class Frisbee2015 : public ProbabilityOfCollisionAlgorithm
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     Frisbee2015 algorithm = {} ;
    ///     Frisbee2015 algorithm = { Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::GCRF(), true } ;
    /// @endcode
    ///
    /// @param aRelativeVelocityThreshold A relative velocity threshold below which the
    /// close approach is considered low-relative velocity and the algorithm is not applicable.
    /// Defaults to 100 m/s
    /// @param aFrameSPtr An inertial (or quasi-inertial) frame to use. Defaults to Frame::GCRF()
    /// @param fallBackToObject2Covariance Whether to fall back to Object 2 covariance when Object 1
    /// has no position uncertainty. Defaults to true
    Frisbee2015(
        const Derived& aRelativeVelocityThreshold = Derived(100.0, Derived::Unit::MeterPerSecond()),
        const Shared<const Frame>& aFrameSPtr = Frame::GCRF(),
        const bool& fallBackToObject2Covariance = true
    );

    /// @brief Destructor
    virtual ~Frisbee2015() override = default;

    /// @brief Get the name of the probability of collision algorithm
    ///
    /// @return "Frisbee 2015"
    virtual String getName() const override;

    /// @brief Check if the algorithm computes a maximum probability of collision
    ///
    /// @return True
    virtual bool isMaximumProbabilityOfCollisionAlgorithm() const override;

    /// @brief Identify unsatisfied assumptions for a close approach
    ///
    /// @param aCloseApproach A close approach
    /// @return A non-exhaustive array of unsatisfied assumption descriptions
    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override;

    /// @brief Compute the probability of collision
    ///
    /// @param aCloseApproach A close approach
    /// @param aCombinedHardBodyRadius A combined hard-body radius
    ///
    /// @return The probability of collision
    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach, const Length& aCombinedHardBodyRadius
    ) const override;

   private:
    /// @brief Compute the probability of collision treating Object 1 as the asset
    ///
    /// @param aCloseApproach A close approach
    /// @param aCombinedHardBodyRadius A combined hard-body radius
    ///
    /// @return The probability of collision
    Real computeProbabilityOfCollision_(const CloseApproach& aCloseApproach, const Length& aCombinedHardBodyRadius)
        const;

    static const Shared<const CoordinateSubset> CartesianPositionSubsetSPtr;

    Derived relativeVelocityThreshold_;
    Shared<const Frame> frameSPtr_;
    bool fallBackToObject2Covariance_;
};

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
