/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Index;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::CoordinatesSubset;

/// @brief                      Define a dynamical system subject to equations of motion

class Dynamics
{
   public:
    typedef std::vector<double> StateVector;  // Container used to hold the state vector
    typedef std::function<void(const StateVector&, StateVector&, const double)> DynamicalEquationWrapper;

    struct DynamicsInformation
    {
        Shared<Dynamics> dynamics;
        Array<Pair<Index, Size>> readIndices;
        Array<Pair<Index, Size>> writeIndices;
        Size reducedStateSize;
    };

    /// @brief              Constructor
    ///
    /// @param              [in] aName A name

    Dynamics(const String& aName = String::Empty());

    /// @brief              Destructor (pure virtual)

    virtual ~Dynamics() = 0;

    /// @brief              Clone dynamics (pure virtual)
    ///
    /// @return             Pointer to cloned dynamics

    virtual Dynamics* clone() const = 0;

    /// @brief              Check if dynamics is defined (pure virtual)
    ///
    /// @return             True if dynamics is defined

    virtual bool isDefined() const = 0;

    /// @brief              Get name
    ///
    /// @return             Name of Dynamics

    String getName() const;

    /// @brief              Returns the coordinates subsets that the instance reads from
    ///
    /// @return             The coordinates subsets that the instance reads from

    virtual Array<Shared<const CoordinatesSubset>> getReadCoordinatesSubsets() const = 0;

    /// @brief              Returns the coordinates subsets that the instance writes to
    ///
    /// @return             The coordinates subsets that the instance writes to

    virtual Array<Shared<const CoordinatesSubset>> getWriteCoordinatesSubsets() const = 0;

    /// @brief              Computes the contribution to the state derivative.
    ///
    /// @param anInstant    An instant
    /// @param x            The 'reduced' state vector (this vector will follow the structure determined by the 'read'
    /// coordinate subsets)
    /// @param aFrame       The 'frame' in which the state vector is expressed
    ///
    /// @return             The 'reduced' derivative state vector (this vector must follow the structure determined by
    /// the 'write' coordinate subsets) expressed in the given frame

    virtual VectorXd computeContribution(const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrame)
        const = 0;

    /// @brief              Print dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief              Get dynamical equations function wrapper (pure virtual)
    ///
    /// @param              [in] aDynamicsArray A array of shared pointers to dynamics
    /// @param              [in] anInstant An instant
    /// @param              [in] aFrame The reference frame in which dynamic equations are resolved
    /// @param              [in] readIndexes An array containing read coordinates subsets indexes and sizes
    /// @param              [in] writeIndexes An array containing write coordinates subsets indexes and sizes
    ///
    /// @return             std::function<void(const std::vector<double>&, std::vector<double>&, const double)>

    static DynamicalEquationWrapper GetDynamicalEquations(
        const Array<DynamicsInformation>& aDynamicsInformationArray,
        const Instant& anInstant,
        const Shared<const Frame>& aFrame
    );

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

   private:
    const String name_;

    /// @brief              Dynamical Equations
    ///
    /// @param              [in] x A state vector
    /// @param              [out] dxdt A state derivative vector
    /// @param              [in] t A step duration from anInstant to the next in seconds
    /// @param              [in] aDynamicsArray A array of shared pointers to dynamics
    /// @param              [in] anInstant An instant
    /// @param              [in] aFrame The reference frame in which dynamic equations are resolved
    /// @param              [in] readIndexes An array containing read coordinates subsets indexes and sizes
    /// @param              [in] writeIndexes An array containing write coordinates subsets indexes and sizes

    static void DynamicalEquations(
        const StateVector& x,
        StateVector& dxdt,
        const double& t,
        const Array<DynamicsInformation>& aDynamicsInformationArray,
        const Instant& anInstant,
        const Shared<const Frame>& aFrame
    );

    static VectorXd ReduceFullStateToReadState(
        const StateVector& x, const Array<Pair<Index, Size>>& readInfo, const Size readSize
    );

    static void AddContributionToFullState(
        StateVector& dxdt, const VectorXd& contribution, const Array<Pair<Index, Size>>& writeInfo
    );
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
