/// Apache License 2.0

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

    // Unsorted instant array
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
                    EXPECT_EQ("{Unsorted or Duplicate Instant Array} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    // Instant array with duplicates
    {
        const Array<Instant> duplicateInstants = {
            Instant::J2000(),
            Instant::J2000() + Duration::Seconds(1.0),
            Instant::J2000() + Duration::Seconds(1.0),
            Instant::J2000() + Duration::Seconds(5.0),
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver(
                        duplicateInstants,
                        defaultAccelerationProfileDefaultFrame_,
                        defaultFrameSPtr_,
                        defaultMassFlowRateProfile_
                    );
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Unsorted or Duplicate Instant Array} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    // Maneuver with some intervals larger than the maximum recommended interpolation interval
    {
        const Array<Instant> spacedOutInstants = {
            Instant::J2000(),
            Instant::J2000() + Maneuver::MaximumRecommendedInterpolationInterval * 2,
            Instant::J2000() + Maneuver::MaximumRecommendedInterpolationInterval * 2.5,
            Instant::J2000() + Maneuver::MaximumRecommendedInterpolationInterval * 3,
        };

        testing::internal::CaptureStdout();
        Maneuver(
            spacedOutInstants, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultMassFlowRateProfile_
        );
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    // Maneuver with duration of less than minimum recommended duration
    {
        const Array<Instant> shortInstants = {
            Instant::J2000(),
            Instant::J2000() + Maneuver::MinimumRecommendedDuration / 4,
            Instant::J2000() + Maneuver::MinimumRecommendedDuration / 3,
            Instant::J2000() + Maneuver::MinimumRecommendedDuration / 2,
        };

        testing::internal::CaptureStdout();
        Maneuver(
            shortInstants, defaultAccelerationProfileDefaultFrame_, defaultFrameSPtr_, defaultMassFlowRateProfile_
        );
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    // Acceleration profile with accelerations of zero magnitude
    {
        const Array<Vector3d> incorrectAccelerationProfile = {
            {0.0e-3, 0.0e-3, 0.0e-3},
            {0.0e-3, 1.0e-3, 0.0e-3},
            {0.0e-3, 0.0e-3, 1.0e-3},
            {1.0e-3, 1.0e-3, 1.0e-3},
        };

        EXPECT_THROW(
            {
                try
                {
                    Maneuver(
                        defaultInstants_, incorrectAccelerationProfile, defaultFrameSPtr_, defaultMassFlowRateProfile_
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Acceleration profile must have strictly positive magnitudes.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    // Mass flow rate profile with zero or positive mass flow rates
    {
        const Array<Real> incorrectMassFlowRateProfile = {0.0e-5, -1.1e-5, -0.9e-5, 1.0e-5};

        EXPECT_THROW(
            {
                try
                {
                    Maneuver(
                        defaultInstants_,
                        defaultAccelerationProfileDefaultFrame_,
                        defaultFrameSPtr_,
                        incorrectMassFlowRateProfile
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Mass flow rate profile must have strictly negative values.", e.getMessage());
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
    const Array<Instant> evenlySpacedInstants = {
        Instant::J2000(),
        Instant::J2000() + Duration::Minutes(1.0),
        Instant::J2000() + Duration::Minutes(2.0),
        Instant::J2000() + Duration::Minutes(3.0),
    };

    const Array<Instant> unevenlySpacedInstants = {
        Instant::J2000(),
        Instant::J2000() + Duration::Minutes(0.5),  // Larger gap in the middle
        Instant::J2000() + Duration::Minutes(2.5),
        Instant::J2000() + Duration::Minutes(3.0),
    };

    // Profile chosen to produce the same dV as equally spaced 1 m/s profiles
    const Array<Vector3d> accelerationProfileDefaultFrame = {
        {0.5, 0.0, 0.0},
        {1.25, 0.0, 0.0},
        {1.25, 0.0, 0.0},
        {0.5, 0.0, 0.0},
    };
    // Constant and non-constant mass flow rates chosen to produce the same dM as equally spaced 0.1 kg/s profiles
    const Real constantMassFlowRate = -1.0e-1;
    const Array<Real> nonConstantMassFlowRateProfile = {-0.5e-1, -1.25e-1, -1.25e-1, -0.5e-1};

    const Maneuver evenlySpacedManeuverWithConstantMassFlowRate = Maneuver::ConstantMassFlowRateProfile(
        evenlySpacedInstants, accelerationProfileDefaultFrame, defaultFrameSPtr_, constantMassFlowRate
    );

    const Maneuver unevenlySpacedManeuverWithConstantMassFlowRate = Maneuver::ConstantMassFlowRateProfile(
        unevenlySpacedInstants, accelerationProfileDefaultFrame, defaultFrameSPtr_, constantMassFlowRate
    );

    const Maneuver evenlySpacedManeuverWithNonConstantMassFlowRate = {
        evenlySpacedInstants, accelerationProfileDefaultFrame, defaultFrameSPtr_, nonConstantMassFlowRateProfile
    };

    const Maneuver unevenlySpacedManeuverWithNonConstantMassFlowRate = {
        unevenlySpacedInstants, accelerationProfileDefaultFrame, defaultFrameSPtr_, nonConstantMassFlowRateProfile
    };

    {
        // dV equivalent to 1.0 m/s * 180 s = 180 m/s
        EXPECT_DOUBLE_EQ(180.0, evenlySpacedManeuverWithConstantMassFlowRate.calculateDeltaV());
        // acceleration is higher for a greater portion of the burn, so total dV is higher
        EXPECT_DOUBLE_EQ(202.5, unevenlySpacedManeuverWithConstantMassFlowRate.calculateDeltaV());
        // dV equivalent to 1.0 m/s * 180 s = 180 m/s
        EXPECT_DOUBLE_EQ(180.0, evenlySpacedManeuverWithNonConstantMassFlowRate.calculateDeltaV());
        // acceleration is higher for a greater portion of the burn, so total dV is higher
        EXPECT_DOUBLE_EQ(202.5, unevenlySpacedManeuverWithNonConstantMassFlowRate.calculateDeltaV());
    }

    {
        // dM equivalent to 0.1 kg/s * 180 s = 18 kg
        EXPECT_DOUBLE_EQ(18.0, evenlySpacedManeuverWithConstantMassFlowRate.calculateDeltaMass().inKilograms());
        // dM equivalent to 0.1 kg/s * 180 s = 18 kg
        EXPECT_DOUBLE_EQ(18.0, unevenlySpacedManeuverWithConstantMassFlowRate.calculateDeltaMass().inKilograms());
        // dM equivalent to 0.1 kg/s * 180 s = 18 kg
        EXPECT_DOUBLE_EQ(18.0, evenlySpacedManeuverWithNonConstantMassFlowRate.calculateDeltaMass().inKilograms());
        // mass flow rate is higher for a greater portion of the burn, so total dM is higher
        EXPECT_DOUBLE_EQ(20.25, unevenlySpacedManeuverWithNonConstantMassFlowRate.calculateDeltaMass().inKilograms());
    }

    {
        EXPECT_DOUBLE_EQ(
            88.0,  // The average thrust is less than 100N because the spacecraft mass is decreasing during the burn
            evenlySpacedManeuverWithConstantMassFlowRate.calculateAverageThrust(Mass(100.0, Mass::Unit::Kilogram))
        );

        EXPECT_DOUBLE_EQ(
            96.9375,  // Higher than evenlySpacedManeuverWithConstantMassFlowRate because acceleration is higher for a
                      // greater portion of the burn
            unevenlySpacedManeuverWithConstantMassFlowRate.calculateAverageThrust(Mass(100.0, Mass ::Unit::Kilogram))
        );

        EXPECT_DOUBLE_EQ(
            87.90625,  // Very slightly lower than evenlySpacedManeuverWithConstantMassFlowRate's because non-constantly
                       // decreasing spacecraft mass
            evenlySpacedManeuverWithNonConstantMassFlowRate.calculateAverageThrust(Mass(100.0, Mass ::Unit::Kilogram))
        );

        EXPECT_DOUBLE_EQ(
            94.4765625,  // Very slightly lower than unevenlySpacedManeuverWithConstantMassFlowRate's because
                         // non-constantly decreasing spacecraft mass
            unevenlySpacedManeuverWithNonConstantMassFlowRate.calculateAverageThrust(Mass(100.0, Mass ::Unit::Kilogram))
        );
    }

    {
        EXPECT_DOUBLE_EQ(
            89.735026742057684,  // The specific impulse is less than 100s because the spacecraft mass is decreasing
                                 // during the burn
            evenlySpacedManeuverWithConstantMassFlowRate.calculateAverageSpecificImpulse(
                Mass(100.0, Mass::Unit::Kilogram)
            )
        );

        EXPECT_DOUBLE_EQ(
            98.848740395547921,  // Higher than evenlySpacedManeuverWithConstantMassFlowRate because acceleration is
                                 // higher for a greater portion of the burn
            unevenlySpacedManeuverWithConstantMassFlowRate.calculateAverageSpecificImpulse(
                Mass(100.0, Mass::Unit::Kilogram)
            )
        );

        EXPECT_DOUBLE_EQ(
            89.639428347090998,  // Very slightly lower than evenlySpacedManeuverWithConstantMassFlowRate's because
                                 // non-constantly decreasing
                                 // spacecraft mass
            evenlySpacedManeuverWithNonConstantMassFlowRate.calculateAverageSpecificImpulse(
                Mass(100.0, Mass::Unit::Kilogram)
            )
        );

        EXPECT_DOUBLE_EQ(
            85.634917802375611,  // Very slightly lower than unevenlySpacedManeuverWithConstantMassFlowRate's because
                                 // non-constantly decreasing
                                 // spacecraft mass
            unevenlySpacedManeuverWithNonConstantMassFlowRate.calculateAverageSpecificImpulse(
                Mass(100.0, Mass::Unit::Kilogram)
            )
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
