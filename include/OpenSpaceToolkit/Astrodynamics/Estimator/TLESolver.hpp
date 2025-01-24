/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver__
#define __OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver__

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
namespace estimation
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

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

        /// @brief Destructor
        ~Analysis() = default;

        /// @brief Stream Operator
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis
        void print(std::ostream& anOutputStream) const;

        /// @brief Get determined TLE
        const TLE& accessDeterminedTLE() const;

        /// @brief Get analysis
        const LeastSquaresSolver::Analysis& accessAnalysis() const;

       private:
        TLE determinedTLE_;
        LeastSquaresSolver::Analysis solverAnalysis_;
    };

    /// @brief Constructor
    ///
    /// @param aSolver Least squares solver
    /// @param aSatelliteNumber Satellite number for TLE
    /// @param anInternationalDesignator International designator for TLE
    /// @param aRevolutionNumber Revolution number
    /// @param aFitWithBStar Whether to fit B* parameter
    TLESolver(
        const LeastSquaresSolver& aSolver,
        const Integer& aSatelliteNumber = 0,
        const String& anInternationalDesignator = "00001A",
        const Integer& aRevolutionNumber = 0,
        const bool aFitWithBStar = true
    );

    /// @brief Access solver
    const LeastSquaresSolver& accessSolver() const;

    /// @brief Access satellite number
    const Integer& accessSatelliteNumber() const;

    /// @brief Access international designator
    const String& accessInternationalDesignator() const;

    /// @brief Access revolution number
    const Integer& accessRevolutionNumber() const;

    /// @brief Access default B* value
    const Real& accessDefaultBStar() const;

    /// @brief Access first derivative of mean motion divided by 2
    const Real& accessFirstDerivativeMeanMotionDividedBy2() const;

    /// @brief Access second derivative of mean motion divided by 6
    const Real& accessSecondDerivativeMeanMotionDividedBy6() const;

    /// @brief Access ephemeris type
    const Integer& accessEphemerisType() const;

    /// @brief Access element set number
    const Integer& accessElementSetNumber() const;

    /// @brief Access whether to fit with B*
    const bool& accessFitWithBStar() const;

    /// @brief Access TLE state builder
    const StateBuilder& accessTLEStateBuilder() const;

    /// @brief Estimate TLE from reference states
    ///
    /// @param anInitialGuess Initial guess (TLE, State+BStar pair, or State)
    /// @param aReferenceStateArray Reference states
    /// @param anInitialGuessSigmas Initial guess sigmas
    /// @param aReferenceStateSigmas Reference state sigmas
    Analysis estimateTLE(
        const std::variant<TLE, Pair<State, Real>, State>& anInitialGuess,
        const Array<State>& aReferenceStateArray,
        const std::unordered_map<CoordinateSubset, Real>& anInitialGuessSigmas = {},
        const std::unordered_map<CoordinateSubset, Real>& aReferenceStateSigmas = {}
    ) const;

   private:
    LeastSquaresSolver solver_;
    Integer satelliteNumber_;
    String internationalDesignator_;
    Integer revolutionNumber_;
    mutable Real defaultBStar_;
    Real firstDerivativeMeanMotionDividedBy2_;
    Real secondDerivativeMeanMotionDividedBy6_;
    Integer ephemerisType_;
    Integer elementSetNumber_;
    bool fitWithBStar_;
    StateBuilder tleStateBuilder_;

    /// @brief Convert TLE to TLE state
    State TLEToTLEState(const TLE& aTLE) const;

    /// @brief Convert TLE state to TLE
    TLE TLEStateToTLE(const State& aTLEState) const;

    /// @brief Convert Cartesian state and B* to TLE state
    State CartesianStateAndBStarToTLEState(const State& aCartesianState, const Real& aBStar = Real::Undefined()) const;
};

}  // namespace estimation
}  // namespace astrodynamics
}  // namespace ostk

#endif
