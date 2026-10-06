/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance__
#define __OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance__

#include <ostream>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace uncertainty
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Covariance matrix at a given instant in time.
///
/// @details Represents the covariance of an object's state at a specific instant,
/// expressed in a given reference frame. The contents of the matrix are identified
/// an ordered set of coordinate subsets.
class Covariance
{
   public:
    /// @brief Constructor. This constructor makes a new Coordinates Broker under the hood for every
    /// Covariance.
    ///
    /// @code{.cpp}
    ///     Covariance covariance = {
    ///         Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC),
    ///         aCoordinates,
    ///         Frame::GCRF(),
    ///         { CartesianPosition::Default(), CartesianVelocity::Default() }
    ///     } ;
    /// @endcode
    ///
    /// @param anInstant An instant
    /// @param aCoordinates The coordinates at the instant in International System of Units
    /// @param aFrameSPtr The reference frame in which the coordinates are referenced to and
    /// resolved in
    /// @param aCoordinateSubsetsArray The coordinate subsets associated with the coordinates
    /// @throw ostk::core::error::runtime::Wrong If the coordinates are not square, do not match the coordinate subsets,
    /// are not exactly symmetric or are not positive semi-definite (within a tolerance relative to the largest
    /// coefficient)
    Covariance(
        const Instant& anInstant,
        const MatrixXd& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
    );

    /// @brief Copy-assignment operator
    ///
    /// @param aCovariance The Covariance to copy
    /// @return The modified Covariance
    Covariance& operator=(const Covariance& aCovariance);

    /// @brief Addition operator (element-wise).
    ///
    /// @param aCovariance The other Covariance
    /// @return A new Covariance equal to the element-wise sum of the two Covariances
    Covariance operator+(const Covariance& aCovariance) const;

    /// @brief Subtraction operator (element-wise).
    ///
    /// @param aCovariance The other Covariance
    /// @return A new Covariance equal to the element-wise difference between the two Covariances
    Covariance operator-(const Covariance& aCovariance) const;

    /// @brief Stream insertion operator.
    ///
    /// @param anOutputStream The output stream to insert into
    /// @param aCovariance The Covariance to insert
    /// @return The output stream with the Covariance inserted
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Covariance& aCovariance);

    /// @brief Get the size (i.e. number of rows/columns)of the Covariance.
    ///
    /// @return The size of the Covariance
    Size getSize() const;

    /// @brief Get the instant associated with the Covariance.
    ///
    /// @return The instant
    const Instant& getInstant() const;

    /// @brief Get the reference frame associated with the Covariance.
    ///
    /// @return The reference frame
    const Shared<const Frame>& getFrame() const;

    /// @brief Get the coordinates of the Covariance.
    ///
    /// @return The coordinates
    const MatrixXd& getCoordinates() const;

    /// @brief Get the coordinate subsets of the Covariance.
    ///
    /// @return The coordinate subsets
    const Array<Shared<const CoordinateSubset>>& getCoordinateSubsets() const;

    /// @brief Extract the coordinates for a single subset.
    ///
    /// @param aSubsetSPtr The subset to extract the coordinates for
    /// @return The coordinates for the subset
    MatrixXd extractCoordinate(const Shared<const CoordinateSubset>& aSubsetSPtr) const;

    /// @brief Extract the coordinates for multiple subsets.
    ///
    /// @param aCoordinateSubsetsArray The array of subsets to extract the coordinates for
    /// @return The coordinates for the subsets
    MatrixXd extractCoordinates(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const;

    /// @brief Return a new Covariance rotated into a different reference frame.
    ///
    /// @details This is a pure rotation, only the following coordinate subsets are rotated:
    /// CartesianPosition, CartesianVelocity, CartesianAcceleration and AngularVelocity. The
    /// rest are left unchanged.
    ///
    /// @param aFrameSPtr The reference frame to rotate into
    /// @return A new Covariance rotated into the given reference frame
    Covariance rotate(const Shared<const Frame>& aFrameSPtr) const;

    /// @brief Return a new diagonalized Covariance.
    ///
    /// @details Returns a new Covariance with the same instant, frame, and coordinate subsets,
    /// whose coordinates retain only the diagonal (variance) terms.
    ///
    /// @return A diagonalized Covariance
    Covariance diagonalize() const;

    /// @brief Return a new Covariance reduced to the given coordinate subsets.
    ///
    /// @details This function can also be used to reorder the coordinate subsets.
    ///
    /// @param aCoordinateSubsetsArray The coordinate subsets to retain
    /// @return A new Covariance containing only the specified coordinate subsets
    Covariance reduce(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const;

    /// @brief Return a new Covariance whose coordinates are multiplied by a scalar.
    ///
    /// @param aScalar The strictly positive scalar to multiply the covariance coordinates by
    /// @return A new Covariance with scaled coordinates
    Covariance scale(const Real& aScalar) const;

    /// @brief Print the Covariance to an output stream.
    ///
    /// @param anOutputStream The output stream to print to
    /// @param displayDecorator Whether or not to display the decorator
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Build a 3x3 Covariance from cartesian position sigmas.
    ///
    /// @details The resulting matrix is diagonal, with each diagonal component equal to the square of
    /// the corresponding position sigma.
    ///
    /// @param anInstant An instant
    /// @param aPositionSigmas The cartesian position sigmas
    /// @param aFrameSPtr The reference frame in which the covariance is referenced to and resolved in
    /// @return A Covariance
    static Covariance FromPositionSigmas(
        const Instant& anInstant, const Vector3d& aPositionSigmas, const Shared<const Frame>& aFrameSPtr
    );

    /// @brief Build a 6x6 Covariance from cartesian position and velocity sigmas.
    ///
    /// @details The resulting matrix is diagonal, with each diagonal component equal to the square of
    /// the corresponding position or velocity sigma.
    ///
    /// @param anInstant An instant
    /// @param aPositionSigmas The cartesian position sigmas
    /// @param aVelocitySigmas The cartesian velocity sigmas
    /// @param aFrameSPtr The reference frame in which the covariance is referenced to and resolved in
    /// @return A Covariance
    static Covariance FromPositionVelocitySigmas(
        const Instant& anInstant,
        const Vector3d& aPositionSigmas,
        const Vector3d& aVelocitySigmas,
        const Shared<const Frame>& aFrameSPtr
    );

   private:
    Instant instant_;
    MatrixXd coordinates_;
    Shared<const Frame> frameSPtr_;
    Shared<const CoordinateBroker> coordinatesBrokerSPtr_;
};

}  // namespace uncertainty
}  // namespace astrodynamics
}  // namespace ostk

#endif
