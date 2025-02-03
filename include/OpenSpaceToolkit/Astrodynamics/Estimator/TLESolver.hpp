/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimator_TLESolver__
#define __OpenSpaceToolkit_Astrodynamics_Estimator_TLESolver__

#include <unordered_map>
#include <variant>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/LeastSquaresSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::Environment;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Time;

using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

/// @brief Class for solving TLE elements using least squares
class TLESolver
{
   public:
    class Analysis
    {
       public:
        /// @brief Constructor
        Analysis(const TLE& aDeterminedTLE, const LeastSquaresSolver::Analysis& anAnalysis);

        /// @brief Stream Operator
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis
        void print(std::ostream& anOutputStream) const;

        TLE determinedTLE;
        LeastSquaresSolver::Analysis solverAnalysis;
    };

    /// @brief Constructor
    ///
    /// @param aSolver Least squares solver, defaults to default Least Squares Solver
    /// @param aSatelliteNumber Satellite number for TLE, defaults to 0
    /// @param anInternationalDesignator International designator for TLE, defaults to "00001A"
    /// @param aRevolutionNumber Revolution number, defaults to 0
    /// @param aFitWithBStar Whether to fit B* parameter, defaults to true
    /// @param anEstimationFrameSPtr Estimation frame, defaults to GCRF
    TLESolver(
        const LeastSquaresSolver& aSolver = LeastSquaresSolver::Default(),
        const Integer& aSatelliteNumber = 0,
        const String& anInternationalDesignator = "00001A",
        const Integer& aRevolutionNumber = 0,
        const bool aFitWithBStar = true,
        const Shared<const Frame>& anEstimationFrameSPtr = Frame::GCRF()
    );

    /// @brief Access solver
    ///
    /// @return Least squares solver
    const LeastSquaresSolver& accessSolver() const;

    /// @brief Access satellite number
    ///
    /// @return Satellite number
    const Integer& accessSatelliteNumber() const;

    /// @brief Access international designator
    ///
    /// @return International designator
    const String& accessInternationalDesignator() const;

    /// @brief Access revolution number
    ///
    /// @return Revolution number
    const Integer& accessRevolutionNumber() const;

    /// @brief Access whether to fit with B*
    ///
    /// @return Whether to fit with B*
    const bool& accessFitWithBStar() const;

    /// @brief Access estimation frame
    ///
    /// @return Estimation frame
    const Shared<const Frame>& accessEstimationFrame() const;

    /// @brief Access default B* value
    ///
    /// @return Default B* value
    const Real& accessDefaultBStar() const;

    /// @brief Access first derivative of mean motion divided by 2
    ///
    /// @return First derivative of mean motion divided by 2
    const Real& accessFirstDerivativeMeanMotionDividedBy2() const;

    /// @brief Access second derivative of mean motion divided by 6
    ///
    /// @return Second derivative of mean motion divided by 6
    const Real& accessSecondDerivativeMeanMotionDividedBy6() const;

    /// @brief Access ephemeris type
    ///
    /// @return Ephemeris type
    const Integer& accessEphemerisType() const;

    /// @brief Access element set number
    ///
    /// @return Element set number
    const Integer& accessElementSetNumber() const;

    /// @brief Access TLE state builder
    ///
    /// @return TLE state builder
    const StateBuilder& accessTLEStateBuilder() const;

    /// @brief Estimate TLE from observations
    ///
    /// @param anInitialGuess Initial guess (TLE, State+BStar pair, or State)
    /// @param anObservationArray Observations
    /// @param anInitialGuessSigmas Initial guess sigmas
    /// @param anObservationSigmas Reference state sigmas
    Analysis estimateTLE(
        const std::variant<TLE, Pair<State, Real>, State>& anInitialGuess,
        const Array<State>& anObservationArray,
        const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas = DEFAULT_INITIAL_GUESS_SIGMAS,
        const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas = DEFAULT_OBSERVATION_SIGMAS
    ) const;

   private:
    LeastSquaresSolver solver_;
    Integer satelliteNumber_;
    String internationalDesignator_;
    Integer revolutionNumber_;
    bool fitWithBStar_;
    Shared<const Frame> estimationFrameSPtr_;

    mutable Real defaultBStar_;
    Real firstDerivativeMeanMotionDividedBy2_;
    Real secondDerivativeMeanMotionDividedBy6_;
    Integer ephemerisType_;
    Integer elementSetNumber_;
    StateBuilder tleStateBuilder_;

    /// @brief Convert TLE to TLE state
    ///
    /// @param aTLE TLE
    ///
    /// @return TLE state
    State TLEToTLEState(const TLE& aTLE) const;

    /// @brief Convert TLE state to TLE
    ///
    /// @param aTLEState TLE state
    ///
    /// @return TLE
    TLE TLEStateToTLE(const State& aTLEState) const;

    /// @brief Convert Cartesian state and B* to TLE state
    ///
    /// @param aCartesianState Cartesian state
    /// @param aBStar B* value. Default is undefined
    ///
    /// @return TLE state
    State CartesianStateAndBStarToTLEState(const State& aCartesianState, const Real& aBStar = Real::Undefined()) const;
};

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
