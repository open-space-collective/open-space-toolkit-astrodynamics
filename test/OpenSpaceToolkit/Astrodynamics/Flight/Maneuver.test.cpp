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

using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Flight_Maneuver : public ::testing::Test
{
    // void SetUp() override {}

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

    const Array<Real> defaultMassFlowRateProfile_ = {1.0e-5, 1.1e-5, 0.9e-5, 1.0e-5};
    const Real defaultConstantMassFlowRate_ = 0.5e-5;

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
        EXPECT_NEAR(0.001394181, defaultManeuver_.calculateDeltaV(), 1.0e-4);
    }

    {
        EXPECT_NEAR(1.26e-4, defaultManeuver_.calculateDeltaMass().inKilograms(), 1.0e-4);
    }

    {
        EXPECT_NEAR(0.1393676608, defaultManeuver_.calculateAverageThrust(Mass(100.0, Mass::Unit::Kilogram)), 1.0e-4);
    }

    {
        EXPECT_NEAR(
            789.816, defaultManeuver_.calculateAverageSpecificImpulse(Mass(100.0, Mass::Unit::Kilogram)), 1.0e-4
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, CalculateAccelerationAt)
{
    // Check at default instants
    {
        for (Size i = 0; i < defaultInstants_.getSize(); i++)
        {
            EXPECT_TRUE(defaultAccelerationProfileDefaultFrame_[i].isApprox(
                defaultManeuver_.calculateAccelerationAt(
                    defaultInstants_[i], defaultFrameSPtr_, Interpolator::Type::Linear
                ),
                1.0e-15
            ));
        }
    }

    // Check at instant in between default instants
    {
        const Instant instantToCheck = defaultInstants_[0] + (defaultInstants_[1] - defaultInstants_[0]) / 2;

        const Vector3d interpolatedAcceleration =
            defaultManeuver_.calculateAccelerationAt(instantToCheck, defaultFrameSPtr_, Interpolator::Type::Linear);

        const Vector3d expectedAcceleration =
            defaultAccelerationProfileDefaultFrame_[0] * 0.5 + defaultAccelerationProfileDefaultFrame_[1] * 0.5;

        EXPECT_TRUE(interpolatedAcceleration.isApprox(expectedAcceleration, 1.0e-15));
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    defaultManeuver_.calculateAccelerationAt(
                        defaultInstants_[0] - Duration::Seconds(1.0), defaultFrameSPtr_, Interpolator::Type::Linear
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Instant must be within the range of the maneuver.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, CalculateAccelerationsAt)
{
    // Check at default instants
    {
        const Array<Vector3d> interpolatedAccelerations =
            defaultManeuver_.calculateAccelerationsAt(defaultInstants_, defaultFrameSPtr_, Interpolator::Type::Linear);

        EXPECT_EQ(defaultInstants_.getSize(), interpolatedAccelerations.getSize());

        for (Size i = 0; i < interpolatedAccelerations.getSize(); i++)
        {
            EXPECT_TRUE(interpolatedAccelerations[i].isApprox(defaultAccelerationProfileDefaultFrame_[i], 1.0e-15));
        }
    }

    // Check at instant in between default instants
    {
        const Array<Instant> instantsToCheck = {
            defaultInstants_[0] + (defaultInstants_[1] - defaultInstants_[0]) / 2,
            defaultInstants_[1] + (defaultInstants_[2] - defaultInstants_[1]) / 2,
            defaultInstants_[2] + (defaultInstants_[3] - defaultInstants_[2]) / 2,
        };

        const Array<Vector3d> interpolatedAccelerations =
            defaultManeuver_.calculateAccelerationsAt(instantsToCheck, defaultFrameSPtr_, Interpolator::Type::Linear);

        EXPECT_EQ(instantsToCheck.getSize(), interpolatedAccelerations.getSize());

        for (Size i = 0; i < interpolatedAccelerations.getSize(); i++)
        {
            const Vector3d expectedAcceleration =
                defaultAccelerationProfileDefaultFrame_[i] * 0.5 + defaultAccelerationProfileDefaultFrame_[i + 1] * 0.5;

            EXPECT_TRUE(interpolatedAccelerations[i].isApprox(expectedAcceleration, 1.0e-15));
        }
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    defaultManeuver_.calculateAccelerationsAt(
                        {defaultInstants_[0] - Duration::Seconds(1.0)}, defaultFrameSPtr_, Interpolator::Type::Linear
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Instants must be within the range of the maneuver.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, CalculateMassFlowRateAt)
{
    // Check at default instants
    {
        for (Size i = 0; i < defaultInstants_.getSize(); i++)
        {
            EXPECT_NEAR(
                defaultMassFlowRateProfile_[i],
                defaultManeuver_.calculateMassFlowRateAt(defaultInstants_[i], Interpolator::Type::Linear),
                1.0e-15
            );
        }
    }

    // Check at instant in between default instants
    {
        const Instant instantToCheck = defaultInstants_[0] + (defaultInstants_[1] - defaultInstants_[0]) / 2;

        const Real interpolatedMassFlowRate =
            defaultManeuver_.calculateMassFlowRateAt(instantToCheck, Interpolator::Type::Linear);

        const Real expectedMassFlowRate = defaultMassFlowRateProfile_[0] * 0.5 + defaultMassFlowRateProfile_[1] * 0.5;

        EXPECT_NEAR(interpolatedMassFlowRate, expectedMassFlowRate, 1.0e-15);
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    defaultManeuver_.calculateMassFlowRateAt(
                        defaultInstants_[0] - Duration::Seconds(1.0), Interpolator::Type::Linear
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Instant must be within the range of the maneuver.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, CalculateMassFlowRatesAt)
{
    // Check at default instants
    {
        const Array<Real> interpolatedMassFlowRates =
            defaultManeuver_.calculateMassFlowRatesAt(defaultInstants_, Interpolator::Type::Linear);

        EXPECT_EQ(defaultInstants_.getSize(), interpolatedMassFlowRates.getSize());

        for (Size i = 0; i < interpolatedMassFlowRates.getSize(); i++)
        {
            EXPECT_NEAR(interpolatedMassFlowRates[i], defaultMassFlowRateProfile_[i], 1.0e-15);
        }
    }

    // Check at instant in between default instants
    {
        const Array<Instant> instantsToCheck = {
            defaultInstants_[0] + (defaultInstants_[1] - defaultInstants_[0]) / 2,
            defaultInstants_[1] + (defaultInstants_[2] - defaultInstants_[1]) / 2,
            defaultInstants_[2] + (defaultInstants_[3] - defaultInstants_[2]) / 2,
        };

        const Array<Real> interpolatedMassFlowRates =
            defaultManeuver_.calculateMassFlowRatesAt(instantsToCheck, Interpolator::Type::Linear);

        EXPECT_EQ(instantsToCheck.getSize(), interpolatedMassFlowRates.getSize());

        for (Size i = 0; i < interpolatedMassFlowRates.getSize(); i++)
        {
            const Real expectedMassFlowRate =
                defaultMassFlowRateProfile_[i] * 0.5 + defaultMassFlowRateProfile_[i + 1] * 0.5;

            EXPECT_NEAR(interpolatedMassFlowRates[i], expectedMassFlowRate, 1.0e-15);
        }
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    defaultManeuver_.calculateMassFlowRatesAt(
                        {defaultInstants_[0] - Duration::Seconds(1.0)}, Interpolator::Type::Linear
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Instants must be within the range of the maneuver.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, ToTabulatedDynamics)
{
    {
        const Shared<TabulatedDynamics> tabulatedDynamicsSPtr =
            defaultManeuver_.toTabulatedDynamics(defaultFrameSPtr_, Interpolator::Type::BarycentricRational);

        EXPECT_EQ(tabulatedDynamicsSPtr->getInstants(), defaultInstants_);

        const MatrixXd contributionProfile = tabulatedDynamicsSPtr->getContributionProfile();

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
        EXPECT_EQ(tabulatedDynamicsSPtr->getFrame(), defaultFrameSPtr_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Maneuver, FromConstantMassFlowRateProfile)
{
    {
        const Maneuver maneuver = Maneuver::FromConstantMassFlowRateProfile(
            defaultInstants_, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultConstantMassFlowRate_
        );

        EXPECT_EQ(maneuver.getInstants(), defaultInstants_);
        EXPECT_EQ(maneuver.getAccelerationProfile(defaultFrameSPtr_), defaultAccelerationProfileDefaultFrame_);
        EXPECT_EQ(
            maneuver.getMassFlowRateProfile(), Array<Real>(defaultInstants_.getSize(), defaultConstantMassFlowRate_)
        );
    }
}
