/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{
namespace model
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;

using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;

/// @brief SGP4 orbit model.
///
/// @details Simplified General Perturbations 4 (SGP4) orbit propagation model using Two-Line Element (TLE) sets.
class SGP4 : public ostk::astrodynamics::trajectory::orbit::Model
{
   public:
    /// @brief Construct an SGP4 model from a TLE. Sets the output frame to GCRF by default.
    ///
    /// @code{.cpp}
    ///     TLE tle = TLE("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
    ///                   "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537");
    ///     SGP4 sgp4 = SGP4(tle);
    /// @endcode
    ///
    /// @param aTle A Two-Line Element set.
    SGP4(const TLE& aTle);

    /// @brief Construct an SGP4 model from a TLE and an output frame. If TEME, the runtime is faster as no frame
    /// transformations are needed.
    ///
    /// @code{.cpp}
    ///     TLE tle = TLE("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
    ///                   "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537");
    ///     SGP4 sgp4 = SGP4(tle, Frame::TEME());
    /// @endcode
    ///
    /// @param aTle A Two-Line Element set.
    /// @param anOutputFrameSPtr An output frame.
    SGP4(const TLE& aTle, const Shared<const Frame>& anOutputFrameSPtr);

    /// @brief Construct an SGP4 model from an array of TLEs and an output frame.
    ///
    /// @details When multiple TLEs are provided, validity intervals are automatically generated
    /// using the midpoints between consecutive TLE epochs as boundaries. The first TLE's interval
    /// extends to the far past and the last TLE's interval extends to the far future.
    ///
    /// @code{.cpp}
    ///     Array<TLE> tles = {TLE("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
    ///                   "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537"),
    ///                   TLE("1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
    ///                   "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537")};
    ///     SGP4 sgp4 = SGP4(tles, Frame::TEME());
    /// @endcode
    ///
    /// @param aTleArray An array of Two-Line Element sets.
    /// @param anOutputFrameSPtr An output frame. Defaults to TEME.
    SGP4(const Array<TLE>& aTleArray, const Shared<const Frame>& anOutputFrameSPtr = Frame::TEME());

    /// @brief Construct an SGP4 model from an array of TLE-Interval pairs and an output frame.
    ///
    /// @details Each pair specifies a TLE and the time interval over which it is valid.
    /// When calculating states, the first TLE whose interval contains the requested instant is used.
    /// Therefore in the case an instant is within multiple TLE intervals, the first TLE whose interval contains the
    /// instant is used.
    ///
    /// @code{.cpp}
    ///     Array<Pair<TLE, Interval>> tleIntervalPairs = {
    ///         {tle1, Interval::Closed(startInstant1, endInstant1)},
    ///         {tle2, Interval::Closed(startInstant2, endInstant2)}
    ///     };
    ///     SGP4 sgp4 = SGP4(tleIntervalPairs, Frame::TEME());
    /// @endcode
    ///
    /// @param aTleIntervalArray An array of TLE-Interval pairs.
    /// @param anOutputFrameSPtr An output frame. Defaults to TEME.
    SGP4(
        const Array<Pair<TLE, Interval>>& aTleIntervalArray,
        const Shared<const Frame>& anOutputFrameSPtr = Frame::TEME()
    );

    /// @brief Copy constructor.
    ///
    /// @param aSGP4Model An SGP4 model to copy.
    SGP4(const SGP4& aSGP4Model);

    /// @brief Destructor.
    ~SGP4();

    /// @brief Copy assignment operator.
    ///
    /// @param aSGP4Model An SGP4 model to copy.
    /// @return A reference to this SGP4 model.
    SGP4& operator=(const SGP4& aSGP4Model);

    /// @brief Clone this SGP4 model.
    ///
    /// @return A pointer to a cloned SGP4 model.
    virtual SGP4* clone() const override;

    /// @brief Check equality with another SGP4 model.
    ///
    /// @param aSGP4Model An SGP4 model to compare against.
    /// @return True if the models are equal.
    bool operator==(const SGP4& aSGP4Model) const;

    /// @brief Check inequality with another SGP4 model.
    ///
    /// @param aSGP4Model An SGP4 model to compare against.
    /// @return True if the models are not equal.
    bool operator!=(const SGP4& aSGP4Model) const;

    /// @brief Stream insertion operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aSGP4Model An SGP4 model to stream.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const SGP4& aSGP4Model);

    /// @brief Check if the SGP4 model is defined.
    ///
    /// @code{.cpp}
    ///     SGP4 sgp4 = SGP4(tle);
    ///     bool isDefined = sgp4.isDefined();
    /// @endcode
    ///
    /// @return True if the model is defined.
    virtual bool isDefined() const override;

    /// @brief Get the TLE associated with this model.
    ///
    /// @code{.cpp}
    ///     SGP4 sgp4 = SGP4(tle);
    ///     TLE tle = sgp4.getTle();
    /// @endcode
    ///
    /// @return The Two-Line Element set.
    TLE getTle() const;

    /// @brief Get the array of TLEs associated with this model.
    ///
    /// @code{.cpp}
    ///     SGP4 sgp4 = SGP4(tleArray);
    ///     Array<TLE> tles = sgp4.getTles();
    /// @endcode
    ///
    /// @return The array of Two-Line Element sets.
    Array<TLE> getTles() const;

    /// @brief Get the validity intervals associated with the TLEs.
    ///
    /// @code{.cpp}
    ///     SGP4 sgp4 = SGP4(tleIntervalPairs);
    ///     Array<Interval> intervals = sgp4.getValidityIntervals();
    /// @endcode
    ///
    /// @return The array of validity intervals.
    Array<Interval> getValidityIntervals() const;

    /// @brief Get the output frame of the SGP4 model.
    ///
    /// @code{.cpp}
    ///     SGP4 sgp4 = SGP4(tle);
    ///     Frame outputFrame = sgp4.getOutputFrame();
    /// @endcode
    ///
    /// @return The output frame.
    Shared<const Frame> getOutputFrame() const;

    /// @brief Get the epoch of the SGP4 model.
    ///
    /// @code{.cpp}
    ///     SGP4 sgp4 = SGP4(tle);
    ///     Instant epoch = sgp4.getEpoch();
    /// @endcode
    ///
    /// @return The epoch instant.
    virtual Instant getEpoch() const override;

    /// @brief Get the revolution number at epoch.
    ///
    /// @code{.cpp}
    ///     SGP4 sgp4 = SGP4(tle);
    ///     Integer revolutionNumber = sgp4.getRevolutionNumberAtEpoch();
    /// @endcode
    ///
    /// @return The revolution number at epoch.
    virtual Integer getRevolutionNumberAtEpoch() const override;

    /// @brief Calculate the state at a given instant.
    ///
    /// @code{.cpp}
    ///     SGP4 sgp4 = SGP4(tle);
    ///     State state = sgp4.calculateStateAt(Instant::Now());
    /// @endcode
    ///
    /// @param anInstant An instant at which to calculate the state.
    /// @return The orbital state at the given instant.
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Calculate states at given instants.
    ///
    /// @details When multiple TLEs are provided, the selected TLE is first one whose interval contains the instant.
    ///
    /// @param anInstantArray An array of instants at which to calculate the states.
    /// @return An array of orbital states at the given instants.
    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const override;

    /// @brief Print the SGP4 model to an output stream.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display a decorator around the output.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   protected:
    virtual bool operator==(const trajectory::Model& aModel) const override;

    virtual bool operator!=(const trajectory::Model& aModel) const override;

   private:
    class Impl;

    Array<TLE> tleArray_;
    Array<Interval> validityIntervals_;
    Shared<const Frame> outputFrameSPtr_;

    Array<Shared<SGP4::Impl>> implArray_;

    mutable Size tleIndex_ = 0;

    static const Duration epochBuffer_;

    Size findTleIndexForInstant(const Instant& anInstant) const;

    static Array<Interval> GenerateIntervalsFromEpochs(const Array<TLE>& aTleArray);
};

}  // namespace model
}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
