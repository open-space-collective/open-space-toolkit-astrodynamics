/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Nadir__

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

using ostk::physics::coordinate::Position;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::trajectory::Model;
using ostk::astrodynamics::trajectory::State;

/// @brief Nadir pointing trajectory model to represent the geodetic nadir position of an orbit on the surface of the
/// earth
class Nadir : public virtual Model
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              Nadir nadir = { ... };
    ///              Nadir nadirModel(orbit);
    /// @endcode
    ///
    /// @param anOrbit The orbit of the nadir model
    /// @param aStepSize The step size for the nadir model. Defaults to 1e-2 seconds
    /// @return A nadir model
    Nadir(const Orbit& anOrbit, const Duration& aStepSize = Duration::Seconds(1e-2));

    /// @brief Clone the nadir model
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              Nadir* clonedModel = nadirModel.clone();
    /// @endcode
    ///
    /// @return A pointer to the cloned nadir model
    virtual Nadir* clone() const override;

    /// @brief Get the orbit of the nadir model
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              Orbit orbit = nadirModel.getOrbit();
    /// @endcode
    ///
    /// @return The orbit of the nadir model
    Orbit getOrbit() const;

    /// @brief Get the step size of the nadir model
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              Duration stepSize = nadirModel.getStepSize();
    /// @endcode
    ///
    /// @return The step size of the nadir model
    Duration getStepSize() const;

    /// @brief Equality operator
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel1 = { ... };
    ///              Nadir nadirModel2 = { ... };
    ///              bool isEqual = (nadirModel1 == nadirModel2);
    /// @endcode
    ///
    /// @param aNadirModel The nadir model to compare with
    /// @return True if the models are equal, false otherwise
    bool operator==(const Nadir& aNadirModel) const;

    /// @brief Inequality operator
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel1 = { ... };
    ///              Nadir nadirModel2 = { ... };
    ///              bool isNotEqual = (nadirModel1 != nadirModel2);
    /// @endcode
    ///
    /// @param aNadirModel The nadir model to compare with
    /// @return True if the models are not equal, false otherwise
    bool operator!=(const Nadir& aNadirModel) const;

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              std::cout << nadirModel;
    /// @endcode
    ///
    /// @param anOutputStream The output stream
    /// @param aNadirModel The nadir model to output
    /// @return The output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Nadir& aNadirModel);

    /// @brief Check if the nadir model is defined
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              bool isDefined = nadirModel.isDefined();
    /// @endcode
    ///
    /// @return True if the model is defined, false otherwise
    virtual bool isDefined() const override;

    /// @brief Calculate the state at a given instant
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              Instant instant = { ... };
    ///              State state = nadirModel.calculateStateAt(instant);
    /// @endcode
    ///
    /// @param anInstant The instant at which to calculate the state
    /// @return The state at the given instant
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Print the nadir model to an output stream
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              nadirModel.print(std::cout, true);
    /// @endcode
    ///
    /// @param anOutputStream The output stream
    /// @param displayDecorator If true, display decorator
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   protected:
    /// @brief Equality operator for Model base class
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              Model model = { ... };
    ///              bool isEqual = (nadirModel == model);
    /// @endcode
    ///
    /// @param aModel The model to compare with
    /// @return True if the models are equal, false otherwise
    virtual bool operator==(const Model& aModel) const override;

    /// @brief Inequality operator for Model base class
    ///
    /// @code{.cpp}
    ///              Nadir nadirModel = { ... };
    ///              Model model = { ... };
    ///              bool isNotEqual = (nadirModel != model);
    /// @endcode
    ///
    /// @param aModel The model to compare with
    /// @return True if the models are not equal, false otherwise
    virtual bool operator!=(const Model& aModel) const override;

   private:
    Orbit orbit_;
    Duration stepSize_;
};

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
