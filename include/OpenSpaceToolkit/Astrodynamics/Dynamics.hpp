/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Index;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::Vector3d;

using ostk::physics::Environment;
using ostk::physics::time::Instant;
using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::trajectory::state::NumericalSolver;
using ostk::astrodynamics::trajectory::state::CoordinatesBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Define a dynamical system subject to equations of motion
class Dynamics
{
   public:
    struct Context
    {
        Context(
            const Shared<Dynamics>& aDynamicsSPtr,
            const Array<Pair<Index, Size>>& aReadIndexes,
            const Array<Pair<Index, Size>>& aWriteIndexes
        );

        Shared<Dynamics> dynamics;
        Array<Pair<Index, Size>> readIndexes;
        Array<Pair<Index, Size>> writeIndexes;
        Size readStateSize;
    };

    /// @brief Constructor
    ///
    /// @param aName A name
    Dynamics(const String& aName = String::Empty());

    /// @brief Destructor
    virtual ~Dynamics();

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << Dynamics(...) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aDynamics A Dynamics
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Dynamics& aDynamics);

    /// @brief Get name
    ///
    /// @return Name of Dynamics
    String getName() const;

    /// @brief Print dynamics
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Check if dynamics is defined (pure virtual)
    ///
    /// @return True if dynamics is defined
    virtual bool isDefined() const = 0;

    /// @brief Return the coordinates subsets that the instance reads from
    ///
    /// @return The coordinates subsets that the instance reads from
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const = 0;

    /// @brief Return the coordinates subsets that the instance writes to
    ///
    /// @return The coordinates subsets that the instance writes to
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const = 0;

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
    ) const = 0;

    /// @brief Get system of equations wrapper
    ///
    /// @param aContextArray An array of Dynamics Information
    /// @param anInstant An instant
    /// @param aFrameSPtr The reference frame in which dynamic equations are resolved
    ///
    /// @return std::function<void(const std::vector<double>&, std::vector<double>&, const double)>
    static NumericalSolver::SystemOfEquationsWrapper GetSystemOfEquations(
        const Array<Context>& aContextArray, const Instant& anInstant, const Shared<const Frame>& aFrameSPtr
    );

    /// @brief Get a list of dynamics from the envrionment
    ///
    /// @param anEnvironment An environment
    ///
    /// @return A list of dynamics
    static Array<Shared<Dynamics>> FromEnvironment(const Environment& anEnvironment);

   private:
    const String name_;

    static void DynamicalEquations(
        const NumericalSolver::StateVector& x,
        NumericalSolver::StateVector& dxdt,
        const double& t,
        const Array<Context>& aContextArray,
        const Instant& anInstant,
        const Shared<const Frame>& aFrameSPtr
    );

    static VectorXd extractReadState(
        const NumericalSolver::StateVector& x, const Array<Pair<Index, Size>>& readInfo, const Size readSize
    );

    static void applyContribution(
        NumericalSolver::StateVector& dxdt, const VectorXd& contribution, const Array<Pair<Index, Size>>& writeInfo
    );
};

}  // namespace astrodynamics
}  // namespace ostk

#endif
