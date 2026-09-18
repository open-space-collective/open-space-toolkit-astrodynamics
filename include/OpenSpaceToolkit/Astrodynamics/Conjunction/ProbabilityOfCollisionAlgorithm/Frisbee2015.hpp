/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015__

#include <optional>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
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

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Frisbee 2015 maximum probability of collision algorithm as described in:
///
/// Frisbee, J.H. (2015). An Upper Bound On High Speed Satellite Collision Probability When Only One Object Has Position
/// Uncertainty Information. AAS 15-717.
///
/// @details The algorithm computes an upper bound on the probability of collision for a high relative speed
/// encounter when the position error covariance of only one object is known. The unknown covariance is replaced by the
/// critical degenerate covariance that maximizes the probability of collision.
///
/// The result is an upper bound: a value below an action threshold shows that no risk mitigation is required, while a
/// value above it is inconclusive.
///
/// The following assumptions apply:
/// - High relative speed encounter
/// - Position uncertainty available for Object 1, or for at least one of the objects when the Object 2 covariance is
/// considered
///
/// @ref https://ntrs.nasa.gov/citations/20150014495
class Frisbee2015 : public ProbabilityOfCollisionAlgorithm
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     Frisbee2015 algorithm = {} ;
    ///     Frisbee2015 algorithm = { std::make_shared<Alfano2005>() } ;
    ///     Frisbee2015 algorithm = {
    ///         std::nullopt, true, true, Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::GCRF()
    ///     } ;
    /// @endcode
    ///
    /// @param aProbabilityOfCollisionAlgorithmSPtr A probability of collision algorithm used to compute the probability
    /// of collision from the critical (degenerate) combined covariance. If std::nullopt, Frisbee's closed-form solution
    /// is used. Defaults to std::nullopt.
    /// @param considerObject2Covariance Whether to use the Object 2 covariance when Object 1 has no position
    /// uncertainty. If false, only the Object 1 covariance is used. Defaults to true.
    /// @param considerWorstCaseWhenBothCovarianceArePresent Whether to use the worst case (i.e. the covariance yielding
    /// the highest probability of collision) when both covariances are present and the Object 2 covariance can be
    /// considered. Defaults to true.
    /// @param aRelativeSpeedThreshold A relative speed threshold below which the close approach is considered a low
    /// relative speed encounter and the algorithm is not applicable. Defaults to 100 m/s.
    /// @param aFrameSPtr An inertial (or quasi-inertial) frame to use. Defaults to Frame::GCRF().
    ///
    /// @note Frisbee's closed-form solution degrades when the combined hard-body radius is large compared to the
    /// smaller standard deviation of the critical combined covariance (sigma_min). In such cases, using a 2D
    /// probability of collision algorithm (e.g. Alfano2005) is recommended.
    Frisbee2015(
        const std::optional<Shared<const ProbabilityOfCollisionAlgorithm>>& aProbabilityOfCollisionAlgorithmSPtr =
            std::nullopt,
        const bool& considerObject2Covariance = true,
        const bool& considerWorstCaseWhenBothCovarianceArePresent = true,
        const Derived& aRelativeSpeedThreshold = Derived(100.0, Derived::Unit::MeterPerSecond()),
        const Shared<const Frame>& aFrameSPtr = Frame::GCRF()
    );

    /// @brief Destructor
    virtual ~Frisbee2015() override = default;

    /// @brief Identify unsatisfied assumptions for a close approach
    ///
    /// @param aCloseApproach A close approach
    /// @return A non-exhaustive array of unsatisfied assumption descriptions
    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override;

    /// @brief Compute the probability of collision
    ///
    /// @details The hard bodies are approximated by a combined hard-body radius, taken as the enclosing radius of
    /// their combined cross-section in the encounter frame.
    ///
    /// @param aCloseApproach A close approach
    /// @param aHardBody1SPtr The hard body of Object 1
    /// @param aHardBody2SPtr The hard body of Object 2
    ///
    /// @return The probability of collision
    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override;

   private:
    /// @brief Compute the probability of collision using the Object 1 covariance only
    ///
    /// @param aCloseApproach A close approach
    /// @param aHardBody1SPtr The hard body of Object 1
    /// @param aHardBody2SPtr The hard body of Object 2
    ///
    /// @return The probability of collision
    Real computeProbabilityOfCollision_(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const;

    static const Shared<const CoordinateSubset> CartesianPositionSubsetSPtr;

    Derived relativeSpeedThreshold_;
    Shared<const Frame> frameSPtr_;
    bool considerObject2Covariance_;
    bool considerWorstCaseWhenBothCovarianceArePresent_;
    std::optional<Shared<const ProbabilityOfCollisionAlgorithm>> probabilityOfCollisionAlgorithmSPtr_;
};

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
