// /// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;
using ostk::core::type::Real;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Flight_Maneuver : public ::testing::Test
{
   protected:
    const Array<Instant> defaultInstants_ = {
        Instant::J2000(),
        Instant::J2000() + Duration::Seconds(1.0),
        Instant::J2000() + Duration::Seconds(5.0),
        Instant::J2000() + Duration::Seconds(7.0),
    };

    const Array<Vector3d> defaultAccelerationProfileDefaultFrame_ = {
        {1.0e-3, 0.0e-3, 0.0e-3},
        {0.0e-3, 1.0e-3, 0.0e-3},
        {0.0e-3, 0.0e-3, 1.0e-3},
        {1.0e-3, 1.0e-3, 1.0e-3},
    };

    const Shared<const Frame> defaultFrameSPtr_ = Frame::GCRF();
    const Shared<const Frame> secondFrameSPtr_ = Frame::ITRF();

    const Array<Real> defaultMassFlowRateProfile_ = {-1.0e-5, -1.1e-5, -0.9e-5, -1.0e-5};
    const Real defaultConstantMassFlowRate_ = -0.5e-5;

    const Maneuver defaultManeuver_ = {
        defaultInstants_, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultMassFlowRateProfile_
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, Constructor)
{
    {
        EXPECT_NO_THROW(Maneuver(
            defaultInstants_, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultMassFlowRateProfile_
        ));
    }

    {
        EXPECT_NO_THROW(Maneuver(
            defaultInstants_, defaultAccelerationProfileDefaultFrame_, secondFrameSPtr_, defaultMassFlowRateProfile_
        ));
    }

    {
        const Array<Instant> shorterInstants = {
            Instant::J2000(), Instant::J2000() + Duration::Seconds(1.0), Instant::J2000() + Duration::Seconds(5.0)
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver(
                        shorterInstants,
                        defaultAccelerationProfileDefaultFrame_,
                        defaultFrameSPtr_,
                        defaultMassFlowRateProfile_
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(
                        "Acceleration profile must have the same number of elements as the number of instants.",
                        e.getMessage()
                    );
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        const Array<Real> shorterMassFlowRateProfile = {-1.0e-5, -1.1e-5, -0.9e-5};

        EXPECT_THROW(
            {
                try
                {
                    Maneuver(
                        defaultInstants_,
                        defaultAccelerationProfileDefaultFrame_,
                        defaultFrameSPtr_,
                        shorterMassFlowRateProfile
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(
                        "Mass flow rate profile must have the same number of elements as the number of instants.",
                        e.getMessage()
                    );
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        const Array<Instant> emptyInstants = {defaultInstants_[0]};
        const Array<Vector3d> emptyAccelerationProfile = {defaultAccelerationProfileDefaultFrame_[0]};
        const Array<Real> emptyMassFlowRateProfile = {defaultMassFlowRateProfile_[0]};

        EXPECT_THROW(
            {
                try
                {
                    Maneuver(emptyInstants, emptyAccelerationProfile, defaultFrameSPtr_, emptyMassFlowRateProfile);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("At least two instants are required to define a maneuver.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Maneuver({}, {}, defaultFrameSPtr_, {});
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("No instants or accompanying accelerations/mass flow rates provided.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        const Array<Instant> unorderedInstants = {
            Instant::J2000() + Duration::Seconds(1.0),
            Instant::J2000(),
            Instant::J2000() + Duration::Seconds(7.0),
            Instant::J2000() + Duration::Seconds(5.0),
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver(
                        unorderedInstants,
                        defaultAccelerationProfileDefaultFrame_,
                        defaultFrameSPtr_,
                        defaultMassFlowRateProfile_
                    );
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Unsorted Instant Array} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Array<Real> positiveMassFlowRateProfile = {1.0e-5, 1.1e-5, 0.9e-5, 1.0e-5};

        EXPECT_THROW(
            {
                try
                {
                    Maneuver(
                        defaultInstants_,
                        defaultAccelerationProfileDefaultFrame_,
                        defaultFrameSPtr_,
                        positiveMassFlowRateProfile
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Mass flow rate profile must be expressed in negative numbers.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultManeuver_ == defaultManeuver_);
    }

    {
        const Maneuver otherManeuver = {
            defaultInstants_, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultMassFlowRateProfile_
        };

        EXPECT_TRUE(defaultManeuver_ == otherManeuver);
    }

    {
        const Maneuver otherManeuver = {
            defaultInstants_, defaultAccelerationProfileDefaultFrame_, secondFrameSPtr_, defaultMassFlowRateProfile_
        };

        EXPECT_FALSE(defaultManeuver_ == otherManeuver);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultManeuver_ != defaultManeuver_);
    }

    {
        const Maneuver otherManeuver = {
            defaultInstants_, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultMassFlowRateProfile_
        };

        EXPECT_FALSE(defaultManeuver_ != otherManeuver);
    }

    {
        const Maneuver otherManeuver = {
            defaultInstants_, defaultAccelerationProfileDefaultFrame_, secondFrameSPtr_, defaultMassFlowRateProfile_
        };

        EXPECT_TRUE(defaultManeuver_ != otherManeuver);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << defaultManeuver_ << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultManeuver_.print(std::cout, true));
    EXPECT_NO_THROW(defaultManeuver_.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, IsDefined)
{
    {
        EXPECT_TRUE(defaultManeuver_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, Getters)
{
    {
        EXPECT_EQ(defaultManeuver_.getInstants(), defaultInstants_);
    }

    {
        EXPECT_EQ(defaultManeuver_.getAccelerationProfile(defaultFrameSPtr_), defaultAccelerationProfileDefaultFrame_);
    }

    {
        EXPECT_NE(defaultManeuver_.getAccelerationProfile(secondFrameSPtr_), defaultAccelerationProfileDefaultFrame_);
    }

    {
        EXPECT_EQ(defaultManeuver_.getMassFlowRateProfile(), defaultMassFlowRateProfile_);
    }

    {
        EXPECT_EQ(
            defaultManeuver_.getInterval(),
            Interval::Closed(defaultInstants_[0], defaultInstants_[defaultInstants_.getSize() - 1])
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, CalculateScalarQuantities)
{
    {
        EXPECT_DOUBLE_EQ(0.0013941812040755494, defaultManeuver_.calculateDeltaV());
    }

    {
        EXPECT_DOUBLE_EQ(1.26e-4, defaultManeuver_.calculateDeltaMass().inKilograms());
    }

    {
        EXPECT_DOUBLE_EQ(
            0.13941817086711084, defaultManeuver_.calculateAverageThrust(Mass(100.0, Mass::Unit::Kilogram))
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            789.81649564955535, defaultManeuver_.calculateAverageSpecificImpulse(Mass(100.0, Mass::Unit::Kilogram))

        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, ToTabulatedDynamics)
{
    {
        const Shared<TabulatedDynamics> tabulatedDynamicsSPtr =
            defaultManeuver_.toTabulatedDynamics(defaultFrameSPtr_, Interpolator::Type::BarycentricRational);

        EXPECT_EQ(tabulatedDynamicsSPtr->accessInstants(), defaultInstants_);

        const MatrixXd contributionProfile = tabulatedDynamicsSPtr->accessContributionProfile();

        EXPECT_EQ(contributionProfile.rows(), 4);
        EXPECT_EQ(contributionProfile.cols(), defaultInstants_.getSize());

        for (Size i = 0; i < defaultInstants_.getSize(); i++)
        {
            EXPECT_NEAR(contributionProfile(i, 0), defaultAccelerationProfileDefaultFrame_[i](0), 1.0e-15);
            EXPECT_NEAR(contributionProfile(i, 1), defaultAccelerationProfileDefaultFrame_[i](1), 1.0e-15);
            EXPECT_NEAR(contributionProfile(i, 2), defaultAccelerationProfileDefaultFrame_[i](2), 1.0e-15);
            EXPECT_NEAR(contributionProfile(i, 3), defaultMassFlowRateProfile_[i], 1.0e-15);
        }

        const Array<Shared<const CoordinateSubset>> writeCoordinateSubsets = {
            CartesianVelocity::Default(), CoordinateSubset::Mass()
        };
        EXPECT_EQ(tabulatedDynamicsSPtr->getWriteCoordinateSubsets(), writeCoordinateSubsets);
        EXPECT_EQ(tabulatedDynamicsSPtr->accessFrame(), defaultFrameSPtr_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, FromTabulatedDynamics)
{
    // Verify creation from tabulated dynamics
    {
        const Shared<TabulatedDynamics> tabulatedDynamicsSPtr =
            defaultManeuver_.toTabulatedDynamics(defaultFrameSPtr_, Interpolator::Type::BarycentricRational);

        const Maneuver maneuver = Maneuver::TabulatedDynamics(*tabulatedDynamicsSPtr);

        EXPECT_EQ(maneuver.getInstants(), defaultInstants_);
        EXPECT_EQ(maneuver.getAccelerationProfile(defaultFrameSPtr_), defaultAccelerationProfileDefaultFrame_);
        EXPECT_EQ(maneuver.getMassFlowRateProfile(), defaultMassFlowRateProfile_);
    }

    // Wrong dynamics coordinate subsets fail
    {
        const Array<Shared<const CoordinateSubset>> writeCoordinateSubsets = {CartesianVelocity::Default()};

        const Shared<TabulatedDynamics> wrongTabulatedDynamicsSPtr = std::make_shared<TabulatedDynamics>(
            defaultInstants_,
            MatrixXd::Zero(defaultInstants_.getSize(), 3),
            writeCoordinateSubsets,
            defaultFrameSPtr_,
            Interpolator::Type::BarycentricRational
        );

        EXPECT_THROW(
            {
                try
                {
                    Maneuver::TabulatedDynamics(*wrongTabulatedDynamicsSPtr);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Coordinate subset not found in write coordinate subsets.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    // Wrong dynamics coordinate subsets fail
    {
        const Array<Shared<const CoordinateSubset>> writeCoordinateSubsets = {CoordinateSubset::Mass()};

        const Shared<TabulatedDynamics> wrongTabulatedDynamicsSPtr = std::make_shared<TabulatedDynamics>(
            defaultInstants_,
            MatrixXd::Zero(defaultInstants_.getSize(), 1),
            writeCoordinateSubsets,
            defaultFrameSPtr_,
            Interpolator::Type::BarycentricRational
        );

        EXPECT_THROW(
            {
                try
                {
                    Maneuver::TabulatedDynamics(*wrongTabulatedDynamicsSPtr);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Coordinate subset not found in write coordinate subsets.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, ConstantMassFlowRateProfile)
{
    {
        const Maneuver maneuver = Maneuver::ConstantMassFlowRateProfile(
            defaultInstants_, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultConstantMassFlowRate_
        );

        EXPECT_EQ(maneuver.getInstants(), defaultInstants_);
        EXPECT_EQ(maneuver.getAccelerationProfile(defaultFrameSPtr_), defaultAccelerationProfileDefaultFrame_);
        EXPECT_EQ(
            maneuver.getMassFlowRateProfile(), Array<Real>(defaultInstants_.getSize(), defaultConstantMassFlowRate_)
        );
    }
}
