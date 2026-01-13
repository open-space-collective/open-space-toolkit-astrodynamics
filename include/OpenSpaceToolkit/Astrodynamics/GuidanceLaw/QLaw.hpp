/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_QLaw__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_QLaw__

#include <OpenSpaceToolkit/Core/Container/Map.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using Vector5d = Eigen::Matrix<double, 5, 1>;
using Matrix5d = Eigen::Matrix<double, 5, 5>;
using Matrix53d = Eigen::Matrix<double, 5, 3>;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector6d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::solver::FiniteDifferenceSolver;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::StateBuilder;

/// @brief    The Q-law is a Lyapunov feedback control law developed by Petropoulos,
///    based on analytic expressions for maximum rates of change of the orbit elements and
///    the desired changes in the elements. Q, the proximity quotient, serves as a candidate Lyapunov
///    function. As the spacecraft approaches the target orbit, Q decreases monotonically (becoming zero at the target
///    orbit).
/// @ref                        https://dataverse.jpl.nasa.gov/api/access/datafile/10307?gbrecs=true
/// @ref
/// https://www.researchgate.net/publication/370849580_Analytic_Calculation_and_Application_of_the_Q-Law_Guidance_Algorithm_Partial_Derivatives
/// @ref                        https://dataverse.jpl.nasa.gov/api/access/datafile/13727?gbrecs=true
class QLaw : public GuidanceLaw
{
   public:
    struct Parameters
    {
        Parameters(
            const Map<COE::Element, Tuple<double, double>>& anElementWeightsMap,
            const Size& aMValue = 3,
            const Size& aNValue = 4,
            const Size& aRValue = 2,
            const double& aBValue = 0.01,
            const Size& aKValue = 100,
            const double& aPeriapsisWeight = 0.0,
            const Length& minimumPeriapsisradius = Length::Kilometers(6578.0),
            const Real& absoluteEffectivityThreshold = Real::Undefined(),
            const Real& relativeEffectivityThreshold = Real::Undefined()
        );

        Vector5d getControlWeights() const;
        Length getMinimumPeriapsisRadius() const;

        const double m;
        const double n;
        const double r;
        const double b;
        const double k;
        const double periapsisWeight;
        const Real absoluteEffectivityThreshold;
        const Real relativeEffectivityThreshold;

        friend QLaw;

       private:
        const double minimumPeriapsisRadius_;
        Vector5d convergenceThresholds_;
        Vector5d controlWeights_;
        const Array<COE::Element> validElements_ = {
            COE::Element::SemiMajorAxis,
            COE::Element::Eccentricity,
            COE::Element::Inclination,
            COE::Element::Raan,
            COE::Element::Aop,
        };
    };

    enum class GradientStrategy
    {
        Analytical,
        FiniteDifference
    };

    enum class COEDomain
    {
        Osculating,
        BrouwerLyddaneMeanLong,
        BrouwerLyddaneMeanShort
    };

    /// @brief Constructor
    ///
    /// @param aCOE A target orbit described by Classical Orbital Elements.
    /// @param aGravitationalParameter The gravitational parameter of the central body.
    /// @param aParameterSet A set of parameters for the QLaw.
    /// @param aCOEDomain The domain of the Classical Orbital Elements. Defaults to Osculating.
    /// @param aGradientStrategy The strategy to compute the gradient of the QLaw. Defaults to
    /// FiniteDifference
    QLaw(
        const COE& aCOE,
        const Derived& aGravitationalParameter,
        const Parameters& aParameterSet,
        const COEDomain& aCOEDomain,
        const GradientStrategy& aGradientStrategy = GradientStrategy::FiniteDifference
    );

    /// @brief Destructor
    virtual ~QLaw();

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << QLaw(...);
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aGuidanceLaw A guidance Law
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const QLaw& aGuidanceLaw);

    /// @brief Get Parameters
    ///
    /// @return Parameters
    Parameters getParameters() const;

    /// @brief Get target COE
    ///
    /// @return target COE
    COE getTargetCOE() const;

    /// @brief Get Gradient Strategy
    ///
    /// @return Gradient Strategy
    GradientStrategy getGradientStrategy() const;

    /// @brief Get COE Domain
    ///
    /// @return COE Domain
    COEDomain getCOEDomain() const;

    /// @brief Print guidance law
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Compute the acceleration at the provided coordinates based on logic specific to the
    /// guidance law
    ///
    /// @param anInstant An instant
    /// @param aPositionCoordinates The position coordinates
    /// @param aVelocityCoordinates The velocity coordinates
    /// @param aThrustAcceleration The thrust acceleration
    /// @param outputFrame The output frame
    ///
    /// @return The acceleration at the provided coordinates
    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override;

    /// @brief Compute the maximal change in orbital elements
    ///
    /// @param aCOEVector A vector of classical orbital elements
    /// @param aThrustAcceleration The thrust acceleration
    ///
    /// @return The maximal change in orbital elements
    Vector5d computeOrbitalElementsMaximalChange(const Vector5d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief Compute the Proximity Quotient value
    ///
    /// @param aCOEVector The vector of classical orbital elements
    /// @param aThrustAcceleration The thrust acceleration
    ///
    /// @return The Q value
    double computeQ(const Vector5d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief Compute the derivative of Q with respect to the orbital elements
    ///
    /// @param aCOEVector The current vector of classical orbital elements
    /// @param aThrustAcceleration The thrust acceleration
    ///
    /// @return The derivative of Q with respect to the orbital elements
    Vector5d compute_dQ_dOE(const Vector5d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief Compute the thrust direction
    ///
    /// @param aCOEVector The current vector of classical orbital elements
    /// @param aThrustAcceleration The thrust acceleration
    ///
    /// @return The thrust direction
    Vector3d computeThrustDirection(const Vector6d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief Compute the derivative of the orbital elements with respect to the thrust vectors
    ///
    /// @param aCOEVector A vector of classical orbital elements
    /// @param aGravitationalParameter The gravitational parameter
    ///
    /// @return The derivative of the orbital elements with respect to the thrust vectors
    static Matrix53d Compute_dOE_dF(const Vector6d& aCOEVector, const Derived& aGravitationalParameter);

    /// @brief Convert from the theta-R-H frame to the GCRF frame
    ///
    /// @param aPositionCoordinates The position coordinates
    /// @param aVelocityCoordinates The velocity coordinates
    ///
    /// @return The coordinates in the GCRF frame
    static Matrix3d ThetaRHToGCRF(const Vector3d& aPositionCoordinates, const Vector3d& aVelocityCoordinates);

    /// @brief Compute the relative and absolute effectivity of the guidance law
    ///
    /// @ref
    /// https://www.researchgate.net/publication/341296727_Q-Law_Aided_Direct_Trajectory_Optimization_of_Many-Revolution_Low-Thrust_Transfers
    /// @param aState The state from which to extract orbital elements
    /// @param aThrustAcceleration The thrust acceleration
    /// @param discretizationStepCount The number of discretization steps for the true anomaly
    ///
    /// @return A tuple containing the relative and absolute effectivity
    Tuple<double, double> computeEffectivity(
        const State& aState,
        const Real& aThrustAcceleration,
        const Size& discretizationStepCount = 50
    ) const;

   private:
    const Parameters parameters_;
    const double mu_;
    const Vector6d targetCOEVector_;
    const Derived gravitationalParameter_;
    const GradientStrategy gradientStrategy_;
    const FiniteDifferenceSolver finiteDifferenceSolver_;
    const StateBuilder stateBuilder_;
    COEDomain coeDomain_;

    const VectorXd trueAnomalyAngles_ = VectorXd::LinSpaced(50, 0.0, 2.0 * M_PI);

    /// @brief Compute the delta of the orbital elements
    ///
    /// @param aCOEVector The 5-dimensional vector of classical orbital elements (exluding true anomaly)
    ///
    /// @return The delta of the orbital elements
    Vector5d computeDeltaCOE(const Vector5d& aCOEVector) const;

    /// @brief Convert a Cartesian state to a COE vector
    ///
    /// @param aCartesianState The Cartesian state
    ///
    /// @return The COE vector
    Vector6d convertCartesianStateToCOEVector(const COE::CartesianState& aCartesianState) const;

    /// @brief Compute the analytical derivative of Q with respect to the orbital elements
    ///
    /// @param aCOEVector The 5-dimensional vector of classical orbital elements (exluding true anomaly)
    /// @param aThrustAcceleration The thrust acceleration
    ///
    /// @return The analytical derivative of Q with respect to the orbital elements
    Vector5d computeAnalytical_dQ_dOE(const Vector5d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief Compute the numerical derivative of Q with respect to the orbital elements
    ///
    /// @param aCOEVector The 5-dimensional vector of classical orbital elements (exluding true anomaly)
    /// @param aThrustAcceleration The thrust acceleration
    ///
    /// @return The numerical derivative of Q with respect to the orbital elements
    Vector5d computeNumerical_dQ_dOE(const Vector5d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief Compute the thrust vector
    ///
    /// @param aCOEVector The 6-dimensional vector of classical orbital elements
    /// @param aThrustAcceleration The thrust acceleration
    ///
    /// @return The thrust vector
    Vector3d computeThrustVector(const Vector6d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief Compute the effectivity of the guidance law
    ///
    /// @param aCOEVector The 6-dimensional vector of classical orbital elements
    /// @param currentThrustVector The current thrust vector
    /// @param aThrustAcceleration The thrust acceleration
    /// @param trueAnomalyAngles The true anomaly angles
    ///
    /// @return The effectivity of the guidance law
    /// @ref
    /// https://www.researchgate.net/publication/341296727_Q-Law_Aided_Direct_Trajectory_Optimization_of_Many-Revolution_Low-Thrust_Transfers
    Tuple<double, double> computeEffectivity_(
        const Vector6d& aCOEVector,
        const Vector3d& currentThrustVector,
        const double& aThrustAcceleration,
        const VectorXd& trueAnomalyAngles
    ) const;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
