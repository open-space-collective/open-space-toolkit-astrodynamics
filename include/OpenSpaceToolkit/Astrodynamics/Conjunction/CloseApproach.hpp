/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach__

#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{

using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::StateBuilder;

/// @brief Close approach between two objects.
///
/// @details This class represents a close approach event between two objects, providing access to the states of both
/// objects at the time of closest approach, the miss distance, and the relative state.
class CloseApproach
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { object1State, object2State };
    /// @endcode
    ///
    /// @param anObject1State The state of Object 1
    /// @param anObject2State The state of Object 2
    CloseApproach(const State& anObject1State, const State& anObject2State);

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = {
    ///                  object1State, object2State, object1CovarianceMatrix, object2CovarianceMatrix
    ///              };
    /// @endcode
    ///
    /// @param anObject1State The state of Object 1
    /// @param anObject2State The state of Object 2
    /// @param anObject1CovarianceMatrix The covariance matrix of Object 1
    /// @param anObject2CovarianceMatrix The covariance matrix of Object 2
    CloseApproach(
        const State& anObject1State,
        const State& anObject2State,
        const CovarianceMatrix& anObject1CovarianceMatrix,
        const CovarianceMatrix& anObject2CovarianceMatrix
    );

    /// @brief Equal to operator
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              closeApproach == anotherCloseApproach ;
    /// @endcode
    ///
    /// @param aCloseApproach A close approach
    /// @return True if close approaches are equal
    bool operator==(const CloseApproach& aCloseApproach) const;

    /// @brief Not equal to operator
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              closeApproach != anotherCloseApproach ;
    /// @endcode
    ///
    /// @param aCloseApproach A close approach
    /// @return True if close approaches are not equal
    bool operator!=(const CloseApproach& aCloseApproach) const;

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///              std::cout << closeApproach ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aCloseApproach A close approach
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const CloseApproach& aCloseApproach);

    /// @brief Check if the close approach is defined
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              closeApproach.isDefined() ;
    /// @endcode
    ///
    /// @return True if close approach is defined
    bool isDefined() const;

    /// @brief Get the state of Object 1
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              State object1State = closeApproach.getObject1State() ;
    /// @endcode
    ///
    /// @return The state of Object 1
    State getObject1State() const;

    /// @brief Get the state of Object 2
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              State object2State = closeApproach.getObject2State() ;
    /// @endcode
    ///
    /// @return The state of Object 2
    State getObject2State() const;

    /// @brief Get the covariance matrix of Object 1
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              CovarianceMatrix object1CovarianceMatrix = closeApproach.getObject1CovarianceMatrix() ;
    /// @endcode
    ///
    /// @return The covariance matrix of Object 1
    CovarianceMatrix getObject1CovarianceMatrix() const;

    /// @brief Get the covariance matrix of Object 2
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              CovarianceMatrix object2CovarianceMatrix = closeApproach.getObject2CovarianceMatrix() ;
    /// @endcode
    ///
    /// @return The covariance matrix of Object 2
    CovarianceMatrix getObject2CovarianceMatrix() const;

    /// @brief Set the covariance matrices of Object 1 and Object 2
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              closeApproach.setCovarianceMatrices(object1CovarianceMatrix, object2CovarianceMatrix) ;
    /// @endcode
    ///
    /// @param anObject1CovarianceMatrix The covariance matrix of Object 1
    /// @param anObject2CovarianceMatrix The covariance matrix of Object 2
    void setCovarianceMatrices(
        const CovarianceMatrix& anObject1CovarianceMatrix, const CovarianceMatrix& anObject2CovarianceMatrix
    );

    /// @brief Return a new Close Approach with the covariance matrices of Object 1 and Object 2 scaled by the given
    /// factors.
    ///
    /// @details If a scale factor is undefined, the corresponding covariance matrix is not scaled. This is useful when
    /// scaling only one covariance or when a covariance is undefined.
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              CloseApproach scaledCloseApproach = closeApproach.scale(2.0, 4.0) ;
    ///              CloseApproach scaledObject1Only = closeApproach.scale(2.0) ;
    /// @endcode
    ///
    /// @param aScaleFactor1 The scale factor for Object 1 covariance. Defaults to Real::Undefined()
    /// @param aScaleFactor2 The scale factor for Object 2 covariance. Defaults to Real::Undefined()
    /// @return A new Close Approach with scaled covariance matrices
    CloseApproach scale(const Real& aScaleFactor1 = Real::Undefined(), const Real& aScaleFactor2 = Real::Undefined())
        const;

    /// @brief Return a new Close Approach with Object 1 and Object 2 swapped
    ///
    /// @details The states and covariance matrices of Object 1 and Object 2 are exchanged.
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              CloseApproach flippedCloseApproach = closeApproach.flip() ;
    /// @endcode
    ///
    /// @return A new Close Approach with Object 1 and Object 2 swapped
    CloseApproach flip() const;

    /// @brief Get the instant of the close approach
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              Instant instant = closeApproach.getInstant() ;
    /// @endcode
    ///
    /// @return The instant
    Instant getInstant() const;

    /// @brief Get the miss distance
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              Length missDistance = closeApproach.getMissDistance() ;
    /// @endcode
    ///
    /// @return The miss distance
    Length getMissDistance() const;

    /// @brief Get the relative state (Object 2 relative to Object 1)
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              State relativeState = closeApproach.getRelativeState() ;
    /// @endcode
    ///
    /// @return The relative state
    State getRelativeState() const;

    /// @brief Get the relative velocity magnitude
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              Derived relativeVelocity = closeApproach.getRelativeVelocity() ;
    /// @endcode
    ///
    /// @return The relative velocity magnitude in meters per second
    Derived getRelativeVelocity() const;

    /// @brief Get the "default" convention of the encounter frame centered on Object 1.
    ///
    /// @details The encounter frame is a local orbital frame centered on Object 1:
    /// - z-axis: Normalized relative velocity (Object 2 velocity - Object 1 velocity)
    /// - y-axis: Normalized cross product of the z-axis and the relative position (Object 2 position - Object 1
    /// position)
    /// - x-axis: Completes the right-handed coordinate system
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              Shared<const Frame> encounterFrame = closeApproach.getEncounterFrame() ;
    /// @endcode
    ///
    /// @param aFrameSPtr The inertial (or quasi-inertial) frame in which relative position and velocity are computed.
    /// Defaults to GCRF.
    /// @return The encounter frame
    Shared<const Frame> getEncounterFrame(const Shared<const Frame>& aFrameSPtr = Frame::GCRF()) const;

    /// @brief Compute the miss distance components in the desired frame
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              Tuple<Length, Length, Length> missDistanceComponents =
    ///                  closeApproach.computeMissDistanceComponentsInFrame(Frame::GCRF()) ;
    /// @endcode
    ///
    /// @param aFrame The frame in which to resolve the miss distance components
    /// @return The miss distance components (x, y, z)
    Tuple<Length, Length, Length> computeMissDistanceComponentsInFrame(const Shared<const Frame>& aFrame) const;

    /// @brief Compute the miss distance components in a local orbital frame (generated from Object 1 state)
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = { ... } ;
    ///              Shared<const LocalOrbitalFrameFactory> lofFactorySPtr = { ... } ;
    ///              Tuple<Length, Length, Length> missDistanceComponents =
    ///                  closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(lofFactorySPtr) ;
    /// @endcode
    ///
    /// @param aLocalOrbitalFrameFactorySPtr The local orbital frame factory (generated from Object 1 state)
    /// @return The miss distance components (radial, in-track, cross-track or similar depending on the factory)
    Tuple<Length, Length, Length> computeMissDistanceComponentsInLocalOrbitalFrame(
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
    ) const;

    /// @brief Print the close approach
    ///
    /// @param [in, out] anOutputStream The output stream where the close approach will be printed
    /// @param displayDecorator A boolean indicating whether or not to display decorator during
    ///                  printing
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Construct an undefined close approach
    ///
    /// @code{.cpp}
    ///              CloseApproach closeApproach = CloseApproach::Undefined() ;
    /// @endcode
    ///
    /// @return Undefined close approach
    static CloseApproach Undefined();

   private:
    State object1State_;
    State object2State_;
    CovarianceMatrix object1CovarianceMatrix_;
    CovarianceMatrix object2CovarianceMatrix_;
    StateBuilder stateBuilder_;

    static void ThrowIfInconsistentCovarianceMatrixInstants(
        const Instant& aStateInstant,
        const CovarianceMatrix& anObject1CovarianceMatrix,
        const CovarianceMatrix& anObject2CovarianceMatrix
    );
};

}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
