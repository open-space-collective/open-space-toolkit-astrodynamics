/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix__
#define __OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix__

#include <ostream>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
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
namespace estimator
{

using ostk::core::container::Array;
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
/// @details Represents the covariance of an object's state coordinates at a specific instant,
/// expressed in a given reference frame. The coordinates are defined by a set of coordinate subsets.
class CovarianceMatrix
{
   public:
    /// @brief Constructor. This constructor makes a new Coordinates Broker under the hood for every
    /// Covariance Matrix.
    ///
    /// @code{.cpp}
    ///     CovarianceMatrix covarianceMatrix = {
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
    /// @param aCoordinateSubsetsArray The coordinate subsets associated to the coordinates
    CovarianceMatrix(
        const Instant& anInstant,
        const MatrixXd& aCoordinates,
        const Shared<const Frame>& aFrameSPtr,
        const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
    );

    /// @brief Copy-assignment operator
    ///
    /// @param aCovarianceMatrix The Covariance Matrix to copy
    /// @return The modified Covariance Matrix
    CovarianceMatrix& operator=(const CovarianceMatrix& aCovarianceMatrix);

    /// @brief Equality operator.
    ///
    /// @param aCovarianceMatrix The Covariance Matrix to compare to
    /// @return True if the Covariance Matrices are equal, false otherwise
    bool operator==(const CovarianceMatrix& aCovarianceMatrix) const;

    /// @brief Inequality operator.
    ///
    /// @param aCovarianceMatrix The Covariance Matrix to compare to
    /// @return True if the Covariance Matrices are not equal, false otherwise
    bool operator!=(const CovarianceMatrix& aCovarianceMatrix) const;

    /// @brief Addition operator.
    ///
    /// @param aCovarianceMatrix The other Covariance Matrix
    /// @return A new Covariance Matrix equal to the sum of the two Covariance Matrices
    CovarianceMatrix operator+(const CovarianceMatrix& aCovarianceMatrix) const;

    /// @brief Subtraction operator.
    ///
    /// @param aCovarianceMatrix The other Covariance Matrix
    /// @return A new Covariance Matrix equal to the difference between the two Covariance Matrices
    CovarianceMatrix operator-(const CovarianceMatrix& aCovarianceMatrix) const;

    /// @brief Stream insertion operator.
    ///
    /// @param anOutputStream The output stream to insert into
    /// @param aCovarianceMatrix The Covariance Matrix to insert
    /// @return The output stream with the Covariance Matrix inserted
    friend std::ostream& operator<<(std::ostream& anOutputStream, const CovarianceMatrix& aCovarianceMatrix);

    /// @brief Check if the Covariance Matrix is defined.
    ///
    /// @return True if the Covariance Matrix is defined, false otherwise
    bool isDefined() const;

    /// @brief Accessor for the instant.
    ///
    /// @return The instant
    const Instant& accessInstant() const;

    /// @brief Accessor for the reference frame.
    ///
    /// @return The reference frame
    const Shared<const Frame> accessFrame() const;

    /// @brief Accessor for the coordinates.
    ///
    /// @return The coordinates
    const MatrixXd& accessCoordinates() const;

    /// @brief Get the size (i.e. number of rows/columns)of the Covariance Matrix.
    ///
    /// @return The size of the Covariance Matrix
    Size getSize() const;

    /// @brief Get the instant associated with the Covariance Matrix.
    ///
    /// @return The instant
    Instant getInstant() const;

    /// @brief Get the reference frame associated with the Covariance Matrix.
    ///
    /// @return The reference frame
    Shared<const Frame> getFrame() const;

    /// @brief Get the coordinates of the Covariance Matrix.
    ///
    /// @return The coordinates
    MatrixXd getCoordinates() const;

    /// @brief Get the coordinate subsets of the Covariance Matrix.
    ///
    /// @return The coordinate subsets
    const Array<Shared<const CoordinateSubset>> getCoordinateSubsets() const;

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

    /// @brief Return a new Covariance Matrix expressed in a different reference frame.
    ///
    /// @details At the moment, only the following coordinate subsets are rotated: CartesianPosition, CartesianVelocity,
    /// CartesianAcceleration and AngularVelocity.
    ///
    /// @param aFrameSPtr The reference frame to transform to
    /// @return A new Covariance Matrix transformed to the given reference frame
    CovarianceMatrix inFrame(const Shared<const Frame>& aFrameSPtr) const;

    /// @brief Return a new diagonalized Covariance Matrix.
    ///
    /// @details Returns a new Covariance Matrix with the same instant, frame, and coordinate subsets,
    /// whose coordinates retain only the diagonal (variance) terms.
    ///
    /// @return A diagonalized Covariance Matrix
    CovarianceMatrix diagonalize() const;

    /// @brief Return a new Covariance Matrix reduced to the given coordinate subsets.
    ///
    /// @details This function can also be used to reorder the coordinate subsets.
    ///
    /// @param aCoordinateSubsetsArray The coordinate subsets to retain
    /// @return A new Covariance Matrix containing only the specified coordinate subsets
    CovarianceMatrix reduce(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const;

    /// @brief Get the cartesian position covariance coordinates associated with the Covariance Matrix (if present).
    ///
    /// @return The cartesian position covariance coordinates
    MatrixXd getPositionCoordinates() const;

    /// @brief Get the cartesian position-velocity covariance coordinates associated with the Covariance Matrix (if
    /// present).
    ///
    /// @return The cartesian position-velocity covariance coordinates
    MatrixXd getPositionVelocityCoordinates() const;

    /// @brief Print the Covariance Matrix to an output stream.
    ///
    /// @param anOutputStream The output stream to print to
    /// @param displayDecorator Whether or not to display the decorator
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Get an undefined Covariance Matrix.
    ///
    /// @code{.cpp}
    ///     CovarianceMatrix covarianceMatrix = CovarianceMatrix::Undefined() ;
    /// @endcode
    ///
    /// @return An undefined Covariance Matrix
    static CovarianceMatrix Undefined();

    /// @brief Build a 3x3 Covariance Matrix from cartesian position sigmas.
    ///
    /// @details The resulting matrix is diagonal, with each diagonal component equal to the square of
    /// the corresponding position sigma.
    ///
    /// @param anInstant An instant
    /// @param aPositionSigmas The cartesian position sigmas
    /// @param aFrameSPtr The reference frame in which the covariance is referenced to and resolved in
    /// @return A Covariance Matrix
    static CovarianceMatrix FromPositionSigmas(
        const Instant& anInstant, const Vector3d& aPositionSigmas, const Shared<const Frame>& aFrameSPtr
    );

    /// @brief Build a 6x6 Covariance Matrix from cartesian position and velocity sigmas.
    ///
    /// @details The resulting matrix is diagonal, with each diagonal component equal to the square of
    /// the corresponding position or velocity sigma.
    ///
    /// @param anInstant An instant
    /// @param aPositionSigmas The cartesian position sigmas
    /// @param aVelocitySigmas The cartesian velocity sigmas
    /// @param aFrameSPtr The reference frame in which the covariance is referenced to and resolved in
    /// @return A Covariance Matrix
    static CovarianceMatrix FromPositionVelocitySigmas(
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

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
