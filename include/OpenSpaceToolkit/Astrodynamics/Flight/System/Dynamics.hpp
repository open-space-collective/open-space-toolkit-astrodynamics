/// Apache License 2.0  

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics__

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

/// @brief                      Defines the a dynamical system subject to equations of motion

class Dynamics
{
   public:
    typedef std::vector<double> StateVector;  // Container used to hold the state vector
    typedef std::function<void(const StateVector&, StateVector&, const double)>
        DynamicalEquationWrapper;  // Function pointer type for returning dynamical equation's pointers

    /// @brief              Constructor (pure virtual)

    Dynamics();

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

    /// @brief              Print dynamics (pure virtual)
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const = 0;

    /// @brief              Obtain dynamical equations function wrapper (pure virtual)
    ///
    /// @return             std::function<void(const std::vector<double>&, std::vector<double>&, const double)>

    virtual DynamicalEquationWrapper getDynamicalEquations() = 0;
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
