/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
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

CovarianceMatrix::CovarianceMatrix(
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
}

CovarianceMatrix& CovarianceMatrix::operator=(const CovarianceMatrix& aCovarianceMatrix)
{
    if (this != &aCovarianceMatrix)
    {
        instant_ = aCovarianceMatrix.instant_;
        coordinates_ = aCovarianceMatrix.coordinates_;
        frameSPtr_ = aCovarianceMatrix.frameSPtr_;
        coordinatesBrokerSPtr_ = aCovarianceMatrix.coordinatesBrokerSPtr_;
    }
    return *this;
}

bool CovarianceMatrix::operator==(const CovarianceMatrix& aCovarianceMatrix) const
{
    if ((!this->isDefined()) || (!aCovarianceMatrix.isDefined()))
    {
        return false;
    }

    if (this->instant_ != aCovarianceMatrix.instant_)
    {
        return false;
    }

    if ((*this->frameSPtr_) != (*aCovarianceMatrix.frameSPtr_))
    {
        return false;
    }

    if (this->getSize() != aCovarianceMatrix.getSize())
    {
        return false;
    }

    for (const Shared<const CoordinateSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        if (!aCovarianceMatrix.coordinatesBrokerSPtr_->hasSubset(subset))
        {
            return false;
        }
    }

    if (this->extractCoordinates(this->coordinatesBrokerSPtr_->accessSubsets()) !=
        aCovarianceMatrix.extractCoordinates(this->coordinatesBrokerSPtr_->accessSubsets()))
    {
        return false;
    }

    return true;
}

bool CovarianceMatrix::operator!=(const CovarianceMatrix& aCovarianceMatrix) const
{
    return !((*this) == aCovarianceMatrix);
}

CovarianceMatrix CovarianceMatrix::operator+(const CovarianceMatrix& aCovarianceMatrix) const
{
    if ((!this->isDefined()) || (!aCovarianceMatrix.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    if (this->instant_ != aCovarianceMatrix.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if ((*this->frameSPtr_) != (*aCovarianceMatrix.frameSPtr_))
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if (*this->coordinatesBrokerSPtr_ != *aCovarianceMatrix.coordinatesBrokerSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Coordinate Subsets");
    }

    return {
        this->instant_,
        this->coordinates_ + aCovarianceMatrix.accessCoordinates(),
        this->frameSPtr_,
        this->getCoordinateSubsets(),
    };
}

CovarianceMatrix CovarianceMatrix::operator-(const CovarianceMatrix& aCovarianceMatrix) const
{
    if ((!this->isDefined()) || (!aCovarianceMatrix.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    if (this->instant_ != aCovarianceMatrix.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant");
    }

    if ((*this->frameSPtr_) != (*aCovarianceMatrix.frameSPtr_))
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if (*this->coordinatesBrokerSPtr_ != *aCovarianceMatrix.coordinatesBrokerSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Coordinate Subsets");
    }

    return {
        this->instant_,
        this->coordinates_ - aCovarianceMatrix.accessCoordinates(),
        this->frameSPtr_,
        this->getCoordinateSubsets(),
    };
}

std::ostream& operator<<(std::ostream& anOutputStream, const CovarianceMatrix& aCovarianceMatrix)
{
    aCovarianceMatrix.print(anOutputStream);

    return anOutputStream;
}

bool CovarianceMatrix::isDefined() const
{
    return this->instant_.isDefined() && this->coordinates_.isDefined() && (this->frameSPtr_ != nullptr) &&
           this->frameSPtr_->isDefined() && (this->coordinatesBrokerSPtr_ != nullptr);
}

const Instant& CovarianceMatrix::accessInstant() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    return this->instant_;
}

const Shared<const Frame> CovarianceMatrix::accessFrame() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    return this->frameSPtr_;
}

const MatrixXd& CovarianceMatrix::accessCoordinates() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    return this->coordinates_;
}

Size CovarianceMatrix::getSize() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    return this->coordinates_.rows();
}

Instant CovarianceMatrix::getInstant() const
{
    return this->accessInstant();
}

Shared<const Frame> CovarianceMatrix::getFrame() const
{
    return this->accessFrame();
}

MatrixXd CovarianceMatrix::getCoordinates() const
{
    return this->accessCoordinates();
}

const Array<Shared<const CoordinateSubset>> CovarianceMatrix::getCoordinateSubsets() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    return this->coordinatesBrokerSPtr_->getSubsets();
}

MatrixXd CovarianceMatrix::extractCoordinate(const Shared<const CoordinateSubset>& aSubsetSPtr) const
{
    const Index startIndex = this->coordinatesBrokerSPtr_->getSubsetIndex(aSubsetSPtr);
    const Size subsetSize = aSubsetSPtr->getSize();

    return this->accessCoordinates().block(startIndex, startIndex, subsetSize, subsetSize);
}

MatrixXd CovarianceMatrix::extractCoordinates(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
) const
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
                this->accessCoordinates().block(inputRowStart, inputColStart, rowSubsetSize, colSubsetSize);

            outputColStart += colSubsetSize;
        }

        outputRowStart += rowSubsetSize;
    }

    return extractedCoordinates;
}

CovarianceMatrix CovarianceMatrix::inFrame(const Shared<const Frame>& aFrameSPtr) const
{
    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
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

    return {
        this->instant_,
        transformedCoordinates,
        aFrameSPtr,
        this->getCoordinateSubsets(),
    };
}

CovarianceMatrix CovarianceMatrix::diagonalize() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    const Size size = this->getSize();
    MatrixXd diagonalizedCoordinates = MatrixXd::Zero(size, size);
    diagonalizedCoordinates.diagonal() = this->accessCoordinates().diagonal();

    return {
        this->instant_,
        diagonalizedCoordinates,
        this->frameSPtr_,
        this->getCoordinateSubsets(),
    };
}

CovarianceMatrix CovarianceMatrix::reduce(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    return {
        this->instant_,
        this->extractCoordinates(aCoordinateSubsetsArray),
        this->frameSPtr_,
        aCoordinateSubsetsArray,
    };
}

CovarianceMatrix CovarianceMatrix::scale(const Real& aScalar) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

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

MatrixXd CovarianceMatrix::getPositionCoordinates() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    return this->extractCoordinate(CartesianPosition::Default());
}

MatrixXd CovarianceMatrix::getPositionVelocityCoordinates() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Covariance Matrix");
    }

    return this->extractCoordinates({CartesianPosition::Default(), CartesianVelocity::Default()});
}

void CovarianceMatrix::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Estimator :: Covariance Matrix") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Instant:" << (this->instant_.isDefined() ? this->instant_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Frame:"
        << ((this->frameSPtr_ != nullptr) && this->frameSPtr_->isDefined() ? this->frameSPtr_->getName() : "Undefined");

    if (!this->isDefined())
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Coordinates: Undefined";
    }
    else
    {
        const Array<Shared<const CoordinateSubset>> subsets = this->coordinatesBrokerSPtr_->getSubsets();

        for (const auto& subset : subsets)
        {
            ostk::core::utils::Print::Line(anOutputStream)
                << subset->getName() << this->extractCoordinate(subset).toString(4);
        }
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

CovarianceMatrix CovarianceMatrix::Undefined()
{
    return {Instant::Undefined(), MatrixXd(0, 0), Frame::Undefined(), {}};
}

CovarianceMatrix CovarianceMatrix::FromPositionSigmas(
    const Instant& anInstant, const Vector3d& aPositionSigmas, const Shared<const Frame>& aFrameSPtr
)
{
    MatrixXd coordinates = MatrixXd::Zero(3, 3);
    coordinates.diagonal() = aPositionSigmas.array().square();

    return {anInstant, coordinates, aFrameSPtr, {CartesianPosition::Default()}};
}

CovarianceMatrix CovarianceMatrix::FromPositionVelocitySigmas(
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

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk
