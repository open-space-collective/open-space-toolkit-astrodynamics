/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

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

using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::ctnr::Array;

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

    virtual Array<Shared<const CoordinatesSubset>> getReadCoordinateSubsets() const = 0;

    /// @brief              Returns the coordinates subsets that the instance writes to
    ///
    /// @return             The coordinates subsets that the instance writes to

    virtual Array<Shared<const CoordinatesSubset>> getWriteCoordinateSubsets() const = 0;

    /// @brief              Declares the coordinates involved in the contribution calculation
    ///
    /// @param              [in] coordinatesBroker The coordinate broker mapping state coordinates and indexes

    virtual void declareCoordinates(const Shared<CoordinatesBroker>& coordinatesBroker) = 0;

    /// @brief              Apply contribution to the state derivative (pure virtual)
    ///
    /// @param              [in] x A state vector
    /// @param              [out] dxdt A state derivative vector
    /// @param              [in] anInstant An instant

    virtual void applyContribution(const StateVector& x, StateVector& dxdt, const Instant& anInstant) const = 0;

    /// @brief              Print dynamics
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief              Get dynamical equations function wrapper (pure virtual)
    ///
    /// @param              [in] aDynamicsArray A array of shared pointers to dynamics
    /// @param              [in] anInstant An instant
    /// @return             std::function<void(const std::vector<double>&, std::vector<double>&, const double)>

    static DynamicalEquationWrapper GetDynamicalEquations(
        const Array<Shared<Dynamics>>& aDynamicsArray, const Instant& anInstant
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

    static void DynamicalEquations(
        const StateVector& x,
        StateVector& dxdt,
        const double& t,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const Instant& anInstant
    );
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
