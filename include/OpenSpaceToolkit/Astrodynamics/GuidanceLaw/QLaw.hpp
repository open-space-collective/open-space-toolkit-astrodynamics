/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_QLaw__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_QLaw__

#include <OpenSpaceToolkit/Core/Containers/Map.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solvers/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astro
{
namespace guidancelaw
{

using ostk::core::types::Real;
using ostk::core::types::Size;
using ostk::core::types::Shared;
using ostk::core::ctnr::Map;

using ostk::math::object::Vector3d;
using ostk::math::object::Matrix3d;
using Vector5d = Eigen::Matrix<double, 5, 1>;
using Matrix5d = Eigen::Matrix<double, 5, 5>;
using Matrix53d = Eigen::Matrix<double, 5, 3>;
using ostk::math::object::Vector6d;
using ostk::math::object::MatrixXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::GuidanceLaw;
using ostk::astro::solvers::FiniteDifferenceSolver;
using ostk::astro::trajectory::StateBuilder;

/// @brief                     The Q-law is a Lyapunov feedback control law developed by Petropoulos,
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
            const Map<COE::Element, double>& anElementWeights,
            const Size& aMValue = 3,
            const Size& aNValue = 4,
            const Size& aRValue = 2,
            const Size& aKValue = 100,
            const Length& minimumPeriapsisradius = Length::Kilometers(6578.0),
            const double& aBValue = 0.01
        );

        Vector5d getControlWeights() const;
        Length getMinimumPeriapsisRadius() const;

        const Size m;
        const Size n;
        const Size r;
        const double k;
        const double b;
        const double periapsisWeight = 1.0;

        friend QLaw;

       private:
        const double minimumPeriapsisRadius_;
        Vector5d controlWeights_ = Vector5d::Zero();
    };

    /// @brief                  Constructor
    ///
    /// @param                  [in] aCOE A target orbit described by Classical Orbital Elements.
    /// @param                  [in] aGravitationalParameter The gravitational parameter of the central body.
    /// @param                  [in] aParameterSet A set of parameters for the QLaw.
    /// @param                  [in] aFiniteDifferenceSolver The finite difference solver.

    QLaw(
        const COE& aCOE,
        const Derived& aGravitationalParameter,
        const Parameters& aParameterSet,
        const FiniteDifferenceSolver& aFiniteDifferenceSolver
    );

    /// @brief                  Destructor

    virtual ~QLaw();

    /// @brief                  Output stream operator
    ///
    /// @code
    ///                         std::cout << QLaw(...) ;
    /// @endcode
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] aGuidanceLaw A guidance Law
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const QLaw& aGuidanceLaw);

    /// @brief                  Get Parameters
    ///
    /// @return                 Parameters

    Parameters getParameters() const;

    /// @brief                  Get target COE
    ///
    /// @return                 target COE

    COE getTargetCOE() const;

    /// @brief                  Print guidance law
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Compute the acceleration at the provided coordinates based on logic specific to the
    /// guidance law
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aPositionCoordinates The position coordinates
    /// @param                  [in] aVelocityCoordinates The velocity coordinates
    /// @param                  [in] aThrustAcceleration The thrust acceleration
    /// @param                  [in] outputFrame The output frame
    ///
    /// @return                 The acceleration at the provided coordinates

    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override;

    /// @brief                  Compute the maximal change in orbital elements
    ///
    /// @param                  [in] aCOEVector A vector of classical orbital elements
    /// @param                  [in] aThrustAcceleration The thrust acceleration
    ///
    /// @return                 The maximal change in orbital elements

    Vector5d computeOrbitalElementsMaximalChange(const Vector5d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief                  Compute the Proximity Quotient value
    ///
    /// @param                  [in] aCOEVector The vector of classical orbital elements
    /// @param                  [in] aThrustAcceleration The thrust acceleration
    ///
    /// @return                 The Q value

    double computeQ(const Vector5d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief                  Compute the derivative of Q with respect to the orbital elements
    ///
    /// @param                  [in] aCOEVector The current vector of classical orbital elements
    /// @param                  [in] aThrustAcceleration The thrust acceleration
    ///
    /// @return                 The derivative of Q with respect to the orbital elements

    Vector5d compute_dQ_dOE(const Vector5d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief                  Compute the thrust direction
    ///
    /// @param                  [in] aCOEVector The current vector of classical orbital elements
    /// @param                  [in] aThrustAcceleration The thrust acceleration
    ///
    /// @return                 The thrust direction

    Vector3d computeThrustDirection(const Vector6d& aCOEVector, const double& aThrustAcceleration) const;

    /// @brief                  Compute the derivative of the orbital elements with respect to the thrust vectors
    ///
    /// @param                  [in] aCOEVector A vector of classical orbital elements
    /// @param                  [in] aGravitationalParameter The gravitational parameter
    ///
    /// @return                 The derivative of the orbital elements with respect to the thrust vectors

    static Matrix53d Compute_dOE_dF(const Vector6d& aCOEVector, const Derived& aGravitationalParameter);

    /// @brief                  Convert from the theta-RH frame to the GCRF frame
    ///
    /// @param                  [in] aPositionCoordinates The position coordinates
    /// @param                  [in] aVelocityCoordinates The velocity coordinates
    ///
    /// @return                 The coordinates in the GCRF frame

    static Matrix3d ThetaRHToGCRF(const Vector3d& aPositionCoordinates, const Vector3d& aVelocityCoordinates);

   private:
    Parameters parameters_;
    const double mu_;
    const Vector6d targetCOEVector_;
    const Derived gravitationalParameter_;
    const FiniteDifferenceSolver finiteDifferenceSolver_;
    const StateBuilder stateBuilder_;
};

}  // namespace guidancelaw
}  // namespace astro
}  // namespace ostk

#endif
