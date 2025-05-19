/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan__

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
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
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::trajectory::Model;
using ostk::astrodynamics::trajectory::State;

/// @brief Target scanning pointing trajectory model to represent the target scanning of a spacecraft
class TargetScan : public virtual Model
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              TargetScan targetScan = { ... };
    ///              TargetScan targetScanModel(startLLA, endLLA, startInstant, endInstant);
    /// @endcode
    ///
    /// @param aStartLLA The starting LLA of the target scan
    /// @param anEndLLA The ending LLA of the target scan
    /// @param aStartInstant The starting instant of the target scan
    /// @param anEndInstant The ending instant of the target scan
    /// @param aCelestialObject The celestial object (default: Earth::WGS84())
    /// @param aStepSize The step size for the target scan (default: 1 second)
    TargetScan(
        const LLA& aStartLLA,
        const LLA& anEndLLA,
        const Instant& aStartInstant,
        const Instant& anEndInstant,
        const Celestial& aCelestialObject = Earth::WGS84(),
        const Duration& aStepSize = Duration::Seconds(1e-2)
    );

    /// @brief Clone the static model
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              TargetScan* clonedModel = targetScanModel.clone();
    /// @endcode
    ///
    /// @return A pointer to the cloned static model
    virtual TargetScan* clone() const override;

    /// @brief Get the starting LLA of the target scan
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              LLA startLLA = targetScanModel.getStartLLA();
    /// @endcode
    ///
    /// @return The starting LLA of the target scan
    LLA getStartLLA() const;

    /// @brief Get the ending LLA of the target scan
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              LLA endLLA = targetScanModel.getEndLLA();
    /// @endcode
    ///
    /// @return The ending LLA of the target scan
    LLA getEndLLA() const;

    /// @brief Get the starting instant of the target scan
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              Instant startInstant = targetScanModel.getStartInstant();
    /// @endcode
    ///
    /// @return The starting instant of the target scan
    Instant getStartInstant() const;

    /// @brief Get the ending instant of the target scan
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              Instant endInstant = targetScanModel.getEndInstant();
    /// @endcode
    ///
    /// @return The ending instant of the target scan
    Instant getEndInstant() const;

    /// @brief Get the celestial object of the target scan
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              Celestial celestialObject = targetScanModel.getCelestial();
    /// @endcode
    ///
    /// @return The celestial object of the target scan
    Celestial getCelestial() const;

    /// @brief Get the step size of the target scan
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              Duration stepSize = targetScanModel.getStepSize();
    /// @endcode
    ///
    /// @return The step size of the target scan
    Duration getStepSize() const;

    /// @brief Equality operator
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel1 = { ... };
    ///              TargetScan targetScanModel2 = { ... };
    ///              bool isEqual = (targetScanModel1 == targetScanModel2);
    /// @endcode
    ///
    /// @param aTargetScanModel The static model to compare with
    /// @return True if the models are equal, false otherwise
    bool operator==(const TargetScan& aTargetScanModel) const;

    /// @brief Inequality operator
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel1 = { ... };
    ///              TargetScan targetScanModel2 = { ... };
    ///              bool isNotEqual = (targetScanModel1 != targetScanModel2);
    /// @endcode
    ///
    /// @param aTargetScanModel The static model to compare with
    /// @return True if the models are not equal, false otherwise
    bool operator!=(const TargetScan& aTargetScanModel) const;

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              std::cout << targetScanModel;
    /// @endcode
    ///
    /// @param anOutputStream The output stream
    /// @param aTargetScanModel The static model to output
    /// @return The output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const TargetScan& aTargetScanModel);

    /// @brief Check if the static model is defined
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              bool isDefined = targetScanModel.isDefined();
    /// @endcode
    ///
    /// @return True if the model is defined, false otherwise
    virtual bool isDefined() const override;

    /// @brief Calculate the state at a given instant
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              Instant instant = { ... };
    ///              State state = targetScanModel.calculateStateAt(instant);
    /// @endcode
    ///
    /// @param anInstant The instant at which to calculate the state
    /// @return The state at the given instant
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Print the static model to an output stream
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              targetScanModel.print(std::cout, true);
    /// @endcode
    ///
    /// @param anOutputStream The output stream
    /// @param displayDecorator If true, display decorator
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Static method to create a TargetScan from ground speed
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = TargetScan::FromGroundSpeed(startLLA, endLLA, groundSpeed,
    ///              startInstant);
    /// @endcode
    ///
    /// @param aStartLLA The starting LLA of the target scan
    /// @param anEndLLA The ending LLA of the target scan
    /// @param aGroundSpeed The ground speed of the target scan
    /// @param aStartInstant The starting instant of the target scan
    /// @param aCelestialObject The celestial object (default: Earth::WGS84())
    /// @param aStepSize The step size for the target scan (default: 1e-2 second)
    static TargetScan FromGroundSpeed(
        const LLA& aStartLLA,
        const LLA& anEndLLA,
        const Derived& aGroundSpeed,
        const Instant& aStartInstant,
        const Celestial& aCelestialObject = Earth::WGS84(),
        const Duration& aStepSize = Duration::Seconds(1e-2)
    );

   protected:
    /// @brief Equality operator for Model base class
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              Model model = { ... };
    ///              bool isEqual = (targetScanModel == model);
    /// @endcode
    ///
    /// @param aModel The model to compare with
    /// @return True if the models are equal, false otherwise
    virtual bool operator==(const Model& aModel) const override;

    /// @brief Inequality operator for Model base class
    ///
    /// @code{.cpp}
    ///              TargetScan targetScanModel = { ... };
    ///              Model model = { ... };
    ///              bool isNotEqual = (targetScanModel != model);
    /// @endcode
    ///
    /// @param aModel The model to compare with
    /// @return True if the models are not equal, false otherwise
    virtual bool operator!=(const Model& aModel) const override;

   private:
    LLA startLLA_;
    LLA endLLA_;
    Instant startInstant_;
    Instant endInstant_;
    Shared<const Celestial> celestialSPtr_;
    Duration stepSize_;
};

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
