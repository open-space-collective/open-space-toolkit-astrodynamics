/// Apache License 2.0

#include <sstream>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Transform;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix : public ::testing::Test
{
   protected:
    const Instant defaultInstant_ = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

    const Shared<const Frame> defaultFrameSPtr_ = Frame::GCRF();

    const CovarianceMatrix defaultCovarianceMatrix_ = {
        defaultInstant_,
        MatrixXd::Identity(6, 6),
        defaultFrameSPtr_,
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Constructor)
{
    {
        EXPECT_NO_THROW(CovarianceMatrix covarianceMatrix(
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()}
        ));
    }

    {
        EXPECT_NO_THROW(CovarianceMatrix covarianceMatrix(
            defaultInstant_, MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        ));
    }

    {
        EXPECT_NO_THROW(CovarianceMatrix covarianceMatrix(defaultInstant_, MatrixXd(0, 0), defaultFrameSPtr_, {}));
    }

    {
        EXPECT_THROW(
            try {
                CovarianceMatrix covarianceMatrix(
                    defaultInstant_,
                    MatrixXd(3, 6),
                    defaultFrameSPtr_,
                    {CartesianPosition::Default(), CartesianVelocity::Default()}
                );
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Matrix not square"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        EXPECT_THROW(
            try {
                CovarianceMatrix covarianceMatrix(
                    defaultInstant_,
                    MatrixXd::Identity(3, 3),
                    defaultFrameSPtr_,
                    {CartesianPosition::Default(), CartesianVelocity::Default()}
                );
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Subset-matrix size mismatch"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        EXPECT_THROW(
            try {
                CovarianceMatrix covarianceMatrix(
                    defaultInstant_, MatrixXd::Identity(6, 6), defaultFrameSPtr_, {CartesianPosition::Default()}
                );
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Subset-matrix size mismatch"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        EXPECT_NO_THROW(CovarianceMatrix covarianceMatrix(defaultCovarianceMatrix_));

        const CovarianceMatrix covarianceMatrix(defaultCovarianceMatrix_);

        EXPECT_EQ(defaultCovarianceMatrix_, covarianceMatrix);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, CopyAssignmentOperator)
{
    {
        CovarianceMatrix aCovarianceMatrix = CovarianceMatrix::Undefined();
        CovarianceMatrix anotherCovarianceMatrix = CovarianceMatrix::Undefined();

        EXPECT_NE(defaultCovarianceMatrix_, aCovarianceMatrix);

        aCovarianceMatrix = defaultCovarianceMatrix_;

        EXPECT_EQ(defaultCovarianceMatrix_, aCovarianceMatrix);

        anotherCovarianceMatrix = aCovarianceMatrix;
        aCovarianceMatrix = CovarianceMatrix::Undefined();

        EXPECT_NE(aCovarianceMatrix, anotherCovarianceMatrix);
        EXPECT_EQ(defaultCovarianceMatrix_, anotherCovarianceMatrix);
    }

    {
        CovarianceMatrix covarianceMatrix = CovarianceMatrix::Undefined();
        covarianceMatrix = defaultCovarianceMatrix_;

        EXPECT_NO_THROW(covarianceMatrix = covarianceMatrix);

        EXPECT_EQ(defaultCovarianceMatrix_, covarianceMatrix);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultCovarianceMatrix_ == defaultCovarianceMatrix_);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_TRUE(defaultCovarianceMatrix_ == covarianceMatrix);
    }

    {
        MatrixXd coordinates(6, 6);
        coordinates << 1.0, 0.25, 0.0, 0.7, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.7,
            0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0;

        // Velocity block, then position block, including the position-velocity cross terms.
        MatrixXd reorderedCoordinates(6, 6);
        reorderedCoordinates << 4.0, 0.0, 0.0, 0.7, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0, 0.0, 0.0,
            0.0, 0.7, 0.0, 0.0, 1.0, 0.25, 0.0, 0.0, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0;

        const CovarianceMatrix aCovarianceMatrix = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };
        const CovarianceMatrix anotherCovarianceMatrix = {
            defaultInstant_,
            reorderedCoordinates,
            defaultFrameSPtr_,
            {CartesianVelocity::Default(), CartesianPosition::Default()},
        };

        EXPECT_TRUE(aCovarianceMatrix == anotherCovarianceMatrix);
        EXPECT_TRUE(anotherCovarianceMatrix == aCovarianceMatrix);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_, MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_FALSE(defaultCovarianceMatrix_ == covarianceMatrix);
    }

    {
        const MatrixXd coordinates = MatrixXd::Identity(4, 4);

        const CovarianceMatrix aCovarianceMatrix = {
            defaultInstant_, coordinates, defaultFrameSPtr_, {CartesianPosition::Default(), CoordinateSubset::Mass()}
        };
        const CovarianceMatrix anotherCovarianceMatrix = {
            defaultInstant_, coordinates, defaultFrameSPtr_, {CartesianVelocity::Default(), CoordinateSubset::Mass()}
        };

        EXPECT_FALSE(aCovarianceMatrix == anotherCovarianceMatrix);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_FALSE(defaultCovarianceMatrix_ == covarianceMatrix);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            2.0 * MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_FALSE(defaultCovarianceMatrix_ == covarianceMatrix);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            Frame::ITRF(),
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_FALSE(defaultCovarianceMatrix_ == covarianceMatrix);
    }

    {
        EXPECT_FALSE(CovarianceMatrix::Undefined() == defaultCovarianceMatrix_);
        EXPECT_FALSE(defaultCovarianceMatrix_ == CovarianceMatrix::Undefined());
        EXPECT_FALSE(CovarianceMatrix::Undefined() == CovarianceMatrix::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultCovarianceMatrix_ != defaultCovarianceMatrix_);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_FALSE(defaultCovarianceMatrix_ != covarianceMatrix);
    }

    {
        MatrixXd coordinates(6, 6);
        coordinates << 1.0, 0.25, 0.0, 0.7, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.7,
            0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0;

        MatrixXd reorderedCoordinates(6, 6);
        reorderedCoordinates << 4.0, 0.0, 0.0, 0.7, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0, 0.0, 0.0,
            0.0, 0.7, 0.0, 0.0, 1.0, 0.25, 0.0, 0.0, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0;

        const CovarianceMatrix aCovarianceMatrix = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };
        const CovarianceMatrix anotherCovarianceMatrix = {
            defaultInstant_,
            reorderedCoordinates,
            defaultFrameSPtr_,
            {CartesianVelocity::Default(), CartesianPosition::Default()},
        };

        EXPECT_FALSE(aCovarianceMatrix != anotherCovarianceMatrix);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_, MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_TRUE(defaultCovarianceMatrix_ != covarianceMatrix);
    }

    {
        const MatrixXd coordinates = MatrixXd::Identity(4, 4);

        const CovarianceMatrix aCovarianceMatrix = {
            defaultInstant_, coordinates, defaultFrameSPtr_, {CartesianPosition::Default(), CoordinateSubset::Mass()}
        };
        const CovarianceMatrix anotherCovarianceMatrix = {
            defaultInstant_, coordinates, defaultFrameSPtr_, {CartesianVelocity::Default(), CoordinateSubset::Mass()}
        };

        EXPECT_TRUE(aCovarianceMatrix != anotherCovarianceMatrix);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_TRUE(defaultCovarianceMatrix_ != covarianceMatrix);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            2.0 * MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_TRUE(defaultCovarianceMatrix_ != covarianceMatrix);
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            Frame::ITRF(),
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_TRUE(defaultCovarianceMatrix_ != covarianceMatrix);
    }

    {
        EXPECT_TRUE(CovarianceMatrix::Undefined() != defaultCovarianceMatrix_);
        EXPECT_TRUE(defaultCovarianceMatrix_ != CovarianceMatrix::Undefined());
        EXPECT_TRUE(CovarianceMatrix::Undefined() != CovarianceMatrix::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, AdditionOperator)
{
    {
        EXPECT_THROW(
            try {
                defaultCovarianceMatrix_ + CovarianceMatrix::Undefined();
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined() + defaultCovarianceMatrix_;
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_THROW(
            try { defaultCovarianceMatrix_ + covarianceMatrix; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Instant"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            Frame::ITRF(),
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_THROW(
            try { defaultCovarianceMatrix_ + covarianceMatrix; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Frame"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_, MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_THROW(
            try { defaultCovarianceMatrix_ + covarianceMatrix; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Coordinate Subsets"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianVelocity::Default(), CartesianPosition::Default()},
        };

        EXPECT_THROW(
            try { defaultCovarianceMatrix_ + covarianceMatrix; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Coordinate Subsets"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Array<Shared<const CoordinateSubset>> subsets = {CartesianPosition::Default()};

        MatrixXd aCoordinates(3, 3);
        aCoordinates << 1.0, 0.2, 0.3, 0.2, 2.0, 0.4, 0.3, 0.4, 3.0;

        MatrixXd anotherCoordinates(3, 3);
        anotherCoordinates << 0.5, 0.1, 0.0, 0.1, 0.5, 0.2, 0.0, 0.2, 0.5;

        MatrixXd coordinatesExpected(3, 3);
        coordinatesExpected << 1.5, 0.3, 0.3, 0.3, 2.5, 0.6, 0.3, 0.6, 3.5;

        const CovarianceMatrix aCovarianceMatrix = {defaultInstant_, aCoordinates, defaultFrameSPtr_, subsets};
        const CovarianceMatrix anotherCovarianceMatrix = {
            defaultInstant_, anotherCoordinates, defaultFrameSPtr_, subsets
        };

        const CovarianceMatrix covarianceMatrixCalculated = aCovarianceMatrix + anotherCovarianceMatrix;

        EXPECT_EQ(defaultInstant_, covarianceMatrixCalculated.accessInstant());
        EXPECT_EQ(defaultFrameSPtr_, covarianceMatrixCalculated.accessFrame());
        EXPECT_TRUE(covarianceMatrixCalculated.accessCoordinates().isNear(coordinatesExpected, 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, SubtractionOperator)
{
    {
        EXPECT_THROW(
            try {
                defaultCovarianceMatrix_ - CovarianceMatrix::Undefined();
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined() - defaultCovarianceMatrix_;
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_THROW(
            try { defaultCovarianceMatrix_ - covarianceMatrix; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Instant"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            Frame::ITRF(),
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_THROW(
            try { defaultCovarianceMatrix_ - covarianceMatrix; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Frame"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_, MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_THROW(
            try { defaultCovarianceMatrix_ - covarianceMatrix; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Coordinate Subsets"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianVelocity::Default(), CartesianPosition::Default()},
        };

        EXPECT_THROW(
            try { defaultCovarianceMatrix_ - covarianceMatrix; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Coordinate Subsets"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Array<Shared<const CoordinateSubset>> subsets = {CartesianPosition::Default()};

        MatrixXd aCoordinates(3, 3);
        aCoordinates << 1.0, 0.2, 0.3, 0.2, 2.0, 0.4, 0.3, 0.4, 3.0;

        MatrixXd anotherCoordinates(3, 3);
        anotherCoordinates << 0.5, 0.1, 0.0, 0.1, 0.5, 0.2, 0.0, 0.2, 0.5;

        MatrixXd coordinatesExpected(3, 3);
        coordinatesExpected << 0.5, 0.1, 0.3, 0.1, 1.5, 0.2, 0.3, 0.2, 2.5;

        const CovarianceMatrix aCovarianceMatrix = {defaultInstant_, aCoordinates, defaultFrameSPtr_, subsets};
        const CovarianceMatrix anotherCovarianceMatrix = {
            defaultInstant_, anotherCoordinates, defaultFrameSPtr_, subsets
        };

        const CovarianceMatrix covarianceMatrixCalculated = aCovarianceMatrix - anotherCovarianceMatrix;

        EXPECT_EQ(defaultInstant_, covarianceMatrixCalculated.accessInstant());
        EXPECT_EQ(defaultFrameSPtr_, covarianceMatrixCalculated.accessFrame());
        EXPECT_TRUE(covarianceMatrixCalculated.accessCoordinates().isNear(coordinatesExpected, 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultCovarianceMatrix_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << CovarianceMatrix::Undefined() << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, IsDefined)
{
    {
        EXPECT_TRUE(defaultCovarianceMatrix_.isDefined());
    }

    {
        const CovarianceMatrix covarianceMatrix = {defaultInstant_, MatrixXd(0, 0), defaultFrameSPtr_, {}};

        EXPECT_TRUE(covarianceMatrix.isDefined());
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            Instant::Undefined(), MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_FALSE(covarianceMatrix.isDefined());
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_, MatrixXd::Undefined(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_FALSE(covarianceMatrix.isDefined());
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_, MatrixXd::Inf(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_FALSE(covarianceMatrix.isDefined());
    }

    {
        const Shared<const Frame> frameSPtr = nullptr;

        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_, MatrixXd::Identity(3, 3), frameSPtr, {CartesianPosition::Default()}
        };

        EXPECT_FALSE(covarianceMatrix.isDefined());
    }

    {
        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_, MatrixXd::Identity(3, 3), Frame::Undefined(), {CartesianPosition::Default()}
        };

        EXPECT_FALSE(covarianceMatrix.isDefined());
    }

    {
        EXPECT_FALSE(CovarianceMatrix::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Accessors)
{
    {
        EXPECT_EQ(defaultInstant_, defaultCovarianceMatrix_.accessInstant());
        EXPECT_EQ(defaultFrameSPtr_, defaultCovarianceMatrix_.accessFrame());
        EXPECT_TRUE(defaultCovarianceMatrix_.accessCoordinates().isNear(MatrixXd::Identity(6, 6), 1e-15));
    }

    {
        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().accessInstant();
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().accessFrame();
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().accessCoordinates();
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Getters)
{
    {
        const Array<Shared<const CoordinateSubset>> subsets = {
            CartesianPosition::Default(), CartesianVelocity::Default()
        };

        EXPECT_EQ(6, defaultCovarianceMatrix_.getSize());
        EXPECT_EQ(defaultInstant_, defaultCovarianceMatrix_.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, defaultCovarianceMatrix_.getFrame());
        EXPECT_TRUE(defaultCovarianceMatrix_.getCoordinates().isNear(MatrixXd::Identity(6, 6), 1e-15));
        EXPECT_EQ(subsets, defaultCovarianceMatrix_.getCoordinateSubsets());
    }

    {
        const Array<Shared<const CoordinateSubset>> subsets = {CartesianPosition::Default()};
        const MatrixXd coordinates = MatrixXd::Identity(3, 3);

        const CovarianceMatrix covarianceMatrix = {defaultInstant_, coordinates, defaultFrameSPtr_, subsets};

        EXPECT_EQ(3, covarianceMatrix.getSize());
        EXPECT_EQ(defaultInstant_, covarianceMatrix.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covarianceMatrix.getFrame());
        EXPECT_TRUE(covarianceMatrix.getCoordinates().isNear(coordinates, 1e-15));
        EXPECT_EQ(subsets, covarianceMatrix.getCoordinateSubsets());
    }

    {
        EXPECT_THROW(
            try { CovarianceMatrix::Undefined().getSize(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try { CovarianceMatrix::Undefined().getInstant(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try { CovarianceMatrix::Undefined().getFrame(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().getCoordinates();
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().getCoordinateSubsets();
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, ExtractCoordinates)
{
    {
        MatrixXd coordinates(6, 6);
        coordinates << 1.0, 0.25, 0.0, 0.7, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.7,
            0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0;

        MatrixXd positionCoordinates(3, 3);
        positionCoordinates << 1.0, 0.25, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 3.0;

        MatrixXd velocityCoordinates(3, 3);
        velocityCoordinates << 4.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 6.0;

        MatrixXd reorderedCoordinates(6, 6);
        reorderedCoordinates << 4.0, 0.0, 0.0, 0.7, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0, 0.0, 0.0,
            0.0, 0.7, 0.0, 0.0, 1.0, 0.25, 0.0, 0.0, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0;

        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_TRUE(covarianceMatrix.extractCoordinate(CartesianPosition::Default()).isNear(positionCoordinates, 1e-15)
        );
        EXPECT_TRUE(covarianceMatrix.extractCoordinate(CartesianVelocity::Default()).isNear(velocityCoordinates, 1e-15)
        );

        EXPECT_TRUE(
            covarianceMatrix.extractCoordinates({CartesianPosition::Default()}).isNear(positionCoordinates, 1e-15)
        );
        EXPECT_TRUE(
            covarianceMatrix.extractCoordinates({CartesianVelocity::Default()}).isNear(velocityCoordinates, 1e-15)
        );
        EXPECT_TRUE(covarianceMatrix.extractCoordinates({CartesianPosition::Default(), CartesianVelocity::Default()})
                        .isNear(coordinates, 1e-15));
        EXPECT_TRUE(covarianceMatrix.extractCoordinates({CartesianVelocity::Default(), CartesianPosition::Default()})
                        .isNear(reorderedCoordinates, 1e-15));
    }

    {
        EXPECT_THROW(
            try {
                defaultCovarianceMatrix_.extractCoordinate(CoordinateSubset::Mass());
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("not found"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );

        EXPECT_THROW(
            try {
                defaultCovarianceMatrix_.extractCoordinates({CoordinateSubset::Mass()});
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("not found"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().extractCoordinate(CartesianPosition::Default());
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("not found"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );

        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().extractCoordinates({CartesianPosition::Default()});
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("not found"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Rotate)
{
    {
        const CovarianceMatrix covarianceMatrix = defaultCovarianceMatrix_.rotate(defaultFrameSPtr_);

        EXPECT_EQ(defaultCovarianceMatrix_, covarianceMatrix);
    }

    // Numerical check (diagonal)
    {
        const Vector3d positionCoordinates(1.0, 2.0, 3.0);
        const Vector3d velocityCoordinates(4.0, 5.0, 6.0);
        const double massCoordinate = 7.0;

        MatrixXd coordinates = MatrixXd::Zero(7, 7);
        coordinates.diagonal() << positionCoordinates(0), positionCoordinates(1), positionCoordinates(2),
            velocityCoordinates(0), velocityCoordinates(1), velocityCoordinates(2), massCoordinate;

        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()},
        };

        const CovarianceMatrix rotatedCovarianceMatrix = covarianceMatrix.rotate(Frame::ITRF());

        const Transform transform = defaultFrameSPtr_->getTransformTo(Frame::ITRF(), defaultInstant_);
        const Matrix3d rotationMatrix = RotationMatrix::Quaternion(transform.getOrientation()).getMatrix();

        MatrixXd transformationMatrix = MatrixXd::Identity(7, 7);
        transformationMatrix.block(0, 0, 3, 3) = rotationMatrix;
        transformationMatrix.block(3, 3, 3, 3) = rotationMatrix;

        const MatrixXd coordinatesExpected = transformationMatrix * coordinates * transformationMatrix.transpose();

        EXPECT_TRUE(rotatedCovarianceMatrix.getCoordinates().isNear(coordinatesExpected, 1e-12));
        EXPECT_TRUE(rotatedCovarianceMatrix.getCoordinates().isApprox(
            rotatedCovarianceMatrix.getCoordinates().transpose(), 1e-12
        ));
        EXPECT_NEAR(massCoordinate, rotatedCovarianceMatrix.getCoordinates()(6, 6), 1e-12);
        EXPECT_NEAR(
            coordinates.block(0, 0, 3, 3).trace(),
            rotatedCovarianceMatrix.getCoordinates().block(0, 0, 3, 3).trace(),
            1e-12
        );
        EXPECT_NEAR(
            coordinates.block(3, 3, 3, 3).trace(),
            rotatedCovarianceMatrix.getCoordinates().block(3, 3, 3, 3).trace(),
            1e-12
        );
    }

    // Numerical check (non-diagonal)
    {
        MatrixXd coordinates(7, 7);
        coordinates << 1.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.1, 2.0, 0.7, 0.8, 0.9, 1.0, 1.1, 0.2, 0.7, 3.0, 1.2, 1.3,
            1.4, 1.5, 0.3, 0.8, 1.2, 4.0, 1.6, 1.7, 1.8, 0.4, 0.9, 1.3, 1.6, 5.0, 1.9, 2.0, 0.5, 1.0, 1.4, 1.7, 1.9,
            6.0, 2.1, 0.6, 1.1, 1.5, 1.8, 2.0, 2.1, 7.0;

        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()},
        };

        const CovarianceMatrix rotatedCovarianceMatrix = covarianceMatrix.rotate(Frame::ITRF());

        const Transform transform = defaultFrameSPtr_->getTransformTo(Frame::ITRF(), defaultInstant_);

        const Matrix3d rotationMatrix = RotationMatrix::Quaternion(transform.getOrientation()).getMatrix();

        MatrixXd transformationMatrix = MatrixXd::Identity(7, 7);
        transformationMatrix.block(0, 0, 3, 3) = rotationMatrix;
        transformationMatrix.block(3, 3, 3, 3) = rotationMatrix;

        const MatrixXd coordinatesExpected = transformationMatrix * coordinates * transformationMatrix.transpose();

        EXPECT_TRUE(rotatedCovarianceMatrix.getCoordinates().isNear(coordinatesExpected, 1e-12));
        EXPECT_TRUE(rotatedCovarianceMatrix.getCoordinates().isApprox(
            rotatedCovarianceMatrix.getCoordinates().transpose(), 1e-12
        ));
        EXPECT_NEAR(7.0, rotatedCovarianceMatrix.getCoordinates()(6, 6), 1e-12);
    }

    // Round trip
    {
        MatrixXd coordinates = MatrixXd::Zero(13, 13);
        coordinates.diagonal() << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0;

        const Array<Shared<const CoordinateSubset>> subsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            CartesianAcceleration::Default(),
            AngularVelocity::Default(),
            CoordinateSubset::Mass(),
        };

        const CovarianceMatrix covarianceMatrix = {defaultInstant_, coordinates, defaultFrameSPtr_, subsets};

        const CovarianceMatrix rotatedCovarianceMatrix = covarianceMatrix.rotate(Frame::ITRF());

        EXPECT_EQ(defaultInstant_, rotatedCovarianceMatrix.getInstant());
        EXPECT_EQ(Frame::ITRF(), rotatedCovarianceMatrix.getFrame());
        EXPECT_EQ(subsets, rotatedCovarianceMatrix.getCoordinateSubsets());
        EXPECT_FALSE(rotatedCovarianceMatrix.getCoordinates().isNear(coordinates, 1e-6));
        EXPECT_NEAR(13.0, rotatedCovarianceMatrix.getCoordinates()(12, 12), 1e-12);

        const CovarianceMatrix roundTripCovarianceMatrix = rotatedCovarianceMatrix.rotate(defaultFrameSPtr_);

        EXPECT_EQ(defaultFrameSPtr_, roundTripCovarianceMatrix.getFrame());
        EXPECT_TRUE(roundTripCovarianceMatrix.getCoordinates().isNear(coordinates, 1e-9));
    }

    {
        const Shared<const Frame> frameSPtr = nullptr;

        EXPECT_THROW(
            try { defaultCovarianceMatrix_.rotate(frameSPtr); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Frame"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                defaultCovarianceMatrix_.rotate(Frame::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Frame"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().rotate(defaultFrameSPtr_);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Diagonalize)
{
    {
        MatrixXd coordinates(3, 3);
        coordinates << 1.0, 0.2, 0.3, 0.2, 2.0, 0.4, 0.3, 0.4, 3.0;

        MatrixXd coordinatesExpected = MatrixXd::Zero(3, 3);
        coordinatesExpected.diagonal() << 1.0, 2.0, 3.0;

        const Array<Shared<const CoordinateSubset>> subsets = {CartesianPosition::Default()};
        const CovarianceMatrix covarianceMatrix = {defaultInstant_, coordinates, defaultFrameSPtr_, subsets};

        const CovarianceMatrix diagonalizedCovarianceMatrix = covarianceMatrix.diagonalize();

        EXPECT_EQ(defaultInstant_, diagonalizedCovarianceMatrix.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, diagonalizedCovarianceMatrix.getFrame());
        EXPECT_EQ(subsets, diagonalizedCovarianceMatrix.getCoordinateSubsets());
        EXPECT_TRUE(diagonalizedCovarianceMatrix.getCoordinates().isNear(coordinatesExpected, 1e-15));
    }

    {
        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().diagonalize();
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Reduce)
{
    {
        MatrixXd coordinates(6, 6);
        coordinates << 1.0, 0.25, 0.0, 0.7, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.7,
            0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0;

        MatrixXd positionCoordinates(3, 3);
        positionCoordinates << 1.0, 0.25, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 3.0;

        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        const CovarianceMatrix reducedCovarianceMatrix = covarianceMatrix.reduce({CartesianPosition::Default()});

        EXPECT_EQ(defaultInstant_, reducedCovarianceMatrix.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, reducedCovarianceMatrix.getFrame());
        EXPECT_EQ(3, reducedCovarianceMatrix.getSize());
        EXPECT_EQ(
            Array<Shared<const CoordinateSubset>>({CartesianPosition::Default()}),
            reducedCovarianceMatrix.getCoordinateSubsets()
        );
        EXPECT_TRUE(reducedCovarianceMatrix.getCoordinates().isNear(positionCoordinates, 1e-15));
    }

    {
        MatrixXd coordinates(6, 6);
        coordinates << 1.0, 0.25, 0.0, 0.7, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.7,
            0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0;

        MatrixXd reorderedCoordinates(6, 6);
        reorderedCoordinates << 4.0, 0.0, 0.0, 0.7, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0, 0.0, 0.0,
            0.0, 0.7, 0.0, 0.0, 1.0, 0.25, 0.0, 0.0, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0;

        const Array<Shared<const CoordinateSubset>> reorderedSubsets = {
            CartesianVelocity::Default(), CartesianPosition::Default()
        };

        const CovarianceMatrix covarianceMatrix = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        const CovarianceMatrix reducedCovarianceMatrix = covarianceMatrix.reduce(reorderedSubsets);

        EXPECT_EQ(reorderedSubsets, reducedCovarianceMatrix.getCoordinateSubsets());
        EXPECT_TRUE(reducedCovarianceMatrix.getCoordinates().isNear(reorderedCoordinates, 1e-15));
    }

    {
        EXPECT_THROW(
            try {
                defaultCovarianceMatrix_.reduce({CoordinateSubset::Mass()});
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("not found"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().reduce({CartesianPosition::Default()});
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Scale)
{
    {
        const CovarianceMatrix scaledCovarianceMatrix = defaultCovarianceMatrix_.scale(Real(2.0));

        EXPECT_EQ(defaultInstant_, scaledCovarianceMatrix.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, scaledCovarianceMatrix.getFrame());
        EXPECT_EQ(defaultCovarianceMatrix_.getCoordinateSubsets(), scaledCovarianceMatrix.getCoordinateSubsets());
        EXPECT_TRUE(scaledCovarianceMatrix.getCoordinates().isNear(2.0 * MatrixXd::Identity(6, 6), 1e-15));
    }

    {
        EXPECT_THROW(
            try {
                CovarianceMatrix::Undefined().scale(Real(2.0));
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Covariance Matrix"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                defaultCovarianceMatrix_.scale(Real::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Scalar"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try { defaultCovarianceMatrix_.scale(Real(0.0)); } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Scalar"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );

        EXPECT_THROW(
            try { defaultCovarianceMatrix_.scale(Real(-1.0)); } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Scalar"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Print)
{
    {
        std::stringstream outputStream;

        EXPECT_NO_THROW(defaultCovarianceMatrix_.print(outputStream, true));

        const std::string output = outputStream.str();

        EXPECT_NE(output.find("Estimator :: Covariance Matrix"), std::string::npos);
        EXPECT_NE(output.find("GCRF"), std::string::npos);
        EXPECT_NE(output.find("CARTESIAN_POSITION"), std::string::npos);
        EXPECT_NE(output.find("CARTESIAN_VELOCITY"), std::string::npos);
    }

    {
        std::stringstream outputStream;

        EXPECT_NO_THROW(defaultCovarianceMatrix_.print(outputStream, false));

        const std::string output = outputStream.str();

        EXPECT_EQ(output.find("Estimator :: Covariance Matrix"), std::string::npos);
        EXPECT_NE(output.find("GCRF"), std::string::npos);
        EXPECT_NE(output.find("CARTESIAN_POSITION"), std::string::npos);
    }

    {
        std::stringstream outputStream;

        EXPECT_NO_THROW(CovarianceMatrix::Undefined().print(outputStream, true));

        const std::string output = outputStream.str();

        EXPECT_NE(output.find("Estimator :: Covariance Matrix"), std::string::npos);
        EXPECT_NE(output.find("Coordinates: Undefined"), std::string::npos);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, Undefined)
{
    {
        EXPECT_NO_THROW(CovarianceMatrix::Undefined());

        EXPECT_FALSE(CovarianceMatrix::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_CovarianceMatrix, FromSigmas)
{
    {
        const Vector3d positionSigmas(1.0, 2.0, 3.0);

        MatrixXd coordinatesExpected = MatrixXd::Zero(3, 3);
        coordinatesExpected.diagonal() << 1.0, 4.0, 9.0;

        const CovarianceMatrix covarianceMatrix =
            CovarianceMatrix::FromPositionSigmas(defaultInstant_, positionSigmas, defaultFrameSPtr_);

        EXPECT_TRUE(covarianceMatrix.isDefined());
        EXPECT_EQ(3, covarianceMatrix.getSize());
        EXPECT_EQ(defaultInstant_, covarianceMatrix.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covarianceMatrix.getFrame());
        EXPECT_EQ(
            Array<Shared<const CoordinateSubset>>({CartesianPosition::Default()}),
            covarianceMatrix.getCoordinateSubsets()
        );
        EXPECT_TRUE(covarianceMatrix.getCoordinates().isNear(coordinatesExpected, 1e-15));
    }

    {
        const Vector3d positionSigmas(1.0, 2.0, 3.0);
        const Vector3d velocitySigmas(4.0, 5.0, 6.0);

        MatrixXd coordinatesExpected = MatrixXd::Zero(6, 6);
        coordinatesExpected.diagonal() << 1.0, 4.0, 9.0, 16.0, 25.0, 36.0;

        const Array<Shared<const CoordinateSubset>> subsets = {
            CartesianPosition::Default(), CartesianVelocity::Default()
        };

        const CovarianceMatrix covarianceMatrix = CovarianceMatrix::FromPositionVelocitySigmas(
            defaultInstant_, positionSigmas, velocitySigmas, defaultFrameSPtr_
        );

        EXPECT_TRUE(covarianceMatrix.isDefined());
        EXPECT_EQ(6, covarianceMatrix.getSize());
        EXPECT_EQ(defaultInstant_, covarianceMatrix.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covarianceMatrix.getFrame());
        EXPECT_EQ(subsets, covarianceMatrix.getCoordinateSubsets());
        EXPECT_TRUE(covarianceMatrix.getCoordinates().isNear(coordinatesExpected, 1e-15));
    }
}
