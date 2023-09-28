/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Index;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;
using ostk::math::obj::Vector3d;

using ostk::physics::Environment;
using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

/// @brief                      Define a dynamical system subject to equations of motion

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

    /// @brief                  Constructor
    ///
    /// @param                  [in] aName A name

    Dynamics(const String& aName = String::Empty());

    /// @brief                  Destructor

    virtual ~Dynamics();

    /// @brief                  Output stream operator
    ///
    /// @code
    ///                         std::cout << Dynamics(...) ;
    /// @endcode
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] aDynamics A Dynamics
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Dynamics& aDynamics);

    /// @brief                  Get name
    ///
    /// @return                 Name of Dynamics

    String getName() const;

    /// @brief                  Print dynamics
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Check if dynamics is defined (pure virtual)
    ///
    /// @return                 True if dynamics is defined

    virtual bool isDefined() const = 0;

    /// @brief                  Return the coordinates subsets that the instance reads from
    ///
    /// @return                 The coordinates subsets that the instance reads from

    virtual Array<Shared<const CoordinatesSubset>> getReadCoordinatesSubsets() const = 0;

    /// @brief                  Return the coordinates subsets that the instance writes to
    ///
    /// @return                 The coordinates subsets that the instance writes to

    virtual Array<Shared<const CoordinatesSubset>> getWriteCoordinatesSubsets() const = 0;

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
    ) const = 0;

    /// @brief                  Get system of equations wrapper
    ///
    /// @param                  [in] aContextArray An array of Dynamics Information
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aFrameSPtr The reference frame in which dynamic equations are resolved
    ///
    /// @return                 std::function<void(const std::vector<double>&, std::vector<double>&, const double)>

    static NumericalSolver::SystemOfEquationsWrapper GetSystemOfEquations(
        const Array<Context>& aContextArray, const Instant& anInstant, const Shared<const Frame>& aFrameSPtr
    );

    /// @brief                  Get a list of dynamics from the envrionment
    ///
    /// @param                  [in] anEnvironment An environment
    ///
    /// @return                A list of dynamics

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

}  // namespace astro
}  // namespace ostk

#endif
