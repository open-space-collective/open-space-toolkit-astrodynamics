/// Apache License 2.0

#include <sstream>

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

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

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::uncertainty::Covariance;

class OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance : public ::testing::Test
{
   protected:
    const Instant defaultInstant_ = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

    const Shared<const Frame> defaultFrameSPtr_ = Frame::GCRF();

    const Covariance defaultCovariance_ = {
        defaultInstant_,
        MatrixXd::Identity(6, 6),
        defaultFrameSPtr_,
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, Constructor)
{
    {
        EXPECT_NO_THROW(Covariance covariance(
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()}
        ));
    }

    {
        EXPECT_NO_THROW(Covariance covariance(
            defaultInstant_, MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        ));
    }

    {
        EXPECT_NO_THROW(Covariance covariance(defaultInstant_, MatrixXd(0, 0), defaultFrameSPtr_, {}));
    }

    {
        EXPECT_THROW(
            try {
                Covariance covariance(
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
                Covariance covariance(
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
                Covariance covariance(
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
        MatrixXd coordinates(3, 3);
        coordinates << 1.0, 0.2, 0.0, 0.3, 2.0, 0.0, 0.0, 0.0, 3.0;

        EXPECT_THROW(
            try {
                Covariance covariance(defaultInstant_, coordinates, defaultFrameSPtr_, {CartesianPosition::Default()});
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Matrix not symmetric"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        MatrixXd coordinates(3, 3);
        coordinates << 1.0, 2.0, 0.0, 2.0, 1.0, 0.0, 0.0, 0.0, 1.0;

        EXPECT_THROW(
            try {
                Covariance covariance(defaultInstant_, coordinates, defaultFrameSPtr_, {CartesianPosition::Default()});
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Matrix not positive semi-definite"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        MatrixXd coordinates(3, 3);
        coordinates << 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0;

        EXPECT_NO_THROW(
            Covariance covariance(defaultInstant_, coordinates, defaultFrameSPtr_, {CartesianPosition::Default()})
        );
    }

    {
        EXPECT_NO_THROW(Covariance covariance(defaultCovariance_));

        const Covariance covariance(defaultCovariance_);

        EXPECT_EQ(defaultInstant_, covariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covariance.getFrame());
        EXPECT_EQ(defaultCovariance_.getCoordinateSubsets(), covariance.getCoordinateSubsets());
        EXPECT_TRUE(covariance.getCoordinates().isNear(defaultCovariance_.getCoordinates(), 1e-15));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, CopyAssignmentOperator)
{
    {
        Covariance aCovariance = {
            defaultInstant_,
            2.0 * MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };
        Covariance anotherCovariance = aCovariance;

        EXPECT_FALSE(aCovariance.getCoordinates().isNear(defaultCovariance_.getCoordinates(), 1e-15));

        aCovariance = defaultCovariance_;

        EXPECT_TRUE(aCovariance.getCoordinates().isNear(defaultCovariance_.getCoordinates(), 1e-15));

        anotherCovariance = aCovariance;
        aCovariance = {
            defaultInstant_,
            3.0 * MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_FALSE(aCovariance.getCoordinates().isNear(anotherCovariance.getCoordinates(), 1e-15));
        EXPECT_TRUE(anotherCovariance.getCoordinates().isNear(defaultCovariance_.getCoordinates(), 1e-15));
    }

    {
        Covariance covariance = defaultCovariance_;

        EXPECT_NO_THROW(covariance = covariance);

        EXPECT_EQ(defaultInstant_, covariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covariance.getFrame());
        EXPECT_EQ(defaultCovariance_.getCoordinateSubsets(), covariance.getCoordinateSubsets());
        EXPECT_TRUE(covariance.getCoordinates().isNear(defaultCovariance_.getCoordinates(), 1e-15));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, AdditionOperator)
{
    {
        const Covariance covariance = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_THROW(
            try { defaultCovariance_ + covariance; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Instant"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Covariance covariance = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            Frame::ITRF(),
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_THROW(
            try { defaultCovariance_ + covariance; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Frame"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Covariance covariance = {
            defaultInstant_, MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_THROW(
            try { defaultCovariance_ + covariance; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Coordinate Subsets"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Covariance covariance = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianVelocity::Default(), CartesianPosition::Default()},
        };

        EXPECT_THROW(
            try { defaultCovariance_ + covariance; } catch (const ostk::core::error::runtime::Wrong& e) {
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

        const Covariance aCovariance = {defaultInstant_, aCoordinates, defaultFrameSPtr_, subsets};
        const Covariance anotherCovariance = {defaultInstant_, anotherCoordinates, defaultFrameSPtr_, subsets};

        const Covariance covarianceCalculated = aCovariance + anotherCovariance;

        EXPECT_EQ(defaultInstant_, covarianceCalculated.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covarianceCalculated.getFrame());
        EXPECT_TRUE(covarianceCalculated.getCoordinates().isNear(coordinatesExpected, 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, SubtractionOperator)
{
    {
        const Covariance covariance = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_THROW(
            try { defaultCovariance_ - covariance; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Instant"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Covariance covariance = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            Frame::ITRF(),
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_THROW(
            try { defaultCovariance_ - covariance; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Frame"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Covariance covariance = {
            defaultInstant_, MatrixXd::Identity(3, 3), defaultFrameSPtr_, {CartesianPosition::Default()}
        };

        EXPECT_THROW(
            try { defaultCovariance_ - covariance; } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Coordinate Subsets"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Covariance covariance = {
            defaultInstant_,
            MatrixXd::Identity(6, 6),
            defaultFrameSPtr_,
            {CartesianVelocity::Default(), CartesianPosition::Default()},
        };

        EXPECT_THROW(
            try { defaultCovariance_ - covariance; } catch (const ostk::core::error::runtime::Wrong& e) {
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

        const Covariance aCovariance = {defaultInstant_, aCoordinates, defaultFrameSPtr_, subsets};
        const Covariance anotherCovariance = {defaultInstant_, anotherCoordinates, defaultFrameSPtr_, subsets};

        const Covariance covarianceCalculated = aCovariance - anotherCovariance;

        EXPECT_EQ(defaultInstant_, covarianceCalculated.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covarianceCalculated.getFrame());
        EXPECT_TRUE(covarianceCalculated.getCoordinates().isNear(coordinatesExpected, 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultCovariance_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, Getters)
{
    {
        const Array<Shared<const CoordinateSubset>> subsets = {
            CartesianPosition::Default(), CartesianVelocity::Default()
        };

        EXPECT_EQ(6, defaultCovariance_.getSize());
        EXPECT_EQ(defaultInstant_, defaultCovariance_.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, defaultCovariance_.getFrame());
        EXPECT_TRUE(defaultCovariance_.getCoordinates().isNear(MatrixXd::Identity(6, 6), 1e-15));
        EXPECT_EQ(subsets, defaultCovariance_.getCoordinateSubsets());
    }

    {
        const Array<Shared<const CoordinateSubset>> subsets = {CartesianPosition::Default()};
        const MatrixXd coordinates = MatrixXd::Identity(3, 3);

        const Covariance covariance = {defaultInstant_, coordinates, defaultFrameSPtr_, subsets};

        EXPECT_EQ(3, covariance.getSize());
        EXPECT_EQ(defaultInstant_, covariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covariance.getFrame());
        EXPECT_TRUE(covariance.getCoordinates().isNear(coordinates, 1e-15));
        EXPECT_EQ(subsets, covariance.getCoordinateSubsets());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, ExtractCoordinates)
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

        const Covariance covariance = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        EXPECT_TRUE(covariance.extractCoordinate(CartesianPosition::Default()).isNear(positionCoordinates, 1e-15));
        EXPECT_TRUE(covariance.extractCoordinate(CartesianVelocity::Default()).isNear(velocityCoordinates, 1e-15));

        EXPECT_TRUE(covariance.extractCoordinates({CartesianPosition::Default()}).isNear(positionCoordinates, 1e-15));
        EXPECT_TRUE(covariance.extractCoordinates({CartesianVelocity::Default()}).isNear(velocityCoordinates, 1e-15));
        EXPECT_TRUE(covariance.extractCoordinates({CartesianPosition::Default(), CartesianVelocity::Default()})
                        .isNear(coordinates, 1e-15));
        EXPECT_TRUE(covariance.extractCoordinates({CartesianVelocity::Default(), CartesianPosition::Default()})
                        .isNear(reorderedCoordinates, 1e-15));
    }

    {
        EXPECT_THROW(
            try {
                defaultCovariance_.extractCoordinate(CoordinateSubset::Mass());
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("not found"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );

        EXPECT_THROW(
            try {
                defaultCovariance_.extractCoordinates({CoordinateSubset::Mass()});
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("not found"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, Rotate)
{
    {
        const Covariance covariance = defaultCovariance_.rotate(defaultFrameSPtr_);

        EXPECT_EQ(defaultInstant_, covariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covariance.getFrame());
        EXPECT_EQ(defaultCovariance_.getCoordinateSubsets(), covariance.getCoordinateSubsets());
        EXPECT_TRUE(covariance.getCoordinates().isNear(defaultCovariance_.getCoordinates(), 1e-15));
    }

    // Numerical check (diagonal)
    {
        const Vector3d positionCoordinates(1.0, 2.0, 3.0);
        const Vector3d velocityCoordinates(4.0, 5.0, 6.0);
        const double massCoordinate = 7.0;

        MatrixXd coordinates = MatrixXd::Zero(7, 7);
        coordinates.diagonal() << positionCoordinates(0), positionCoordinates(1), positionCoordinates(2),
            velocityCoordinates(0), velocityCoordinates(1), velocityCoordinates(2), massCoordinate;

        const Covariance covariance = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()},
        };

        const Covariance rotatedCovariance = covariance.rotate(Frame::ITRF());

        const Transform transform = defaultFrameSPtr_->getTransformTo(Frame::ITRF(), defaultInstant_);
        const Matrix3d rotationMatrix = RotationMatrix::Quaternion(transform.getOrientation()).getMatrix();

        MatrixXd transformationMatrix = MatrixXd::Identity(7, 7);
        transformationMatrix.block(0, 0, 3, 3) = rotationMatrix;
        transformationMatrix.block(3, 3, 3, 3) = rotationMatrix;

        const MatrixXd coordinatesExpected = transformationMatrix * coordinates * transformationMatrix.transpose();

        EXPECT_TRUE(rotatedCovariance.getCoordinates().isNear(coordinatesExpected, 1e-12));
        EXPECT_TRUE(rotatedCovariance.getCoordinates().isApprox(rotatedCovariance.getCoordinates().transpose(), 1e-12));
        EXPECT_NEAR(massCoordinate, rotatedCovariance.getCoordinates()(6, 6), 1e-12);
        EXPECT_NEAR(
            coordinates.block(0, 0, 3, 3).trace(), rotatedCovariance.getCoordinates().block(0, 0, 3, 3).trace(), 1e-12
        );
        EXPECT_NEAR(
            coordinates.block(3, 3, 3, 3).trace(), rotatedCovariance.getCoordinates().block(3, 3, 3, 3).trace(), 1e-12
        );
    }

    // Numerical check (non-diagonal)
    {
        MatrixXd coordinates(7, 7);
        coordinates << 1.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.1, 2.0, 0.7, 0.8, 0.9, 1.0, 1.1, 0.2, 0.7, 3.0, 1.2, 1.3,
            1.4, 1.5, 0.3, 0.8, 1.2, 4.0, 1.6, 1.7, 1.8, 0.4, 0.9, 1.3, 1.6, 5.0, 1.9, 2.0, 0.5, 1.0, 1.4, 1.7, 1.9,
            6.0, 2.1, 0.6, 1.1, 1.5, 1.8, 2.0, 2.1, 7.0;

        const Covariance covariance = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()},
        };

        const Covariance rotatedCovariance = covariance.rotate(Frame::ITRF());

        const Transform transform = defaultFrameSPtr_->getTransformTo(Frame::ITRF(), defaultInstant_);

        const Matrix3d rotationMatrix = RotationMatrix::Quaternion(transform.getOrientation()).getMatrix();

        MatrixXd transformationMatrix = MatrixXd::Identity(7, 7);
        transformationMatrix.block(0, 0, 3, 3) = rotationMatrix;
        transformationMatrix.block(3, 3, 3, 3) = rotationMatrix;

        const MatrixXd coordinatesExpected = transformationMatrix * coordinates * transformationMatrix.transpose();

        EXPECT_TRUE(rotatedCovariance.getCoordinates().isNear(coordinatesExpected, 1e-12));
        EXPECT_TRUE(rotatedCovariance.getCoordinates().isApprox(rotatedCovariance.getCoordinates().transpose(), 1e-12));
        EXPECT_NEAR(7.0, rotatedCovariance.getCoordinates()(6, 6), 1e-12);
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

        const Covariance covariance = {defaultInstant_, coordinates, defaultFrameSPtr_, subsets};

        const Covariance rotatedCovariance = covariance.rotate(Frame::ITRF());

        EXPECT_EQ(defaultInstant_, rotatedCovariance.getInstant());
        EXPECT_EQ(Frame::ITRF(), rotatedCovariance.getFrame());
        EXPECT_EQ(subsets, rotatedCovariance.getCoordinateSubsets());
        EXPECT_FALSE(rotatedCovariance.getCoordinates().isNear(coordinates, 1e-6));
        EXPECT_NEAR(13.0, rotatedCovariance.getCoordinates()(12, 12), 1e-12);

        const Covariance roundTripCovariance = rotatedCovariance.rotate(defaultFrameSPtr_);

        EXPECT_EQ(defaultFrameSPtr_, roundTripCovariance.getFrame());
        EXPECT_TRUE(roundTripCovariance.getCoordinates().isNear(coordinates, 1e-9));
    }

    {
        const Shared<const Frame> frameSPtr = nullptr;

        EXPECT_THROW(
            try { defaultCovariance_.rotate(frameSPtr); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Frame"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                defaultCovariance_.rotate(Frame::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Frame"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, Diagonalize)
{
    {
        MatrixXd coordinates(3, 3);
        coordinates << 1.0, 0.2, 0.3, 0.2, 2.0, 0.4, 0.3, 0.4, 3.0;

        MatrixXd coordinatesExpected = MatrixXd::Zero(3, 3);
        coordinatesExpected.diagonal() << 1.0, 2.0, 3.0;

        const Array<Shared<const CoordinateSubset>> subsets = {CartesianPosition::Default()};
        const Covariance covariance = {defaultInstant_, coordinates, defaultFrameSPtr_, subsets};

        const Covariance diagonalizedCovariance = covariance.diagonalize();

        EXPECT_EQ(defaultInstant_, diagonalizedCovariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, diagonalizedCovariance.getFrame());
        EXPECT_EQ(subsets, diagonalizedCovariance.getCoordinateSubsets());
        EXPECT_TRUE(diagonalizedCovariance.getCoordinates().isNear(coordinatesExpected, 1e-15));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, Reduce)
{
    {
        MatrixXd coordinates(6, 6);
        coordinates << 1.0, 0.25, 0.0, 0.7, 0.0, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.7,
            0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0;

        MatrixXd positionCoordinates(3, 3);
        positionCoordinates << 1.0, 0.25, 0.0, 0.25, 2.0, 0.0, 0.0, 0.0, 3.0;

        const Covariance covariance = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        const Covariance reducedCovariance = covariance.reduce({CartesianPosition::Default()});

        EXPECT_EQ(defaultInstant_, reducedCovariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, reducedCovariance.getFrame());
        EXPECT_EQ(3, reducedCovariance.getSize());
        EXPECT_EQ(
            Array<Shared<const CoordinateSubset>>({CartesianPosition::Default()}),
            reducedCovariance.getCoordinateSubsets()
        );
        EXPECT_TRUE(reducedCovariance.getCoordinates().isNear(positionCoordinates, 1e-15));
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

        const Covariance covariance = {
            defaultInstant_,
            coordinates,
            defaultFrameSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        const Covariance reducedCovariance = covariance.reduce(reorderedSubsets);

        EXPECT_EQ(reorderedSubsets, reducedCovariance.getCoordinateSubsets());
        EXPECT_TRUE(reducedCovariance.getCoordinates().isNear(reorderedCoordinates, 1e-15));
    }

    {
        EXPECT_THROW(
            try {
                defaultCovariance_.reduce({CoordinateSubset::Mass()});
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_NE(e.getMessage().find("not found"), std::string::npos);
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, Scale)
{
    {
        const Covariance scaledCovariance = defaultCovariance_.scale(Real(2.0));

        EXPECT_EQ(defaultInstant_, scaledCovariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, scaledCovariance.getFrame());
        EXPECT_EQ(defaultCovariance_.getCoordinateSubsets(), scaledCovariance.getCoordinateSubsets());
        EXPECT_TRUE(scaledCovariance.getCoordinates().isNear(2.0 * MatrixXd::Identity(6, 6), 1e-15));
    }

    {
        EXPECT_THROW(
            try {
                defaultCovariance_.scale(Real::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_NE(e.getMessage().find("Scalar"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try { defaultCovariance_.scale(Real(0.0)); } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Scalar"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );

        EXPECT_THROW(
            try { defaultCovariance_.scale(Real(-1.0)); } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_NE(e.getMessage().find("Scalar"), std::string::npos);
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, Print)
{
    {
        std::stringstream outputStream;

        EXPECT_NO_THROW(defaultCovariance_.print(outputStream, true));

        const std::string output = outputStream.str();

        EXPECT_NE(output.find("Uncertainty :: Covariance"), std::string::npos);
        EXPECT_NE(output.find("GCRF"), std::string::npos);
        EXPECT_NE(output.find("CARTESIAN_POSITION"), std::string::npos);
        EXPECT_NE(output.find("CARTESIAN_VELOCITY"), std::string::npos);
    }

    {
        std::stringstream outputStream;

        EXPECT_NO_THROW(defaultCovariance_.print(outputStream, false));

        const std::string output = outputStream.str();

        EXPECT_EQ(output.find("Uncertainty :: Covariance"), std::string::npos);
        EXPECT_NE(output.find("GCRF"), std::string::npos);
        EXPECT_NE(output.find("CARTESIAN_POSITION"), std::string::npos);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Uncertainty_Covariance, FromSigmas)
{
    {
        const Vector3d positionSigmas(1.0, 2.0, 3.0);

        MatrixXd coordinatesExpected = MatrixXd::Zero(3, 3);
        coordinatesExpected.diagonal() << 1.0, 4.0, 9.0;

        const Covariance covariance =
            Covariance::FromPositionSigmas(defaultInstant_, positionSigmas, defaultFrameSPtr_);

        EXPECT_EQ(3, covariance.getSize());
        EXPECT_EQ(defaultInstant_, covariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covariance.getFrame());
        EXPECT_EQ(
            Array<Shared<const CoordinateSubset>>({CartesianPosition::Default()}), covariance.getCoordinateSubsets()
        );
        EXPECT_TRUE(covariance.getCoordinates().isNear(coordinatesExpected, 1e-15));
    }

    {
        const Vector3d positionSigmas(1.0, 2.0, 3.0);
        const Vector3d velocitySigmas(4.0, 5.0, 6.0);

        MatrixXd coordinatesExpected = MatrixXd::Zero(6, 6);
        coordinatesExpected.diagonal() << 1.0, 4.0, 9.0, 16.0, 25.0, 36.0;

        const Array<Shared<const CoordinateSubset>> subsets = {
            CartesianPosition::Default(), CartesianVelocity::Default()
        };

        const Covariance covariance =
            Covariance::FromPositionVelocitySigmas(defaultInstant_, positionSigmas, velocitySigmas, defaultFrameSPtr_);

        EXPECT_EQ(6, covariance.getSize());
        EXPECT_EQ(defaultInstant_, covariance.getInstant());
        EXPECT_EQ(defaultFrameSPtr_, covariance.getFrame());
        EXPECT_EQ(subsets, covariance.getCoordinateSubsets());
        EXPECT_TRUE(covariance.getCoordinates().isNear(coordinatesExpected, 1e-15));
    }
}
