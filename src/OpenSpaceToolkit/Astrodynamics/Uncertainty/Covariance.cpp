/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>

// Must come after the OSTk Mathematics headers, which configure the Eigen MatrixBase plugin
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

#include <Eigen/Eigenvalues>

namespace ostk
{
namespace astrodynamics
{
namespace uncertainty
{

using ostk::core::type::Index;

using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Transform;

using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

Covariance::Covariance(
    const Instant& anInstant,
    const MatrixXd& aCoordinates,
    const Shared<const Frame>& aFrameSPtr,
    const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
)
    : instant_(anInstant),
      coordinates_(aCoordinates),
      frameSPtr_(aFrameSPtr),
      coordinatesBrokerSPtr_(std::make_shared<CoordinateBroker>(CoordinateBroker(aCoordinateSubsetsArray)))
{
    if ((Size)coordinates_.rows() != (Size)coordinates_.cols())
    {
        throw ostk::core::error::runtime::Wrong("Matrix not square");
    }

    if (coordinatesBrokerSPtr_ && ((Size)coordinates_.rows() != coordinatesBrokerSPtr_->getNumberOfCoordinates()))
    {
        throw ostk::core::error::runtime::Wrong("Subset-matrix size mismatch");
    }

    if (coordinates_ != coordinates_.transpose())
    {
        throw ostk::core::error::runtime::Wrong("Matrix not symmetric");
    }

    if (coordinates_.size() == 0)
    {
        return;
    }

    // Computed eigenvalues are only accurate up to rounding errors (relative to the matrix norm), so a tolerance is
    // required to accept singular (positive semi-definite) matrices
    const Real tolerance = 1e-12 * coordinates_.cwiseAbs().maxCoeff();

    const Eigen::SelfAdjointEigenSolver<MatrixXd> eigenSolver(coordinates_, Eigen::EigenvaluesOnly);

    if (eigenSolver.eigenvalues().minCoeff() < -tolerance)
    {
        throw ostk::core::error::runtime::Wrong("Matrix not positive semi-definite");
    }
}

Covariance& Covariance::operator=(const Covariance& aCovariance)
{
    if (this != &aCovariance)
    {
        instant_ = aCovariance.instant_;
        coordinates_ = aCovariance.coordinates_;
        frameSPtr_ = aCovariance.frameSPtr_;
        coordinatesBrokerSPtr_ = aCovariance.coordinatesBrokerSPtr_;
    }
    return *this;
}

Covariance Covariance::operator+(const Covariance& aCovariance) const
{
    if (this->instant_ != aCovariance.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if ((*this->frameSPtr_) != (*aCovariance.frameSPtr_))
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if (*this->coordinatesBrokerSPtr_ != *aCovariance.coordinatesBrokerSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Coordinate Subsets");
    }

    return {
        this->instant_,
        this->coordinates_ + aCovariance.coordinates_,
        this->frameSPtr_,
        this->getCoordinateSubsets(),
    };
}

Covariance Covariance::operator-(const Covariance& aCovariance) const
{
    if (this->instant_ != aCovariance.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if ((*this->frameSPtr_) != (*aCovariance.frameSPtr_))
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if (*this->coordinatesBrokerSPtr_ != *aCovariance.coordinatesBrokerSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Coordinate Subsets");
    }

    return {
        this->instant_,
        this->coordinates_ - aCovariance.coordinates_,
        this->frameSPtr_,
        this->getCoordinateSubsets(),
    };
}

std::ostream& operator<<(std::ostream& anOutputStream, const Covariance& aCovariance)
{
    aCovariance.print(anOutputStream);

    return anOutputStream;
}

Size Covariance::getSize() const
{
    return this->coordinates_.rows();
}

const Instant& Covariance::getInstant() const
{
    return this->instant_;
}

const Shared<const Frame>& Covariance::getFrame() const
{
    return this->frameSPtr_;
}

const MatrixXd& Covariance::getCoordinates() const
{
    return this->coordinates_;
}

const Array<Shared<const CoordinateSubset>>& Covariance::getCoordinateSubsets() const
{
    return this->coordinatesBrokerSPtr_->accessSubsets();
}

MatrixXd Covariance::extractCoordinate(const Shared<const CoordinateSubset>& aSubsetSPtr) const
{
    const Index startIndex = this->coordinatesBrokerSPtr_->getSubsetIndex(aSubsetSPtr);
    const Size subsetSize = aSubsetSPtr->getSize();

    return this->coordinates_.block(startIndex, startIndex, subsetSize, subsetSize);
}

MatrixXd Covariance::extractCoordinates(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const
{
    Size extractedSize = 0;
    for (const auto& subset : aCoordinateSubsetsArray)
    {
        extractedSize += subset->getSize();
    }

    MatrixXd extractedCoordinates(extractedSize, extractedSize);

    Index outputRowStart = 0;
    for (const auto& rowSubset : aCoordinateSubsetsArray)
    {
        const Index inputRowStart = this->coordinatesBrokerSPtr_->getSubsetIndex(rowSubset);
        const Size rowSubsetSize = rowSubset->getSize();

        Index outputColStart = 0;
        for (const auto& colSubset : aCoordinateSubsetsArray)
        {
            const Index inputColStart = this->coordinatesBrokerSPtr_->getSubsetIndex(colSubset);
            const Size colSubsetSize = colSubset->getSize();

            extractedCoordinates.block(outputRowStart, outputColStart, rowSubsetSize, colSubsetSize) =
                this->coordinates_.block(inputRowStart, inputColStart, rowSubsetSize, colSubsetSize);

            outputColStart += colSubsetSize;
        }

        outputRowStart += rowSubsetSize;
    }

    return extractedCoordinates;
}

Covariance Covariance::rotate(const Shared<const Frame>& aFrameSPtr) const
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if ((aFrameSPtr == this->frameSPtr_) || (*aFrameSPtr == *this->frameSPtr_))
    {
        return {this->instant_, this->coordinates_, this->frameSPtr_, this->getCoordinateSubsets()};
    }

    const Transform transform = this->frameSPtr_->getTransformTo(aFrameSPtr, this->instant_);
    const Matrix3d rotationMatrix = RotationMatrix::Quaternion(transform.getOrientation()).getMatrix();

    const Size size = this->getSize();
    MatrixXd transformationMatrix = MatrixXd::Identity(size, size);

    Index startIndex = 0;
    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        const Size subsetSize = subset->getSize();

        const bool isRotatable = (std::dynamic_pointer_cast<const CartesianPosition>(subset) != nullptr) ||
                                 (std::dynamic_pointer_cast<const CartesianVelocity>(subset) != nullptr) ||
                                 (std::dynamic_pointer_cast<const CartesianAcceleration>(subset) != nullptr) ||
                                 (std::dynamic_pointer_cast<const AngularVelocity>(subset) != nullptr);

        if (isRotatable)
        {
            transformationMatrix.block(startIndex, startIndex, subsetSize, subsetSize) = rotationMatrix;
        }

        startIndex += subsetSize;
    }

    const MatrixXd transformedCoordinates =
        transformationMatrix * this->coordinates_ * transformationMatrix.transpose();

    // The product is only symmetric up to rounding errors: average it with its transpose to make it exactly symmetric
    const MatrixXd symmetricTransformedCoordinates =
        0.5 * (transformedCoordinates + transformedCoordinates.transpose());

    return {
        this->instant_,
        symmetricTransformedCoordinates,
        aFrameSPtr,
        this->getCoordinateSubsets(),
    };
}

Covariance Covariance::diagonalize() const
{
    const Size size = this->getSize();
    MatrixXd diagonalizedCoordinates = MatrixXd::Zero(size, size);
    diagonalizedCoordinates.diagonal() = this->coordinates_.diagonal();

    return {
        this->instant_,
        diagonalizedCoordinates,
        this->frameSPtr_,
        this->getCoordinateSubsets(),
    };
}

Covariance Covariance::reduce(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const
{
    return {
        this->instant_,
        this->extractCoordinates(aCoordinateSubsetsArray),
        this->frameSPtr_,
        aCoordinateSubsetsArray,
    };
}

Covariance Covariance::scale(const Real& aScalar) const
{
    if (!aScalar.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Scalar");
    }

    if (!aScalar.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Scalar");
    }

    return {
        this->instant_,
        this->coordinates_ * aScalar,
        this->frameSPtr_,
        this->getCoordinateSubsets(),
    };
}

void Covariance::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Uncertainty :: Covariance") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->instant_.isDefined() ? this->instant_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Frame:"
        << ((this->frameSPtr_ != nullptr) && this->frameSPtr_->isDefined() ? this->frameSPtr_->getName() : "Undefined");

    const Array<Shared<const CoordinateSubset>> subsets = this->coordinatesBrokerSPtr_->getSubsets();

    for (const auto& subset : subsets)
    {
        ostk::core::utils::Print::Line(anOutputStream)
            << subset->getName() << this->extractCoordinate(subset).toString(4);
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Covariance Covariance::FromPositionSigmas(
    const Instant& anInstant, const Vector3d& aPositionSigmas, const Shared<const Frame>& aFrameSPtr
)
{
    MatrixXd coordinates = MatrixXd::Zero(3, 3);
    coordinates.diagonal() = aPositionSigmas.array().square();

    return {anInstant, coordinates, aFrameSPtr, {CartesianPosition::Default()}};
}

Covariance Covariance::FromPositionVelocitySigmas(
    const Instant& anInstant,
    const Vector3d& aPositionSigmas,
    const Vector3d& aVelocitySigmas,
    const Shared<const Frame>& aFrameSPtr
)
{
    MatrixXd coordinates = MatrixXd::Zero(6, 6);

    VectorXd variances(6);
    variances.segment(0, 3) = aPositionSigmas.array().square();
    variances.segment(3, 3) = aVelocitySigmas.array().square();
    coordinates.diagonal() = variances;

    return {
        anInstant,
        coordinates,
        aFrameSPtr,
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };
}

}  // namespace uncertainty
}  // namespace astrodynamics
}  // namespace ostk
